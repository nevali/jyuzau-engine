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

#ifndef JYUZAU_CORE_HH_
# define JYUZAU_CORE_HH_               1

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#  include <OIS/OISEvents.h>
#  include <OIS/OISInputManager.h>
#  include <OIS/OISKeyboard.h>
#  include <OIS/OISMouse.h>

#  include <OGRE/SdkTrays.h>
#  include <OGRE/SdkCameraMan.h>
#else
#  include <OISEvents.h>
#  include <OISInputManager.h>
#  include <OISKeyboard.h>
#  include <OISMouse.h>

#  include <SdkTrays.h>
#  include <SdkCameraMan.h>
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
#  include "OgreStaticPluginLoader.h"
#endif

# include "scene.hh"

namespace Jyuzau
{

	class Core : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::KeyListener, public OIS::MouseListener, OgreBites::SdkTrayListener
	{
	public:
		Core *getInstance(void);
		
		Core(void);
		virtual ~Core(void);

		virtual bool init();
		virtual bool cleanup();
		virtual bool go();
		virtual bool render(Ogre::Real interval);
	protected:
		virtual bool configure(void);
		virtual void chooseSceneManager(void);
		virtual void createCamera(void);
		virtual void createFrameListener(void);
		virtual void createScene(void);
		virtual void destroyScene(void);
		virtual void createViewports(void);
		virtual void setupResources(void);
		virtual void createResourceListener(void);
		virtual void loadResources(void);
		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

		virtual bool keyPressed(const OIS::KeyEvent &arg);
		virtual bool keyReleased(const OIS::KeyEvent &arg);
		virtual bool mouseMoved(const OIS::MouseEvent &arg);
		virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
		virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

		// Adjust mouse clipping area
		virtual void windowResized(Ogre::RenderWindow* rw);
		// Unattach OIS before window shutdown (very important under Linux)
		virtual void windowClosed(Ogre::RenderWindow* rw);

		Scene *activeScene;

		Ogre::Root*                 mRoot;
		Ogre::Camera*               mCamera;
		Ogre::SceneManager*         mSceneMgr;
		Ogre::RenderWindow*         mWindow;
		Ogre::String                mResourcesCfg;
		Ogre::String                mPluginsCfg;

		Ogre::OverlaySystem*        mOverlaySystem;

		// OgreBites
		OgreBites::InputContext     mInputContext;
		OgreBites::SdkTrayManager*	mTrayMgr;
		OgreBites::SdkCameraMan*    mCameraMan;     	// Basic camera controller
		OgreBites::ParamsPanel*     mDetailsPanel;   	// Sample details panel
		bool                        mCursorWasVisible;	// Was cursor visible before dialog appeared?
		bool                        mShutDown;

		//OIS Input devices
		OIS::InputManager*          mInputManager;
		OIS::Mouse*                 mMouse;
		OIS::Keyboard*              mKeyboard;

		// Added for Mac compatibility
		Ogre::String                 m_ResourcePath;

		#ifdef OGRE_STATIC_LIB
		Ogre::StaticPluginLoader m_StaticPluginLoader;
		#endif
	};

};

#endif /*!JYUZAU_CORE_HH_*/