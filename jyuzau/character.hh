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

# define CHAR_CURRENCY_MAX             5
# define CHAR_WEAPON_MAX               10

namespace Jyuzau
{
	
	class Player;
	
	/* A Character represents the identity and properties of a player, and is
	 * attached to a Player object. Persistent state such as the player's name,
	 * skin selection, currency, ammo, etc., are maintained in the Character
	 * instance.
	 */
	class Character
	{
	public:
		Character(Ogre::String name = "Player");
		virtual ~Character();
		
		virtual void attach(Player *player);
	protected:
		Ogre::String m_name;
		Player *m_player;
		unsigned currency[CHAR_CURRENCY_MAX + 1];
		unsigned ammo[CHAR_WEAPON_MAX + 1];
	};
	
};

#endif 