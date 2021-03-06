# TODO: move -lz to m4 script
LIBZ_LDFLAGS = -lz

AM_CFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/config
AM_CXXFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/config

# headers to be installed in /include subdirectory
include_HEADERS = \
	scr.h \
	scrf.h

# headers that should not be installed into /include
noinst_HEADERS = \
	scr_conf.h \
	scr_globals.h \
	scr_err.h \
	scr_io.h \
	scr_path.h \
	scr_path_mpi.h \
	scr_compress.h \
	scr_util.h \
	scr_util_mpi.h \
	scr_split.h \
	scr_hash.h \
	scr_hash_util.h \
	scr_hash_mpi.h \
	scr_flush_file_mpi.h \
	tv_data_display.h \
	scr_filemap.h \
	scr_index_api.h \
	scr_meta.h \
	scr_dataset.h \
	scr_storedesc.h \
	scr_groupdesc.h \
	scr_reddesc.h \
	scr_reddesc_apply.h \
	scr_reddesc_recover.h \
	scr_summary.h \
	scr_cache.h \
	scr_cache_rebuild.h \
	scr_fetch.h \
	scr_flush.h \
	scr_flush_sync.h \
	scr_flush_async.h \
	scr_config.h \
	scr_param.h \
	scr_env.h \
	scr_log.h \
	scr_halt.h \
	queue.h

noinst_LTLIBRARIES = \
	libscr_cmds.la \
    libscr_tlcc.la
#    libscr_cray_xt.la
#	libscr_base.la \
	libscr_cmds.la
#	libscr_interpose.la

if INTERNAL_HAVE_LIBDATAWARP
	noinst_LTLIBRARIES += libscr_cray_xt.la
endif

lib_LTLIBRARIES = \
	libscr.la \
	libscrf.la
#	libscr_base.la


CMDS_INCLUDES = \
	$(DEFS) \
	$(DEFAULT_INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) \
	$(AM_CFLAGS) $(CFLAGS) \
	$(MYSQL_CPPFLAGS) \
	$(PMIX_CFLAGS) \
	$(CPPR_CFLAGS)

# source files that are used by scr commands and runtime library
#libscr_base_la_SOURCES = \
	scr_io.c \
	scr_path.c \
	scr_compress.c \
	scr_util.c \
	scr_hash.c \
	scr_hash_util.c \
	tv_data_display.c \
	scr_filemap.c \
	scr_index_api.c \
	scr_meta.c \
	scr_dataset.c \
	scr_config.c \
	scr_param.c \
	scr_log.c \
	scr_halt.c

#libscr_base_la_CFLAGS = $(CMDS_INCLUDES)
#libscr_base_la_LIBADD = $(MYSQL_LDFLAGS) $(MYSQL_LIBS) $(TV_LIBS) $(LIBZ_LDFLAGS) $(CPPR_LDFLAGS) $(CPPR_LIBS) $(PMIX_LDFLAGS)
#libscr_base_la_LDFLAGS = -avoid-version

# source files that are only used for commands
libscr_cmds_la_SOURCES = \
	scr_err_serial.c \
	scr_config_serial.c \
	scr_io.c \
	scr_path.c \
	scr_compress.c \
	scr_util.c \
	scr_hash.c \
	scr_hash_util.c \
	tv_data_display.c \
	scr_filemap.c \
	scr_index_api.c \
	scr_meta.c \
	scr_dataset.c \
	scr_config.c \
	scr_param.c \
	scr_env.c \
	scr_log.c \
	scr_halt.c
libscr_cmds_la_CFLAGS = $(CMDS_INCLUDES) $(CPPR_CFLAGS)
libscr_cmds_la_LIBADD = $(MYSQL_LDFLAGS) $(MYSQL_LIBS) $(TV_LIBS) $(LIBZ_LDFLAGS) $(CPPR_LDFLAGS) $(CPPR_LIBS) $(PMIX_LDFLAGS)
libscr_cmds_la_LDFLAGS = -avoid-version

INCLUDES = \
	$(DEFS) \
	$(DEFAULT_INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) \
	$(AM_CFLAGS) $(CFLAGS) \
	$(YOGRT_CPPFLAGS) \
	$(CPPR_CFLAGS) \
	$(MYSQL_CPPFLAGS)

