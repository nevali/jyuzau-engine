/* Copyright 2014-2015 Mo McRoberts.
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

Scene::Scene(const Scene &scene):
	Loadable::Loadable(scene),
	m_manager(NULL),
	m_broadphase(NULL),
	m_collisionConfig(NULL),
	m_dispatcher(NULL),
	m_dynamics(NULL),
	m_solver(NULL),
	m_gravity(scene.m_gravity),
	m_hasAmbientLight(scene.m_hasAmbientLight),
	m_ambientColour(scene.m_ambientColour)
{
}

Scene::Scene(Ogre::String className, State *state):
	Loadable::Loadable(className, state, "scene", false),
	m_manager(NULL),
	m_broadphase(NULL),
	m_collisionConfig(NULL),
	m_dispatcher(NULL),
	m_dynamics(NULL),
	m_solver(NULL),
	m_gravity(0.0f, 0.0f, 0.0f),
	m_hasAmbientLight(false),
	m_ambientColour(0.5f, 0.5f, 0.5f, 1.0f)
{
}

Scene::~Scene()
{
	if(m_manager)
	{
		detach();
	}
	delete m_dynamics;
	delete m_solver;
	delete m_dispatcher;
	delete m_collisionConfig;
	delete m_broadphase;
}

Loadable *
Scene::clone(void)
{
	return new Scene(*this);
}

Ogre::SceneManager *
Scene::sceneManager(void) const
{
	return m_manager;
}

Ogre::SceneNode *
Scene::rootNode(void) const
{
	if(!m_manager)
	{
		return NULL;
	}
	return m_manager->getRootSceneNode();
}

btDynamicsWorld *
Scene::dynamics(void) const
{
	return m_dynamics;
}

btBroadphaseInterface *
Scene::broadphase(void) const
{
	return m_broadphase;
}


Ogre::Vector3
Scene::gravity(void) const
{
	return bulletVecToOgre(m_gravity);
}

bool
Scene::setGravity(const Ogre::Vector3 &vec)
{
	m_gravity = ogreVecToBullet(vec);
	if(!m_dynamics)
	{
		return true;
	}
	m_dynamics->setGravity(m_gravity);
	return true;
}

bool
Scene::setGravity(const btVector3 &vec)
{
	m_gravity = vec;
	if(!m_dynamics)
	{
		return true;
	}
	m_dynamics->setGravity(m_gravity);
	return true;
}

/* Attach the loaded objects to the State's scene manager */
bool
Scene::attach(void)
{
	std::vector<LoadableSceneObject *>::iterator it;
	Ogre::SceneManager *manager;
	
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
	manager = m_state->sceneManager();
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
	/* Apply scene properties */
	if(m_hasAmbientLight)
	{
		manager->setAmbientLight(m_ambientColour);
	}
	/* Attach all of the objects to the scene */
	m_root->attach();
	/* Inform the State that this scene has been attached */
	m_state->sceneAttached(this);
	return true;
}

bool
Scene::detach(void)
{
	std::vector<Loadable *>::iterator it;

	m_state->sceneDetached(this);
	/* Delete the nodes in the scene that we own */
	for(it = m_objects.begin(); it != m_objects.end(); it++)
	{
		delete (*it);
	}
	m_objects.clear();
	m_manager = NULL;
	return true;
}

bool
Scene::load(void)
{
	if(!Loadable::load())
	{
		return false;
	}
	if(!m_dynamics)
	{
		createPhysics();
	}
	return true;
}

