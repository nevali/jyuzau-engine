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
	
	if(!m_node)
	{
		/* Can't create a camera if we don't yet have a node */
		return NULL;
	}
	cam = new Camera(m_group + "::camera[" + std::to_string((int) type) + "]", m_node->getCreator());
	cam->attach(m_node);
	/* XXX adjust position, orientation, etc. */
	return cam;
}

void
Actor::forward(MoveSpeed speed)
{
}

void
Actor::backward(MoveSpeed speed)
{
}

void
Actor::turnLeft(MoveSpeed speed)
{
}

void
Actor::turnRight(MoveSpeed speed)
{
}

void
Actor::strafeLeft(MoveSpeed speed)
{
}

void
Actor::strafeRight(MoveSpeed speed)
{
}

void
Actor::primaryFire(void)
{
}

void
Actor::secondaryFire(void)
{
	
}

void
Actor::special(void)
{
}

void
Actor::crouch(void)
{
}

void
Actor::jump(void)
{
}

void
Actor::zoom(void)
{
}

void
Actor::switchWeapon(int index)
{
}

void
Actor::prevWeapon(void)
{
}

void
Actor::nextWeapon(void)
{
}
