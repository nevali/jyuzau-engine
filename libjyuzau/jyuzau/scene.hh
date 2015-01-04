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

#ifndef JYUZAU_SCENE_HH_
# define JYUZAU_SCENE_HH_              1

# include "jyuzau/loadable.hh"

# include <utility>

# include <OGRE/OgreString.h>
# include <OGRE/OgreSceneManager.h>
# include <OGRE/OgreVector3.h>
# include <OGRE/OgreColourValue.h>

# include <btBulletDynamicsCommon.h>

namespace Jyuzau
{
	class Node;
	class Prop;
	class Light;
	class LoadableSceneObject;
	class LoadableSceneProp;
	class LoadableSceneLight;
	class LoadableSceneAmbientLight;
	class LoadableSceneGravity;

	class Scene: public Loadable
	{
		friend class LoadableSceneProp;
		friend class LoadableSceneLight;
		friend class LoadableSceneAmbientLight;
		friend class LoadableSceneGravity;
		friend class LoadableSceneActor;
	public:
		Scene(const Scene &scene);
		Scene(Ogre::String className, State *state);
		virtual ~Scene();
		
		virtual Loadable *clone(void);
		
		/* Properties */
		Ogre::SceneManager *sceneManager(void) const;
		Ogre::SceneNode *rootNode(void) const;
		virtual btDynamicsWorld *dynamics(void) const;
		virtual Ogre::Vector3 gravity(void) const;
		virtual bool setGravity(const Ogre::Vector3 &vec);
		virtual bool setGravity(const btVector3 &vec);
		
		bool attach(void);
		bool detach(void);
		
	protected:
		Ogre::SceneManager *m_manager;
		bool m_hasAmbientLight;
		Ogre::ColourValue m_ambientColour;
		btBroadphaseInterface *m_broadphase;
		btCollisionConfiguration *m_collisionConfig;
		btCollisionDispatcher *m_dispatcher;
		btDynamicsWorld *m_dynamics;
		btConstraintSolver *m_solver;
		btVector3 m_gravity;
		
		virtual bool load(void);
		virtual LoadableObject *factory(Ogre::String kind, AttrList &attrs);
		
		/* Physics */
		virtual void createPhysics(void);
		virtual bool addRigidBody(btRigidBody *body);
		virtual bool removeRigidBody(btRigidBody *body);
		
	};
	
	/* LoadableScene encapsulates the <scene> root element */
	class LoadableScene: public LoadableObject
	{
		friend class Scene;
	public:
		LoadableScene(const LoadableScene &object);
		LoadableScene(Loadable *owner, Ogre::String kind, AttrList &attrs);

		virtual LoadableObject *clone(void) const;
	};
	
	/* LoadableSceneProperty is the base class for encapsulating any
	 * XML node which does nothing more than parse property values which
	 * apply to its parent or the scene. By nature, all LoadableSceneProperty
	 * instances are discardable.
	 */
	class LoadableSceneProperty: public LoadableObject
	{
	public:
		/* No copy constructor is required because discardable loadable
		 * objects are never cloned.
		 */
		LoadableSceneProperty(Scene *owner, LoadableSceneObject *parent, Ogre::String kind, AttrList &attrs);
	};
	
	/* LoadableSceneObject is the base class for any object which can be
	 * loaded into a scene and has some kind of manifestation (including
	 * both props and dynamic lights).
	 */
	class LoadableSceneObject: public LoadableObject
	{
		friend class Scene;
		friend class LoadableSceneTransform;
		friend class LoadableSceneRotation;
	public:
		LoadableSceneObject(const LoadableSceneObject &object);
		LoadableSceneObject(Scene *owner, LoadableSceneObject *parent, Ogre::String kind, AttrList &attrs);
		
		/* Properties */
		virtual Ogre::String id(void) const;
		virtual Node *node(void) const;
		virtual bool complete(void) const;

