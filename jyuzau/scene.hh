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

# include "loadable.hh"

namespace Jyuzau
{
	class Prop;
	class LoadableSceneProp;
	
	typedef std::pair<LoadableSceneProp *, Prop *> SceneProp;
	
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
	protected:
		Ogre::SceneManager *m_manager;
		std::vector<SceneProp> m_props;
		
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
		LoadableSceneProp(Loadable *owner, Ogre::String name, AttrList &attrs);
		
		virtual Ogre::String id(void);
		virtual bool complete(void);
		virtual Ogre::Vector3 pos();
	protected:
		Ogre::String m_id, m_class;
		Ogre::Vector3 m_pos;
	};
	
	class LoadableSceneActor: public LoadableSceneProp
	{
		friend class Scene;
	public:
		LoadableSceneActor(Loadable *owner, Ogre::String name, AttrList &attrs);
	protected:
		virtual bool addResources(Ogre::String group);
	};
};

#endif /*!JYUZAU_SCENE_HH_*/
