## Copyright 2014 Mo McRoberts.
##
##  Licensed under the Apache License, Version 2.0 (the "License");
##  you may not use this file except in compliance with the License.
##  You may obtain a copy of the License at
##
##      http://www.apache.org/licenses/LICENSE-2.0
##
##  Unless required by applicable law or agreed to in writing, software
##  distributed under the License is distributed on an "AS IS" BASIS,
##  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
##  See the License for the specific language governing permissions and
##  limitations under the License.

dnl m4_syscmd([cd freetype2 && ./autogen.sh])

AC_INIT([jyuzau-engine],m4_esyscmd([/bin/sh m4/get-version.sh]),)
dummy="m4_esyscmd([/bin/sh gen-ft2-config.sh])"

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([.])

orig_CFLAGS="$CFLAGS"
orig_CXXFLAGS="$CXXFLAGS"
orig_OBJCFLAGS="$OBJCFLAGS"
orig_OBJCXXFLAGS="$OBJCCXXFLAGS"

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

BT_PROG_XCODE
AC_PROG_CC
AC_PROG_CXX
AC_PROG_OBJC
AC_PROG_OBJCXX

# If autoconf magic set CFLAGS to include -O2, change it to -O0
if test x"$orig_CFLAGS" = x"" ; then
	if test x"$CFLAGS" = x"-g -O2" ; then
		CFLAGS="-g -O0"
	elif test x"$CFLAGS" = x"-O2" ; then
		CFLAGS="-O0"
	fi
fi
if test x"$orig_CXXFLAGS" = x"" ; then
	if test x"$CXXFLAGS" = x"-g -O2" ; then
		CXXFLAGS="-g -O0"
	elif test x"$CXFLAGS" = x"-O2" ; then
		CXXFLAGS="-O0"
	fi
fi
if test x"$orig_OBJCFLAGS" = x"" ; then
	if test x"$OBJCFLAGS" = x"-g -O2" ; then
		OBJCFLAGS="-g -O0"
	elif test x"$OBJCFLAGS" = x"-O2" ; then
		OBJCFLAGS="-O0"
	fi
fi
if test x"$orig_OBJCXXFLAGS" = x"" ; then
	if test x"$OBJCXXFLAGS" = x"-g -O2" ; then
		OBJCXXFLAGS="-g -O0"
	elif test x"$OBJCXXFLAGS" = x"-O2" ; then
		OBJCXXFLAGS="-O0"
	fi
fi


LT_INIT

AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])

AM_CPPFLAGS='-I${top_builddir} -I${top_srcdir} -I${top_builddir}/sdk/include -F${top_builddir}/sdk/Frameworks'
AM_LDFLAGS='-L${abs_top_builddir}/sdk/lib -F${top_builddir}/sdk/Frameworks'

AC_CONFIG_HEADER([config.h])

OGRE_CMAKE_FLAGS=''

dnl Handle Objective-C++ compilation

OBJCXXCOMPILE='$(OBJCXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_OBJCXXFLAGS) $(OBJCXXFLAGS)'
LTOBJCXXCOMPILE='$(LIBTOOL) --tag=CXX $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=compile $(OBJCXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_OBJCXXFLAGS) $(OBJCXXFLAGS)'

AC_SUBST([OBJCXXCOMPILE])
AC_SUBST([LTOBJCXXCOMPILE])

dnl -------------------------------------------------------------------
dnl Configure Jyuzau's own dependencies
dnl -------------------------------------------------------------------

BT_REQUIRE_LIBXML2

COCOA_CPPFLAGS=''
COCOA_LDFLAGS=''
COCOA_LIBS='-framework Cocoa -framework Foundation'

AC_SUBST([COCOA_CPPFLAGS])
AC_SUBST([COCOA_LDFLAGS])
AC_SUBST([COCOA_LIBS])

dnl -------------------------------------------------------------------
dnl Configure FreeType
dnl -------------------------------------------------------------------

