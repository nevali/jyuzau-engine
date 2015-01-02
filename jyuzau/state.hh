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

#ifndef JYUZAU_STATE_HH_
# define JYUZAU_STATE_HH_              1

# include "jyuzau/defs.hh"

# include <OGRE/OgreFrameListener.h>
# include <OGRE/OgreSceneManager.h>

# include <OIS/OISEvents.h>
# include <OIS/OISKeyboard.h>
# include <OIS/OISMouse.h>

# include <btBulletDynamicsCommon.h>

namespace Jyuzau
{
	
	class Core;
	class Loadable;
	class Actor;
	class Camera;
	class Controller;
	class Scene;

	/* The State class encapsulates the game logic at any given point,
	 * including cut-scenes, menus, and so on.
	 *
	 * States can be stacked (i.e., pushed and popped), as well as
	 * replaced when needed, via Jyuzau::Core methods.
	 */
	class State: public Ogre::FrameListener
	{
		friend class Core;
	public:
		State();
		virtual ~State();
		
		virtual void preload(void);
		
		virtual Ogre::SceneManager *sceneManager(void);

		virtual int cameras(void);
		virtual Camera *camera(int index = 0);
		
		virtual Loadable *factory(Ogre::String m_kind, Ogre::String m_name);
		
		virtual bool overlay(void);
		
		virtual btDynamicsWorld *dynamics(void);
		
		virtual void sceneAttached(Scene *scene);
	protected:
		Core *m_core;
		State *m_prev, *m_next;
		bool m_loaded;
		Ogre::SceneManager *m_sceneManager;
		std::vector<Camera *> m_cameras;
		std::vector<Actor *> m_actors;
		CameraType m_defaultPlayerCameraType;
		Controller *m_controller;
		btDynamicsWorld *m_dynamics;
		bool m_overlay;
		
		virtual void load(void);
		virtual void createScenes(void);
		virtual void createSceneManager(void);
		virtual void attachScenes(void);
		virtual void createPlayers(void);
		virtual void deletePlayers(void);
		
		virtual void activated(Ogre::RenderWindow *window);
		virtual void deactivated(Ogre::RenderWindow *window);
		virtual void paused(Ogre::RenderWindow *window);
		virtual void resumed(Ogre::RenderWindow *window);

		virtual void playersChanged(void);
		
		virtual void addViewports(Ogre::RenderWindow *window);
		virtual void removeViewports(Ogre::RenderWindow *window);
		
		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
		virtual bool keyPressed(const OIS::KeyEvent &arg);
		virtual bool keyReleased(const OIS::KeyEvent &arg);
		virtual bool mouseMoved(const OIS::MouseEvent &arg);
		virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
		virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
		
		virtual void updatePhysics(btScalar timeSinceListFrame);
	};

};

#endif /*!JYUZAU_STATE_HH_*/