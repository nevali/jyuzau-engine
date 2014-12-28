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

#include "jyuzau.hh"

using namespace Jyuzau;

Prop::Prop(Ogre::String name, Ogre::String kind):
	Loadable::Loadable(name, kind, true)
{
}

Prop::~Prop()
{
}

LoadableObject *
Prop::factory(Ogre::String name, AttrList &attrs)
{
	if(!m_root)
	{
		if(!name.compare(m_kind))
		{
			return new LoadableProp(name, attrs);
		}
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: unexpected root element <" + name + ">");
		return NULL;
	}
	if(m_cur != m_root)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: unexpected child element <" + name + ">");
		return NULL;
	}
	if(!name.compare("mesh"))
	{
		return new LoadableMesh(name, attrs);
	}
	if(!name.compare("material"))
	{
		return new LoadableMaterial(name, attrs);
	}
	Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: unexpected element <" + name + ">");
	return NULL;
}

void
Prop::loaded(void)
{
	Loadable::loaded();
}


bool
Prop::attach(void)
{
	return true;
}

bool
Prop::detach(void)
{
	return true;
}




LoadableProp::LoadableProp(Ogre::String name, AttrList &attrs):
	LoadableObject(name, attrs),
	m_mesh(NULL),
	m_material(NULL)
{
}

bool
LoadableProp::add(LoadableObject *child)
{
	if(!child->name().compare("mesh"))
	{
		m_mesh = dynamic_cast<LoadableMesh *>(child);
	}
	else if(!child->name().compare("material"))
	{
		m_material = dynamic_cast<LoadableMaterial *>(child);
	}
	return LoadableObject::add(child);
}

bool
LoadableProp::complete()
{
	if(!m_mesh)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: prop is missing a mesh");
		return false;
	}
	if(!m_material)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: prop is missing material");
		return false;
	}
	return LoadableObject::complete();
}




LoadableMesh::LoadableMesh(Ogre::String name, AttrList &attrs):
	LoadableObject(name, attrs),
	m_source("")
{
	AttrListIterator it;
	
	for(it = m_attrs.begin(); it != m_attrs.end(); it++)
	{
		Attr p = *it;
		
		if(!p.first.compare("src"))
		{
			m_source = p.second;
		}
	}
}

bool
LoadableMesh::complete(void)
{
	return m_source.length();
}




LoadableMaterial::LoadableMaterial(Ogre::String name, AttrList &attrs):
	LoadableObject(name, attrs),
	m_source("")
{
	AttrListIterator it;
	
	for(it = m_attrs.begin(); it != m_attrs.end(); it++)
	{
		Attr p = *it;
		
		if(!p.first.compare("src"))
		{
			m_source = p.second;
		}
	}
}

bool
LoadableMaterial::complete(void)
{
	return m_source.length();
}
