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

#include "jyuzau/light.hh"
#include "jyuzau/scene.hh"

using namespace Jyuzau;

Light *
Light::create(Ogre::String name, Scene *scene, Ogre::Vector3 pos)
{
	Light *p;
	
	p = new Light(name);
	if(!p->load())
	{
		delete p;
		return NULL;
	}
	if(scene)
	{
		if(!p->attach(scene, name, pos))
		{
			delete p;
			return NULL;
		}
	}
	return p;
}

Light::Light(Ogre::String name, Ogre::String kind):
	Loadable::Loadable(name, kind, false),
	m_light(NULL)
{
}

Light::~Light()
{
	if(m_light)
	{
		delete m_light;
	}
}

Ogre::Light *
Light::node(void)
{
	return m_light;
}

bool
Light::attach(Scene *scene, Ogre::String name, Ogre::Vector3 pos)
{
	return attach(scene->rootNode()->getCreator(), name, pos);
}

bool
Light::attach(Ogre::SceneManager *manager, Ogre::String name, Ogre::Vector3 pos)
{
	if(m_light)
	{
		return true;
	}
	if(!name.length())
	{
		name = m_group;
	}
	m_light = manager->createLight(name);
	if(!m_light)
	{
		return false;
	}
	m_light->setPosition(pos);
	return true;
}

bool
Light::detach(void)
{
	if(m_light)
	{
		delete m_light;
		m_light = NULL;
	}
	return true;
}


/* Lights are created entirely in-line, so don't actually need to load anything
 * from anywhere.
 */
bool
Light::load(void)
{
	if(m_loaded)
	{
		return m_load_status;
	}
	m_loaded = true;
	m_load_status = false;
	if(m_root)
	{
		if(!m_root->complete())
		{
			Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: " + m_name + "[" + m_kind + "] from " + m_path + " has an incomplete definition");
			return false;
		}
	}
	m_load_status = true;
	loaded();
	return true;
}

bool
Light::loadDocument(Ogre::String path)
{
	(void) path;
	
	return true;
}