LoadableObject *
Scene::factory(Ogre::String kind, AttrList &attrs)
{
	LoadableSceneObject *obj;
	
	if(!m_root)
	{
		if(!kind.compare(m_kind))
		{
			return new LoadableScene(this, kind, attrs);
		}
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: unexpected root element <" + kind + ">");
		return NULL;
	}
	if(m_cur == m_root)
	{
		/* Scene properties */
		if(!kind.compare("ambientlight"))
		{
			return new LoadableSceneAmbientLight(this, kind, attrs);
		}
		if(!kind.compare("gravity"))
		{
			return new LoadableSceneGravity(this, kind, attrs);
		}
	}
	/* Props can have transforms applied to them */
	if((obj = dynamic_cast<LoadableSceneObject *>(m_cur)))
	{
		if(!kind.compare("scale") || !kind.compare("translate"))
		{
			return new LoadableSceneTransform(this, obj, kind, attrs);
		}
		if(!kind.compare("yaw") || !kind.compare("pitch") || !kind.compare("roll"))
		{
			return new LoadableSceneRotation(this, obj, kind, attrs);
		}
	}
	if((m_cur == m_root) || obj)
	{
		/* Scene objects which can occur either at the root, or within
		 * another scene object.
		 */
		if(!kind.compare("prop"))
		{
			return new LoadableSceneProp(this, obj, kind, attrs);
		}
		if(!kind.compare("actor"))
		{
			return new LoadableSceneActor(this, obj, kind, attrs);
		}
		if(!kind.compare("light"))
		{
			return new LoadableSceneLight(this, obj, kind, attrs);
		}
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: unexpected child element <" + kind + ">");
		return NULL;
	}
	Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: unexpected element <" + kind + ">");
	return NULL;
}

/* Create the physics engine for the scene */
void
Scene::createPhysics(void)
{
	m_broadphase = new btDbvtBroadphase();
	m_collisionConfig = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfig);
	m_solver = new btSequentialImpulseConstraintSolver();
	m_dynamics = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfig);
	m_dynamics->setGravity(m_gravity);
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

LoadableScene::LoadableScene(const LoadableScene &object):
	LoadableObject::LoadableObject(object)
{
}

LoadableScene::LoadableScene(Loadable *owner, Ogre::String name, AttrList &attrs):
	LoadableObject(owner, NULL, name, attrs)
{
}

LoadableObject *
LoadableScene::clone(void) const
{
	return new LoadableScene(*this);
}



/* LoadableSceneProperty is the base class for encapsulating any
 * XML node which does nothing more than parse property values which
 * apply to its parent or the scene. By nature, all LoadableSceneProperty
 * instances are discardable.
 */
LoadableSceneProperty::LoadableSceneProperty(Scene *owner, LoadableSceneObject *parent, Ogre::String name, AttrList &attrs):
	LoadableObject(owner, parent, name, attrs)
{
	m_discardable = true;
}




/* LoadableSceneObject is the base class for any object which can be
 * loaded into a scene and has some kind of manifestation (including
 * both props and lights).
 */

LoadableSceneObject::LoadableSceneObject(const LoadableSceneObject &object):
	LoadableObject(object)
{
	m_id = object.m_id;
	m_scale = object.m_scale;
	m_translate = object.m_translate;
	m_yaw = object.m_yaw;
	m_pitch = object.m_pitch;
	m_roll = object.m_roll;
}

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
LoadableSceneObject::complete(void) const
{
	if(!m_id.length())
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: scene object <" + m_kind + "> is missing an ID");
		return false;
	}
	return LoadableObject::complete();
}

Ogre::String
LoadableSceneObject::id(void) const
{
	return m_id;
}

Node *
LoadableSceneObject::node(void) const
{
	return m_node;
}

/* Invoked by Scene::attach() in order to ask the object (and its children) to
 * instantiate itself and add it to the scene.
 */
bool
LoadableSceneObject::attach(void)
{
	LoadableSceneObject *parent;
	Node *parentNode, *node;
	State *state;
	Scene *scene;

	state = m_owner->state();
	node = createNode(state);
	if(!node)
	{
		return false;
	}
	scene = dynamic_cast<Scene *>(m_owner);
	parent = dynamic_cast<LoadableSceneObject *>(m_parent);
	if(parent)
	{
		parentNode = parent->node();
	}
	else
	{
		parentNode = NULL;
	}
	if(!applyProperties(node))
	{
		return false;
	}
	if(!attachNode(scene, parentNode, node))
	{
		return false;
	}
	/* Give ownership of the new Node to the scene */
	scene->addObject(node);
	m_node = node;
	/* Now attach any children */
	return LoadableObject::attach();
}

