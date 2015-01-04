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

#include "jyuzau/node.hh"
#include "jyuzau/scene.hh"

#include <utility>

using namespace Jyuzau;

Node::Node(const Node &object):
	Loadable::Loadable(object),
	m_scene(NULL),
	m_node(NULL)
{
	m_position = object.m_position;
	m_scale = object.m_scale;
	m_orientation = object.m_orientation;
}

Node::Node(Ogre::String className, State *state, Ogre::String kind, bool subdir):
	Loadable::Loadable(className, state, kind, subdir),
	m_scene(NULL),
	m_node(NULL),
	m_position(0, 0, 0),
	m_scale(1, 1, 1),
	m_orientation()
{
}

Node::~Node()
{
	Ogre::Node *parentNode;
	
	if(m_node)
	{
		detach();
	}
}

Scene *
Node::scene(void) const
{
	return m_scene;
}

Ogre::SceneNode *
Node::sceneNode(void) const
{
	return m_node;
}

bool
Node::attachToScene(Scene *scene, Ogre::String id)
{
	if(m_scene)
	{
		detach();
	}
	if(!attachToSceneNode(scene, scene->rootNode(), id))
	{
		return false;
	}
	m_scene = scene;
	return true;
}

bool 
Node::attachToScene(Node *node, Ogre::String id)
{
	Scene *scene;
	Ogre::SceneNode *parentNode;
	
	scene = node->scene();
	if(!scene)
	{
		return false;
	}
	parentNode = node->sceneNode();
	if(!parentNode)
	{
		return false;
	}
	if(m_scene)
	{
		detach();
	}
	if(!attachToSceneNode(scene, parentNode, id))
	{
		return false;
	}
	m_scene = scene;
	return true;
}

void
Node::detach(void)
{
	Ogre::Node *parentNode;
	
	if(m_node)
	{
		parentNode = m_node->getParent();
		if(parentNode)
		{
			parentNode->removeChild(m_node);
		}
		delete m_node;
	}
	m_node = NULL;
	m_scene = NULL;
}

/* Attach the prop to a scene, creating the entity if necessary. Note that
 * the id supplied must be unique in the scene (and if the entity is being
 * created, a unique entity name, too). The position defaults to [0, 0, 0],
 * and the id defaults to the group name (kind::class).
 */
bool
Node::attachToSceneNode(Scene *scene, Ogre::SceneNode *parentNode, Ogre::String id)
{
	if(!id.length())
	{
		id = m_group;
	}
	if(!m_loaded)
	{
		if(!load())
		{
			return false;
		}
	}
	if(!m_load_status)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: cannot attach a " + m_kind + " " + m_className + "(" + id + ") which has not been properly loaded");
		return false;
	}
	if(m_node)
	{
		detach();
	}
	if(!createNode(parentNode, id))
	{
		return false;
	}
	return true;
}

/* Utility method invoked by attachToSceneNode() to create the node itself */
bool
Node::createNode(Ogre::SceneNode *parentNode, Ogre::String id)
{
	m_node = parentNode->createChildSceneNode(id);
	if(!m_node)
	{
		return false;
	}
	m_node->setPosition(m_position);
	m_node->setScale(m_scale);
	m_node->setOrientation(m_orientation);
	return true;
}

void
Node::scale(const Ogre::Vector3 &vec)
{
	if(m_node)
	{
		m_node->scale(vec);
	}
	else
	{
		m_scale *= vec;
	}
}

void 
Node::translate(const Ogre::Vector3 &vec)
{
	if(m_node)
	{
		m_node->translate(vec);
	}
	else
	{
		m_position += m_orientation * vec;
	}
}

void
Node::setOrientation(const Ogre::Quaternion &q)
{
	if(m_node)
	{
		m_node->setOrientation(q);
	}
	else
	{
		m_orientation = q;
	}
}

void
Node::setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z)
{
	if(m_node)
	{
		m_node->setPosition(x, y, z);
	}
	else
	{
		m_position = Ogre::Vector3(x, y, z);
	}
}

void
Node::setPosition(const Ogre::Vector3 &vec)
{
	if(m_node)
	{
		m_node->setPosition(vec);
	}
	else
	{
		m_position = vec;
	}
}

void
Node::yaw(const Ogre::Radian &angle)
{
	Ogre::Quaternion q;
	
	if(m_node)
	{
		m_node->yaw(angle);
	}
	else
	{
		q.FromAngleAxis(angle, Ogre::Vector3::UNIT_Y);
		q.normalise();
		m_orientation = m_orientation * q;
	}
}

void
Node::pitch(const Ogre::Radian &angle)
{
	Ogre::Quaternion q;
	
	if(m_node)
	{
		m_node->pitch(angle);
	}
	else
	{
		q.FromAngleAxis(angle, Ogre::Vector3::UNIT_X);
		q.normalise();
		m_orientation = m_orientation * q;
	}
}

void
Node::roll(const Ogre::Radian &angle)
{
	Ogre::Quaternion q;
	
	if(m_node)
	{
		m_node->roll(angle);
	}
	else
	{
		q.FromAngleAxis(angle, Ogre::Vector3::UNIT_Z);
		q.normalise();
		m_orientation = m_orientation * q;
	}
}
