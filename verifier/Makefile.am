# -*- makefile -*-
#
#
# Copyright (c) 2014      Mellanox Technologies, Inc.
#                         All rights reserved.
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#
#

#
# OpenSHMEM Verification Tool.
#
#
ACLOCAL_AMFLAGS=-I m4
dist_doc_DATA = README
EXTRA_DIST=autogen.sh

bin_PROGRAMS = oshmem_test

# This lib is used for basic:tc3
lib_LIBRARIES = liboshmem_basic_tc3.a
liboshmem_basic_tc3_a_SOURCES = basic/osh_basic_tc3_lib.c

oshmem_test_CFLAGS =  $(CFLAGS) -Werror -Wall -D__LINUX__ \
					  -I. \
					  -I$(top_srcdir) \
					  -I$(top_srcdir)/cmn

oshmem_test_LDFLAGS = $(LDFLAGS) -L. -loshmem_basic_tc3 -lm

noinst_HEADERS = \
	osh_tests.h \
	cmn/aopt.h \
	cmn/osh_log.h \
	cmn/osh_cmn.h \
	cmn/osh_def.h \
	cmn/rnd_mt.h \
	atomic/osh_atomic_tests.h \
	basic/osh_basic_tests.h \
	coll/osh_coll_tests.h \
	data/osh_data_tests.h \
	lock/osh_lock_tests.h \
	reduce/osh_reduce_tests.h \
	sync/osh_sync_tests.h \
	sync/osh_wait_template.c \
	sync/osh_wait_until_template.c \
	strided/osh_strided_tests.h \
	analysis/osh_analysis.h \
	nbi/osh_nbi_tests.h \
	misc/osh_misc_tests.h

