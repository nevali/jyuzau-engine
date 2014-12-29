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

#include <OgreLogManager.h>

#include "jyuzau/scene.hh"
#include "jyuzau/prop.hh"
#include "jyuzau/actor.hh"
#include "jyuzau/light.hh"

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
	m_props(),
	m_lights(),
	m_ambient(NULL)
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
	std::vector<SceneProp>::iterator pit;
	std::vector<SceneLight>::iterator lit;

	if(!m_loaded || !m_load_status)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: cannot attach a scene which has not been properly loaded");
	}
	if(m_manager)
	{
		detach();
	}
	m_manager = manager;
	/* Apply the ambient light to the scene */
	if(m_ambient)
	{
		manager->setAmbientLight(m_ambient->colorValue());
	}
	/* Attach all of the props to the scene */
	for(pit = m_props.begin(); pit != m_props.end(); pit++)
	{
		(*pit).second->attach(this, (*pit).first->id(), (*pit).first->pos());
	}
	/* Attach all of the lights to the scene */
	for(lit = m_lights.begin(); lit != m_lights.end(); lit++)
	{
		(*lit).second->attach(this, (*lit).first->id(), (*lit).first->pos());
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
	if(!name.compare("ambientlight"))
	{
		return new LoadableSceneAmbientLight(this, name, attrs);
	}
	if(!name.compare("light"))
	{
		return new LoadableSceneLight(this, name, attrs);
	}
	Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: unexpected element <" + name + ">");
	return NULL;
}

void
Scene::addProp(LoadableSceneProp *sceneProp, Prop *prop)
{
	/* Take ownership of the prop itself */
	add(prop);
	/* Add a pair so that we can create the prop with the correct properties */
	m_props.push_back(std::make_pair(sceneProp, prop));
}

void
Scene::addLight(LoadableSceneLight *sceneLight, Light *light)
{
	/* Take ownership of the light itself */
	add(light);
	/* Add a pair so that we can create the light with the correct properties */
	m_lights.push_back(std::make_pair(sceneLight, light));
}

void
Scene::addAmbientLight(LoadableSceneAmbientLight *light)
{
	m_ambient = light;
}




/* LoadableScene encapsulates the <scene> root element */

LoadableScene::LoadableScene(Loadable *owner, Ogre::String name, AttrList &attrs):
	LoadableObject(owner, name, attrs)
{
}




/* LoadableSceneProp encapsulates a <prop> within a scene
 * <prop id="scene-local-name" class="asset-name" x="n" y="n" z="n">
 */

LoadableSceneProp::LoadableSceneProp(Scene *owner, Ogre::String name, AttrList &attrs):
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
	}
	m_pos = parseXYZ(attrs);
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

bool
LoadableSceneProp::addResources(Ogre::String group)
{
	Prop *prop;
	
	prop = Prop::create(m_class);
	if(!prop)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: failed to create scene prop instance");
		return false;
	}
	(dynamic_cast<Scene *> (m_owner))->addProp(this, prop);
	return true;
}



/* LoadableSceneActor encapsulates an <actor> within a scene, and is
 * simply a specialisation of LoadableSceneProp
 */

LoadableSceneActor::LoadableSceneActor(Scene *owner, Ogre::String name, AttrList &attrs):
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




/* LoadableSceneAmbientLight encapsulates an <ambientlight> within a <scene>.
 * <ambientlight r="n" g="n" b="n" a="n" />
 */
LoadableSceneAmbientLight::LoadableSceneAmbientLight(Scene *owner, Ogre::String name, AttrList &attrs):
	LoadableObject(owner, name, attrs)
{
	m_col = parseColourValue(attrs);
}

Ogre::ColourValue
LoadableSceneAmbientLight::colorValue(void)
{
	return m_col;
}

bool
LoadableSceneAmbientLight::addResources(Ogre::String group)
{
	(dynamic_cast<Scene *> (m_owner))->addAmbientLight(this);
	return true;
}

/* LoadableSceneLight encapsulates a <light> within a <scene>
 * <light id="scene-local-name" x="n" y="n" z="n">
 */

LoadableSceneLight::LoadableSceneLight(Scene *owner, Ogre::String name, AttrList &attrs):
	LoadableObject(owner, name, attrs),
	m_id(""),
	m_pos()
{
	AttrListIterator it;
	
	for(it = m_attrs.begin(); it != m_attrs.end(); it++)
	{
		Attr p = *it;
		
		if(!p.first.compare("id"))
		{
			m_id = p.second;
		}
	}
	m_pos = parseXYZ(attrs);
}

Ogre::String
LoadableSceneLight::id(void)
{
	return m_id;
}

Ogre::Vector3
LoadableSceneLight::pos(void)
{
	return m_pos;
}

bool
LoadableSceneLight::complete(void)
{
	if(!m_id.length())
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: light is missing an ID");
		return false;
	}
	return LoadableObject::complete();
}

bool
LoadableSceneLight::addResources(Ogre::String group)
{
	Light *light;
	
	light = Light::create(m_id);
	if(!light)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: failed to create scene light instance");
		return false;
	}
	(dynamic_cast<Scene *> (m_owner))->addLight(this, light);
	return true;
}

