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

#include "jyuzau/scene.hh"
#include "jyuzau/prop.hh"
#include "jyuzau/actor.hh"
#include "jyuzau/light.hh"
#include "jyuzau/state.hh"

#include <OGRE/OgreLogManager.h>

#include "p_utils.hh"

using namespace Jyuzau;

Scene::Scene(Ogre::String name, State *state):
	Loadable::Loadable(name, "scene", false, state),
	m_manager(NULL),
	m_objects(),
	m_ambient(NULL),
	m_broadphase(NULL),
	m_collisionConfig(NULL),
	m_dispatcher(NULL),
	m_dynamics(NULL),
	m_solver(NULL),
	m_gravity(0.0f, -1000.0f, 0.0f)
{
}


Scene::~Scene()
{
	delete m_dynamics;
	delete m_solver;
	delete m_dispatcher;
	delete m_collisionConfig;
	delete m_broadphase;
	
	if(m_manager)
	{
		detach();
	}
}

/* Attach the loaded objects to the scene manager */
bool
Scene::attach(Ogre::SceneManager *manager)
{
	std::vector<LoadableSceneObject *>::iterator it;

	if(!m_loaded)
	{
		if(!load())
		{
			return false;
		}
	}
	if(!m_load_status)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: cannot attach a scene which has not been properly loaded");
		return false;
	}
	if(!manager && m_state)
	{
		manager = m_state->sceneManager();
	}
	if(!manager)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: cannot attach a scene because no scene manager is available");
		return false;
	}
	if(m_manager)
	{
		detach();
	}
	m_manager = manager;
	if(!m_dynamics)
	{
		attachPhysics();
	}
	if(m_state)
	{
		m_state->sceneAttached(this);
	}
	/* Apply the ambient light to the scene */
	if(m_ambient)
	{
		manager->setAmbientLight(m_ambient->colorValue());
	}
	/* Attach all of the objects to the scene */
	for(it = m_objects.begin(); it != m_objects.end(); it++)
	{
		(*it)->attach(this);
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
	LoadableSceneObject *obj;
	
	if(!m_root)
	{
		if(!name.compare(m_kind))
		{
			return new LoadableScene(this, name, attrs);
		}
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: unexpected root element <" + name + ">");
		return NULL;
	}
	/* Props can have transforms applied to them */
	if((obj = dynamic_cast<LoadableSceneObject *>(m_cur)))
	{
		if(!name.compare("scale") || !name.compare("translate"))
		{
			return new LoadableSceneTransform(this, obj, name, attrs);
		}
		if(!name.compare("yaw") || !name.compare("pitch") || !name.compare("roll"))
		{
			return new LoadableSceneRotation(this, obj, name, attrs);
		}
	}
	if(m_cur != m_root)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: unexpected child element <" + name + ">");
		return NULL;
	}

	if(!name.compare("ambientlight"))
	{
		return new LoadableSceneAmbientLight(this, name, attrs);
	}
	
	obj = NULL;
	if(!name.compare("prop"))
	{
		return new LoadableSceneProp(this, obj, name, attrs);
	}
	if(!name.compare("actor"))
	{
		return new LoadableSceneActor(this, obj, name, attrs);
	}
	if(!name.compare("light"))
	{
		return new LoadableSceneLight(this, obj, name, attrs);
	}
	Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: unexpected element <" + name + ">");
	return NULL;
}

/* Add a LoadableSceneObject to the list of objects to be attached to the
 * scene, and take ownership of the actual loaded object.
 */
void
Scene::addSceneObject(LoadableSceneObject *sceneObject, Loadable *object)
{
	/* Take ownership of the object itself */
	add(object);
	/* Add the LoadableSceneObject to our object list (see the attach()
	 * method for this list's traversal)
	 */
	m_objects.push_back(sceneObject);
}

void
Scene::addAmbientLight(LoadableSceneAmbientLight *light)
{
	m_ambient = light;
}

/* Attach the physics engine to the scene */
void
Scene::attachPhysics(void)
{
	m_broadphase = new btDbvtBroadphase();
	m_collisionConfig = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfig);
	m_solver = new btSequentialImpulseConstraintSolver();
	m_dynamics = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfig);
	m_dynamics->setGravity(m_gravity);
}