# source files that are used by libraries (include MPI)
libscr_tlcc_la_SOURCES = \
	scr_globals.c \
	scr_err_mpi.c \
	scr_path_mpi.c \
	scr_config_mpi.c \
	scr_util_mpi.c \
	scr_split.c \
	scr_hash_mpi.c \
	scr_flush_file_mpi.c \
	scr_storedesc.c \
	scr_groupdesc.c \
	scr_reddesc.c \
	scr_reddesc_apply.c \
	scr_reddesc_recover.c \
	scr_summary.c \
	scr_cache.c \
	scr_cache_rebuild.c \
	scr_fetch.c \
	scr_flush.c \
	scr_flush_sync.c \
	scr_flush_async_tlcc.c \
	scr.c \
	scr_io.c \
	scr_path.c \
	scr_compress.c \
	scr_util.c \
	scr_hash.c \
	scr_hash_util.c \
	tv_data_display.c \
	scr_filemap.c \
	scr_index_api.c \
	scr_meta.c \
	scr_dataset.c \
	scr_config.c \
	scr_param.c \
	scr_env.c \
	scr_log.c \
	scr_halt.c

libscr_tlcc_la_CFLAGS = $(INCLUDES) $(DTCMP_CFLAGS) $(PMIX_CFLAGS) $(CPPR_CFLAGS) $(MPI_CFLAGS) $(CMDS_INCLUDES)
libscr_tlcc_la_LIBADD = $(YOGRT_LDFLAGS) $(YOGRT_LIBS) $(DTCMP_LDFLAGS) $(DTCMP_LIBS) $(MPI_CLDFLAGS) $(MYSQL_LDFLAGS) $(MYSQL_LIBS) $(TV_LIBS) $(LIBZ_LDFLAGS) $(PMIX_LIBS) $(CPPR_LDFLAGS) $(CPPR_LIBS) #libscr_base.la
#(avoids numbers at end of library name)
libscr_tlcc_la_LDFLAGS = -avoid-version
#libscr_la_LDFLAGS = -rpath $(DESTDIR)$(libdir)

# source files that are used by libraries (include MPI)
libscr_cray_xt_la_SOURCES = \
	scr_globals.c \
	scr_err_mpi.c \
	scr_path_mpi.c \
	scr_config_mpi.c \
	scr_util_mpi.c \
	scr_split.c \
	scr_hash_mpi.c \
	scr_flush_file_mpi.c \
	scr_storedesc.c \
	scr_groupdesc.c \
	scr_reddesc.c \
	scr_reddesc_apply.c \
	scr_reddesc_recover.c \
	scr_summary.c \
	scr_cache.c \
	scr_cache_rebuild.c \
	scr_fetch.c \
	scr_flush.c \
	scr_flush_sync.c \
	scr_flush_async_cray_xt.c \
	scr.c \
	scr_io.c \
	scr_path.c \
	scr_compress.c \
	scr_util.c \
	scr_hash.c \
	scr_hash_util.c \
	tv_data_display.c \
	scr_filemap.c \
	scr_index_api.c \
	scr_meta.c \
	scr_dataset.c \
	scr_config.c \
	scr_param.c \
	scr_env.c \
	scr_log.c \
	scr_halt.c

libscr_cray_xt_la_CFLAGS = $(INCLUDES) $(DTCMP_CFLAGS) $(PMIX_CFLAGS) $(CPPR_CFLAGS) $(MPI_CFLAGS) $(CMDS_INCLUDES)
libscr_cray_xt_la_LIBADD = $(YOGRT_LDFLAGS) $(YOGRT_LIBS) $(DTCMP_LDFLAGS) $(DTCMP_LIBS) $(MPI_CLDFLAGS) $(MYSQL_LDFLAGS) $(MYSQL_LIBS) $(TV_LIBS) $(LIBZ_LDFLAGS) $(PMIX_LIBS) $(CPPR_LDFLAGS) $(CPPR_LIBS) #libscr_base.la
#(avoids numbers at end of library name)
libscr_cray_xt_la_LDFLAGS = -avoid-version
#libscr_la_LDFLAGS = -rpath $(DESTDIR)$(libdir)

libscr_la_SOURCES =
libscr_la_LIBADD = libscr$(FILE_EXT).la
libscr_la_DEPENDENCIES = libscr$(FILE_EXT).la
libscr_la_CFLAGS = $(INCLUDES) $(DTCMP_CFLAGS) $(PMIX_CFLAGS) $(CPPR_CFLAGS) $(MPI_CFLAGS) $(CMDS_INCLUDES)
libscr_la_LIBADD += $(YOGRT_LDFLAGS) $(YOGRT_LIBS) $(DTCMP_LDFLAGS) $(DTCMP_LIBS) $(MPI_CLDFLAGS) $(MYSQL_LDFLAGS) $(MYSQL_LIBS) $(TV_LIBS) $(LIBZ_LDFLAGS) $(PMIX_LIBS) $(CPPR_LDFLAGS) $(CPPR_LDLIBS) #libscr_base.la
#(avoids numbers at end of library name)
libscr_la_LDFLAGS = -avoid-version


