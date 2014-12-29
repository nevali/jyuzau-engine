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

# include <OGRE/OgreCamera.h>
# include <OGRE/OgreEntity.h>
# include <OGRE/OgreRoot.h>
# include <OGRE/OgreSceneManager.h>
# include <OGRE/OgreRenderWindow.h>
# include <OGRE/OgreWindowEventUtilities.h>

# include <OGRE/Overlay/OgreOverlaySystem.h>

# include <OIS/OISEvents.h>
# include <OIS/OISInputManager.h>
# include <OIS/OISKeyboard.h>
# include <OIS/OISMouse.h>

# include "scene.hh"

namespace Jyuzau
{
	class State;

	class Core : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::KeyListener, public OIS::MouseListener
	{
		friend class State;
	public:
		static Core *getInstance(void);
		
		Core(void);
		virtual ~Core(void);

		/* Single-method run-loop */
		virtual bool go();

		/* Alternative interface to the run-loop */
		virtual bool init();
		virtual bool cleanup();
		virtual bool render(Ogre::Real interval);
		
		virtual Ogre::Camera *camera(void);
		virtual Ogre::SceneManager *sceneManager(void);
		
		virtual void pushState(State *state);
		virtual void popState(void);
		virtual void setState(State *state);
		virtual void removeState(State *state);
		
		virtual void shutdown();
	protected:
		virtual void createInitialState(void);
		virtual void chooseSceneManager(void);
		virtual void createCamera(void);
		virtual void createFrameListener(void);
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
		virtual void windowResized(Ogre::RenderWindow* rw);
		virtual void windowClosed(Ogre::RenderWindow* rw);

		Scene *activeScene;

		Ogre::Root*                 mRoot;
		Ogre::Camera*               mCamera;
		Ogre::SceneManager*         mSceneMgr;
		Ogre::RenderWindow*         mWindow;
		Ogre::String                mResourcesCfg;
		Ogre::String                mPluginsCfg;

		Ogre::OverlaySystem*        mOverlaySystem;

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
		
		State *m_firstState, *m_lastState;
	};

};

#endif /*!JYUZAU_CORE_HH_*/