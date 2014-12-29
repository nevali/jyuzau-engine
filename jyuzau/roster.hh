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

#ifndef JYUZAU_ROSTER_HH_
# define JYUZAU_ROSTER_HH_             1

# include <utility>

# include <OGRE/OgreString.h>

namespace Jyuzau
{
	class Actor;
	class Character;
	class Scene;
	
	struct RosterEntry
	{
		Character *character;
		Ogre::String title;
	};
	
	/* The Roster provides character selection capabilities */
	class Roster
	{
	public:
		Roster();
		~Roster();
		
		/* Return the count of selectable characters */
		virtual int count(void);
		
		/* Return an actor representing the character, optionally adding it
		 * to the scene. Note that the character is not attached to the actor.
		 */
		virtual Actor *dummy(int index = 0, Scene *scene = NULL);
		
		/* Return the character's title */
		virtual Ogre::String title(int index = 0);
		
		/* Obtain an instance of a character. Note that this does not create
		 * the actor as well (invoke Character::createActor() to do that).
		 */
		virtual Character *character(int index);
	protected:
		bool m_populated;
		std::vector<RosterEntry *> m_entries;

		virtual void populate(void);
	};

};

#endif /*!JYUZAU_ROSTER_HH_*/
