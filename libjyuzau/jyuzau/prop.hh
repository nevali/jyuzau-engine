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

#ifndef JYUZAU_PROP_HH_
# define JYUZAU_PROP_HH_               1

# include "jyuzau/node.hh"

# include <OGRE/OgreVector3.h>
# include <OGRE/OgreSceneManager.h>

# include <btBulletDynamicsCommon.h>

namespace Jyuzau
{
	class Scene;
	
	class LoadableProp;
	class LoadablePropMesh;
	class LoadablePropMaterial;
	class LoadablePropPrefab;
	
	/* A Prop is an object which can be attached to scenes. They have meshes
	 * and textures, and descendants (e.g., Actor) can include particular
	 * behaviours.
	 *
	 * Props have physical properties (mass, intertia, a collision shape, etc.)
	 */	
	class Prop: public Node, public btMotionState
	{
		friend class LoadableProp;
		friend class LoadablePropMesh;
		friend class LoadablePropMaterial;
		friend class LoadablePropPrefab;
	public:
		Prop(const Prop &object);
		Prop(Ogre::String className, State *state, Ogre::String kind = "prop");
		virtual ~Prop();
	
		virtual Loadable *clone(void) const;
	
		Ogre::Entity *entity(void) const;
		btRigidBody *rigidBody(void) const;
		virtual btScalar mass(void) const;
		virtual btVector3 inertia(void) const;
		
		virtual void setFixed(bool isFixed = false);

		/* btMotionState interface */
		virtual void getWorldTransform(btTransform &worldTrans) const;
		virtual void setWorldTransform(const btTransform &worldTrans);
	protected:
		bool m_attached;
		Ogre::String m_mesh;
		Ogre::String m_material;
		Ogre::SceneManager::PrefabType m_prefabType;
		Ogre::Entity *m_entity;
		btCollisionShape *m_collisionShape;
		btVector3 m_inertia;
		btScalar m_mass;
		btRigidBody *m_rigidBody;
		btScalar m_restitution;
		btScalar m_friction;
		
		virtual bool attachToSceneNode(Scene *scene, Ogre::SceneNode *parentNode, Ogre::String id);
		virtual Ogre::Entity *createEntity(Ogre::SceneManager *sceneManager, Ogre::String name = "");
		virtual bool createNode(Ogre::SceneNode *parentNode, Ogre::String name);
		virtual bool createPhysics(btDynamicsWorld *dynamics);
		virtual bool attachPhysics(btDynamicsWorld *dynamics);
		
		virtual LoadableObject *factory(Ogre::String name, AttrList &attrs);
		virtual bool addResources(Ogre::String groupName);
		
	};
	
	class LoadableProp: public LoadableObject
	{
		friend class Prop;
	public:
		LoadableProp(Prop *owner, Ogre::String name, AttrList &attrs);

		virtual bool complete(void);
	protected:
		LoadablePropPrefab *m_prefab;
		LoadablePropMesh *m_mesh;
		LoadablePropMaterial *m_material;
		
		virtual bool addChild(LoadableObject *child);
	};
	
	class LoadablePropMesh: public LoadableObject
	{
		friend class Prop;
		friend class LoadableProp;
	public:
		LoadablePropMesh(Prop *owner, LoadableProp *parent, Ogre::String name, AttrList &attrs);
		virtual bool complete(void);
	protected:
		Ogre::String m_source;

		virtual bool addResources(Ogre::String group);
	};
	
	class LoadablePropPrefab: public LoadableObject
	{
		friend class Prop;
		friend class LoadableProp;
	public:
		LoadablePropPrefab(Prop *owner, LoadableProp *parent, Ogre::String name, AttrList &attrs);
	protected:
		Ogre::SceneManager::PrefabType m_type;
		Ogre::Vector3 m_dimensions;
		
		virtual bool addResources(Ogre::String group);
	};

	class LoadablePropMaterial: public LoadableObject
	{
		friend class Prop;
		friend class LoadableProp;
	public:
		LoadablePropMaterial(Prop *owner, LoadableProp *parent, Ogre::String name, AttrList &attrs);
		virtual bool complete(void);
	protected:
		Ogre::String m_source;
		Ogre::String m_class;
		
		virtual bool addResources(Ogre::String group);
	};
	
};

#endif /*!JYUZAU_PROP_HH_*/