btDynamicsWorld *
Scene::dynamics(void)
{
	return m_dynamics;
}

bool
Scene::setGravity(const Ogre::Vector3 &vec)
{
	if(!m_dynamics)
	{
		return false;
	}
	m_gravity = ogreVecToBullet(vec);
	m_dynamics->setGravity(m_gravity);
	return true;
}

bool
Scene::setGravity(const btVector3 &vec)
{
	if(!m_dynamics)
	{
		return false;
	}
	m_gravity = vec;
	m_dynamics->setGravity(m_gravity);
	return true;
}

bool
Scene::addRigidBody(btRigidBody *body)
{
	if(!m_dynamics)
	{
		return false;
	}
	m_dynamics->addRigidBody(body);
	return true;
}

bool
Scene::removeRigidBody(btRigidBody *body)
{
	if(!m_dynamics)
	{
		return false;
	}
	m_dynamics->removeRigidBody(body);
	return true;
}




/* LoadableScene encapsulates the <scene> root element */

LoadableScene::LoadableScene(Loadable *owner, Ogre::String name, AttrList &attrs):
	LoadableObject(owner, NULL, name, attrs)
{
}




/* LoadableSceneObject is the base class for any object which can be
 * loaded into a scene and has some kind of manifestation (including
 * both props and lights).
 */

LoadableSceneObject::LoadableSceneObject(Scene *owner, LoadableSceneObject *parent, Ogre::String name, AttrList &attrs):
	LoadableObject(owner, parent, name, attrs),
	m_id(""),
	m_scale(1, 1, 1),
	m_translate(0, 0, 0),
	m_yaw(0), m_pitch(0), m_roll(0)
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
	m_translate = parseXYZ(attrs);
}

bool
LoadableSceneObject::complete(void)
{
	if(!m_id.length())
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: scene object <" + m_name + "> is missing an ID");
		return false;
	}
	return LoadableObject::complete();
}

Ogre::String
LoadableSceneObject::id(void)
{
	return m_id;
}




/* LoadableSceneProp encapsulates a <prop> within a scene
 * <prop id="scene-local-name" class="asset-name" x="n" y="n" z="n">
 */

LoadableSceneProp::LoadableSceneProp(Scene *owner, LoadableSceneObject *parent, Ogre::String name, AttrList &attrs):
	LoadableSceneObject(owner, parent, name, attrs),
	m_class(""),
	m_fixed(false)
{
	AttrListIterator it;
	
	for(it = m_attrs.begin(); it != m_attrs.end(); it++)
	{
		Attr p = *it;
		
		if(!p.first.compare("class"))
		{
			m_class = p.second;
		}
		else if(!p.first.compare("fixed"))
		{
			if(!p.second.compare("yes"))
			{
				m_fixed = true;
			}
		}
	}
	if(!m_id.length())	
	{
		m_id = m_class;
	}
}

bool
LoadableSceneProp::complete(void)
{
	if(!m_class.length())
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: scene prop is missing a class");
		return false;
	}
	return LoadableSceneObject::complete();
}

bool
LoadableSceneProp::addResources(Ogre::String group)
{
	State *state;
	
	if((state = m_owner->state()))
	{
		m_prop = dynamic_cast<Prop *>(state->factory(m_name, m_class));
	}
	else
	{
		m_prop = new Prop(m_class, m_name);
	}
	if(!m_prop)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: failed to create scene prop instance");
		return false;
	}
	/* Allow the scene to take ownership of the prop */
	(dynamic_cast<Scene *> (m_owner))->addSceneObject(this, m_prop);
	return true;
}

bool
LoadableSceneProp::attach(Scene *scene)
{
	Ogre::SceneNode *node;
	Ogre::Matrix3 mat;
	
	/* TODO: attach to parent node if there is one */
	if(!m_prop->attach(scene, m_id))
	{
		return false;
	}
	m_prop->scale(m_scale);
	mat.FromEulerAnglesXYZ(m_yaw, m_pitch, m_roll);
	m_prop->setOrientation(Ogre::Quaternion(mat));
	m_prop->translate(m_translate);
	m_prop->attachPhysics();
	if(m_fixed)
	{
		m_prop->setFixed();
	}
	return true;
}