		virtual bool attach(void);
		virtual bool detach(void);
	protected:
		Node *m_node;
		Ogre::String m_id;
		Ogre::Vector3 m_scale, m_translate;
		Ogre::Radian m_yaw, m_pitch, m_roll;
		
		virtual Node *createNode(State *state) = 0;
		virtual bool attachNode(Scene *scene, Node *parentNode, Node *newNode);
		virtual bool applyProperties(Node *newNode);
	};
	
	/* LoadableSceneTransform encapsulates a <scale> or <translate>
	 * within a scene object; the x, y and z attributes are parsed
	 * and then applied to the appropriate properties of the parent.
	 */
	class LoadableSceneTransform: public LoadableSceneProperty
	{
	public:
		LoadableSceneTransform(Scene *owner, LoadableSceneObject *parent, Ogre::String kind, AttrList &attrs);
	protected:
		Ogre::Vector3 m_vector;
	};

	/* LoadableSceneRotation encapsulates a <yaw>, <pitch> or <roll>
	 * within a scene object; the rad or deg attributes are parsed
	 * and then applied to the appropriate properties of the parent.
	 */
	class LoadableSceneRotation: public LoadableSceneProperty
	{
	public:
		LoadableSceneRotation(Scene *owner, LoadableSceneObject *parent, Ogre::String kind, AttrList &attrs);
	protected:
		Ogre::Radian m_angle;
	};
	
	/* LoadableSceneAmbientLight encapsulates an <ambientlight> within a
	 * <scene>.
	 * <ambientlight r="n" g="n" b="n" a="n" />
	 */
	class LoadableSceneAmbientLight: public LoadableSceneProperty
	{
	public:
		LoadableSceneAmbientLight(Scene *owner, Ogre::String kind, AttrList &attrs);
	};

	/* LoadableSceneGravity encapsulates a <gravity> within a <scene>.
	 * <gravity x="n" y="n" z="n" />
	 */
	class LoadableSceneGravity: public LoadableSceneProperty
	{
		friend class Scene;
	public:
		LoadableSceneGravity(Scene *owner, Ogre::String kind, AttrList &attrs);
	};
	
	
	/* LoadableSceneProp encapsulates a <prop> within a scene
	 * <prop id="scene-local-name" class="asset-name" x="n" y="n" z="n">
	 */
	class LoadableSceneProp: public LoadableSceneObject
	{
	public:
		LoadableSceneProp(const LoadableSceneProp &object);
		LoadableSceneProp(Scene *owner, LoadableSceneObject *parent, Ogre::String kind, AttrList &attrs);

		virtual LoadableObject *clone(void) const;
		
		virtual bool complete(void) const;
		virtual Ogre::String className(void) const;
		virtual bool fixed(void) const;
	protected:
		Ogre::String m_className;
		bool m_fixed;
		
		virtual Node *createNode(State *state);
		virtual bool applyProperties(Node *newNode);
	};
	
	/* LoadableSceneActor encapsulates an <actor> within a scene, and is
	 * simply a specialisation of LoadableSceneProp
	 */
	class LoadableSceneActor: public LoadableSceneProp
	{
	public:
		LoadableSceneActor(const LoadableSceneActor &object);
		LoadableSceneActor(Scene *owner, LoadableSceneObject *parent, Ogre::String kind, AttrList &attrs);
		
		virtual LoadableObject *clone(void) const;
	protected:
	};
	
	/* LoadableSceneLight encapsulates a <light> within a <scene>
	 * <light id="scene-local-name" x="n" y="n" z="n">
	 */
	class LoadableSceneLight: public LoadableSceneObject
	{
	public:
		LoadableSceneLight(const LoadableSceneLight &object);
		LoadableSceneLight(Scene *owner, LoadableSceneObject *parent, Ogre::String kind, AttrList &attrs);
		
		virtual LoadableObject *clone(void) const;
	protected:
		virtual Node *createNode(State *state);
	};

};

#endif /*!JYUZAU_SCENE_HH_*/
