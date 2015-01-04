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
#include "jyuzau/light.hh"

#include <utility>

using namespace Jyuzau;

struct StateOverlapCallback: public btBroadphaseAabbCallback
{
	btVector3 m_queryAabbMin;
	btVector3 m_queryAabbMax;
	int m_numOverlap;

	StateOverlapCallback(const btVector3 &aabbMin, const btVector3 &aabbMax );
	
	virtual bool process(const btBroadphaseProxy *proxy);
};

StateOverlapCallback::StateOverlapCallback(const btVector3 &aabbMin, const btVector3 &aabbMax ):
	m_queryAabbMin(aabbMin),
	m_queryAabbMax(aabbMax),
	m_numOverlap(0)
{
}

bool
StateOverlapCallback::process(const btBroadphaseProxy* proxy)
{
	btVector3 proxyAabbMin,proxyAabbMax;
	btCollisionObject* colObj0 = (btCollisionObject*)proxy->m_clientObject;
	
	colObj0->getCollisionShape()->getAabb(colObj0->getWorldTransform(), proxyAabbMin,proxyAabbMax);
	if (TestAabbAgainstAabb2(proxyAabbMin,proxyAabbMax,m_queryAabbMin,m_queryAabbMax))
	{
		m_numOverlap++;
	}
	return true;
}




State::State():
	m_prev(NULL), m_next(NULL), m_loaded(false),
	m_sceneManager(NULL),
	m_currentScene(NULL),
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
	if(m_currentScene)
	{
		deletePlayers(m_currentScene);
	}
}

Ogre::SceneManager *
State::sceneManager(void) const
{
	return m_sceneManager;
}

int
State::cameras(void) const
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

btDynamicsWorld *
State::dynamics(void) const
{
	return m_dynamics;
}

bool
State::overlay(void) const
{
	return m_overlay;
}

void
State::playersChanged(void)
{
	if(m_currentScene)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: players have changed; re-creating");
		deletePlayers(m_currentScene);
		createPlayers(m_currentScene);
	}
}

void
State::preload(void)
{
	if(!m_loaded)
	{
		load();
	}
}

/* Add a non-unique asset to the object pool for later re-use */
void
State::addToPool(const Loadable *object)
{
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
		loadable = new Prop(m_name, this, m_kind);
	}
	else if(!m_kind.compare("actor"))
	{
		loadable = new Actor(m_name, this);
	}
	else if(!m_kind.compare("light"))
	{
		loadable = new Light(m_name, this, m_kind);
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

/* Invoked by preload() or activated() to demand-load the resources for the
 * state.
 */
void
State::load(void)
{
	m_loaded = true;
	createScenes();
	createSceneManager();
	attachScenes();
}

/* This is a utility method invoked by load() which should be overidden to
 * create and store any Scene objects.
 */

void
State::createScenes(void)
{
}

/* This is a utility method invoked by load() to create a scene manager for
 * the State.
 */
void
State::createSceneManager(void)
{
	/* Create a generic scene manager by default; descendants can override
	 * this to create specific scene managers as required.
	 */
	m_sceneManager = Ogre::Root::getSingletonPtr()->createSceneManager(Ogre::ST_GENERIC);
}

/* This is a utility method invoked by load() to attach any Scene objects to
 * the OGRE SceneManager.
 */
void
State::attachScenes(void)
{
}

/* This is a utility method invoked by the sceneAttached() callback in order
 * to create any player-actors and associated cameras.
 */
void
State::createPlayers(Scene *scene)
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
		a = c->createActor(this, scene);
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

/* This is a utility method invoked primarily by sceneDetached() to remove
 * any cameras and actors from the scene.
 */
void
State::deletePlayers(Scene *scene)
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

/* This is a utility method invoked by activated() in order to create
 * viewports for our cameras and bind playable actors to the Controller.
 */
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
			/* Inform the Actor that the camera is now active */
			m_cameras[0]->actor->setActiveCamera(m_cameras[0]);
		}
	}
	if(m_actors.size())
	{
		m_controller->bind(m_actors[0]);
	}
}

/* This is a utility method invoked by deactivated() in order to destroy
 * any viewports that we created and un-bind any actors from the Controller.
 */
void
State::removeViewports(Ogre::RenderWindow *window)
{
	std::vector<Actor *>::iterator ait;
	std::vector<Camera *>::iterator cit;
	
	m_controller->unbindAll();
	for(ait = m_actors.begin(); ait != m_actors.end(); ait++)
	{
		/* Note that this does not destroy the cameras, as they are owned
		 * by us, it merely removes the association (created by
		 * Actor::setActiveCamera()) between the camera and the actor.
		 */
		(*ait)->resetActiveCameras();
	}
	for(cit = m_cameras.begin(); cit != m_cameras.end(); cit++)
	{
		/* Remove the viewports associated with the cameras */
		(*cit)->deleteViewport();
	}
}

/* Utility method invoked by frameEventQueued() to ensure that the dynamics
 * world is updated before the frame is rendered and to perform any
 * collision-related processing.
 */
void
State::updatePhysics(btScalar timeSinceLastFrame)
{
/*	btVector3 aabbMin(1,1,1);
	btVector3 aabbMax(2,2,2);
	StateOverlapCallback aabbOverlap(aabbMin,aabbMax);*/

	m_dynamics->stepSimulation(timeSinceLastFrame, 60);
/*	m_dynamics->getBroadphase()->aabbTest(aabbMin, aabbMax, aabbOverlap); */
}

/* Event listeners */

/* Invoked by Scene::attach() once a scene has been created and attached to
 * the scene manager
 */
void
State::sceneAttached(Scene *scene)
{
	m_currentScene = scene;
	m_dynamics = scene->dynamics();
	createPlayers(scene);
}

/* Invoked by Scene::detach() when a scene is about to be torn down */
void
State::sceneDetached(Scene *scene)
{
	deletePlayers(scene);
	m_dynamics = NULL;
	m_currentScene = NULL;
}

/* Invoked by Core::activateState() when this state reaches the head of the
 * state stack and becomes active.
 */
void
State::activated(Ogre::RenderWindow *window)
{
	if(!m_loaded)
	{
		load();
	}
	addViewports(window);
}

/* Invoked by Core::deactivateState() when this state stops being at the head
 * of the state stack and so becomes inactive.
 */
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

/* Invoked by Core::frameRenderingQueued() when we are the active state in
 * order for rendering events to be processed.
 */
bool
State::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	std::vector<Actor *>::iterator ait;
	
	if(m_dynamics)
	{
		updatePhysics(evt.timeSinceLastFrame);
	}
	for(ait = m_actors.begin(); ait != m_actors.end(); ait++)
	{
		(*ait)->frameRenderingQueued(evt);
	}
	return true;
}

/* Keyboard and mouse events, forwarded by corresponding Core methods when
 * we are the active state.
 */
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

