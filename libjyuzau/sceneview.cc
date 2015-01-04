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

#include "jyuzau/sceneview.hh"
#include "jyuzau/scene.hh"
#include "jyuzau/core.hh"
#include "jyuzau/camera.hh"

#include <OIS/OISEvents.h>
#include <OIS/OISInputManager.h>
#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

#include <OgreSceneManager.h>

#include <OGRE/SdkCameraMan.h>

using namespace Jyuzau;

SceneViewState::SceneViewState(Ogre::String name):
	State::State(),
	m_cameraMan(NULL),
	m_scene(NULL),
	m_name(name)
{
}

SceneViewState::~SceneViewState()
{
	if(m_scene)
	{
		delete m_scene;
	}
	if(m_cameraMan)
	{
		delete m_cameraMan;
	}
}

void
SceneViewState::createScenes()
{
	m_scene = dynamic_cast<Scene *>(factory("scene", m_name));
}

void
SceneViewState::attachScenes(void)
{
	if(m_scene)
	{
		m_scene->attach();
	}
}

void
SceneViewState::createPlayers(void)
{
	Camera *cam;
	
	/* Create a simple camera which can be controlled by the camera manager */
	cam = new Camera("PlayerCam", m_sceneManager);
	cam->setPosition(Ogre::Vector3(0,0,80));
	cam->lookAt(Ogre::Vector3(0,0,-300));
	cam->setNearClipDistance(5);
	m_cameras.push_back(cam);
	
	m_cameraMan = new OgreBites::SdkCameraMan(cam->camera);
}

void
SceneViewState::deletePlayers(void)
{
	/* No-op */
}

bool
SceneViewState::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if(m_cameraMan->frameRenderingQueued(evt))
	{
		return true;
	}
	return State::frameRenderingQueued(evt);
}

/* Pass all keyboard and mouse events not handled by the base class to the
 * camera manager.
 */
bool
SceneViewState::keyPressed(const OIS::KeyEvent &arg)
{
	if(!State::keyPressed(arg))
	{
		m_cameraMan->injectKeyDown(arg);
	}
	return true;
}

bool
SceneViewState::keyReleased(const OIS::KeyEvent &arg)
{
	if(!State::keyReleased(arg))
	{
		m_cameraMan->injectKeyUp(arg);
	}
	return true;
}

bool
SceneViewState::mouseMoved(const OIS::MouseEvent &arg)
{
	if(!State::mouseMoved(arg))
	{
		m_cameraMan->injectMouseMove(arg);
	}
	return true;
}

bool
SceneViewState::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if(!State::mousePressed(arg, id))
	{
		m_cameraMan->injectMouseDown(arg, id);
	}
	return true;
}

bool
SceneViewState::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if(!State::mouseReleased(arg, id))
	{
		m_cameraMan->injectMouseUp(arg, id);
	}
	return true;
}
