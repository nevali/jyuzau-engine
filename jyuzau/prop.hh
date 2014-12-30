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

# include <OGRE/OgreVector3.h>

# include "jyuzau/loadable.hh"

namespace Jyuzau
{
	class Scene;
	
	class LoadablePropMesh;
	class LoadablePropMaterial;
	
	/* A Prop is an object which can be attached to scenes. They have meshes
	 * and textures, and descendants (e.g., Actor) can include particular
	 * behaviours.
	 */	
	class Prop: public Loadable
	{
	public:
		static Prop *create(Ogre::String name, Scene *scene = NULL, Ogre::Vector3 pos = Ogre::Vector3::ZERO);
	
		Prop(Ogre::String name, Ogre::String kind = "prop");
		virtual ~Prop();
	
		virtual Ogre::Entity *entity(Ogre::SceneManager *sceneManager, Ogre::String name = "");
	
		virtual bool attach(Scene *scene, Ogre::String name = "", Ogre::Vector3 pos = Ogre::Vector3::ZERO);
		virtual bool attach(Ogre::SceneManager *scene, Ogre::String name = "", Ogre::Vector3 pos = Ogre::Vector3::ZERO);
		virtual bool attach(Ogre::SceneNode *node, Ogre::String name = "", Ogre::Vector3 pos = Ogre::Vector3::ZERO);
		
		virtual void setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z);
		virtual void setPosition(const Ogre::Vector3 &vec);
	protected:
		bool m_attached;
		Ogre::Entity *m_entity;
		Ogre::SceneNode *m_node;
	
		virtual LoadableObject *factory(Ogre::String name, AttrList &attrs);
		virtual void loaded(void);
	};
	
	class LoadableProp: public LoadableObject
	{
		friend class Prop;
	public:
		LoadableProp(Prop *owner, Ogre::String name, AttrList &attrs);
		virtual bool add(LoadableObject *child);
		virtual bool complete(void);
	protected:
		LoadablePropMesh *m_mesh;
		LoadablePropMaterial *m_material;
	};
	
	class LoadablePropMesh: public LoadableObject
	{
		friend class Prop;
		friend class LoadableProp;
	public:
		LoadablePropMesh(Prop *owner, Ogre::String name, AttrList &attrs);
		virtual bool complete(void);
	protected:
		Ogre::String m_source;

		virtual bool addResources(Ogre::String group);
	};

	class LoadablePropMaterial: public LoadableObject
	{
		friend class Prop;
		friend class LoadableProp;
	public:
		LoadablePropMaterial(Prop *owner, Ogre::String name, AttrList &attrs);
		virtual bool complete(void);
	protected:
		Ogre::String m_source;

		virtual bool addResources(Ogre::String group);
	};
};

#endif /*!JYUZAU_PROP_HH_*/
