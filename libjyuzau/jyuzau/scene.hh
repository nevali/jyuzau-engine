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

# include "jyuzau/loadable.hh"

# include <utility>

# include <OGRE/OgreString.h>
# include <OGRE/OgreSceneManager.h>
# include <OGRE/OgreVector3.h>
# include <OGRE/OgreColourValue.h>

# include <btBulletDynamicsCommon.h>

namespace Jyuzau
{
	class Prop;
	class Light;
	class LoadableSceneObject;
	class LoadableSceneProp;
	class LoadableSceneLight;
	class LoadableSceneAmbientLight;
	class LoadableSceneGravity;

	class Scene: public Loadable
	{
		friend class LoadableSceneGravity;
	public:
		Scene(Ogre::String name, State *state);
		virtual ~Scene();
		
		bool attach(Ogre::SceneManager *sceneManager);
		bool detach(void);
		
		Ogre::SceneNode *rootNode(void);
		
		/* Add a Prop to the scene and take ownership of it */
		virtual void addSceneObject(LoadableSceneObject *sceneObject, Loadable *object);
		virtual void addAmbientLight(LoadableSceneAmbientLight *sceneAmblientLight);
		
		/* Physics */
		virtual btDynamicsWorld *dynamics(void);
		virtual bool setGravity(const Ogre::Vector3 &vec);
		virtual bool setGravity(const btVector3 &vec);
		virtual bool addRigidBody(btRigidBody *body);
		virtual bool removeRigidBody(btRigidBody *body);
	protected:
		Ogre::SceneManager *m_manager;
		std::vector<LoadableSceneObject *> m_objects;
		LoadableSceneAmbientLight *m_ambient;
		btBroadphaseInterface *m_broadphase;
		btCollisionConfiguration *m_collisionConfig;
		btCollisionDispatcher *m_dispatcher;
		btDynamicsWorld *m_dynamics;
		btConstraintSolver *m_solver;
		btVector3 m_gravity;
		
		virtual LoadableObject *factory(Ogre::String name, AttrList &attrs);
		
		virtual void attachPhysics(void);
	};
	
	/* LoadableScene encapsulates the <scene> root element */
	class LoadableScene: public LoadableObject
	{
		friend class Scene;
	public:
		LoadableScene(Loadable *owner, Ogre::String name, AttrList &attrs);
	protected:
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
		LoadableSceneObject(Scene *owner, LoadableSceneObject *parent, Ogre::String name, AttrList &attrs);
		
		virtual Ogre::String id(void);
		virtual bool complete(void);
	protected:
		Ogre::String m_id;
		Ogre::Vector3 m_scale, m_translate;
		Ogre::Radian m_yaw, m_pitch, m_roll;
		
		virtual bool attach(Scene *scene) = 0;
	};
	
	/* LoadableSceneTransform encapsulates a <scale> or <translate>
	 * within a scene object; the x, y and z attributes are parsed
	 * and then applied to the appropriate properties of the parent.
	 */
	class LoadableSceneTransform: public LoadableObject
	{
		friend class Scene;
	public:
		LoadableSceneTransform(Scene *owner, LoadableSceneObject *parent, Ogre::String name, AttrList &attrs);
	protected:
		Ogre::Vector3 m_vector;
	};

	/* LoadableSceneRotation encapsulates a <yaw>, <pitch> or <roll>
	 * within a scene object; the rad or deg attributes are parsed
	 * and then applied to the appropriate properties of the parent.
	 */
	class LoadableSceneRotation: public LoadableObject
	{
		friend class Scene;
	public:
		LoadableSceneRotation(Scene *owner, LoadableSceneObject *parent, Ogre::String name, AttrList &attrs);
	protected:
		Ogre::Radian m_angle;
	};
	
	/* LoadableSceneAmbientLight encapsulates an <ambientlight> within a
	 * <scene>.
	 * <ambientlight r="n" g="n" b="n" a="n" />
	 */
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

	/* LoadableSceneGravity encapsulates a <gravity> within a <scene>.
	 * <gravity x="n" y="n" z="n" />
	 */
	class LoadableSceneGravity: public LoadableObject
	{
		friend class Scene;
	public:
		LoadableSceneGravity(Scene *owner, Ogre::String name, AttrList &attrs);
	};
	
	
	/* LoadableSceneProp encapsulates a <prop> within a scene
	 * <prop id="scene-local-name" class="asset-name" x="n" y="n" z="n">
	 */
	class LoadableSceneProp: public LoadableSceneObject
	{
		friend class Scene;
	public:
		LoadableSceneProp(Scene *owner, LoadableSceneObject *parent, Ogre::String name, AttrList &attrs);
		
		virtual bool complete(void);
	protected:
		Ogre::String m_class;
		Prop *m_prop;
		bool m_fixed;
		
		virtual bool addResources(Ogre::String group);
		virtual bool attach(Scene *scene);
	};
	
	/* LoadableSceneActor encapsulates an <actor> within a scene, and is
	 * simply a specialisation of LoadableSceneProp
	 */
	class LoadableSceneActor: public LoadableSceneProp
	{
		friend class Scene;
	public:
		LoadableSceneActor(Scene *owner, LoadableSceneObject *parent, Ogre::String name, AttrList &attrs);
	protected:
		virtual bool addResources(Ogre::String group);
	};
	
	/* LoadableSceneLight encapsulates a <light> within a <scene>
	 * <light id="scene-local-name" x="n" y="n" z="n">
	 */
	class LoadableSceneLight: public LoadableSceneObject
	{
		friend class Scene;
	public:
		LoadableSceneLight(Scene *owner, LoadableSceneObject *parent, Ogre::String name, AttrList &attrs);
	protected:
		Light *m_light;
		
		virtual bool addResources(Ogre::String group);
		virtual bool attach(Scene *scene);
	};

};

#endif /*!JYUZAU_SCENE_HH_*/
