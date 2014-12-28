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

Scene *
Scene::create(Ogre::String name, Ogre::SceneManager *sceneManager)
{
	Scene *p;
	
	p = new Scene(name);
	if(!p->load())
	{
		delete p;
		return NULL;
	}
	if(sceneManager)
	{
		if(!p->attach(sceneManager))
		{
			delete p;
			return NULL;
		}
	}
	return p;
}

Scene::Scene(Ogre::String name):
	Loadable::Loadable(name, "scene", false),
	m_manager(NULL),
	m_props()
{
}

Scene::~Scene()
{
	if(m_manager)
	{
		detach();
	}
}

bool
Scene::attach(Ogre::SceneManager *manager)
{
	std::vector<SceneProp>::iterator it;

	if(!m_loaded || !m_load_status)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: cannot attach a scene which has not been properly loaded");
	}
	if(m_manager)
	{
		detach();
	}
	m_manager = manager;
	
	/* Attach all of the props to the scene */
	for(it = m_props.begin(); it != m_props.end(); it++)
	{
		(*it).second->attach(this, (*it).first->id(), (*it).first->pos());
	}
	return true;
}

bool
Scene::detach(void)
{
	m_manager = NULL;
	return true;
}

Ogre::SceneNode *
Scene::rootNode(void)
{
	if(!m_manager)
	{
		return NULL;
	}
	return m_manager->getRootSceneNode();
}

LoadableObject *
Scene::factory(Ogre::String name, AttrList &attrs)
{
	if(!m_root)
	{
		if(!name.compare(m_kind))
		{
			return new LoadableScene(this, name, attrs);
		}
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: unexpected root element <" + name + ">");
		return NULL;
	}
	if(m_cur != m_root)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: unexpected child element <" + name + ">");
		return NULL;
	}
	if(!name.compare("prop"))
	{
		return new LoadableSceneProp(this, name, attrs);
	}
	if(!name.compare("actor"))
	{
		return new LoadableSceneActor(this, name, attrs);
	}
	Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: unexpected element <" + name + ">");
	return NULL;
}

void
Scene::addProp(LoadableSceneProp *sceneProp, Prop *prop)
{
	add(prop);
	m_props.push_back(std::make_pair(sceneProp, prop));
}




/* LoadableScene encapsulates the <scene> root element */

LoadableScene::LoadableScene(Loadable *owner, Ogre::String name, AttrList &attrs):
	LoadableObject(owner, name, attrs)
{
	Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: loading a scene");
}




/* LoadableSceneProp encapsulates a <prop> within a scene
 * <prop id="scene-local-name" class="asset-name" x="n" y="n" z="n">
 */

LoadableSceneProp::LoadableSceneProp(Loadable *owner, Ogre::String name, AttrList &attrs):
	LoadableObject(owner, name, attrs),
	m_id(""),
	m_class(""),
	m_pos()
{
	AttrListIterator it;
	double x = 0, y = 0, z = 0;
	
	for(it = m_attrs.begin(); it != m_attrs.end(); it++)
	{
		Attr p = *it;
		
		if(!p.first.compare("id"))
		{
			m_id = p.second;
		}
		else if(!p.first.compare("class"))
		{
			m_class = p.second;
		}
		else if(!p.first.compare("x"))
		{
			x = atof(p.second.c_str());
		}
		else if(!p.first.compare("y"))
		{
			y = atof(p.second.c_str());
		}
		else if(!p.first.compare("z"))
		{
			z = atof(p.second.c_str());
		}
	}
	m_pos = Ogre::Vector3(x, y, z);
	if(!m_id.length())	
	{
		m_id = m_class;
	}
}

bool
LoadableSceneProp::complete(void)
{
	if(!m_id.length())
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: scene prop is missing an ID");
		return false;
	}
	if(!m_class.length())
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: scene prop is missing a class");
		return false;
	}
	return LoadableObject::complete();
}

Ogre::String
LoadableSceneProp::id(void)
{
	return m_id;
}

Ogre::Vector3
LoadableSceneProp::pos(void)
{
	return m_pos;
}




/* LoadableSceneActor encapsulates an <actor> within a scene, and is
 * simply a specialisation of LoadableSceneProp
 */

LoadableSceneActor::LoadableSceneActor(Loadable *owner, Ogre::String name, AttrList &attrs):
	LoadableSceneProp(owner, name, attrs)
{
}

bool
LoadableSceneActor::addResources(Ogre::String group)
{
	Prop *prop;
	
	prop = Actor::create(m_class);
	if(!prop)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: failed to create scene actor instance");
		return false;
	}
	(dynamic_cast<Scene *> (m_owner))->addProp(this, prop);
	return true;
}