OGRE_CMAKE_FLAGS="$OGRE_CMAKE_FLAGS -DFREETYPE_HOME=\${abs_top_builddir}/sdk -DFREETYPE_FT2BUILD_INCLUDE_DIR=\${abs_top_builddir}/sdk/include"


dnl -------------------------------------------------------------------
dnl Configure FreeImage
dnl -------------------------------------------------------------------

AC_CONFIG_SUBDIRS([freeimage])

OGRE_CMAKE_FLAGS="$OGRE_CMAKE_FLAGS -DFREEIMAGE_HOME=\${abs_top_builddir}/sdk"

dnl -------------------------------------------------------------------
dnl Configure zziplib
dnl -------------------------------------------------------------------

AC_CONFIG_SUBDIRS([zzip])

OGRE_CMAKE_FLAGS="$OGRE_CMAKE_FLAGS -DZZIP_HOME=\${abs_top_builddir}/sdk"

ZZIP_CPPFLAGS="-I${top_builddir}/sdk/include"
ZZIP_LDFLAGS="-L${top_builddir}/sdk/lib"
ZZIP_LIBS="-lzzip"

AC_SUBST([ZZIP_CPPFLAGS])
AC_SUBST([ZZIP_LDFLAGS])
AC_SUBST([ZZIP_LIBS])

dnl -------------------------------------------------------------------
dnl Configure OIS
dnl -------------------------------------------------------------------

AC_CONFIG_SUBDIRS([ois])

OGRE_CMAKE_FLAGS="$OGRE_CMAKE_FLAGS -DOIS_HOME=\${abs_top_builddir}/sdk"

OIS_CPPFLAGS='-I${top_builddir}/sdk/include/ois'
OIS_LDFLAGS='-L${top_builddir}/sdk/lib'
OIS_LIBS='-lOIS'

AC_SUBST([OIS_CPPFLAGS])
AC_SUBST([OIS_LDFLAGS])
AC_SUBST([OIS_LIBS])

dnl -------------------------------------------------------------------
dnl Find CMake
dnl -------------------------------------------------------------------

