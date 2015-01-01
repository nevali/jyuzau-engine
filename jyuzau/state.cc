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

#include "jyuzau/state.hh"
#include "jyuzau/core.hh"
#include "jyuzau/prop.hh"
#include "jyuzau/scene.hh"
#include "jyuzau/character.hh"
#include "jyuzau/actor.hh"
#include "jyuzau/camera.hh"
#include "jyuzau/controller.hh"

#include <utility>

using namespace Jyuzau;

State::State():
	m_prev(NULL), m_next(NULL), m_loaded(false),
	m_sceneManager(NULL),
	m_cameras(),
	m_actors(),
	m_defaultPlayerCameraType(CT_FIRSTPERSON),
	m_dynamics(NULL),
	m_overlay(false)
{
	m_core = Core::getInstance();
	m_controller = m_core->controller();
}

State::~State()
{
	m_core->removeState(this);
	deletePlayers();
}

Ogre::SceneManager *
State::sceneManager(void)
{
	return m_sceneManager;
}

int
State::cameras(void)
{
	return m_cameras.size();
}

Camera *
State::camera(int index)
{
	if(index >= m_cameras.size())
	{
		return NULL;
	}
	return m_cameras[index];
}

void
State::playersChanged(void)
{
	Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: players have changed");
	deletePlayers();
	createPlayers();
}

void
State::preload(void)
{
	if(!m_loaded)
	{
		load();
	}
}

/* Create an instance of an asset */
Loadable *
State::factory(Ogre::String m_kind, Ogre::String m_name)
{
	Loadable *loadable;
	
	/* TODO: check pool */
	if(!m_kind.compare("scene"))
	{
		loadable = new Scene(m_name, this);
	}
	else if(!m_kind.compare("prop"))
	{
		loadable = new Prop(m_name, m_kind, this);
	}
	else if(!m_kind.compare("actor"))
	{
		loadable = new Actor(m_name, this);
	}
	else
	{
		return NULL;
	}
	if(!loadable->load())
	{
		delete loadable;
		return NULL;
	}
	return loadable;
}

bool
State::overlay(void)
{
	return m_overlay;
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
State::attachScenes(void)
{
	/* This method should be overridden to attach the loaded scenes to
	 * the scene manager and add any players to it.
	 */
}

void
State::createPlayers(void)
{
	int i, n;
	Character *c;
	Actor *a;
	Camera *cam;
	
	/* This default implementation creates a first-person camera for each
	 * registered player. Descendants can override this behaviour if needed.
	 */
	n = m_core->players();
	Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: there are " + std::to_string(n) + " players registered");
	for(i = 0; i < n; i++)
	{
		c = m_core->player(i);
		if(!c)
		{
			Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: failed to obtain character #" + std::to_string(i));
			continue;
		}
		a = c->createActor(m_sceneManager);
		if(!a)
		{
			Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: failed to create actor for Character '" + c->title() + "'");
			continue;
		}
		m_actors.push_back(a);
		/* Temporary hack until spawn points are implemented */
		a->setPosition(0, 0, 200);
		cam = a->createCamera(m_defaultPlayerCameraType);
		if(!cam)
		{
			Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: failed to create player-camera for Character '" + c->title() + "'");
			continue;
		}
		m_cameras.push_back(cam);
	}
}

void
State::deletePlayers(void)
{
	std::vector<Camera *>::iterator cit;
	std::vector<Actor *>::iterator ait;
	
	/* Delete any cameras that we've created */
	for(cit = m_cameras.begin(); cit != m_cameras.end(); cit++)
	{
		delete (*cit);
	}
	m_cameras.clear();
	/* Delete any actors */
	for(ait = m_actors.begin(); ait != m_actors.end(); ait++)
	{
		delete (*ait);
	}
	m_actors.clear();
}

void
State::addViewports(Ogre::RenderWindow *window)
{
	Ogre::Viewport *vp;
	
	/* By default, if there is at least one camera, create a full-window
	 * viewport for the first one at Z-Order 0
	 */
	if(m_cameras.size())
	{
		vp = m_cameras[0]->createViewport(window);
		vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
		m_cameras[0]->matchAspectRatio();
		if(m_cameras[0]->actor)
		{
			m_cameras[0]->actor->setActiveCamera(m_cameras[0]);
		}
	}
	if(m_actors.size())
	{
		m_controller->bind(m_actors[0]);
	}
}

void
State::removeViewports(Ogre::RenderWindow *window)
{
	std::vector<Actor *>::iterator ait;
	std::vector<Camera *>::iterator cit;
	
	m_controller->unbindAll();
	for(ait = m_actors.begin(); ait != m_actors.end(); ait++)
	{
		(*ait)->resetActiveCameras();
	}
	for(cit = m_cameras.begin(); cit != m_cameras.end(); cit++)
	{
		(*cit)->deleteViewport();
	}
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

/* Invoked instead of deactivated() when an overlay state is pushed in front
 * of us. Under normal circumstances, it should be matched by a later call
 * to resumed() when the overlay is popped and we're made the active state
 * again.
 */
void
State::paused(Ogre::RenderWindow *window)
{
}

/* Invoked when an overlay state is popped from in front of us */
void
State::resumed(Ogre::RenderWindow *window)
{
	if(!m_loaded)
	{
		load();
	}
}

bool
State::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	std::vector<Actor *>::iterator ait;
	
	if(m_dynamics)
	{
		m_dynamics->stepSimulation(evt.timeSinceLastFrame, DYNAMICS_MAX_SUBSTEPS);
	}
	for(ait = m_actors.begin(); ait != m_actors.end(); ait++)
	{
		(*ait)->frameRenderingQueued(evt);
	}
	return true;
}

bool
State::keyPressed(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_ESCAPE)
	{
		Core::getInstance()->popState();
		return true;
	}
	return false;
}

bool
State::keyReleased(const OIS::KeyEvent &arg)
{
	return false;
}

bool
State::mouseMoved(const OIS::MouseEvent &arg)
{
	return false;
}

bool
State::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return false;
}

bool
State::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return false;
}