oshmem_test_SOURCES = \
	osh_main.c \
	osh_exec.c \
	cmn/aopt.c \
	cmn/osh_log.c \
	cmn/osh_cmn.c \
	atomic/osh_atomic.c \
	atomic/osh_atomic_tc1.c \
	atomic/osh_atomic_tc2.c \
	atomic/osh_atomic_tc3.c \
	atomic/osh_atomic_tc4.c \
	atomic/osh_atomic_tc5.c \
	atomic/osh_atomic_tc6.c \
	atomic/osh_atomic_tc7.c \
	atomic/osh_atomic_tc8.c \
	atomic/osh_atomic_tc9.c \
	atomic/osh_atomic_tc10.c \
	atomic/osh_atomic_tc11.c \
	atomic/osh_atomic_tc12.c \
	atomic/osh_atomic_tc13.c \
	atomic/osh_atomic_tc14.c \
	atomic/osh_atomic_tc15.c \
	atomic/osh_atomic_tc16.c \
	atomic/osh_atomic_tc17.c \
	atomic/osh_atomic_tc18.c \
	atomic/osh_atomic_tc19.c \
	atomic/osh_atomic_tc20.c \
	atomic/osh_atomic_tc21.c \
	atomic/osh_atomic_tc22.c \
	basic/osh_basic.c \
	basic/osh_basic_tc1.c \
	basic/osh_basic_tc2.c \
	basic/osh_basic_tc3.c \
	basic/osh_basic_tc4.c \
	basic/osh_basic_tc5.c \
	basic/osh_basic_tc6.c \
	basic/osh_basic_tc7.c \
	basic/osh_basic_tc8.c \
	basic/osh_basic_tc9.c \
	data/osh_data.c \
	data/osh_data_tc1.c \
	data/osh_data_tc2.c \
	data/osh_data_tc3.c \
	data/osh_data_tc4.c \
	data/osh_data_tc5.c \
	data/osh_data_tc6.c \
	data/osh_data_tc7.c \
	data/osh_data_tc8.c \
	data/osh_data_tc9.c \
	data/osh_data_tc10.c \
	data/osh_data_tc11.c \
	data/osh_data_tc12.c \
	data/osh_data_tc13.c \
	data/osh_data_tc14.c \
	data/osh_data_tc15.c \
	data/osh_data_tc16.c \
	data/osh_data_tc17.c \
	data/osh_data_tc18.c \
	data/osh_data_tc19.c \
	data/osh_data_tc20.c \
	data/osh_data_tc21.c \
	data/osh_data_tc22.c \
	data/osh_data_tc23.c \
	data/osh_data_tc24.c \
	data/osh_data_tc25.c \
	data/osh_data_tc26.c \
	data/osh_data_tc27.c \
	data/osh_data_tc28.c \
	data/osh_data_tc29.c \
	data/osh_data_tc30.c \
	data/osh_data_tc31.c \
	data/osh_data_tc32.c \
	data/osh_data_tc33.c \
	data/osh_data_tc34.c \
	data/osh_data_tc35.c \
	data/osh_data_tc36.c \
	data/osh_data_tc37.c \
	data/osh_data_tc38.c \
	data/osh_data_tc39.c \
	data/osh_data_tc40.c \
	lock/osh_lock.c \
	lock/osh_lock_tc1.c \
	lock/osh_lock_tc2.c \
	lock/osh_lock_tc3.c \
	lock/osh_lock_tc4.c \
	coll/osh_coll.c \
	coll/osh_coll_tc1.c \
	coll/osh_coll_tc2.c \
	coll/osh_coll_tc3.c \
	coll/osh_coll_tc4.c \
	coll/osh_coll_tc5.c \
	coll/osh_coll_tc6.c \
	coll/osh_coll_tc7.c \
	coll/osh_coll_tc8.c \
	coll/osh_coll_tc9.c \
	coll/osh_coll_tc10.c \
	reduce/osh_reduce.c \
	reduce/osh_reduce_tc1.c \
	reduce/osh_reduce_tc2.c \
	reduce/osh_reduce_tc3.c \
	reduce/osh_reduce_tc4.c \
	reduce/osh_reduce_tc5.c \
	reduce/osh_reduce_tc6.c \
	reduce/osh_reduce_tc7.c \
	reduce/osh_reduce_tc8.c \
	reduce/osh_reduce_tc9.c \
	reduce/osh_reduce_tc10.c \
	reduce/osh_reduce_tc11.c \
	reduce/osh_reduce_tc12.c \
	reduce/osh_reduce_tc13.c \
	reduce/osh_reduce_tc14.c \
	reduce/osh_reduce_tc15.c \
	reduce/osh_reduce_tc16.c \
	reduce/osh_reduce_tc17.c \
	reduce/osh_reduce_tc18.c \
	reduce/osh_reduce_tc19.c \
	reduce/osh_reduce_tc20.c \
	reduce/osh_reduce_tc21.c \
	reduce/osh_reduce_tc22.c \
	reduce/osh_reduce_tc23.c \
	reduce/osh_reduce_tc24.c \
	reduce/osh_reduce_tc25.c \
	reduce/osh_reduce_tc26.c \
	reduce/osh_reduce_tc27.c \
	reduce/osh_reduce_tc28.c \
	reduce/osh_reduce_tc29.c \
	reduce/osh_reduce_tc30.c \
	reduce/osh_reduce_tc31.c \
	reduce/osh_reduce_tc32.c \
	reduce/osh_reduce_tc33.c \
	reduce/osh_reduce_tc34.c \
	reduce/osh_reduce_tc35.c \
	reduce/osh_reduce_tc36.c \
	reduce/osh_reduce_tc37.c \
	reduce/osh_reduce_tc38.c \
	reduce/osh_reduce_tc39.c \
	reduce/osh_reduce_tc40.c \
	reduce/osh_reduce_tc41.c \
	reduce/osh_reduce_tc42.c \
	reduce/osh_reduce_tc43.c \
	reduce/osh_reduce_tc44.c \
	sync/osh_sync.c \
	sync/osh_sync_tc1.c \
	sync/osh_sync_tc2.c \
	sync/osh_sync_tc3.c \
	sync/osh_sync_tc4.c \
	sync/osh_sync_tc5.c \
	sync/osh_sync_tc6.c \
	sync/osh_sync_tc7.c \
	strided/osh_strided.c \
	strided/osh_strided_tc1.c \
	strided/osh_strided_tc2.c \
	strided/osh_strided_tc3.c \
	strided/osh_strided_tc4.c \
	strided/osh_strided_tc5.c \
	strided/osh_strided_tc6.c \
	strided/osh_strided_tc7.c \
	strided/osh_strided_tc8.c \
	strided/osh_strided_tc9.c \
	strided/osh_strided_tc11.c \
	strided/osh_strided_tc12.c \
	strided/osh_strided_tc13.c \
	strided/osh_strided_tc14.c \
	strided/osh_strided_tc15.c \
	strided/osh_strided_tc16.c \
	strided/osh_strided_tc17.c \
	strided/osh_strided_tc18.c \
	strided/osh_strided_tc19.c \
	strided/osh_strided_tc10.c \
	strided/osh_strided_tc20.c \
	analysis/osh_analysis.c \
	analysis/osh_analysis_tc1.c \
	analysis/osh_analysis_tc2.c \
	analysis/osh_analysis_tc3.c \
	analysis/osh_analysis_tc4.c \
	analysis/osh_analysis_tc5.c \
	nbi/osh_nbi.c \
	misc/osh_misc.c \
	misc/osh_misc_tc1.c

if HAVE_NBI
oshmem_test_SOURCES += \
	nbi/osh_nbi_tc1.c \
	nbi/osh_nbi_tc2.c
endif

if HAVE_ALLTOALL
oshmem_test_SOURCES += \
	coll/osh_coll_tc11.c \
	coll/osh_coll_tc12.c
endif

if ENABLE_MPI
noinst_HEADERS += \
	mix/osh_mix.h

oshmem_test_SOURCES += \
	mix/osh_mix.c \
	mix/osh_mix_tc1.c \
	mix/osh_mix_tc2.c \
	mix/osh_mix_tc3.c
endif


clean-local:
	rm -f oshmem_test
