/*
 * Copyright (c) 2009, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 * Written by Adam Moody <moody20@llnl.gov>.
 * LLNL-CODE-411039.
 * All rights reserved.
 * This file is part of The Scalable Checkpoint / Restart (SCR) library.
 * For details, see https://sourceforge.net/projects/scalablecr/
 * Please also read this file: LICENSE.TXT.
*/

/* Defines a recursive hash data structure, where at the top level,
 * there is a list of elements indexed by string.  Each
 * of these elements in turn consists of a list of elements
 * indexed by string, and so on. */

#include "scr.h"
#include "scr_io.h"
#include "scr_err.h"
#include "scr_util.h"
#include "scr_hash.h"
#include "scr_hash_mpi.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

/*
=========================================
Hash MPI transfer functions
=========================================
*/

/* packs and send the given hash to the specified rank */
int scr_hash_send(const scr_hash* hash, int rank, MPI_Comm comm)
{
  /* first get the size of the hash */
  size_t size = scr_hash_pack_size(hash);
  
  /* tell rank how big the pack size is */
  MPI_Send(&size, sizeof(size), MPI_BYTE, rank, 0, comm);

  /* pack the hash and send it */
  if (size > 0) {
    /* allocate a buffer big enough to pack the hash */
    char* buf = (char*) malloc(size);
    if (buf != NULL) {
      /* pack the hash, send it, and free our buffer */
      scr_hash_pack(buf, hash);
      MPI_Send(buf, size, MPI_BYTE, rank, 0, comm);
      scr_free(&buf);
    } else {
      scr_abort(-1, "scr_hash_send: Failed to malloc buffer to pack hash @ %s:%d",
              __FILE__, __LINE__
      );
    }
  }

  return SCR_SUCCESS;
}

/* receives a hash from the specified rank and unpacks it into specified hash */
int scr_hash_recv(scr_hash* hash, int rank, MPI_Comm comm)
{
  /* check that we got a hash to receive into */
  if (hash == NULL) {
    return SCR_FAILURE;
  }

  /* clear the hash */
  scr_hash_unset_all(hash);

  /* get the size of the incoming hash */
  MPI_Status status;
  size_t size = 0;
  MPI_Recv(&size, sizeof(size), MPI_BYTE, rank, 0, comm, &status);
  
  /* receive the hash and unpack it */
  if (size > 0) {
    /* allocate a buffer big enough to receive the packed hash */
    char* buf = (char*) malloc(size);
    if (buf != NULL) {
      /* receive the hash, unpack it, and free our buffer */
      MPI_Recv(buf, size, MPI_BYTE, rank, 0, comm, &status);
      scr_hash_unpack(buf, hash);
      scr_free(&buf);
    } else {
      scr_abort(-1, "scr_hash_recv: Failed to malloc buffer to receive hash @ %s:%d",
              __FILE__, __LINE__
      );
    }
  }

  return SCR_SUCCESS;
}

/* send and receive a hash in the same step */
int scr_hash_sendrecv(const scr_hash* hash_send, int rank_send,
                            scr_hash* hash_recv, int rank_recv,
                            MPI_Comm comm)
{
  int rc = SCR_SUCCESS;

  int num_req;
  MPI_Request request[2];
  MPI_Status  status[2];

  /* determine whether we have a rank to send to and a rank to receive from */
  int have_outgoing = 0;
  int have_incoming = 0;
  if (rank_send != MPI_PROC_NULL) {
    have_outgoing = 1;
  }
  if (rank_recv != MPI_PROC_NULL) {
    scr_hash_unset_all(hash_recv);
    have_incoming = 1;
  }

  /* exchange hash pack sizes in order to allocate buffers */
  num_req = 0;
  size_t size_send = 0;
  size_t size_recv = 0;
  if (have_incoming) {
    MPI_Irecv(&size_recv, sizeof(size_recv), MPI_BYTE, rank_recv, 0, comm, &request[num_req]);
    num_req++;
  }
  if (have_outgoing) {
    size_send = scr_hash_pack_size(hash_send);
    MPI_Isend(&size_send, sizeof(size_send), MPI_BYTE, rank_send, 0, comm, &request[num_req]);
    num_req++;
  }
  if (num_req > 0) {
    MPI_Waitall(num_req, request, status);
  }

  /* allocate space to pack our hash and space to receive the incoming hash */
  num_req = 0;
  char* buf_send = NULL;
  char* buf_recv = NULL;
  if (size_recv > 0) {
    /* allocate space to receive a packed hash, and receive it */
    buf_recv = (char*) malloc(size_recv);
    /* TODO: check for errors */
    MPI_Irecv(buf_recv, size_recv, MPI_BYTE, rank_recv, 0, comm, &request[num_req]);
    num_req++;
  }
  if (size_send > 0) {
    /* allocate space, pack our hash, and send it */
    buf_send = (char*) malloc(size_send);
    /* TODO: check for errors */
    scr_hash_pack(buf_send, hash_send);
    MPI_Isend(buf_send, size_send, MPI_BYTE, rank_send, 0, comm, &request[num_req]);
    num_req++;
  }
  if (num_req > 0) {
    MPI_Waitall(num_req, request, status);
  }

  /* unpack the hash into the hash_recv provided by the caller */
  if (size_recv > 0) {
    scr_hash_unpack(buf_recv, hash_recv);
  }

  /* free the pack buffers */
  scr_free(&buf_recv);
  scr_free(&buf_send);

  return rc;
}

