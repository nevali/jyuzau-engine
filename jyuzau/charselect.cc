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

#include "jyuzau/charselect.hh"
#include "jyuzau/core.hh"
#include "jyuzau/roster.hh"
#include "jyuzau/character.hh"

using namespace Jyuzau;

CharacterSelectionState::CharacterSelectionState():
	State::State(),
	m_roster(NULL)
{
}

void
CharacterSelectionState::load(void)
{
	m_roster = Core::getInstance()->roster();
	State::load();
}

void
CharacterSelectionState::createPlayers(void)
{
	/* We don't want to create any actors or cameras for the existing
	 * players, if any, so this is a no-op.
	 */
}

void
CharacterSelectionState::activated(Ogre::RenderWindow *window)
{
	Character *c;
	
	State::activated(window);
	m_core->resetPlayers();
	if(m_roster->count() < 1)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: No characters are available to select");
		m_core->popState();
		return;
	}
	if(m_roster->count() == 1)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: Only one character is available to select");
		c = m_roster->character(0);
		if(!c)
		{
			Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: Roster returned NULL for character #0");
		}
		else
		{
			Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: Adding '" + c->title() + "' as first player");
			m_core->addPlayer(m_roster->character(0));
		}
		m_core->popState();
		return;
	}
	Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: Will perform character selection");
}