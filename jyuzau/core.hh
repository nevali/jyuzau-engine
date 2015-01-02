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

namespace Jyuzau
{
	class State;
	class Scene;
	class Character;
	class Roster;
	class Camera;
	class Controller;
	
	class Core: public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::KeyListener, public OIS::MouseListener
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
		
		/* Properties */
		virtual Ogre::Root *root(void);
		virtual Ogre::RenderWindow *window(void);
		virtual Ogre::OverlaySystem *overlays(void);
		virtual State *state(void);
		virtual Roster *roster(void);
		virtual Camera *camera(int index = 0);
		virtual Ogre::SceneManager *sceneManager(void);
		virtual Controller *controller(void);
		
		/* State management */
		virtual void pushState(State *state);
		virtual void popState(void);
		virtual void setState(State *state);
		virtual void removeState(State *state);
		virtual int enableStateActivation(void);
		virtual int disableStateActivation(void);
		
		/* Players */
		virtual int players(void);
		virtual Character *player(int index);
		virtual void resetPlayers(void);
		virtual void addPlayer(Character *character);
				
		/* Trigger application termination */
		virtual void shutdown();
	protected:
		Scene *m_activeScene;
		State *m_firstState, *m_lastState;
		Ogre::Root *m_root;
		Ogre::RenderWindow *m_window;
		Ogre::OverlaySystem *m_overlaySystem;
		Ogre::String m_resourcePath;
		Ogre::String m_pluginsCfg;
		bool m_cursorWasVisible;
		bool m_shutdown;
		OIS::InputManager *m_inputManager;
		OIS::Mouse *m_mouse;
		OIS::Keyboard *m_keyboard;
# ifdef OGRE_STATIC_LIB
		Ogre::StaticPluginLoader m_staticPluginLoader;
# endif
		Roster *m_roster;
		int m_inhibitStateActivation;
		State *m_preInhibitState;
		std::vector<Character *>m_players;
		bool m_playersChanged;
		Controller *m_controller;
		Ogre::String m_caption;
		
		virtual void activateState(State *state);
		virtual void deactivateState(State *state);
		
		virtual void createResourceGroups(void);
		virtual void createRoster(void);
		virtual void createInitialState(void);
		virtual void createFrameListener(void);
		virtual void createController(void);

		/* Event listeners */
		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
		virtual bool keyPressed(const OIS::KeyEvent &arg);
		virtual bool keyReleased(const OIS::KeyEvent &arg);
		virtual bool mouseMoved(const OIS::MouseEvent &arg);
		virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
		virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
		virtual void windowResized(Ogre::RenderWindow* rw);
		virtual void windowClosed(Ogre::RenderWindow* rw);

	};

};

#endif /*!JYUZAU_CORE_HH_*/