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

Prop *
Prop::create(Ogre::String name, Scene *scene, Ogre::Vector3 pos)
{
	Prop *p;
	
	p = new Prop(name);
	if(!p->load())
	{
		delete p;
		return NULL;
	}
	if(scene)
	{
		if(!p->attach(scene, name, pos))
		{
			delete p;
			return NULL;
		}
	}
	return p;
}

Prop::Prop(Ogre::String name, Ogre::String kind):
	Loadable::Loadable(name, kind, true),
	m_entity(NULL),
	m_node(NULL)
{
}

Prop::~Prop()
{
	if(m_node)
	{
		m_node->detachObject(m_entity);
		delete m_node;
	}
	if(m_entity)
	{
		delete m_entity;
	}
}

LoadableObject *
Prop::factory(Ogre::String name, AttrList &attrs)
{
	if(!m_root)
	{
		if(!name.compare(m_kind))
		{
			return new LoadableProp(this, name, attrs);
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
		return new LoadableMesh(this, name, attrs);
	}
	if(!name.compare("material"))
	{
		return new LoadableMaterial(this, name, attrs);
	}
	Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: unexpected element <" + name + ">");
	return NULL;
}

void
Prop::loaded(void)
{
	Ogre::ResourceGroupManager *gm;
	
	gm = Ogre::ResourceGroupManager::getSingletonPtr();
	gm->addResourceLocation(m_container, "FileSystem", m_group);
	Loadable::loaded();
	gm->initialiseResourceGroup(m_group);
}

Ogre::Entity *
Prop::entity(Ogre::SceneManager *scene)
{
	if(m_entity)
	{
		return m_entity;
	}
	if(!m_loaded || !m_load_status)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: cannot attach a prop which has not been properly loaded");
	}
	m_entity = scene->createEntity(m_name, dynamic_cast<LoadableProp *>(m_root)->m_mesh->m_source, m_group);
	return m_entity;
}

bool
Prop::attach(Scene *scene, Ogre::String name, Ogre::Vector3 pos)
{
	return attach(scene->rootNode(), name, pos);
}

bool
Prop::attach(Ogre::SceneManager *scene, Ogre::String name, Ogre::Vector3 pos)
{
	return attach(scene->getRootSceneNode(), name, pos);
}

bool
Prop::attach(Ogre::SceneNode *node, Ogre::String name, Ogre::Vector3 pos)
{
	Ogre::Entity *ent;
	Ogre::SceneManager *manager = node->getCreator();
	
	if(!name.length())
	{
		name = m_group;
	}
	if(m_node)
	{
		return m_node;
	}
	ent = entity(manager);
	if(!ent)
	{
		return false;
	}
	m_node = node->createChildSceneNode(name, pos);
	if(!m_node)
	{
		return false;
	}
	m_node->attachObject(ent);
	return true;
}


LoadableProp::LoadableProp(Loadable *owner, Ogre::String name, AttrList &attrs):
	LoadableObject(owner, name, attrs),
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




LoadableMesh::LoadableMesh(Loadable *owner, Ogre::String name, AttrList &attrs):
	LoadableObject(owner, name, attrs),
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

bool
LoadableMesh::addResources(Ogre::String group)
{
	Ogre::ResourceGroupManager::getSingleton().declareResource(m_source, "Mesh", group);
	return true;
}



LoadableMaterial::LoadableMaterial(Loadable *owner, Ogre::String name, AttrList &attrs):
	LoadableObject(owner, name, attrs),
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

bool
LoadableMaterial::addResources(Ogre::String group)
{
	Ogre::ResourceGroupManager::getSingleton().declareResource(m_source, "Material", group);
	return true;
}
