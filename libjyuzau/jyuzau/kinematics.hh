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

#ifndef JYUZAU_KINEMATICS_HH_
# define JYUZAU_KINEMATICS_HH_         1

# include <OGRE/OgreFrameListener.h>

# include <LinearMath/btMotionState.h>

class btDynamicsWorld;
class btPairCachingGhostObject;
class btKinematicCharacterController;
class btBroadphaseInterface;
class btConvexShape;
class btRigidBody;

namespace Jyuzau
{

	class Actor;
	class Scene;
	
	/* The Kinematics class implements controllable physics for an Actor,
	 * for example one which is player-controlled.
	 */
	class Kinematics: public btMotionState
	{
	public:
		Kinematics(Actor *actor);
		virtual ~Kinematics();
		
		virtual bool walkDirection(const Ogre::Vector3 &vec);
		virtual bool jump(void);
		
		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
		
		virtual void getWorldTransform(btTransform &worldTrans) const;
		virtual void setWorldTransform(const btTransform &worldTrans);
	protected:
		Actor *m_actor;
		Scene *m_scene;
		btRigidBody *m_rigidBody;
		btDynamicsWorld *m_dynamics;
		btPairCachingGhostObject *m_ghost;
		btKinematicCharacterController *m_actionController;
		btBroadphaseInterface *m_pairCache;
		btConvexShape *m_shape;
	};

};

#endif /*!JYUZAU_KINEMATICS_HH_*/
