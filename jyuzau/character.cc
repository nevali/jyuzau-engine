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

#include "jyuzau/character.hh"
#include "jyuzau/actor.hh"

using namespace Jyuzau;

Character::Character(Ogre::String title, Ogre::String actorName):
	m_title(title),
	m_actorName(actorName),
	m_actor(NULL),
	m_level(1)
{
	memset(m_currency, 0, sizeof(m_currency));
	memset(m_ammo, 0, sizeof(m_ammo));
}

Character::Character(Character &character):
	m_actor(NULL)
{
	m_title = character.title();
	m_actorName = character.actorName();
	m_level = character.level();
	memcpy(m_currency, character.m_currency, sizeof(m_currency));
	memcpy(m_ammo, character.m_ammo, sizeof(m_ammo));
}

Character::~Character()
{
	if(m_actor && m_actor->m_character == this)
	{
		m_actor->characterDetached();
		m_actor->m_character = NULL;
	}
}

/* Create a dummy Actor representing the character, optionally placing it
 * within a scene. The caller must delete the Actor when finished. Note that
 * a dummy Actor is not attached to the Character, and so won't inherit its
 * traits (it's intended for use in character selection states, and similar
 * situations where a simple actor is required instead of a playable or
 * otherwise interactive character).
 */
Actor *
Character::createDummy(Scene *scene)
{
	Actor *actor;
	
	actor = new Actor(m_actorName);
	if(!actor)
	{
		return NULL;
	}
	if(scene)
	{
		actor->attach(scene);
	}
	return actor;
}

Actor *
Character::createDummy(Ogre::SceneManager *sceneManager)
{
	Actor *actor;
	
	actor = new Actor(m_actorName);
	if(!actor)
	{
		return NULL;
	}
	if(sceneManager)
	{
		actor->attach(sceneManager);
	}
	return actor;
}

/* Create a new Actor for the character, optionally placing it within
 * a scene. Note that it's the caller's responsibility to delete the
 * Actor when finished.
 */
Actor *
Character::createActor(Scene *scene)
{
	Actor *actor;
	
	actor = new Actor(m_actorName);
	if(!actor)
	{
		return NULL;
	}
	if(scene)
	{
		actor->attach(scene);
	}
	attach(actor);
	return actor;
}

Actor *
Character::createActor(Ogre::SceneManager *sceneManager)
{
	Actor *actor;
	
	actor = new Actor(m_actorName);
	if(!actor)
	{
		return NULL;
	}
	if(sceneManager)
	{
		actor->attach(sceneManager);
	}
	attach(actor);
	return actor;
}

/* Attach the character to an existing actor */
void
Character::attach(Actor *actor)
{
	if(m_actor && m_actor->m_character == this)
	{
		m_actor->characterDetached();
		m_actor->m_character = NULL;
	}
	m_actor = actor;
	actor->m_character = this;
	actor->characterAttached();
}

Actor *
Character::actor(void)
{
	return m_actor;
}

Ogre::String
Character::actorName(void)
{
	if(m_actor)
	{
		return m_actor->name();
	}
	return m_actorName;
}

void
Character::setActorName(Ogre::String newName)
{
	m_actorName = newName;
}

Ogre::String
Character::title(void)
{
	return m_title;
}

void
Character::setTitle(Ogre::String newTitle)
{
	m_title = newTitle;
}

unsigned 
Character::level(void)
{
	return m_level;
}

unsigned
Character::currency(int slot)
{
	if(slot >= CHAR_CURRENCY_MAX)
	{
		return 0;
	}
	return m_currency[slot];
}

unsigned
Character::ammo(int slot)
{
	if(slot >= CHAR_WEAPON_MAX)
	{
		return 0;
	}
	return m_ammo[slot];
}
