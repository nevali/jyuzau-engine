/* Copyright 2014 Mo McRoberts.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *	  http://www.apache.org/licenses/LICENSE-2.0
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

#include <OGRE/OgreLogManager.h>
#include <OGRE/OgreConfigFile.h>
#include <OGRE/OgreViewport.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
# include <OGRE/OSX/macUtils.h>
#endif

#ifdef OGRE_STATIC_LIB
#  define OGRE_STATIC_GL
#  if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#    define OGRE_STATIC_Direct3D9
#    if OGRE_USE_D3D10
#      define OGRE_STATIC_Direct3D10
#    endif
#  endif
#  define OGRE_STATIC_BSPSceneManager
#  define OGRE_STATIC_ParticleFX
#  define OGRE_STATIC_CgProgramManager
#  ifdef OGRE_USE_PCZ
#    define OGRE_STATIC_PCZSceneManager
#    define OGRE_STATIC_OctreeZone
#  else
#    define OGRE_STATIC_OctreeSceneManager
#  endif
#  include <OGRE/OgreStaticPluginLoader.h>
#endif

#include "jyuzau/core.hh"
#include "jyuzau/state.hh"
#include "jyuzau/roster.hh"
#include "jyuzau/character.hh"
#include "jyuzau/controller.hh"

using namespace Jyuzau;

static Core *singleton = NULL;

/* Obtain the singleton instance of Jyuzau::Core, if one exists */
Core *
Core::getInstance(void)
{
	return singleton;
}

Core::Core(void)
	: m_root(NULL),
	m_window(NULL),
	m_pluginsCfg(Ogre::StringUtil::BLANK),
	m_cursorWasVisible(false),
	m_shutdown(true),
	m_inputManager(NULL),
	m_mouse(NULL),
	m_keyboard(NULL),
	m_overlaySystem(NULL),
	m_activeScene(NULL),
	m_firstState(NULL),
	m_lastState(NULL),
	m_inhibitStateActivation(0),
	m_preInhibitState(NULL),
	m_playersChanged(false),
	m_controller(NULL),
	m_caption("Jyuzau")
{
	singleton = this;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
	m_resourcePath = Ogre::macBundlePath() + "/Contents/Resources/";
#else
	m_resourcePath = "";
#endif
}

Core::~Core(void)
{
	std::vector<Character *>::iterator it;
	singleton = NULL;
	
	delete m_controller;
	for(it = m_players.begin(); it != m_players.end(); it++)
	{
		delete (*it);
	}
	if (m_overlaySystem) delete m_overlaySystem;
	Ogre::WindowEventUtilities::removeWindowEventListener(m_window, this);
	windowClosed(m_window);
	delete m_root;
}

/* Enter the rendering run-loop on non-Apple platforms */
bool
Core::go()
{
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
	return false;
#else
	if(!init())
	{
		return false;
	}
	m_root->startRendering();
	return cleanup();
#endif
}

Ogre::Root *
Core::root(void)
{
	return m_root;
}

Ogre::RenderWindow *
Core::window(void)
{
	return m_window;
}

Ogre::OverlaySystem *
Core::overlays(void)
{
	return m_overlaySystem;
}

State *
Core::state(void)
{
	return m_firstState;
}

Roster *
Core::roster(void)
{
	return m_roster;
}

/* Return a pointer to an active camera */
Camera *
Core::camera(int index)
{
	if(m_firstState)
	{
		return m_firstState->camera(index);
	}
	return NULL;
}

/* Return a pointer to the scene manager */
Ogre::SceneManager *
Core::sceneManager(void)
{
	if(m_firstState)
	{
		return m_firstState->sceneManager();
	}
	return NULL;
}

/* Return the Controller instance */
Controller *
Core::controller(void)
{
	return m_controller;
}

/* Trigger application termination */
void
Core::shutdown()
{
	m_shutdown = true;
}

/* Reset the player list */
void
Core::resetPlayers()
{
	std::vector<Character *>::iterator it;
	
	for(it = m_players.begin(); it != m_players.end(); it++)
	{
		delete (*it);
	}
	m_players.clear();
	m_playersChanged = true;
}

