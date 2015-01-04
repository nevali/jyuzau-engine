/* Copyright 2014-2015 Mo McRoberts.
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

Prop::Prop(const Prop &object):
	Node::Node(object),
	m_entity(NULL),
	m_collisionShape(NULL),
	m_rigidBody(NULL),
	m_prefabType(Ogre::SceneManager::PT_CUBE)
{
	m_mass = object.m_mass;
	m_inertia = object.m_inertia;
	m_mesh = object.m_mesh;
	m_material = object.m_material;
	m_prefabType = object.m_prefabType;
	m_restitution = object.m_restitution;
	m_friction = object.m_friction;
}

Prop::Prop(Ogre::String name, State *state, Ogre::String kind):
	Node::Node(name, state, kind, true),
	m_entity(NULL),
	m_mass(0),
	m_collisionShape(NULL),
	m_inertia(0.0f, 0.0f, 0.0f),
	m_rigidBody(NULL),
	m_mesh(""),
	m_material(""),
	m_prefabType(Ogre::SceneManager::PT_CUBE),
	m_restitution(0.25f),
	m_friction(0.5f)
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
		m_node = NULL;
	}
	if(m_entity)
	{
		delete m_entity;
	}
}

Loadable *
Prop::clone(void) const
{
	return new Prop(*this);
}

/* Define this object as physically immovable (used for walls, etc.) */
void
Prop::setFixed(bool isFixed)
{
	int flags;
	
	if(m_rigidBody)
	{
		flags = m_rigidBody->getCollisionFlags();
		if(isFixed)
		{
			m_rigidBody->setCollisionFlags(flags | btCollisionObject::CF_STATIC_OBJECT);
			m_rigidBody->setMassProps(0, btVector3(0, 0, 0));
			m_rigidBody->clearForces();
		}
		else
		{
			m_rigidBody->setCollisionFlags(flags & ~btCollisionObject::CF_STATIC_OBJECT);
			m_rigidBody->setMassProps(m_mass, btVector3(0, 0, 0));
			m_rigidBody->clearForces();
		}
	}
}

/* Attach the prop to a scene, creating the entity if necessary. Note that
 * the id supplied must be unique in the scene (and if the entity is being
 * created, a unique entity name, too). The position defaults to [0, 0, 0],
 * and the id defaults to the group name (kind::class).
 */
bool
Prop::attachToSceneNode(Scene *scene, Ogre::SceneNode *parentNode, Ogre::String id)
{
	btDynamicsWorld *dynamics;

	if(!Node::attachToSceneNode(scene, parentNode, id))
	{
		return false;
	}
	dynamics = scene->dynamics();
	if(dynamics)
	{
		if(!createPhysics(dynamics))
		{
			return false;
		}
		if(!attachPhysics(dynamics))
		{
			return false;
		}
	}
	return true;
}

/* Obtain an Ogre::Entity for this prop. If unspecified, the name will default
 * to the group name (class::name) of the prop. When invoked by attach(),
 * the node name supplied will be re-used as the entity name here unless the
 * the entity has already been created.
 */
Ogre::Entity *
Prop::createEntity(Ogre::SceneManager *sceneManager, Ogre::String id)
{
	LoadableProp *prop;
	
	/* If an entity already exists, return it */
	if(m_entity)
	{
		return m_entity;
	}
	if(m_mesh.length())
	{
		/* If the prop has a mesh, create the entity using that */
		m_entity = sceneManager->createEntity(id, m_mesh, m_group);
	}
	else
	{
		/* Otherwise, use the prefab type */
		m_entity = sceneManager->createEntity(id, m_prefabType);
	}
	if(m_material.length())
	{
		/* If there's a specific material to apply, do so */
		m_entity->setMaterialName(m_material, m_group);
	}
	return m_entity;
}

/* Utility method invoked by attachSceneNode() in order to create the scene
 * node.
 */
bool
Prop::createNode(Ogre::SceneNode *parentNode, Ogre::String id)
{
	if(!createEntity(parentNode->getCreator(), id))
	{
		return false;
	}
	if(!Node::createNode(parentNode, id))
	{
		return false;
	}
	m_node->attachObject(m_entity);
	return true;
}

/* Utility method invoked by attachSceneNode() in order to define the physical
 * properties of the prop.
 */
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
	ci.m_restitution = m_restitution;
	ci.m_friction = m_friction;
	m_rigidBody = new btRigidBody(ci);
	return true;
}

/* Utility method invoked by attachSceneNode() in order to attach the rigid
 * body to the dynamics world.
 */
bool
Prop::attachPhysics(btDynamicsWorld *dynamics)
{
	if(!m_rigidBody)
	{
		return false;
	}
	dynamics->addRigidBody(m_rigidBody);
	return true;
}

/* btMotionState interface */
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

/* Construct a new LoadableObject as the <prop> is being loaded */
LoadableObject *
Prop::factory(Ogre::String kind, AttrList &attrs)
{
	LoadableProp *prop;
	
	if(!m_root)
	{
		if(!kind.compare(m_kind))
		{
			return new LoadableProp(this, kind, attrs);
		}
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: unexpected root element <" + kind + ">");
		return NULL;
	}
	if(m_cur == m_root)
	{
		prop = dynamic_cast<LoadableProp *>(m_cur);
		if(!kind.compare("mesh"))
		{
			return new LoadablePropMesh(this, prop, kind, attrs);
		}
		if(!kind.compare("material"))
		{
			return new LoadablePropMaterial(this, prop, kind, attrs);
		}
		if(!kind.compare("cube") || !kind.compare("sphere") || !kind.compare("plane"))
		{
			return new LoadablePropPrefab(this, prop, kind, attrs);
		}
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: unexpected element <" + kind + ">");
		return NULL;
	}
	Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: unexpected child element <" + kind + ">");
	return NULL;
}

/* Inform the OGRE Resource Manager about the prop */
bool
Prop::addResources(Ogre::String groupName)
{
	Ogre::ResourceGroupManager *gm;
	
	gm = Ogre::ResourceGroupManager::getSingletonPtr();
	gm->addResourceLocation(m_container, "FileSystem", m_group);
	if(!Loadable::addResources(groupName))
	{
		return false;
	}
	gm->initialiseResourceGroup(m_group);
	return true;
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
		else if(!(*it).first.compare("restitution"))
		{
			owner->m_restitution = atof((*it).second.c_str());
		}
		else if(!(*it).first.compare("friction"))
		{
			owner->m_friction = atof((*it).second.c_str());
		}
	}
}

bool
LoadableProp::addChild(LoadableObject *child)
{
	LoadablePropMesh *mesh;
	LoadablePropMaterial *material;
	LoadablePropPrefab *prefab;
	
	if((mesh = dynamic_cast<LoadablePropMesh *>(child)))
	{
		m_mesh = mesh;
	}
	else if((material = dynamic_cast<LoadablePropMaterial *>(child)))
	{
		m_material = material;
	}
	else if((prefab = dynamic_cast<LoadablePropPrefab *>(child)))
	{
		m_prefab = prefab;
	}
	return LoadableObject::addChild(child);
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
