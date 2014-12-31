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

AC_INIT([jyuzau-engine],m4_esyscmd([/bin/sh m4/get-version.sh]),)
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

CMAKE=cmake
AC_SUBST([CMAKE])

dnl -------------------------------------------------------------------
dnl Configure OGRE
dnl -------------------------------------------------------------------

OGRE_CMAKE_FLAGS="$OGRE_CMAKE_FLAGS -DCMAKE_OSX_ARCHITECTURES=$host_cpu"

ogre_builddir="ogre-build"
test -d ${ogre_builddir} && rm -f ${ogre_builddir}/configure-stamp || exit $?

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
prefix="$save_prefix"
])

AC_CONFIG_FILES([
Makefile
jyuzau/Makefile
demos/Makefile
demos/SceneView/Makefile
demos/SceneView/Info.plist
demos/SceneWalk/Makefile
demos/SceneWalk/Info.plist
])

AC_OUTPUT
