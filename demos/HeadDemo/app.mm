/* Copyright 2014 Mo McRoberts.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "core.hh"
#include "delegate.hh"

class HeadDemo: public JyuzauCore
{
protected:
	virtual void createScene(void);
};

void HeadDemo::createScene(void)
{
	// Set the scene's ambient light
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
 
    // Create an Entity
    Ogre::Entity* ogreHead = mSceneMgr->createEntity("Head", "ogrehead.mesh");
 
    // Create a SceneNode and attach the Entity to it
    Ogre::SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("HeadNode");
    headNode->attachObject(ogreHead);
 
    // Create a Light and set its position
    Ogre::Light* light = mSceneMgr->createLight("MainLight");
    light->setPosition(20.0f, 80.0f, 50.0f);
}

int
main(int argc, char **argv)
{
	id delegate;
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	JyuzauCore *app = new HeadDemo();
	
	delegate = [[JyuzauDelegate alloc] init:app];
	[[NSApplication sharedApplication] setDelegate:delegate];
	int retVal = NSApplicationMain(argc, (const char **) argv);

	[pool release];
	
	delete app;
	return retVal;
}