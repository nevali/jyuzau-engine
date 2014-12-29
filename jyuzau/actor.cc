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

#include "jyuzau/actor.hh"
#include "jyuzau/character.hh"

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

Actor::Actor(Ogre::String name):
	Prop::Prop(name, "actor"),
	m_character(NULL),
	m_health(100.0f),
	m_level(1)
{
	memset(m_cameras, 0, sizeof(m_cameras));
}

Actor::~Actor()
{
	size_t c;
	
	for(c = 0; c < CT_COUNT; c++)
	{
		if(m_cameras[c])
		{
			delete m_cameras[c];
		}
	}
}

Ogre::Camera *
Actor::camera(CameraType type)
{
	if(type >= CT_COUNT)
	{
		return NULL;
	}
	if(!m_cameras[type])
	{
		m_cameras[type] = createCamera(type);
	}
	return m_cameras[type];
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

Ogre::Camera *
Actor::createCamera(CameraType type)
{
	return NULL;
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