bool
LoadableSceneObject::detach(void)
{
	/* Detach any children */
	LoadableObject::detach();
	/* Discard our associated node */
	/* As the Scene owns the node itself, we don't have to be responsible
	 * for freeing it.
	 */
	m_node = NULL;
	return true;
}

/* Utility method invoked by attach() to attach the scene object's Node
 * to the scene itself.
 */
bool
LoadableSceneObject::attachNode(Scene *scene, Node *parentNode, Node *newNode)
{
	if(parentNode)
	{
		if(!newNode->attachToScene(parentNode, m_id))
		{
			return false;
		}
	}
	else
	{
		if(!newNode->attachToScene(scene, m_id))
		{
			return false;
		}
	}
	return true;
}

/* Utility method invoked by attach() to apply any properties to a new
 * node.
 */
bool
LoadableSceneObject::applyProperties(Node *newNode)
{
	Ogre::Matrix3 mat;
	
	newNode->translate(m_translate);
	newNode->scale(m_scale);
	newNode->yaw(m_yaw);
	newNode->pitch(m_pitch);
	newNode->roll(m_roll);
	return true;
}




/* LoadableSceneProp encapsulates a <prop> within a scene
 * <prop id="scene-local-name" class="asset-name" x="n" y="n" z="n">
 */

LoadableSceneProp::LoadableSceneProp(const LoadableSceneProp &object):
	LoadableSceneObject(object)
{
	m_className = object.m_className;
	m_fixed = object.m_fixed;
}

LoadableSceneProp::LoadableSceneProp(Scene *owner, LoadableSceneObject *parent, Ogre::String kind, AttrList &attrs):
	LoadableSceneObject(owner, parent, kind, attrs),
	m_className(""),
	m_fixed(false)
{
	AttrListIterator it;
	
	for(it = m_attrs.begin(); it != m_attrs.end(); it++)
	{
		Attr p = *it;
		
		if(!p.first.compare("class"))
		{
			m_className = p.second;
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
		m_id = m_className;
	}
}

LoadableObject *
LoadableSceneProp::clone(void) const
{
	return new LoadableSceneProp(*this);
}

bool
LoadableSceneProp::complete(void) const
{
	if(!m_className.length())
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: scene " + m_kind + " is missing a class");
		return false;
	}
	return LoadableSceneObject::complete();
}

Ogre::String
LoadableSceneProp::className(void) const
{
	return m_className;
}

bool
LoadableSceneProp::fixed(void) const
{
	return m_fixed;
}

Node *
LoadableSceneProp::createNode(State *state)
{
	Loadable *p;
	Prop *prop;

	p = state->factory(m_kind, m_className);
	if(!p)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: failed to create " + m_kind + " instance");
		return NULL;
	}
	prop = dynamic_cast<Prop *>(p);
	if(!prop)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: factory-returned " + m_kind + " (" + m_className + ") instance was not a prop");
		delete p;
		return NULL;
	}
	return prop;
}

bool
LoadableSceneProp::applyProperties(Node *newNode)
{
	Prop *prop;
	
	if(!LoadableSceneObject::applyProperties(newNode))
	{
		return false;
	}
	prop = dynamic_cast<Prop *>(newNode);
	if(m_fixed)
	{
		prop->setFixed();
	}
	return true;
}

/*
bool
LoadableSceneProp::addResources(Ogre::String group)
{
	State *state;
	Ogre::Matrix3 mat;
	
	state = m_owner->state();
	m_prop = dynamic_cast<Prop *>(state->factory(m_name, m_class));
	if(!m_prop)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: failed to create scene prop instance");
		return false;
	}
	(dynamic_cast<Scene *> (m_owner))->addSceneObject(this, m_prop);
	m_prop->scale(m_scale);
	mat.FromEulerAnglesXYZ(m_yaw, m_pitch, m_roll);
	m_prop->setOrientation(Ogre::Quaternion(mat));
	m_prop->translate(m_translate);
	if(m_fixed)
	{
		m_prop->setFixed();
	}
	return true;
} */

/*
bool
LoadableSceneProp::attach(Scene *scene)
{
	Ogre::SceneNode *node;

	if(!m_prop->attach(scene, m_id))
	{
		return false;
	}
	return true;
} */