libscrf_la_SOURCES = scrf.c
libscrf_la_CFLAGS = $(INCLUDES) $(MPI_CFLAGS)
libscrf_la_LIBADD = libscr.la
libscrf_la_LDFLAGS = -avoid-version

#libscr_interpose_la_SOURCES = scr_interpose.c
#libscr_interpose_la_CFLAGS = $(INCLUDES) $(MPI_CFLAGS)
#libscr_interpose_la_LIBADD = libscr.la
#libscr_interpose_la_LDFLAGS = -avoid-version

bin_PROGRAMS = \
	scr_inspect_cache \
	scr_copy \
	scr_rebuild_xor \
	scr_halt_cntl \
	scr_flush_file \
	scr_nodes_file \
	scr_retries_halt \
	scr_crc32 \
	scr_print_hash_file \
	scr_log_event \
	scr_log_transfer \
	scr_transfer \
	scr_index

if INTERNAL_HAVE_LIBPMIX
  bin_PROGRAMS += scr_pmix_spawn

  scr_pmix_spawn_SOURCES = scr_pmix_spawn.c
  scr_pmix_spawn_LDADD = $(PMIX_LIBS) $(PMIX_LDFLAGS)
  scr_pmix_spawn_CFLAGS = $(AM_CFLAGS) $(PMIX_CFLAGS) $(MPI_CFLAGS)
endif

scr_copy_SOURCES = scr_copy.c
scr_copy_LDADD = $(LIBZ_LDFLAGS) libscr_cmds.la
scr_copy_CFLAGS = $(AM_CFLAGS)

scr_inspect_cache_SOURCES = scr_inspect_cache.c
scr_inspect_cache_LDADD = $(LIBZ_LDFLAGS) libscr_cmds.la
scr_inspect_cache_CFLAGS = $(AM_CFLAGS)

scr_rebuild_xor_SOURCES = scr_rebuild_xor.c
scr_rebuild_xor_LDADD = $(LIBZ_LDFLAGS) libscr_cmds.la
scr_rebuild_xor_CFLAGS = $(AM_CFLAGS)

scr_halt_cntl_SOURCES = scr_halt_cntl.c
scr_halt_cntl_LDADD = $(LIBZ_LDFLAGS) libscr_cmds.la
scr_halt_cntl_CFLAGS = $(AM_CFLAGS)

scr_flush_file_SOURCES = scr_flush_file.c
scr_flush_file_LDADD = $(LIBZ_LDFLAGS) libscr_cmds.la
scr_flush_file_CFLAGS = $(AM_CFLAGS)

scr_nodes_file_SOURCES = scr_nodes_file.c
scr_nodes_file_LDADD = $(LIBZ_LDFLAGS) libscr_cmds.la
scr_nodes_file_CFLAGS = $(AM_CFLAGS)

scr_retries_halt_SOURCES = scr_retries_halt.c
scr_retries_halt_LDADD = $(LIBZ_LDFLAGS) libscr_cmds.la
scr_retries_halt_CFLAGS = $(AM_CFLAGS)

scr_crc32_SOURCES = scr_crc32.c
scr_crc32_LDADD = $(LIBZ_LDFLAGS) libscr_cmds.la
scr_crc32_CFLAGS = $(AM_CFLAGS)

scr_print_hash_file_SOURCES = scr_print_hash_file.c
scr_print_hash_file_LDADD = $(LIBZ_LDFLAGS) libscr_cmds.la
scr_print_hash_file_CFLAGS = $(AM_CFLAGS)

scr_log_event_SOURCES = scr_log_event.c
scr_log_event_LDADD = $(LIBZ_LDFLAGS) libscr_cmds.la $(MYSQL_LDFLAGS) $(MYSQL_LIBS)
scr_log_event_CFLAGS = $(AM_CFLAGS)

scr_log_transfer_SOURCES = scr_log_transfer.c
scr_log_transfer_LDADD = $(LIBZ_LDFLAGS) libscr_cmds.la $(MYSQL_LDFLAGS) $(MYSQL_LIBS)
scr_log_transfer_CFLAGS = $(AM_CFLAGS)

scr_transfer_SOURCES = scr_transfer.c
scr_transfer_LDADD = $(LIBZ_LDFLAGS) libscr_cmds.la
scr_transfer_CFLAGS = $(AM_CFLAGS)

scr_index_SOURCES = scr_index.c
scr_index_LDADD = $(LIBZ_LDFLAGS) libscr_cmds.la
scr_index_CFLAGS = $(AM_CFLAGS)

