AC_INIT([lua],[5.2.3])

AC_CONFIG_MACRO_DIR([../m4])
AC_CONFIG_AUX_DIR([..])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

BT_PROG_XCODE
AC_PROG_CC

LT_INIT

AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_CFLAGS])

AC_CANONICAL_HOST

AC_MSG_CHECKING([which system-specific features to enable for $host_os])
case "$host_os" in
	linux-gnu)
		AC_DEFINE_UNQUOTED([LUA_USE_LINUX],[1],[Define to enable Linux-specific features])
		AC_MSG_RESULT([Linux])
		;;
	darwin*)
		AC_DEFINE_UNQUOTED([LUA_USE_MACOSX],[1],[Define to enable Darwin-specific features])
		AC_MSG_RESULT([Darwin])
		;;
	*)
		AC_MSG_RESULT([none])
		;;
esac

AC_CHECK_LIB([readline],[hist_init])

AC_CONFIG_FILES([
Makefile
])

AC_OUTPUT