/* broadcasts a hash from a root and unpacks it into specified hash on all other tasks */
int scr_hash_bcast(scr_hash* hash, int root, MPI_Comm comm)
{
  /* get our rank in the communicator */
  int rank;
  MPI_Comm_rank(comm, &rank);

  /* determine whether we are the root of the bcast */
  if (rank == root) {
    /* first get the size of the hash */
    size_t size = scr_hash_pack_size(hash);
  
    /* broadcast the size */
    MPI_Bcast(&size, sizeof(size), MPI_BYTE, root, comm);

    /* pack the hash and send it */
    if (size > 0) {
      /* allocate a buffer big enough to pack the hash */
      char* buf = (char*) malloc(size);
      if (buf != NULL) {
        /* pack the hash, broadcast it, and free our buffer */
        scr_hash_pack(buf, hash);
        MPI_Bcast(buf, size, MPI_BYTE, root, comm);
        scr_free(&buf);
      } else {
        scr_abort(-1, "scr_hash_bcast: Failed to malloc buffer to pack hash @ %s:%d",
                __FILE__, __LINE__
        );
      }
    }
  } else {
    /* clear the hash */
    scr_hash_unset_all(hash);

    /* get the size of the incoming hash */
    size_t size = 0;
    MPI_Bcast(&size, sizeof(size), MPI_BYTE, root, comm);
  
    /* receive the hash and unpack it */
    if (size > 0) {
      /* allocate a buffer big enough to receive the packed hash */
      char* buf = (char*) malloc(size);
      if (buf != NULL) {
        /* receive the hash, unpack it, and free our buffer */
        MPI_Bcast(buf, size, MPI_BYTE, root, comm);
        scr_hash_unpack(buf, hash);
        scr_free(&buf);
      } else {
        scr_abort(-1, "scr_hash_bcast: Failed to malloc buffer to receive hash @ %s:%d",
                __FILE__, __LINE__
        );
      }
    }
  }

  return SCR_SUCCESS;
}

/* execute a (sparse) global exchange, similar to an alltoallv operation
 * 
 * hash_send specifies destinations as:
 * <rank_X>
 *   <hash_to_send_to_rank_X>
 * <rank_Y>
 *   <hash_to_send_to_rank_Y>
 *
 * hash_recv returns hashes sent from remote ranks as:
 * <rank_A>
 *   <hash_received_from_rank_A>
 * <rank_B>
 *   <hash_received_from_rank_B> */
int scr_hash_exchange(const scr_hash* hash_send, scr_hash* hash_recv, MPI_Comm comm)
{
  /* get our rank and number of ranks in comm */
  int rank, ranks;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &ranks);

  /* since we have two paths, we try to be more efficient by sending
   * each item in the direction of fewest hops */
  scr_hash* left  = scr_hash_new();
  scr_hash* right = scr_hash_new();

  /* iterate through elements and assign to left or right hash */
  scr_hash_elem* elem;
  for (elem = scr_hash_elem_first(hash_send);
       elem != NULL;
       elem = scr_hash_elem_next(elem))
  {
    /* get dest rank and pointer to hash for that rank */
    int dest = scr_hash_elem_key_int(elem);
    scr_hash* elem_hash = scr_hash_elem_hash(elem);

    /* compute distance to our left */
    int dist_left = rank - dest;
    if (dist_left < 0) {
      dist_left += ranks;
    }

    /* compute distance to our right */
    int dist_right = dest - rank;
    if (dist_right < 0) {
      dist_right += ranks;
    }

    /* count hops in each direction */
    int hops_left = 0;
    int hops_right = 0;
    int bit = 1;
    int step = 1;
    while (step < ranks) {
      /* if distance is odd in this bit,
       * we'd send it during this step */
      if (dist_left & bit) {
        hops_left++;
      }
      if (dist_right & bit) {
        hops_right++;
      }

      /* go to the next step */
      bit <<= 1;
      step *= 2;
    }

    /* assign to hash having the fewest hops */
    scr_hash* tmp = scr_hash_new();
    scr_hash_merge(tmp, elem_hash);
    if (hops_left < hops_right) {
      scr_hash_setf(left, tmp, "%d", dest);
    } else {
      scr_hash_setf(right, tmp, "%d", dest);
    }
  }

  /* deletegate work to scr_hash_exchange_direction */
  int rc = scr_hash_exchange_direction(
    left, hash_recv, comm, SCR_HASH_EXCHANGE_LEFT
  );
  int right_rc = scr_hash_exchange_direction(
    right, hash_recv, comm, SCR_HASH_EXCHANGE_RIGHT
  );
  if (rc == SCR_SUCCESS) {
    rc = right_rc;
  }

  /* free our left and right hashes */
  scr_hash_delete(&right);
  scr_hash_delete(&left);

  return rc;
}

