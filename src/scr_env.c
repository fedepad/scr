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

/* All rights reserved. This program and the accompanying materials
 * are made available under the terms of the BSD-3 license which accompanies this
 * distribution in LICENSE.TXT
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the BSD-3  License in
 * LICENSE.TXT for more details.
 *
 * GOVERNMENT LICENSE RIGHTS-OPEN SOURCE SOFTWARE
 * The Government's rights to use, modify, reproduce, release, perform,
 * display, or disclose this software are subject to the terms of the BSD-3
 * License as provided in Contract No. B609815.
 * Any reproduction of computer software, computer software documentation, or
 * portions thereof marked with this legend must also reproduce the markings.
 *
 * Author: Christopher Holguin <christopher.a.holguin@intel.com>
 *
 * (C) Copyright 2015-2016 Intel Corporation.
 */

/* This implements the scr_err.h interface, but for serial jobs,
 * like the SCR utilities. */

#include <config.h>

#include "scr_env.h"
#include "scr_err.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef HAVE_LIBYOGRT
#include "yogrt.h"
#else
#include <time.h>
#endif /* HAVE_LIBYOGRT */

#if SCR_MACHINE_TYPE == SCR_PMIX
#include "pmix.h"
#include "scr_globals.h"
#endif /* SCR_MACHINE_TYPE == SCR_PMIX */

#if (SCR_MACHINE_TYPE == SCR_TLCC) || (SCR_MACHINE_TYPE == SCR_CRAY_XT) || (SCR_MACHINE_TYPE == SCR_PMIX) || (SCR_MACHINE_TYPE == SCR_LSF)
#include <unistd.h> /* gethostname */
#endif

#if SCR_MACHINE_TYPE == SCR_BGQ
#include "firmware/include/personality.h" /* Personality_t */
#include "spi/include/kernel/location.h"  /* Kernel_GetPersonality */
#include "hwi/include/common/uci.h"       /* bg_decodeComputeCardCoreOnNodeBoardUCI */
#endif

/*
=========================================
This file contains functions that read / write
machine-dependent information.
=========================================
*/

/* returns the number of seconds remaining in the time allocation */
long int scr_env_seconds_remaining()
{
  /* returning a negative number tells the caller this functionality is disabled */
  long int secs = -1;

  /* call libyogrt if we have it */
  #ifdef HAVE_LIBYOGRT
    secs = yogrt_remaining();
    if (secs < 0) {
      secs = 0;
    }
  #else
    char* scr_end_time = getenv("SCR_END_TIME");
    if (scr_end_time){//return -1 if SCR_END_TIME not set
      long int end_time = atol(scr_end_time);
      if (end_time > 0){//return -1 if SCR_END_TIME is not convertible to long int
	secs = end_time - (long int)time(NULL);
	if (secs < 0){
	  secs = 0;
	}
      }
    }
  #endif /* HAVE_LIBYOGRT */

  return secs;
}

/* allocate and return a string containing the current username */
char* scr_env_username()
{
  char* name = NULL;

  /* read $USER environment variable for username */
  char* value;
  if ((value = getenv("USER")) != NULL) {
    name = strdup(value);
    if (name == NULL) {
      scr_err("Failed to allocate memory to record username (%s) @ %s:%d",
              value, __FILE__, __LINE__
      );
    }
  }

  return name;
}

