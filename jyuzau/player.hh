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

#ifndef JYUZAU_PLAYER_HH_
# define JYUZAU_PLAYER_HH_             1

# include "jyuzau/actor.hh"

namespace Jyuzau
{
	
	class Character;
	
	/* A Player is a specialisation of Actor which has the ability to be
	 * controlled by the user.
	 *
	 * Note that players share an asset namespace with actors (i.e., player
	 * assets are loaded from assets/actors/foo, rather than a specific
	 * assets/players/foo).
	 */
	class Player: public Actor
	{
		friend class Character;
	public:
		static Player *create(Ogre::String name, Scene *scene = NULL);
		
		Player(Ogre::String name);
		virtual ~Player();
	protected:
		Character *m_character;
		
		virtual void characterAttached(void);
		virtual void characterDetached(void);
	};
};

#endif /*!JYUZAU_PLAYER_HH_*/
