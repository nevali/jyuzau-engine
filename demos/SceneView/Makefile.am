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

noinst_PROGRAMS = SceneView

SceneView_SOURCES = app.cc ../demoapp.cc
SceneView_LDADD = ${top_builddir}/jyuzau/libjyuzau.la

EXTRA_DIST = Info.plist.in resources.cfg.in

noinst_DATA = resources.cfg plugins.cfg Info.plist \
	assets/actors/Ogre/ogrehead.mesh assets/actors/Ogre/Ogre.material \
	assets/actors/Ogre/GreenSkin.jpg assets/actors/Ogre/tusk.jpg \
	assets/actors/Ogre/spheremap.png

CLEANFILES = plugins.cfg Resources Frameworks \
	assets/actors/Ogre/ogrehead.mesh assets/actors/Ogre/Ogre.material \
	assets/actors/Ogre/GreenSkin.jpg assets/actors/Ogre/tusk.jpg \
	assets/actors/Ogre/spheremap.png

DISTCLEANFILES = resources.cfg Info.plist

assets/actors/Ogre/ogrehead.mesh: ${top_builddir}/ogre-build/sdk/Media/models/ogrehead.mesh
	rm -f $@ && cp $< $@

assets/actors/Ogre/Ogre.material: ${top_builddir}/ogre-build/sdk/Media/materials/scripts/Ogre.material
	rm -f $@ && cp $< $@

assets/actors/Ogre/tusk.jpg: ${top_builddir}/ogre-build/sdk/Media/materials/textures/tusk.jpg
	rm -f $@ && cp $< $@

assets/actors/Ogre/GreenSkin.jpg: ${top_builddir}/ogre-build/sdk/Media/materials/textures/GreenSkin.jpg
	rm -f $@ && cp $< $@

assets/actors/Ogre/spheremap.png: ${top_builddir}/ogre-build/sdk/Media/materials/textures/spheremap.png
	rm -f $@ && cp $< $@
	
plugins.cfg: ${top_builddir}/ogre-build/sdk/bin/plugins.cfg
	rm -f $@ && cp $< $@

# For Mac OS X: fix up .libs/Contents to be a symlink back here
# For Mac OS X: link Resources to this directory
# For Mac OS X: link Frameworks to ${top_builddir}/ogre-build/sdk/lib/RelWithDebInfo
all-local:
	test -d .libs || mkdir .libs
	cd .libs && rm -f Contents && ln -s .. Contents
	rm -f Resources
	ln -s . Resources
	rm -f Frameworks
	ln -s ${top_builddir}/ogre-build/sdk/lib/RelWithDebInfo Frameworks
	