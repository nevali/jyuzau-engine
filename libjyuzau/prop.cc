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
#include "jyuzau/state.hh"

#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreLogManager.h>
#include <OGRE/OgreResourceGroupManager.h>

#include "p_utils.hh"

using namespace Jyuzau;

Prop::Prop(Ogre::String name, Ogre::String kind, State *state):
	Loadable::Loadable(name, kind, true, state),
	m_entity(NULL),
	m_node(NULL),
	m_mass(0),
	m_collisionShape(NULL),
	m_inertia(0.0f, 0.0f, 0.0f),
	m_rigidBody(NULL),
	m_mesh(""),
	m_material(""),
	m_prefabType(Ogre::SceneManager::PT_CUBE)
{
}

Prop::~Prop()
{
	if(m_rigidBody)
	{
		/* m_state->removeRigidBody(m_rigidBody); */
	}
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
 * created, a unique entity name, too). The position defaults to [0, 0, 0],
 * and the name defaults to the group name (class::name).
 */
bool
Prop::attach(Scene *scene, Ogre::String name)
{
	/* Attach the Prop to the root node of the Scene */
	return attach(scene->rootNode(), name);
}

bool
Prop::attach(Ogre::SceneManager *scene, Ogre::String name)
{
	/* Attach the Prop to the root node of the SceneManager */
	return attach(scene->getRootSceneNode(), name);
}

bool
Prop::attach(Ogre::SceneNode *node, Ogre::String name)
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
	if(!entity(manager, name))
	{
		return false;
	}
	if(!createNode(node, name))
	{
		return false;
	}
	return true;
}

bool
Prop::attachPhysics(void)
{
	btDynamicsWorld *dynamics;
	
	if(!m_state || !(dynamics = m_state->dynamics()))
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: cannot attach physics to a prop " + m_group + " which has no associated state with dynamics");
		return false;
	}
	if(!createPhysics(dynamics) || !m_rigidBody)
	{
		return false;
	}
	dynamics->addRigidBody(m_rigidBody);
	return true;
}

bool
Prop::createNode(Ogre::SceneNode *parentNode, Ogre::String name)
{
	m_node = parentNode->createChildSceneNode(name);
	if(!m_node)
	{
		return false;
	}
	m_node->attachObject(m_entity);
	return true;
}

bool
Prop::createPhysics(btDynamicsWorld *dynamics)
{
	btVector3 scale;
	
	scale = ogreVecToBullet(m_node->getScale());
	if(m_mesh.length())
	{
		/* ... */
	}
	else
	{
		switch(m_prefabType)
		{
			case Ogre::SceneManager::PT_CUBE:
				m_collisionShape = new btBoxShape(btVector3(50, 50, 50));
				break;
			case Ogre::SceneManager::PT_SPHERE:
				m_collisionShape = new btSphereShape(50);
				break;
			case Ogre::SceneManager::PT_PLANE:
				m_collisionShape = new btBoxShape(btVector3(100, 100, 0));
				break;
		}
	}
	if(!m_collisionShape)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: failed to create collision shape for " + m_group);
		return false;
	}
	m_collisionShape->setLocalScaling(scale);
	m_collisionShape->setUserPointer(this);
	m_collisionShape->calculateLocalInertia(m_mass, m_inertia);
	btRigidBody::btRigidBodyConstructionInfo ci(m_mass, this, m_collisionShape, m_inertia);
/*	ci.m_restitution = 1.0f;
	ci.m_friction = 0.5f; */
	m_rigidBody = new btRigidBody(ci);
	return true;
}

void
Prop::scale(const Ogre::Vector3 &vec)
{
	if(m_node)
	{
		m_node->scale(vec);
	}
}

void 
Prop::translate(const Ogre::Vector3 &vec)
{
	if(m_node)
	{
		m_node->translate(vec);
	}
}

void
Prop::setOrientation(const Ogre::Quaternion &quaternion)
{
	if(m_node)
	{
		m_node->setOrientation(quaternion);
	}
}


void
Prop::setFixed(void)
{
	if(m_rigidBody)
	{
		m_rigidBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
		m_rigidBody->setMassProps(0, btVector3(0, 0, 0));
		m_rigidBody->clearForces();
	}
}

void
Prop::getWorldTransform(btTransform &worldTrans) const
{
	Ogre::Quaternion rot;
	Ogre::Vector3 pos;
	
	if(!m_node)
	{
		return;
	}
	rot = m_node->getOrientation();
	pos = m_node->getPosition();
	worldTrans.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
	worldTrans.setOrigin(btVector3(pos.x, pos.y, pos.z));
}

void 
Prop::setWorldTransform(const btTransform &worldTrans)
{
	btQuaternion rot = worldTrans.getRotation();
	btVector3 pos = worldTrans.getOrigin();

	if(!m_node)
	{
		return;
	}
	m_node->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
	m_node->setPosition(pos.x(), pos.y(), pos.z());
}

void
Prop::updatePhysics(void)
{
	
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
	if(m_mesh.length())
	{
		m_entity = scene->createEntity(name, m_mesh, m_group);
	}
	else
	{
		m_entity = scene->createEntity(name, m_prefabType);
	}
	if(m_material.length())
	{
		m_entity->setMaterialName(m_material, m_group);
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
	AttrListIterator it;
	m_discardable = true;

	for(it = m_attrs.begin(); it != m_attrs.end(); it++)
	{
		if(!(*it).first.compare("mass"))
		{
			owner->m_mass = atof((*it).second.c_str());
		}
	}
}

bool
LoadableProp::add(LoadableObject *child)
{
	/* TODO: use dynamic_cast as the test mechanism instead of the names */
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

	m_discardable = true;
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
	dynamic_cast<Prop *>(m_owner)->m_mesh = m_source;
	return true;
}




/* A LoadablePropPrefab object encapsulates a prefabricated object within a
 * <prop>, one of <cube>, <sphere> or <plane>.
 */
LoadablePropPrefab::LoadablePropPrefab(Prop *owner, LoadableProp *parent, Ogre::String name, AttrList &attrs):
	LoadableObject(owner, parent, name, attrs),
	m_dimensions(1.0f, 1.0f, 1.0f)
{
	m_discardable = true;
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

bool
LoadablePropPrefab::addResources(Ogre::String group)
{
	dynamic_cast<Prop *>(m_owner)->m_prefabType = m_type;
	return true;
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
	
	m_discardable = true;
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
	/* XXX this should be moved to a check in Prop::Complete() conditional
	 * upon whether this is a prefab prop or not.
	 */
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
	dynamic_cast<Prop *>(m_owner)->m_material = m_class;
	return true;
}
