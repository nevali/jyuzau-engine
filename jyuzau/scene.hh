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

#ifndef JYUZAU_SCENE_HH_
# define JYUZAU_SCENE_HH_              1

# include <utility>

# include <OGRE/OgreString.h>
# include <OGRE/OgreSceneManager.h>
# include <OGRE/OgreVector3.h>
# include <OGRE/OgreColourValue.h>

# include "jyuzau/loadable.hh"

namespace Jyuzau
{
	class Prop;
	class Light;
	class LoadableSceneProp;
	class LoadableSceneLight;
	class LoadableSceneAmbientLight;
	
	typedef std::pair<LoadableSceneProp *, Prop *> SceneProp;
	typedef std::pair<LoadableSceneLight *, Light *> SceneLight;
	
	class Scene: public Loadable
	{
	public:
		static Scene *create(Ogre::String name, Ogre::SceneManager *sceneManager = NULL);
		
		Scene(Ogre::String name);
		virtual ~Scene();
		
		bool attach(Ogre::SceneManager *sceneManager);
		bool detach(void);
		
		Ogre::SceneNode *rootNode(void);
		
		/* Add a Prop to the scene and take ownership of it */
		virtual void addProp(LoadableSceneProp *sceneProp, Prop *prop);
		virtual void addLight(LoadableSceneLight *sceneProp, Light *light);
		virtual void addAmbientLight(LoadableSceneAmbientLight *sceneAmblientLight);
	protected:
		Ogre::SceneManager *m_manager;
		std::vector<SceneProp> m_props;
		std::vector<SceneLight> m_lights;
		LoadableSceneAmbientLight *m_ambient;
		
		virtual LoadableObject *factory(Ogre::String name, AttrList &attrs);
	};
	
	class LoadableScene: public LoadableObject
	{
		friend class Scene;
	public:
		LoadableScene(Loadable *owner, Ogre::String name, AttrList &attrs);
	protected:
	};
	
	class LoadableSceneProp: public LoadableObject
	{
		friend class Scene;
	public:
		LoadableSceneProp(Scene *owner, Ogre::String name, AttrList &attrs);
		
		virtual Ogre::String id(void);
		virtual bool complete(void);
		virtual Ogre::Vector3 pos();
	protected:
		Ogre::String m_id, m_class;
		Ogre::Vector3 m_pos;

		virtual bool addResources(Ogre::String group);
	};
	
	class LoadableSceneActor: public LoadableSceneProp
	{
		friend class Scene;
	public:
		LoadableSceneActor(Scene *owner, Ogre::String name, AttrList &attrs);
	protected:
		virtual bool addResources(Ogre::String group);
	};
	
	class LoadableSceneAmbientLight: public LoadableObject
	{
		friend class Scene;
	public:
		LoadableSceneAmbientLight(Scene *owner, Ogre::String name, AttrList &attrs);
		virtual Ogre::ColourValue colorValue(void);
	protected:
		Ogre::ColourValue m_col;
		
		virtual bool addResources(Ogre::String group);
	};

	class LoadableSceneLight: public LoadableObject
	{
		friend class Scene;
	public:
		LoadableSceneLight(Scene *owner, Ogre::String name, AttrList &attrs);
		
		virtual Ogre::String id(void);
		virtual Ogre::Vector3 pos(void);
		virtual bool complete(void);
	protected:
		Ogre::String m_id;
		Ogre::Vector3 m_pos;
		
		virtual bool addResources(Ogre::String group);
	};

};

#endif /*!JYUZAU_SCENE_HH_*/
