#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)

AC_INIT(Sherpya JavaScript Shell, 1.0, sherpya@netfarm.it)
AC_CONFIG_SRCDIR([sjs.cpp])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE

AC_PREFIX_DEFAULT([/usr])

AC_DISABLE_STATIC
AC_PROG_LIBTOOL

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET

# Checks for libraries.
AC_CHECK_LIB([curl], [curl_easy_init], [CURL="-lcurl"])
AC_CHECK_LIB([z], [zlibVersion], [ZLIB="-lz"])

AC_SUBST(CURL)
AC_SUBST(ZLIB)

AC_CHECK_LIB([dl], [dlopen])
AC_CHECK_LIB([pthread], [pthread_join])
### TODO params on sm js lib
AC_CHECK_LIB([smjs], [JS_ShutDown])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stddef.h stdint.h stdlib.h string.h unistd.h utime.h])

# SpiderMonkey includes
AC_ARG_WITH(smincludes, 
            [  --with-sm-includes=DIR  DIR=path of SpiderMonkey includes directory],
               if test [ x$withval != x ] -a [ -d $withval ]; then
                  smincludes=$withval
               fi ,
               smincdir=/usr/include/smjs)

AC_CHECK_HEADER($smincdir/jsautocfg.h, , 
                echo "SpiderMonkey include directory does not contain jsautocfg.h!";
                echo "Please specify a suitable SpiderMonkey include directory" ; exit 1)

AC_SUBST(smincdir)

PLUGIN_LDFLAGS='-module -avoid-version -shared'
AC_SUBST(PLUGIN_LDFLAGS)

sjs_libexec=$libdir/sjs
AC_SUBST(sjs_libexec)

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_STAT
AC_FUNC_UTIME_NULL
AC_CHECK_FUNCS([getcwd memset mkdir pow rmdir sqrt strcasecmp uname utime])

AC_CONFIG_FILES([Makefile plugins/Makefile scripts/Makefile])
AC_OUTPUT
