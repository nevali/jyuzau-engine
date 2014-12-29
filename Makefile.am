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

DIST_SUBDIRS = freeimage zzip ois ogre jyuzau demos

SUBDIRS = @subdirs@ . jyuzau demos

noinst_HEADERS = jyuzau.hh

# OGRE is built with CMake, and doesn't understand the notion of to-be-installed
# dependent packages. Instead, we install all of our bundled packages to
# ${top_builddir}/sdk and then invoke CMake to configure Ogre. The configuration
# step won't be performed if ogre-build/configure-stamp is present - it's
# removed by our configure script, so that OGRE will be re-configured every
# time the engine tree is.

all-local-subdirs:
	if ! test x"@subdirs@" = x"" ; then \
		for i in @subdirs@ ; do \
			( cd $$i && $(MAKE) install ) ; \
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

all-local: all-local-subdirs all-local-ogre

