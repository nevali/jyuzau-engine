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

#ifndef JYUZAU_CHARACTER_HH_
# define JYUZAU_CHARACTER_HH_          1

# include <OGRE/OgreString.h>

# include "jyuzau/defs.hh"

namespace Jyuzau
{
	
	class Actor;
	class Scene;
	
	/* A Character represents the identity and properties of a player, and is
	 * attached to an Actor object. Persistent state such as the player's name,
	 * skin selection, currency, ammo, etc., are maintained in the Character
	 * instance.
	 *
	 * A Character instance is also used to define the properties of special
	 * NPCs - those that the player interacts with in particular ways to
	 * advance the story, for example.
	 */
	class Character
	{
	public:
		Character(Character &character);
		Character(Ogre::String title = "Player", Ogre::String actor = "player");
		virtual ~Character();
		
		/* Create a new Actor for the character, optionally placing it within
		 * a scene.
		 */
		virtual Actor *createActor(Scene *scene = NULL);
		
		/* Create a dummy Actor for the character - that is, an actor which
		 * represents the character (because it uses the same assets), but is
		 * not attached to the character itself */
		virtual Actor *createDummy(Scene *scene = NULL);
		
		/* Attach the character to an existing actor */
		virtual void attach(Actor *actor);
		

		/* Properties */
		virtual Actor *actor(void);
		virtual Ogre::String actorName(void);
		virtual void setActorName(Ogre::String newName);
		virtual Ogre::String title(void);
		virtual void setTitle(Ogre::String newTitle);
		virtual unsigned level(void);
		virtual unsigned currency(int slot);
		virtual unsigned ammo(int slot);
	protected:
		Actor *m_actor;
		Ogre::String m_actorName;
		Ogre::String m_title;
		unsigned m_level;
		unsigned m_currency[CHAR_CURRENCY_MAX + 1];
		unsigned m_ammo[CHAR_WEAPON_MAX + 1];
	};
	
};

#endif 