/* execute a (sparse) global exchange, similar to an alltoallv operation
 * 
 * hash_send specifies destinations as:
 * <rank_X>
 *   <hash_to_send_to_rank_X>
 * <rank_Y>
 *   <hash_to_send_to_rank_Y>
 *
 * hash_recv returns hashes sent from remote ranks as:
 * <rank_A>
 *   <hash_received_from_rank_A>
 * <rank_B>
 *   <hash_received_from_rank_B> */
int scr_hash_exchange_direction(
  const scr_hash* hash_in,
        scr_hash* hash_out,
  MPI_Comm comm,
  scr_hash_exchange_enum direction)
{
  scr_hash_elem* elem;

  /* get the size of our communicator and our position within it */
  int rank, ranks;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &ranks);

  /* set up current hash using input hash */
  scr_hash* current = scr_hash_new();
  for (elem = scr_hash_elem_first(hash_in);
       elem != NULL;
       elem = scr_hash_elem_next(elem))
  {
    /* get destination rank */
    int dest_rank = scr_hash_elem_key_int(elem);

    /* copy data into current hash with DEST and SRC keys */
    scr_hash* data_hash = scr_hash_elem_hash(elem);
    scr_hash* dest_hash = scr_hash_set_kv_int(current,   "D", dest_rank);
    scr_hash* src_hash  = scr_hash_set_kv_int(dest_hash, "S", rank);
    scr_hash_merge(src_hash, data_hash);
  }

  /* now run through Bruck's index algorithm to exchange data */
  int factor = 1;
  while (factor < ranks) {
    /* determine our source and destination ranks for this step */
    int dst, src;
    if (direction == SCR_HASH_EXCHANGE_RIGHT) {
      dst = (rank + factor + ranks) % ranks;
      src = (rank - factor + ranks) % ranks;
    } else {
      dst = (rank - factor + ranks) % ranks;
      src = (rank + factor + ranks) % ranks;
    }

    /* create hashes for those we'll keep, send, and receive */
    scr_hash* keep = scr_hash_new();
    scr_hash* send = scr_hash_new();
    scr_hash* recv = scr_hash_new();

    /* identify hashes we'll send and keep in this step */
    scr_hash* dest_hash = scr_hash_get(current, "D");
    for (elem = scr_hash_elem_first(dest_hash);
         elem != NULL;
         elem = scr_hash_elem_next(elem))
    {
      /* get destination rank and pointer to its hash */
      int dest_rank = scr_hash_elem_key_int(elem);
      scr_hash* elem_hash = scr_hash_elem_hash(elem);

      /* compute relative distance from our rank */
      int relative_rank;
      if (direction == SCR_HASH_EXCHANGE_RIGHT) {
        relative_rank = (dest_rank - rank + ranks) % ranks;
      } else {
        relative_rank = (rank - dest_rank + ranks) % ranks;
      }

      /* we send the hash if the distance is not divisible by 2 */
      int relative_id = (relative_rank / factor) % 2;
      if (relative_id > 0) {
        /* copy hash to send */
        scr_hash* dest_send = scr_hash_set_kv_int(send, "D", dest_rank);
        scr_hash_merge(dest_send, elem_hash);
      } else {
        /* copy hash to keep */
        scr_hash* dest_keep = scr_hash_set_kv_int(keep, "D", dest_rank);
        scr_hash_merge(dest_keep, elem_hash);
      }
    }

    /* exchange hashes with our partners */
    scr_hash_sendrecv(send, dst, recv, src, comm);

    /* merge received hash into keep */
    scr_hash_merge(keep, recv);

    /* delete current hash and point it to keep instead */
    scr_hash_delete(&current);
    current = keep;

    /* prepare for next rount */
    scr_hash_delete(&recv);
    scr_hash_delete(&send);
    factor *= 2;
  }

  /* copy current into output hash */
  scr_hash* dest_hash = scr_hash_get_kv_int(current, "D", rank);
  scr_hash* elem_hash = scr_hash_get(dest_hash, "S");
  scr_hash_merge(hash_out, elem_hash);

  /* free the current hash */
  scr_hash_delete(&current);

  return SCR_SUCCESS;
}
