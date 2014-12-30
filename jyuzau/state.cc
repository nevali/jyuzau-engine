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
	m_cameras(),
	m_viewports()
{
}

State::~State()
{
	std::vector<Ogre::Viewport *>::iterator vit;
	std::vector<Ogre::Camera *>::iterator cit;
	
	Core::getInstance()->removeState(this);

	/* Delete any viewports that we've created */
	for(vit = m_viewports.begin(); vit != m_viewports.end(); vit++)
	{
		delete (*vit);
	}
	/* Delete any cameras that we've created */
	for(cit = m_cameras.begin(); cit != m_cameras.end(); cit++)
	{
		delete (*vit);
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
State::camera(int index)
{
	return m_cameras[index];
}

int
State::cameraCount(void)
{
	return m_cameras.size();
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
	m_loaded = true;
	createScenes();
	createSceneManager();
	attachScenes();
	createPlayers();
}

void
State::createScenes()
{
	/* This method should be overridden to perform scene-loading */
}

void
State::createSceneManager()
{
	/* Create a generic scene manager by default; descendants can override
	 * this to create specific scene managers as required.
	 */
	m_sceneManager = Ogre::Root::getSingletonPtr()->createSceneManager(Ogre::ST_GENERIC);
}

void
State::attachScenes()
{
	/* This method should be overridden to attach the loaded scenes to
	 * the scene manager
	 */
}

void
State::createPlayers()
{
	/* This method should be overidden to create players and cameras */
}

void
State::addViewports(Ogre::RenderWindow *window)
{
	Ogre::Viewport *viewport;
	
	/* By default, if there is at least one camera, create a full-window
	 * viewport for the first one
	 */
	if(m_cameras.size())
	{
		viewport = window->addViewport(m_cameras[0]);
		viewport->setBackgroundColour(Ogre::ColourValue(0,0,0));
		m_cameras[0]->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));
		m_viewports.push_back(viewport);
	}
}

void
State::removeViewports(Ogre::RenderWindow *window)
{
	std::vector<Ogre::Viewport *>::iterator it;

	/* Delete any viewports that we've created */
	for(it = m_viewports.begin(); it != m_viewports.end(); it++)
	{
		delete (*it);
	}
	m_viewports.clear();
}


/* Event listeners */

void
State::activated(Ogre::RenderWindow *window)
{
	if(!m_loaded)
	{
		load();
	}
	addViewports(window);
}

void
State::deactivated(Ogre::RenderWindow *window)
{
	State::removeViewports(window);
}

bool
State::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	return true;
}

bool
State::keyPressed(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_ESCAPE)
	{
		Core::getInstance()->popState();
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

