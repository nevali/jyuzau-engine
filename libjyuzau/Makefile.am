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

SUBDIRS = jyuzau

AM_CPPFLAGS = @AM_CPPFLAGS@ @OGRE_CPPFLAGS@ @OGREOVERLAY_CPPFLAGS@ @OIS_CPPFLAGS@ @BULLET_CPPFLAGS@ @LUA_CPPFLAGS@

AM_LDFLAGS = @AM_LDFLAGS@ @OGRE_LDFLAGS@ @OGREOVERLAY_LDFLAGS@ @COCOA_LDFLAGS@  @BULLET_LDFLAGS@ @LUA_LDFLAGS@

LIBS = @LIBS@ \
	@OGREOVERLAY_LIBS@ @OGRE_LIBS@ \
	@COCOA_LIBS@ @OIS_LIBS@ @BULLET_LIBS@ @LUA_LIBS@

lib_LTLIBRARIES = libjyuzau.la

include_HEADERS = jyuzau.hh

libjyuzau_la_SOURCES = p_utils.hh \
	app.cc nsapp.mm delegate.mm core.cc loadable.cc state.cc \
	scene.cc prop.cc actor.cc light.cc character.cc roster.cc \
	camera.cc controller.cc sceneview.cc splash.cc mainmenu.cc \
	menu.cc charselect.cc scenewalk.cc node.cc

libjyuzau_la_LDFLAGS = $(AM_LDFLAGS) -avoid-version -no-undefined

# Suffix rules for ObjC++
.mm.o:
	$(OBJCXXCOMPILE) -c -o $@ $<

.mm.lo:
	$(LTOBJCXXCOMPILE) -c -o $@ $<