/* allocate and return a string containing the current job id */
char* scr_env_jobid()
{
  char* jobid = NULL;

  char* value;
  #if (SCR_MACHINE_TYPE == SCR_TLCC) || (SCR_MACHINE_TYPE == SCR_BGQ)
    /* read $SLURM_JOBID environment variable for jobid string */
    if ((value = getenv("SLURM_JOBID")) != NULL) {
      jobid = strdup(value);
      if (jobid == NULL) {
        scr_err("Failed to allocate memory to record jobid (%s) @ %s:%d",
                value, __FILE__, __LINE__
        );
      }
    }
  #elif SCR_MACHINE_TYPE == SCR_CRAY_XT
    /* read $PBS_JOBID environment variable for jobid string */
    if ((value = getenv("PBS_JOBID")) != NULL) {
      jobid = strdup(value);
      if (jobid == NULL) {
        scr_err("Failed to allocate memory to record jobid (%s) @ %s:%d",
                value, __FILE__, __LINE__
        );
      }
    }
  #elif SCR_MACHINE_TYPE == SCR_PMIX
    /* todo: must replace this in the scr_env script as well */
    pmix_pdata_t *pmix_query_data = NULL;
    PMIX_PDATA_CREATE(pmix_query_data, 1);

    /* todo: pmix_pdata_destroy ?? */

    /* specify that we want our jobid from pmix */
    strncpy(pmix_query_data[0].key, PMIX_JOBID, PMIX_MAX_KEYLEN);

    /* query pmix for our job id */
    pmix_status_t retval = PMIx_Lookup(pmix_query_data, 1, NULL, 0);
    if (retval == PMIX_SUCCESS) {
      /* got it, strdup the value from pmix */
      jobid = strdup(pmix_query_data[0].value.data.string);
      scr_dbg(1, "PMIx_Lookup for jobid success '%s'", jobid);
    } else {
      /* failed to get our jobid from pmix, make one up */
      char *pmix_hardcoded_id = "pmix_hardcoded_jobid";
      jobid = strdup(pmix_hardcoded_id);
      scr_dbg(1, "PMIx_Lookup for jobid failed: rc=%d, using hardcoded jobid '%s'",
        retval, jobid
      );
    }

    /* free pmix query structure */
    PMIX_PDATA_FREE(pmix_query_data, 1);
  #elif SCR_MACHINE_TYPE == SCR_LSF
    /* read $PBS_JOBID environment variable for jobid string */
    if ((value = getenv("LSB_JOBID")) != NULL) {
      jobid = strdup(value);
      if (jobid == NULL) {
        scr_err("Failed to allocate memory to record jobid (%s) @ %s:%d",
                value, __FILE__, __LINE__
        );
      }
    }
  #endif

  return jobid;
}

/* allocate and return a string containing the node name */
char* scr_env_nodename()
{
  char* name = NULL;

  #if (SCR_MACHINE_TYPE == SCR_TLCC) || (SCR_MACHINE_TYPE == SCR_CRAY_XT) || (SCR_MACHINE_TYPE == SCR_PMIX) || (SCR_MACHINE_TYPE == SCR_LSF)
    /* we just use the string returned by gethostname */
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
      name = strdup(hostname);
    } else {
      scr_err("Call to gethostname failed @ %s:%d",
        __FILE__, __LINE__
      );
    }
  #elif SCR_MACHINE_TYPE == SCR_BGQ
    /* here, we derive a string from the personality */
    Personality_t personality;
    unsigned int x, y, m, n, j, c;
    Kernel_GetPersonality(&personality, sizeof(personality));
    bg_decodeComputeCardCoreOnNodeBoardUCI(personality.Kernel_Config.UCI,&x,&y,&m,&n,&j,&c);

    /* construct the hostname */
    char hostname[256];
    int num = snprintf(
      hostname, sizeof(hostname),
      "R%X%X-M%d-N%02d-J%02d-A%dof%d-B%dof%d-C%dof%d-D%dof%d-E%dof%d",
      x, y, m, n, j,
      personality.Network_Config.Acoord, personality.Network_Config.Anodes,
      personality.Network_Config.Bcoord, personality.Network_Config.Bnodes,
      personality.Network_Config.Ccoord, personality.Network_Config.Cnodes,
      personality.Network_Config.Dcoord, personality.Network_Config.Dnodes,
      personality.Network_Config.Ecoord, personality.Network_Config.Enodes
    );

    /* check that we constructed the hostname correctly */
    if (num < 0) {
      /* error */
      scr_err("Error calling snprintf when building hostname rc=%d @ %s:%d",
        num, __FILE__, __LINE__
      );
    } else if (num >= sizeof(hostname)) {
      /* name was truncated */
      scr_err("Temporary buffer of %d bytes too small to construct hostname, needed %d bytes @ %s:%d",
        sizeof(hostname), num, __FILE__, __LINE__
      );
    } else {
      /* duplicate the name */
      name = strdup(hostname);
    }
  #endif

  return name;
}

/* allocate and return a string containing the current cluster name */
char* scr_env_cluster()
{
  char* name = NULL;

  /* TODO: compute name of cluster */

  return name;
}
