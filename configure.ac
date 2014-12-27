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

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

BT_PROG_XCODE
AC_PROG_CC
AC_PROG_CXX
AC_PROG_OBJC
AC_PROG_OBJCXX
LT_INIT

AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])

OGRE_CMAKE_FLAGS=''

dnl Handle Objective-C++ compilation

OBJCXXCOMPILE='$(OBJCXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_OBJCXXFLAGS) $(OBJCXXFLAGS)'
LTOBJCXXCOMPILE='$(LIBTOOL) --tag=CXX $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=compile $(OBJCXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_OBJCXXFLAGS) $(OBJCXXFLAGS)'

AC_SUBST([OBJCXXCOMPILE])
AC_SUBST([LTOBJCXXCOMPILE])

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
OGRE_CPPFLAGS='-I${top_builddir}/ogre-build/sdk/include -I${top_builddir}/ogre-build/sdk/include/OGRE -I${top_builddir}/ogre-build/sdk/include/OGRE/OSX -I${top_builddir}/ogre-build/sdk/include/OGRE/Overlay -F${top_builddir}/ogre-build/sdk/lib/RelWithDebInfo'
OGRE_LDFLAGS='-F${top_builddir}/ogre-build/sdk/lib/RelWithDebInfo'
OGRE_LIBS='-framework Ogre -framework OgreOverlay'
AC_SUBST([OGRE_CPPFLAGS])
AC_SUBST([OGRE_LDFLAGS])
AC_SUBST([OGRE_LIBS])
AC_SUBST([OGRE_CMAKE_FLAGS])
AC_SUBST([ogre_builddir])

dnl -------------------------------------------------------------------
dnl Override _AC_OUTPUT_SUBDIRS to install bundled packages to
dnl ${top_builddir}/sdk
dnl -------------------------------------------------------------------

m4_rename([_AC_OUTPUT_SUBDIRS],[_AC_ORIG_OUTPUT_SUBDIRS])
m4_define([_AC_OUTPUT_SUBDIRS],[

save_prefix="$prefix"
prefix="$ac_abs_confdir/sdk"
_AC_ORIG_OUTPUT_SUBDIRS
prefix="$save_prefix"
])

AC_CONFIG_FILES([
Makefile
demos/Makefile
demos/HeadDemo/Makefile
demos/HeadDemo/Info.plist
demos/HeadDemo/resources.cfg
])

AC_OUTPUT
