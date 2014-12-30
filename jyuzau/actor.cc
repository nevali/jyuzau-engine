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

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreSceneNode.h>

#include "jyuzau/actor.hh"
#include "jyuzau/character.hh"
#include "jyuzau/camera.hh"

using namespace Jyuzau;

Actor *
Actor::create(Ogre::String name, Scene *scene)
{
	Actor *p;
	
	p = new Actor(name);
	if(!p->load())
	{
		delete p;
		return NULL;
	}
	if(scene)
	{
		if(!p->attach(scene))
		{
			delete p;
			return NULL;
		}
	}
	return p;
}

Actor *
Actor::create(Ogre::String name, Ogre::SceneManager *sceneManager)
{
	Actor *p;
	
	p = new Actor(name);
	if(!p->load())
	{
		delete p;
		return NULL;
	}
	if(sceneManager)
	{
		if(!p->attach(sceneManager))
		{
			delete p;
			return NULL;
		}
	}
	return p;
}

Actor::Actor(Ogre::String name):
	Prop::Prop(name, "actor"),
	m_character(NULL),
	m_health(100.0f),
	m_level(1)
{
	resetActiveCameras();
}

Actor::~Actor()
{
}

Character *
Actor::character(void)
{
	return m_character;
}

void
Actor::characterAttached(void)
{
	m_level = m_character->level();
}

void
Actor::characterDetached(void)
{
}

Camera *
Actor::createCamera(CameraType type)
{
	Camera *cam;
	
	if(type >= CT_COUNT)
	{
		return NULL;
	}
	if(!m_node)
	{
		/* Can't create a camera if we don't yet have a node */
		return NULL;
	}
	cam = new Camera(m_group + "::camera[" + std::to_string((int) type) + "]", m_node->getCreator());
	cam->cameraType = type;
	cam->attach(m_node);
	if(type == CT_FIRSTPERSON)
	{
		cam->limitPitch = true;
	}
	/* XXX adjust position, orientation, etc. */
	setActiveCamera(cam);
	return cam;
}

void
Actor::resetActiveCameras(void)
{
	memset(m_cameras, 0, sizeof(m_cameras));
}

void
Actor::setActiveCamera(Camera *cam)
{
	if(cam->cameraType >= CT_COUNT)
	{
		return;
	}
	m_cameras[cam->cameraType] = cam;
}

bool
Actor::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if(!m_node)
	{
		return false;
	}
	return true;
}

void
Actor::forward(MoveSpeed speed)
{
	if(!m_node)
	{
		return;
	}
	m_node->translate(0, 0, -CONTROL_WALK_DISTANCE, Ogre::Node::TS_LOCAL);
}

void
Actor::beginForward(MoveSpeed speed)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::endForward(void)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::backward(MoveSpeed speed)
{
	if(!m_node)
	{
		return;
	}
	m_node->translate(0, 0, CONTROL_WALK_DISTANCE, Ogre::Node::TS_LOCAL);
}

void
Actor::beginBackward(MoveSpeed speed)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::endBackward(void)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::turnLeft(MoveSpeed speed)
{
	if(!m_node)
	{
		return;
	}
	m_node->yaw((Ogre::Radian) CONTROL_YAW_ANGLE);
}

void
Actor::beginTurnLeft(void)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::endTurnLeft(void)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::turnRight(MoveSpeed speed)
{
	if(!m_node)
	{
		return;
	}
	m_node->yaw((Ogre::Radian) -CONTROL_YAW_ANGLE);
}

void
Actor::beginTurnRight(void)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::endTurnRight(void)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::turnLeftRight(int distance)
{
	int i;
	Ogre::Radian angle;
	
	if(!m_node)
	{
		return;
	}
	angle = (Ogre::Radian) -distance * CONTROL_YAW_FACTOR;
	m_node->yaw(angle);
}

void
Actor::strafeLeft(MoveSpeed speed)
{
	if(!m_node)
	{
		return;
	}
	m_node->translate(-CONTROL_WALK_DISTANCE, 0, 0);
}

void
Actor::beginStrafeLeft(MoveSpeed speed)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::endStrafeLeft(void)
{
	if(!m_node)
	{
		return;
	}
}


void
Actor::strafeRight(MoveSpeed speed)
{
	if(!m_node)
	{
		return;
	}
	m_node->translate(CONTROL_WALK_DISTANCE, 0, 0);
}

void
Actor::beginStrafeRight(MoveSpeed speed)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::endStrafeRight(void)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::lookUp(void)
{
	if(!m_node)
	{
		return;
	}
	if(m_cameras[CT_FIRSTPERSON])
	{
		m_cameras[CT_FIRSTPERSON]->pitch((Ogre::Radian) -CONTROL_PITCH_ANGLE);
	}
}

void
Actor::beginLookUp(void)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::endLookUp(void)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::lookDown(void)
{
	if(!m_node)
	{
		return;
	}
	if(m_cameras[CT_FIRSTPERSON])
	{
		m_cameras[CT_FIRSTPERSON]->pitch((Ogre::Radian) CONTROL_PITCH_ANGLE);
	}
}

void
Actor::beginLookDown(void)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::endLookDown(void)
{
	if(!m_node)
	{
		return;
	}
}


void
Actor::lookUpDown(int distance)
{
	int i;
	Ogre::Radian angle;
	
	if(!m_node)
	{
		return;
	}
	angle = (Ogre::Radian) distance * CONTROL_PITCH_FACTOR;
	if(m_cameras[CT_FIRSTPERSON])
	{
		m_cameras[CT_FIRSTPERSON]->pitch(angle);
	}
}

void
Actor::resetCamera(void)
{
	if(!m_node)
	{
		return;
	}
	if(m_cameras[CT_FIRSTPERSON])
	{
		m_cameras[CT_FIRSTPERSON]->pitchNode->resetOrientation();
	}
}

void
Actor::primaryFire(void)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::secondaryFire(void)
{
	if(!m_node)
	{
		return;
	}
	
}

void
Actor::special(void)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::crouch(void)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::jump(void)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::zoom(void)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::switchWeapon(int index)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::prevWeapon(void)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::nextWeapon(void)
{
	if(!m_node)
	{
		return;
	}
}
