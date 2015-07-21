define([scm_r], esyscmd([sh -c "git rev-parse --short=7 HEAD"])) dnl
define([ver_major], 1)
define([ver_minor], 2)
define([revcount], esyscmd([git rev-list HEAD | wc -l | sed -e 's/ *//g' | xargs -n1 printf])) dnl

AC_INIT([OSHMEM verifier], [ver_major.ver_minor.revcount])
AM_INIT_AUTOMAKE([1.10 foreign tar-ustar silent-rules subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_PROG_CC([oshcc])
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
AM_MAINTAINER_MODE
AC_CONFIG_MACRO_DIR([m4])

if [ $CC --version | grep icc ]; then
    CFLAGS="$CFLAGS -wd188,981,1419,810"
else
    CFLAGS="$CFLAGS -Wundef"
fi

AC_CHECK_LIB(oshmem, opal_progress, [],
			     AC_MSG_ERROR([opal_progress() not found. It seems not OpenMPI.
							   ]))
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
