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
CharacterSelectionState::activated(Ogre::RenderWindow *window)
{
	State::activated(window);
	if(m_roster->count() < 1)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: No characters are available to select");
		Core::getInstance()->popState();
		return;
	}
	if(m_roster->count() == 1)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: Only one character is available to select");
		Core::getInstance()->popState();
		return;
	}
	Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: Will perform character selection");
}