AC_CHECK_PROG([CMAKE],[cmake],[cmake],[no])
if test x"$CMAKE" = x"no" ; then
	AC_MSG_ERROR([CMake is required to build OGRE (which is used by Jyuzau) - see http://www.cmake.org/download/ or install it via your operating system's package manager.])
fi

dnl -------------------------------------------------------------------
dnl Configure OGRE
dnl -------------------------------------------------------------------

OGRE_CMAKE_FLAGS="$OGRE_CMAKE_FLAGS -DCMAKE_OSX_ARCHITECTURES=$host_cpu"

ogre_builddir="ogre-build"
if test -d ${ogre_builddir} ; then
	rm -f ${ogre_builddir}/configure-stamp || exit $?
fi

OGRE_CPPFLAGS='-I${top_builddir}/sdk/include -I${top_builddir}/sdk/include/OGRE -F${top_builddir}/sdk/Frameworks'
OGRE_LDFLAGS='-F${top_builddir}/sdk/Frameworks'
OGRE_LIBS='-framework Ogre'

AC_SUBST([OGRE_CPPFLAGS])
AC_SUBST([OGRE_LDFLAGS])
AC_SUBST([OGRE_LIBS])

OGREOVERLAY_CPPFLAGS='-I${top_builddir}/sdk/include/OGRE/Overlay -F${top_builddir}/sdk/Frameworks'
OGREOVERLAY_LDFLAGS='-F${top_builddir}/sdk/Frameworks'
OGREOVERLAY_LIBS='-framework OgreOverlay'

AC_SUBST([OGREOVERLAY_CPPFLAGS])
AC_SUBST([OGREOVERLAY_LDFLAGS])
AC_SUBST([OGREOVERLAY_LIBS])

AC_SUBST([OGRE_CMAKE_FLAGS])
AC_SUBST([ogre_builddir])


dnl -------------------------------------------------------------------
dnl Configure Bullet
dnl -------------------------------------------------------------------

AC_CONFIG_SUBDIRS([bullet])

BULLET_CPPFLAGS='-I${top_builddir}/sdk/include/bullet'
BULLET_LIBS='-lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath'

AC_SUBST([BULLET_CPPFLAGS])
AC_SUBST([BULLET_LDFLAGS])
AC_SUBST([BULLET_LIBS])

dnl -------------------------------------------------------------------
dnl Configure Lua
dnl -------------------------------------------------------------------

AC_CONFIG_SUBDIRS([liblua])

LUA_CPPFLAGS=""
LUA_LDFLAGS=""
LUA_LIBS="-llua"

AC_SUBST([LUA_CPPFLAGS])
AC_SUBST([LUA_LDFLAGS])
AC_SUBST([LUA_LIBS])

dnl -------------------------------------------------------------------
dnl Set the engine's flags
dnl -------------------------------------------------------------------

ENGINE_CPPFLAGS="-I\${top_srcdir}/libjyuzau -I\${top_builddir}/sdk/include $OGRE_CPPFLAGS $OGREOVERLAY_CPPFLAGS $OIS_CPPFLAGS $BULLET_CPPFLAGS $LUA_CPPFLAGS $COCOA_CPPFLAGS"
ENGINE_LDFLAGS="-L\${top_builddir}/sdk/lib $OGRE_LDFLAGS $OGREOVERLAY_LDFLAGS $OIS_LDFLAGS $BULLET_LDFLAGS $LUA_LDFLAGS $COCOA_LDFLAGS"
ENGINE_LIBS="\${top_builddir}/libjyuzau/libjyuzau.la $OGRE_LIBS $OGREOVERLAY_LIBS $OIS_LIBS $BULLET_LIBS $LUA_LIBS $COCOA_LIBS"

AC_SUBST([ENGINE_CPPFLAGS])
AC_SUBST([ENGINE_LDFLAGS])
AC_SUBST([ENGINE_LIBS])

dnl -------------------------------------------------------------------
dnl Override _AC_OUTPUT_SUBDIRS to install bundled packages to
dnl ${top_builddir}/sdk
dnl -------------------------------------------------------------------

m4_rename([_AC_OUTPUT_SUBDIRS],[_AC_ORIG_OUTPUT_SUBDIRS])
m4_define([_AC_OUTPUT_SUBDIRS],[

export CC
export CXX
export OBJC
export OBJCXX
export CPPFLAGS
export CFLAGS
export CXXFLAGS
export LDFLAGS

save_prefix="$prefix"
prefix="$ac_abs_confdir/sdk"
_AC_ORIG_OUTPUT_SUBDIRS

ac_msg="=== configuring in freetype2 (`pwd`/freetype2)"
ac_popdir=`pwd`
ac_dir="freetype2"
_AS_ECHO_LOG([$ac_msg])
_AS_ECHO([$ac_msg])
AS_MKDIR_P(["$ac_dir"])
cd "$ac_dir"
ac_sub_configure="$ac_srcdir/configure"
AC_MSG_NOTICE([running $SHELL $ac_sub_configure $ac_sub_configure_args --cache-file=$ac_sub_cache_file --srcdir=$ac_srcdir])
# The eval makes quoting arguments work.
eval "\$SHELL \"\$ac_sub_configure\" $ac_sub_configure_args \
 --cache-file=\"\$ac_sub_cache_file\"" ||
AC_MSG_ERROR([$ac_sub_configure failed for $ac_dir])
cd "$ac_popdir"

prefix="$save_prefix"
])

AC_CONFIG_FILES([
Makefile
libjyuzau/Makefile
libjyuzau/jyuzau/Makefile
demos/Makefile
demos/SceneView/Makefile
demos/SceneView/Info.plist
demos/SceneWalk/Makefile
demos/SceneWalk/Info.plist
])

AC_OUTPUT