/* LoadableSceneActor encapsulates an <actor> within a scene, and is
 * simply a specialisation of LoadableSceneProp
 */

LoadableSceneActor::LoadableSceneActor(Scene *owner, LoadableSceneObject *parent, Ogre::String name, AttrList &attrs):
	LoadableSceneProp(owner, parent, name, attrs)
{
}

bool
LoadableSceneActor::addResources(Ogre::String group)
{
	State *state;
	
	if((state = m_owner->state()))
	{
		m_prop = dynamic_cast<Actor *>(state->factory(m_name, m_class));
	}
	else
	{
		m_prop = new Actor(m_class);
	}
	if(!m_prop)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: failed to create scene actor instance");
		return false;
	}
	(dynamic_cast<Scene *> (m_owner))->addSceneObject(this, m_prop);
	return true;
}




/* LoadableSceneAmbientLight encapsulates an <ambientlight> within a <scene>.
 * <ambientlight r="n" g="n" b="n" a="n" />
 */

LoadableSceneAmbientLight::LoadableSceneAmbientLight(Scene *owner,  Ogre::String name, AttrList &attrs):
	LoadableObject(owner, NULL, name, attrs)
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

LoadableSceneLight::LoadableSceneLight(Scene *owner, LoadableSceneObject *parent, Ogre::String name, AttrList &attrs):
	LoadableSceneObject(owner, parent, name, attrs)
{
}

bool
LoadableSceneLight::addResources(Ogre::String group)
{
	State *state;
	
	if((state = m_owner->state()))
	{
		m_light = dynamic_cast<Light *>(state->factory(m_name, m_id));
	}
	else
	{
		m_light = new Light(m_id);
	}
	if(!m_light)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: failed to create scene light instance");
		return false;
	}
	(dynamic_cast<Scene *> (m_owner))->addSceneObject(this, m_light);
	return true;
}

bool
LoadableSceneLight::attach(Scene *scene)
{
	Ogre::Light *node;
	
	/* TODO: attach to parent node if there is one */
	if(!m_light->attach(scene, m_id))
	{
		return false;
	}
	node = m_light->node();
/*	node->scale(m_scale); */
/*	node->rotate(m_rotate); */
	node->setPosition(m_translate);
	return true;
}




/* LoadableSceneTransform encapsulates a <scale>, <translate> or
 * <rotate> within a scene object; the x, y and z attributes are parsed
 * and then applied to the appropriate properties of the parent.
 */
LoadableSceneTransform::LoadableSceneTransform(Scene *owner, LoadableSceneObject *parent, Ogre::String name, AttrList &attrs):
	LoadableObject(owner, parent, name, attrs)
{
	m_vector = parseXYZ(attrs);
	if(!name.compare("scale"))
	{
		parent->m_scale = m_vector;
	}
	else if(!name.compare("translate"))
	{
		parent->m_translate = m_vector;
	}
}




/* LoadableSceneRotate encapsulates a <yaw>, <pitch> or <roll> within a 
 * scene object; the rad or deg attributes specify radians or degrees
 * respectively.
 */
LoadableSceneRotation::LoadableSceneRotation(Scene *owner, LoadableSceneObject *parent, Ogre::String name, AttrList &attrs):
	LoadableObject(owner, parent, name, attrs),
	m_angle()
{
	AttrListIterator it;
	
	for(it = m_attrs.begin(); it != m_attrs.end(); it++)
	{
		Attr p = *it;
		
		if(!p.first.compare("rad"))
		{
			m_angle = Ogre::Radian(std::atof(p.second.c_str()));
		}
		else if(!p.first.compare("deg"))
		{
			m_angle = Ogre::Degree(std::atof(p.second.c_str()));
		}
	}
	if(!name.compare("yaw"))
	{
		parent->m_yaw = m_angle;
	}
	else if(!name.compare("pitch"))
	{
		parent->m_pitch = m_angle;
	}
	else if(!name.compare("roll"))
	{
		parent->m_roll = m_angle;
	}
}
