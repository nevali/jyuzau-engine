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

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = README.md

DIST_SUBDIRS = freeimage freetype2 zzip ois ogre bullet libjyuzau demos

SUBDIRS = @subdirs@ freetype2 . libjyuzau demos

noinst_HEADERS = jyuzau.hh

subdir_list=freetype2 @subdirs@

# OGRE is built with CMake, and doesn't understand the notion of to-be-installed
# dependent packages. Instead, we install all of our bundled packages to
# ${top_builddir}/sdk and then invoke CMake to configure Ogre. The configuration
# step won't be performed if ogre-build/configure-stamp is present - it's
# removed by our configure script, so that OGRE will be re-configured every
# time the engine tree is.

all-local-subdirs:
	if ! test x"$(subdir_list)" = x"" ; then \
		for i in $(subdir_list) ; do \
			( cd $$i && $(MAKE) ) || exit 1 ; \
		done ; \
		for i in $(subdir_list) ; do \
			( cd $$i && $(MAKE) install ) || exit 1 ; \
		done ; \
	fi
	cd sdk/lib && for i in *.la ; do \
		mv $$i $$i.old ; cat $$i.old | sed 's!^installed=yes!installed=no!g' > $$i ; chmod 755 $$i ; rm $$i.old ; \
	done

all-local-ogre-configure: all-local-subdirs
	test -d "@ogre_builddir@" || mkdir "@ogre_builddir@"
	if test -f "@ogre_builddir@/configure-stamp" ; then \
		result=0 ; \
	else \
		( cd "@ogre_builddir@" && $(CMAKE) @OGRE_CMAKE_FLAGS@ $(abs_top_srcdir)/ogre ) ; \
		result=$$? ; \
	fi ; \
	exit $$result
	touch "@ogre_builddir@/configure-stamp"

all-local-ogre: all-local-ogre-configure
	cd "@ogre_builddir@" && $(MAKE) && $(MAKE) install

all-local-ogre-sdk: all-local-ogre
	test -d sdk || mkdir sdk
	test -d sdk/Frameworks || mkdir sdk/Frameworks
	( cd "@ogre_builddir@/sdk" && tar cf - bin include ) | ( cd sdk && tar xf - )
	( cd "@ogre_builddir@/sdk/lib/RelWithDebInfo" && tar cf - . ) | ( cd sdk/Frameworks && tar xf - )

all-local: all-local-subdirs all-local-ogre-sdk

