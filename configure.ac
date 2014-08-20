AC_INIT([OSHMEM verifier], [1.1])
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

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
