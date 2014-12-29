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

#include "jyuzau/roster.hh"
#include "jyuzau/character.hh"

using namespace Jyuzau;

Roster::Roster():
	m_entries(),
	m_populated(false)
{
}

Roster::~Roster()
{
	std::vector<RosterEntry *>::iterator i;
	
	for(i = m_entries.begin(); i != m_entries.end(); i++)
	{
		delete (*i)->character;
		delete (*i);
	}
}

int
Roster::count(void)
{
	if(!m_populated)
	{
		populate();
	}
	return (int) m_entries.size();
}

Actor *
Roster::dummy(int index, Scene *scene)
{
	if(index > m_entries.size())
	{
		return NULL;
	}
	return m_entries[index]->character->createDummy(scene);
}

Ogre::String
Roster::title(int index)
{
	if(index > m_entries.size())
	{
		return Ogre::String("<Invalid>");
	}
	return m_entries[index]->title;
}

Character *
Roster::character(int index)
{
	if(index > m_entries.size())
	{
		return NULL;
	}
	return new Character(*m_entries[index]->character);
}

void
Roster::populate(void)
{
	RosterEntry *p;
	
	/* Dummy implementation which creates a single stock character */
	m_populated = true;
	
	p = new RosterEntry;
	p->character = new Character();
	p->title = p->character->title();
	m_entries.push_back(p);
}
