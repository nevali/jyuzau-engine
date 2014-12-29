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

#include <utility>

#include "jyuzau/state.hh"
#include "jyuzau/core.hh"

using namespace Jyuzau;

State::State():
	m_prev(NULL), m_next(NULL), m_loaded(false),
	m_sceneManager(NULL),
	m_camera(NULL),
	m_viewport(NULL)
{
}

State::~State()
{
	Core::getInstance()->removeState(this);
	if(m_viewport)
	{
		delete m_viewport;
	}
	if(m_camera)
	{
		delete m_camera;
	}
	if(m_sceneManager)
	{
		delete m_sceneManager;
	}
}

Ogre::SceneManager *
State::sceneManager(void)
{
	return m_sceneManager;
}

Ogre::Camera *
State::camera(void)
{
	return m_camera;
}


void
State::preload(void)
{
	if(!m_loaded)
	{
		load();
	}
}

void
State::load(void)
{
	m_sceneManager = Ogre::Root::getSingletonPtr()->createSceneManager(Ogre::ST_GENERIC);
	m_camera = m_sceneManager->createCamera("PlayerCam");

	// Position it at 500 in Z direction
	m_camera->setPosition(Ogre::Vector3(0,0,80));
	// Look back along -Z
	m_camera->lookAt(Ogre::Vector3(0,0,-300));
	m_camera->setNearClipDistance(5);

	m_loaded = true;
}

void
State::activated(void)
{
	Ogre::RenderWindow *window;
	
	if(!m_loaded)
	{
		load();
	}
	window = Core::getInstance()->window();
	m_viewport = window->addViewport(m_camera);
	m_viewport->setBackgroundColour(Ogre::ColourValue(0,0,0));
	m_camera->setAspectRatio(Ogre::Real(m_viewport->getActualWidth()) / Ogre::Real(m_viewport->getActualHeight()));
}

void
State::deactivated(void)
{
	if(m_viewport)
	{
		delete m_viewport;
		m_viewport = NULL;
	}
}

bool
State::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	return true;
}

/* OIS event handlers */
bool
State::keyPressed(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_ESCAPE)
	{
		Core::getInstance()->shutdown();
	}
	return true;
}

bool
State::keyReleased(const OIS::KeyEvent &arg)
{
	return true;
}

bool
State::mouseMoved(const OIS::MouseEvent &arg)
{
	return true;
}

bool
State::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}

bool
State::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}

