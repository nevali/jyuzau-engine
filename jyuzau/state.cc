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
	m_core = Core::getInstance();
}

State::~State()
{
	std::vector<StateViewportEntry>::iterator vit;
	std::vector<Ogre::Camera *>::iterator cit;
	
	m_core->removeState(this);

	/* Delete any viewports that we've created (note that we don't own
	 * the Ogre::Viewport instance, so we can't simply delete them) */
	for(vit = m_viewports.begin(); vit != m_viewports.end(); vit++)
	{
		(*vit).vp->getTarget()->removeViewport((*vit).zorder);
	}
	/* Delete any cameras that we've created */
	for(cit = m_cameras.begin(); cit != m_cameras.end(); cit++)
	{
		delete (*cit);
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
	if(index >= m_cameras.size())
	{
		return NULL;
	}
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
	StateViewportEntry viewport;
	
	/* By default, if there is at least one camera, create a full-window
	 * viewport for the first one at Z-Order 0
	 */
	if(m_cameras.size())
	{
		viewport.zorder = 0;
		viewport.camera = m_cameras[0];
		viewport.vp = window->addViewport(m_cameras[0], 0);
		viewport.vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
		m_cameras[0]->setAspectRatio(Ogre::Real(viewport.vp->getActualWidth()) / Ogre::Real(viewport.vp->getActualHeight()));
		m_viewports.push_back(viewport);
	}
}

void
State::removeViewports(Ogre::RenderWindow *window)
{
	std::vector<StateViewportEntry>::iterator vit;
	
	for(vit = m_viewports.begin(); vit != m_viewports.end(); vit++)
	{
		window->removeViewport((*vit).zorder);
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