/* Add a player to the player list. Note that Core will own the Character it's
 * added.
 */
void
Core::addPlayer(Character *player)
{
	m_players.push_back(player);
	m_playersChanged = true;
}

/* Return the number of players */
int
Core::players(void)
{
	return m_players.size();
}

/* Return the player at a specific index (0 = first player) */
Character *
Core::player(int index)
{
	if(index >= m_players.size())
	{
		return NULL;
	}
	return m_players[index];
}

/* Initialise the engine, create the initial scene */
bool
Core::init(void)
{
#ifdef _DEBUG
#ifndef OGRE_STATIC_LIB
	m_pluginsCfg = m_resourcePath + "plugins_d.cfg";
#else
	m_pluginsCfg = "plugins_d.cfg";
#endif
#else
#ifndef OGRE_STATIC_LIB
	m_pluginsCfg = m_resourcePath + "plugins.cfg";
#else
	m_pluginsCfg = "plugins.cfg";
#endif
#endif
	
	m_root = new Ogre::Root(m_pluginsCfg);

	Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: " + Ogre::String(PACKAGE_STRING));

	createResourceGroups();

	if(!m_root->showConfigDialog())
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: aborted at configuration dialog");
		return false;
	}
	
	m_window = m_root->initialise(true, m_caption);
	m_overlaySystem = new Ogre::OverlaySystem();

	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	/* Create the Controller */
	createController();
	
	/* Create the Roster */
	createRoster();

	/* Create the initial State objects */
	disableStateActivation();
	createInitialState();
	if(!m_firstState)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: Core has no initial state; aborting");
		return false;
	}
	createFrameListener();
	m_shutdown = false;
	enableStateActivation();
	return true;
};

bool
Core::cleanup()
{
	if(m_firstState)
	{
		m_firstState->m_next = NULL;
		popState();
	}
	return true;
}

/* Render a single frame */
bool
Core::render(Ogre::Real interval)
{
	m_root->renderOneFrame(interval);
	if(m_shutdown)
	{
		return false;
	}
	return true;
}

/* Push a new state onto the stack */
void
Core::pushState(State *state)
{
	state->m_prev = NULL;
	state->m_next = m_firstState;
	if(!m_lastState)
	{
		m_lastState = state;
	}
	m_firstState = state;
	if(state->m_next)
	{
		state->m_next->m_prev = state;
		if(state->overlay())
		{
			state->m_next->paused(m_window);
		}
		else
		{
			deactivateState(state->m_next);
		}
	}
	activateState(state);
}

/* Pop the current state off the stack; if it was the last, trigger shutdown */
void
Core::popState()
{
	bool overlay;
	
	if(!m_firstState)
	{
		return;
	}
	overlay = m_firstState->overlay();
	deactivateState(m_firstState);
	if(m_firstState->m_next)
	{
		m_firstState = m_firstState->m_next;
		m_firstState->m_prev = NULL;
		if(overlay)
		{
			m_firstState->resumed(m_window);
		}
		else
		{
			activateState(m_firstState);
		}
	}
	else
	{
		m_firstState = NULL;
		m_lastState = NULL;
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: no states remain in stack; shutting down");
		shutdown();
	}	
}

/* Replace the current state stack tail, preserving the remainder */
void
Core::setState(State *state)
{
	state->m_prev = NULL;
	if(m_firstState)
	{
		deactivateState(m_firstState);
		state->m_next = m_firstState->m_next;
		m_firstState->m_next = NULL;
	}
	else
	{
		state->m_next = NULL;
		m_lastState = state;
	}
	m_firstState = state;
	activateState(m_firstState);
}

/* Remove a state from the stack, if it's present; invoked by State::~State(),
 * so will not invoke ->deactivated() if state == m_firstState.
 */
void
Core::removeState(State *state)
{
	if(state == m_lastState)
	{
		m_lastState = state->m_prev;
	}
	if(state == m_firstState)
	{
		m_firstState = state->m_next;
		activateState(m_firstState);
	}
	if(!m_firstState)
	{
		shutdown();
	}
}

