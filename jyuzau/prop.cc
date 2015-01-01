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

#include "jyuzau/prop.hh"
#include "jyuzau/scene.hh"

#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreLogManager.h>
#include <OGRE/OgreResourceGroupManager.h>

using namespace Jyuzau;

Prop::Prop(Ogre::String name, Ogre::String kind, State *state):
	Loadable::Loadable(name, kind, true, state),
	m_entity(NULL),
	m_node(NULL),
	m_mass(0.0f),
	m_collisionShape(NULL),
	m_motionState(NULL),
	m_rigidBody(NULL)
{
}

Prop::~Prop()
{
	if(m_rigidBody)
	{
		/* Scene->removeRigidBody(m_rigidbody); */
	}
	delete m_motionState;
	delete m_rigidBody;
	delete m_collisionShape;
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

Ogre::SceneNode *
Prop::node(void)
{
	return m_node;
}

/* Attach the prop to a scene, creating the entity if necessary. Note that
 * the name supplied must be unique in the scene (and if the entity is being
 * created, a unique entity name, too). The position defaults to [0, 0, 0]
 * if unspecified, and the name defaults to the group name (class::name).
 */
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
	ent = entity(manager, name);
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

/* Obtain an Ogre::Entity for this prop. If unspecified, the name will default
 * to the group name (class::name) of the prop. When invoked by attach(),
 * the node name supplied will be re-used as the entity name here unless the
 * the entity has already been created.
 */
Ogre::Entity *
Prop::entity(Ogre::SceneManager *scene, Ogre::String name)
{
	LoadableProp *prop;
	
	if(m_entity)
	{
		return m_entity;
	}
	if(!name.length())
	{
		name = m_group;
	}
	if(!m_loaded)
	{
		if(!load())
		{
			return NULL;
		}
	}
	if(!m_load_status)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: cannot attach a prop (" + name + ") which has not been properly loaded");
		return NULL;
	}
	prop = dynamic_cast<LoadableProp *>(m_root);
	if(prop->m_mesh)
	{
		m_entity = scene->createEntity(name, prop->m_mesh->m_source, m_group);
	}
	else
	{
		m_entity = scene->createEntity(name, prop->m_prefab->m_type);
	}
	if(prop->m_material)
	{
		m_entity->setMaterialName(prop->m_material->m_class, m_group);
	}
	return m_entity;
}

LoadableObject *
Prop::factory(Ogre::String name, AttrList &attrs)
{
	LoadableProp *prop;
	
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
	prop = dynamic_cast<LoadableProp *>(m_cur);
	if(!name.compare("mesh"))
	{
		return new LoadablePropMesh(this, prop, name, attrs);
	}
	if(!name.compare("material"))
	{
		return new LoadablePropMaterial(this, prop, name, attrs);
	}
	if(!name.compare("cube") || !name.compare("sphere") || !name.compare("plane"))
	{
		return new LoadablePropPrefab(this, prop, name, attrs);
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

void
Prop::setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z)
{
	if(m_node)
	{
		m_node->setPosition(x, y, z);
	}
}

void
Prop::setPosition(const Ogre::Vector3 &vec)
{
	if(m_node)
	{
		m_node->setPosition(vec);
	}
}




/* A LoadableProp object encapsulates the <prop> root XML element */

LoadableProp::LoadableProp(Prop *owner, Ogre::String name, AttrList &attrs):
	LoadableObject(owner, NULL, name, attrs),
	m_mesh(NULL),
	m_material(NULL),
	m_prefab(NULL)
{
}

bool
LoadableProp::add(LoadableObject *child)
{
	if(!child->name().compare("mesh"))
	{
		m_mesh = dynamic_cast<LoadablePropMesh *>(child);
	}
	else if(!child->name().compare("material"))
	{
		m_material = dynamic_cast<LoadablePropMaterial *>(child);
	}
	else if(!child->name().compare("cube") || !child->name().compare("sphere") || !child->name().compare("plane"))
	{
		m_prefab = dynamic_cast<LoadablePropPrefab *>(child);
	}
	return LoadableObject::add(child);
}

bool
LoadableProp::complete()
{
	if(!m_mesh && !m_prefab)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: prop is missing a mesh or pre-fabricated shape");
		return false;
	}
	if(m_prefab && !m_material)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: prefabricated prop is missing a material");
		return false;
	}
	return LoadableObject::complete();
}




/* A LoadablePropMesh object encapsulates a <mesh> within a <prop>.
 * <mesh src="foo.mesh" />
 */

LoadablePropMesh::LoadablePropMesh(Prop *owner, LoadableProp *parent, Ogre::String name, AttrList &attrs):
	LoadableObject(owner, parent, name, attrs),
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
LoadablePropMesh::complete(void)
{
	if(!m_source.length())
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: prop mesh is missing a source");
		return false;
	}
	return true;
}

bool
LoadablePropMesh::addResources(Ogre::String group)
{
	Ogre::ResourceGroupManager::getSingleton().declareResource(m_source, "Mesh", group);
	return true;
}




/* A LoadablePropPrefab object encapsulates a prefabricated object within a
 * <prop>, one of <cube>, <sphere> or <plane>.
 */
LoadablePropPrefab::LoadablePropPrefab(Prop *owner, LoadableProp *parent, Ogre::String name, AttrList &attrs):
	LoadableObject(owner, parent, name, attrs),
	m_dimensions(1.0f, 1.0f, 1.0f)
{
	if(!name.compare("cube"))
	{
		m_type =  Ogre::SceneManager::PT_CUBE;
	}
	else if(!name.compare("sphere"))
	{
		m_type =  Ogre::SceneManager::PT_SPHERE;
	}
	else
	{
		m_type =  Ogre::SceneManager::PT_PLANE;
	}
}




/* A LoadablePropMaterial object encapsulates a <material> within a <prop>.
 * <material src="../some/material/path.material" class="stipple" />
 *
 * Only useful for prefab props.
 */

LoadablePropMaterial::LoadablePropMaterial(Prop *owner, LoadableProp *parent, Ogre::String name, AttrList &attrs):
	LoadableObject(owner, parent, name, attrs),
	m_source(""),
	m_class("")
{
	AttrListIterator it;
	
	for(it = m_attrs.begin(); it != m_attrs.end(); it++)
	{
		Attr p = *it;
		
		if(!p.first.compare("src"))
		{
			m_source = p.second;
		}
		if(!p.first.compare("class"))
		{
			m_class = p.second;
		}
	}
}

bool
LoadablePropMaterial::complete(void)
{
	if(!m_class.length())
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: prop material is missing a class");
		return false;
	}
	return true;
}

bool
LoadablePropMaterial::addResources(Ogre::String group)
{
	if(m_source.length())
	{
		Ogre::ResourceGroupManager::getSingleton().declareResource(m_source, "Material", group);
	}
	return true;
}
