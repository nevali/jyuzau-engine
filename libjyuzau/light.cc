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

Light::Light(const Light &object):
	Node::Node(object)
{
}

Light::Light(Ogre::String name, State *state, Ogre::String kind):
	Node::Node(name, state, kind, false),
	m_light(NULL)
{
}

Light::~Light()
{
	if(m_light)
	{
		detach();
	}
}

Loadable *
Light::clone(void) const
{
	return new Light(*this);
}

Ogre::Light *
Light::light(void) const
{
	return m_light;
}

bool
Light::attachToSceneNode(Scene *scene, Ogre::SceneNode *parentNode, Ogre::String id)
{
	Ogre::SceneManager *manager;
	
	manager = scene->sceneManager();
	m_light = manager->createLight(id);
	if(!m_light)
	{
		return false;
	}
	parentNode->attachObject(m_light);
	return true;
}

void
Light::detach(void)
{
	if(m_light)
	{
		m_light->detachFromParent();
		delete m_light;
		m_light = NULL;
	}
}


/* Lights are created entirely in-line, so don't actually need to load anything
 * from anywhere.
 */
bool
Light::loadDocument(Ogre::String path)
{
	(void) path;
	
	return true;
}

bool
Light::complete(void) const
{
	return true;
}

