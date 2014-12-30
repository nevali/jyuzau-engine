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

AM_CPPFLAGS = @AM_CPPFLAGS@ @OGRE_CPPFLAGS@ @OIS_CPPFLAGS@
AM_LDFLAGS = @AM_LDFLAGS@ @OGRE_LDFLAGS@ @OIS_LDFLAGS@
LIBS = @LIBS@ @OGRE_LIBS@ @OIS_LIBS@ -framework Cocoa -framework Foundation

lib_LTLIBRARIES = libjyuzau.la

libjyuzau_la_SOURCES = defs.hh \
	main.hh app.cc nsapp.mm delegate.hh delegate.mm \
	core.hh core.cc loadable.hh loadable.cc state.hh state.cc \
	scene.hh scene.cc prop.hh prop.cc actor.hh actor.cc \
	light.hh light.cc character.hh character.cc roster.hh roster.cc \
	sceneview.hh sceneview.mm splash.hh splash.mm mainmenu.hh mainmenu.mm \
	menu.hh menu.mm charselect.hh charselect.cc

libjyuzau_la_LDFLAGS = $(AM_LDFLAGS) -avoid-version

# Suffix rules for ObjC++
.mm.o:
	$(OBJCXXCOMPILE) -c -o $@ $<

.mm.lo:
	$(LTOBJCXXCOMPILE) -c -o $@ $<
