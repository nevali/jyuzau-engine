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

#include "jyuzau/core.hh"
#include "jyuzau/state.hh"

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

using namespace Jyuzau;

static Core *singleton = NULL;

Core::Core(void)
	: mRoot(0),
	mCamera(0),
	mSceneMgr(0),
	mWindow(0),
	mPluginsCfg(Ogre::StringUtil::BLANK),
	mCursorWasVisible(false),
	mShutDown(true),
	mInputManager(0),
	mMouse(0),
	mKeyboard(0),
	mOverlaySystem(0),
	activeScene(NULL),
	m_firstState(NULL),
	m_lastState(NULL)
{
	singleton = this;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
	m_ResourcePath = Ogre::macBundlePath() + "/Contents/Resources/";
#else
	m_ResourcePath = "";
#endif
}

Core::~Core(void)
{
	singleton = NULL;

	if (mOverlaySystem) delete mOverlaySystem;
	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);
	delete mRoot;
}

/* Obtain the singleton instance of Jyuzau::Core, if one exists */
Core *
Core::getInstance(void)
{
	return singleton;
}

/* Initialise the engine, create the initial scene */
bool
Core::init(void)
{
#ifdef _DEBUG
#ifndef OGRE_STATIC_LIB
	mPluginsCfg = m_ResourcePath + "plugins_d.cfg";
#else
	mPluginsCfg = "plugins_d.cfg";
#endif
#else
#ifndef OGRE_STATIC_LIB
	mPluginsCfg = m_ResourcePath + "plugins.cfg";
#else
	mPluginsCfg = "plugins.cfg";
#endif
#endif
	
	mRoot = new Ogre::Root(mPluginsCfg);

	Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: " + Ogre::String(PACKAGE_STRING));

	setupResources();

	if(!mRoot->showConfigDialog())
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: aborted at configuration dialog");
		return false;
	}
	
	mWindow = mRoot->initialise(true, "Jyuzau");
	
	chooseSceneManager();
	createCamera();
	createViewports();
	createResourceListener();
	loadResources();

	/* Create the initial state */
	createInitialState();
	if(!m_firstState)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: Core has no initial state; aborting");
		return false;
	}
	createFrameListener();
	mShutDown = false;
	return true;
};

void
Core::shutdown()
{
	mShutDown = true;
}

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
	mRoot->startRendering();
	return cleanup();
#endif
}

/* Render a single frame */
bool
Core::render(Ogre::Real interval)
{
	mRoot->renderOneFrame(interval);
	if(mShutDown)
	{
		return false;
	}
	return true;
}

/* Return a pointer to the active camera */
Ogre::Camera *
Core::camera(void)
{
	return mCamera;
}

/* Return a pointer to the scene manager */
Ogre::SceneManager *
Core::sceneManager(void)
{
	return mSceneMgr;
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
		state->m_next->deactivated();
	}
	state->activated();
}

/* Pop the current state off the stack; if it was the last, trigger shutdown */
void
Core::popState()
{
	if(!m_firstState)
	{
		return;
	}
	m_firstState->deactivated();
	if(m_firstState->m_next)
	{
		m_firstState = m_firstState->m_next;
		m_firstState->m_prev = NULL;
		m_firstState->activated();
	}
	else
	{
		m_firstState = NULL;
		m_lastState = NULL;
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
		m_firstState->deactivated();
		state->m_next = m_firstState->m_next;
		m_firstState->m_next = NULL;
	}
	else
	{
		state->m_next = NULL;
		m_lastState = state;
	}
	m_firstState = state;
	state->activated();
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
		m_firstState->activated();
	}
	if(!m_firstState)
	{
		shutdown();
	}
}

void
Core::chooseSceneManager(void)
{
	// Get the SceneManager, in this case a generic one
	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);

	// Initialize the OverlaySystem (changed for Ogre 1.9)
	mOverlaySystem = new Ogre::OverlaySystem();
	mSceneMgr->addRenderQueueListener(mOverlaySystem);
}


void
Core::createCamera(void)
{
	// Create the camera
	mCamera = mSceneMgr->createCamera("PlayerCam");

	// Position it at 500 in Z direction
	mCamera->setPosition(Ogre::Vector3(0,0,80));
	// Look back along -Z
	mCamera->lookAt(Ogre::Vector3(0,0,-300));
	mCamera->setNearClipDistance(5);
}

void
Core::createFrameListener(void)
{
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;

	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	mInputManager = OIS::InputManager::createInputSystem(pl);

	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));

	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	// Set initial mouse clipping size
	windowResized(mWindow);

	// Register as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

	mRoot->addFrameListener(this);
}

void
Core::createViewports(void)
{
	// Create one viewport, entire window
	Ogre::Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}

void
Core::setupResources(void)
{
}

void
Core::createResourceListener(void)
{
}

void
Core::loadResources(void)
{
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

bool
Core::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if(mWindow->isClosed())
	{
		mShutDown = true;
		return false;
	}

	if(mShutDown)
	{
		return false;
	}
	
	// Need to capture/update each device
	mKeyboard->capture();
	mMouse->capture();

	if(!m_firstState)
	{
		return false;
	}
	return m_firstState->frameRenderingQueued(evt);
}

bool
Core::keyPressed(const OIS::KeyEvent &arg)
{
	if(!m_firstState)
	{
		return false;
	}
	return m_firstState->keyPressed(arg);
}

bool
Core::keyReleased(const OIS::KeyEvent &arg)
{
	if(!m_firstState)
	{
		return false;
	}
	return m_firstState->keyReleased(arg);
}

bool
Core::mouseMoved(const OIS::MouseEvent &arg)
{
	if(!m_firstState)
	{
		return false;
	}
	return m_firstState->mouseMoved(arg);
}

bool
Core::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if(!m_firstState)
	{
		return false;
	}
	return m_firstState->mousePressed(arg, id);
}

bool
Core::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if(!m_firstState)
	{
		return false;
	}
	return m_firstState->mouseReleased(arg, id);
}

void
Core::windowResized(Ogre::RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

void
Core::windowClosed(Ogre::RenderWindow* rw)
{
	if(rw == mWindow)
	{
		if(mInputManager)
		{
			mInputManager->destroyInputObject(mMouse);
			mInputManager->destroyInputObject(mKeyboard);

			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = NULL;
		}
	}
}

void
Core::createInitialState()
{
	/* To be overridden by subclasses */
}
