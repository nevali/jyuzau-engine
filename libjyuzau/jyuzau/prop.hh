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

#ifndef JYUZAU_PROP_HH_
# define JYUZAU_PROP_HH_               1

# include "jyuzau/loadable.hh"

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
	 */	
	class Prop: public Loadable, public btMotionState
	{
		friend class LoadableProp;
		friend class LoadablePropMesh;
		friend class LoadablePropMaterial;
		friend class LoadablePropPrefab;
	public:
		Prop(Ogre::String name, Ogre::String kind = "prop", State *state = NULL);
		virtual ~Prop();
	
		virtual Ogre::Entity *entity(Ogre::SceneManager *sceneManager, Ogre::String name = "");
		virtual Ogre::SceneNode *node(void);
	
		virtual bool attach(Scene *scene, Ogre::String name = "");
		virtual bool attach(Ogre::SceneManager *scene, Ogre::String name = "");
		virtual bool attach(Ogre::SceneNode *node, Ogre::String name = "");
		virtual bool attachPhysics(void);
		
		virtual void setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z);
		virtual void setPosition(const Ogre::Vector3 &vec);
		virtual void scale(const Ogre::Vector3 &vec);
		virtual void translate(const Ogre::Vector3 &vec);
		virtual void setOrientation(const Ogre::Quaternion &quaternion);
		
		virtual void setFixed(void);
		virtual void updatePhysics(void);

		virtual void getWorldTransform(btTransform &worldTrans) const;
		virtual void setWorldTransform(const btTransform &worldTrans);
	protected:
		bool m_attached;
		Ogre::String m_mesh;
		Ogre::String m_material;
		Ogre::SceneManager::PrefabType m_prefabType;
		Ogre::Entity *m_entity;
		Ogre::SceneNode *m_node;
		btCollisionShape *m_collisionShape;
		btVector3 m_inertia;
		btScalar m_mass;
		btRigidBody *m_rigidBody;
		
		virtual LoadableObject *factory(Ogre::String name, AttrList &attrs);
		virtual void loaded(void);
		
		virtual bool createNode(Ogre::SceneNode *parentNode, Ogre::String name);
		virtual bool createPhysics(btDynamicsWorld *dynamics);
	};
	
	class LoadableProp: public LoadableObject
	{
		friend class Prop;
	public:
		LoadableProp(Prop *owner, Ogre::String name, AttrList &attrs);
		virtual bool add(LoadableObject *child);
		virtual bool complete(void);
	protected:
		LoadablePropPrefab *m_prefab;
		LoadablePropMesh *m_mesh;
		LoadablePropMaterial *m_material;
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