void
Core::activateState(State *state)
{
	State *p;
	OIS::MouseState mstate;
	
	if(m_playersChanged)
	{
		for(p = m_firstState; p; p = p->m_next)
		{
			p->playersChanged();
		}
		m_playersChanged = false;
	}
	if(m_inhibitStateActivation)
	{
		state->preload();
	}
	else
	{
		state->activated(m_window);
		state->sceneManager()->addRenderQueueListener(m_overlaySystem);
	}
}

void
Core::deactivateState(State *state)
{
	state->sceneManager()->removeRenderQueueListener(m_overlaySystem);
	state->deactivated(m_window);
}

int
Core::disableStateActivation()
{
	if(!m_inhibitStateActivation)
	{
		m_preInhibitState = m_firstState;
	}
	m_inhibitStateActivation++;
	return m_inhibitStateActivation;
}

int
Core::enableStateActivation()
{
	if(m_inhibitStateActivation)
	{
		m_inhibitStateActivation--;
		if(!m_inhibitStateActivation && m_preInhibitState != m_firstState)
		{
			activateState(m_firstState);
		}
	}
	return m_inhibitStateActivation;
}

/* Initialisation methods */

void
Core::createResourceGroups(void)
{
	/* Can be overidden to initialise any resource groups */
}

void
Core::createRoster(void)
{
	/* Can be overidden to create a custom Roster */
	m_roster = new Roster();
}

void
Core::createController(void)
{
	/* Can be overidden to create a custom Controller */
	m_controller = new Controller();
}

void
Core::createInitialState()
{
	/* To be overridden by subclasses in order to push a new state */
}

void
Core::createFrameListener(void)
{
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	m_window->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;

	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	m_inputManager = OIS::InputManager::createInputSystem(pl);

	m_keyboard = static_cast<OIS::Keyboard*>(m_inputManager->createInputObject(OIS::OISKeyboard, true));
	m_mouse = static_cast<OIS::Mouse*>(m_inputManager->createInputObject(OIS::OISMouse, true));

	m_mouse->setEventCallback(this);
	m_keyboard->setEventCallback(this);

	windowResized(m_window);

	Ogre::WindowEventUtilities::addWindowEventListener(m_window, this);

	m_root->addFrameListener(this);
}

/* Event listeners */

bool
Core::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if(m_window->isClosed())
	{
		m_shutdown = true;
		return false;
	}
	if(m_shutdown)
	{
		return false;
	}
	m_keyboard->capture();
	m_mouse->capture();

	if(!m_firstState)
	{
		return false;
	}
	return m_firstState->frameRenderingQueued(evt);
}

bool
Core::keyPressed(const OIS::KeyEvent &arg)
{
	if(m_firstState && m_firstState->keyPressed(arg))
	{
		return true;
	}
	return m_controller->keyPressed(arg);
}

bool
Core::keyReleased(const OIS::KeyEvent &arg)
{
	if(m_firstState && m_firstState->keyReleased(arg))
	{
		return true;
	}
	return m_controller->keyReleased(arg);
}

bool
Core::mouseMoved(const OIS::MouseEvent &arg)
{
	if(m_firstState && m_firstState->mouseMoved(arg))
	{
		return false;
	}
	return m_controller->mouseMoved(arg);
}

bool
Core::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if(m_firstState && m_firstState->mousePressed(arg, id))
	{
		return false;
	}
	return m_controller->mousePressed(arg, id);
}

bool
Core::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if(m_firstState && m_firstState->mouseReleased(arg, id))
	{
		return false;
	}
	return m_controller->mouseReleased(arg, id);
}

void
Core::windowResized(Ogre::RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = m_mouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

void
Core::windowClosed(Ogre::RenderWindow* rw)
{
	if(rw == m_window)
	{
		if(m_inputManager)
		{
			m_inputManager->destroyInputObject(m_mouse);
			m_inputManager->destroyInputObject(m_keyboard);

			OIS::InputManager::destroyInputSystem(m_inputManager);
			m_inputManager = NULL;
		}
	}
}