/* LoadableSceneActor encapsulates an <actor> within a scene, and is
 * simply a specialisation of LoadableSceneProp
 */

LoadableSceneActor::LoadableSceneActor(const LoadableSceneActor &object):
	LoadableSceneProp(object)
{
}

LoadableSceneActor::LoadableSceneActor(Scene *owner, LoadableSceneObject *parent, Ogre::String name, AttrList &attrs):
	LoadableSceneProp(owner, parent, name, attrs)
{
}

LoadableObject *
LoadableSceneActor::clone(void) const
{
	return new LoadableSceneActor(*this);
}




/* LoadableSceneLight encapsulates a <light> within a <scene>
 * <light id="scene-local-name" x="n" y="n" z="n">
 */

LoadableSceneLight::LoadableSceneLight(const LoadableSceneLight &object):
	LoadableSceneObject(object)
{
}

LoadableSceneLight::LoadableSceneLight(Scene *owner, LoadableSceneObject *parent, Ogre::String name, AttrList &attrs):
	LoadableSceneObject(owner, parent, name, attrs)
{
}

LoadableObject *
LoadableSceneLight::clone(void) const
{
	return new LoadableSceneLight(*this);
}

/* Because lights don't have classes (i.e., no XML description), they're not
 * a descendant of LoadableSceneProp and so can't share the createNode
 * implementation.
 */
Node *
LoadableSceneLight::createNode(State *state)
{
	Loadable *p;
	Light *light;

	p = state->factory(m_kind, m_id);
	if(!p)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: failed to create " + m_kind + " instance");
		return NULL;
	}
	light = dynamic_cast<Light *>(p);
	if(!light)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: factory-returned " + m_kind + " (" + m_id + ") instance was not a light");
		delete p;
		return NULL;
	}
	return light;
}




/* LoadableSceneAmbientLight encapsulates an <ambientlight> within a <scene>.
 * <ambientlight r="n" g="n" b="n" a="n" />
 */

LoadableSceneAmbientLight::LoadableSceneAmbientLight(Scene *owner,  Ogre::String name, AttrList &attrs):
	LoadableSceneProperty(owner, NULL, name, attrs)
{
	owner->m_hasAmbientLight = true;
	owner->m_ambientColour = parseColourValue(attrs);
}




/* LoadableSceneTransform encapsulates a <scale>, <translate> or
 * <rotate> within a scene object; the x, y and z attributes are parsed
 * and then applied to the appropriate properties of the parent.
 */
LoadableSceneTransform::LoadableSceneTransform(Scene *owner, LoadableSceneObject *parent, Ogre::String kind, AttrList &attrs):
	LoadableSceneProperty(owner, parent, kind, attrs)
{
	m_vector = parseXYZ(attrs);
	if(!kind.compare("scale"))
	{
		parent->m_scale = m_vector;
	}
	else if(!kind.compare("translate"))
	{
		parent->m_translate = m_vector;
	}
}




/* LoadableSceneRotate encapsulates a <yaw>, <pitch> or <roll> within a 
 * scene object; the rad or deg attributes specify radians or degrees
 * respectively.
 */
LoadableSceneRotation::LoadableSceneRotation(Scene *owner, LoadableSceneObject *parent, Ogre::String kind, AttrList &attrs):
	LoadableSceneProperty(owner, parent, kind, attrs),
	m_angle()
{
	AttrListIterator it;
	
	m_discardable = true;
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
	if(!kind.compare("yaw"))
	{
		parent->m_yaw = m_angle;
	}
	else if(!kind.compare("pitch"))
	{
		parent->m_pitch = m_angle;
	}
	else if(!kind.compare("roll"))
	{
		parent->m_roll = m_angle;
	}
}




/* LoadableSceneGravity encapsulates a <gravity> within a <scene>.
 * <gravity x="n" y="n" z="n" />
 */

LoadableSceneGravity::LoadableSceneGravity(Scene *owner,  Ogre::String kind, AttrList &attrs):
	LoadableSceneProperty(owner, NULL, kind, attrs)
{
	owner->m_gravity = ogreVecToBullet(parseXYZ(attrs));
}
