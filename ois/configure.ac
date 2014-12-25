#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.50)
AC_INIT( [OIS], 1.3.0 )
AC_CONFIG_MACRO_DIR([../m4])
AC_CONFIG_AUX_DIR([..])

AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER([includes/config.h])

dnl Check for programs
BT_PROG_XCODE
AC_PROG_CC([clang gcc c99 cc])
AC_PROG_CXX([clang++ g++ c++])
AC_PROG_OBJC
LT_INIT

AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])

dnl Checking for STLPort
OIS_USE_STLPORT

CFLAGS="$CFLAGS"
CXXFLAGS="$CXXFLAGS"
LIBS="$LIBS"

AC_MSG_CHECKING([which back-end to use])
case "$host" in
	*-apple-darwin*)
		backend=mac
		AM_LDFLAGS="$AM_LDFLAGS -framework Carbon"
		;;
	*)
		backend=linux
		;;
esac
AC_MSG_RESULT([$backend])

AM_CONDITIONAL([OIS_BACKEND_MAC],[test x$backend = xmac])
AM_CONDITIONAL([OIS_BACKEND_LINUX],[test x$backend = xlinux])

if test x$backend = xlinux ; then
	AC_CHECK_HEADERS([X11/Xaw/Command.h],, [AC_MSG_ERROR("libxaw headers not found")])
fi

AC_CONFIG_FILES([Makefile
                 includes/Makefile
                 src/Makefile
                 demos/Makefile
                 OIS.pc])
AC_OUTPUT
