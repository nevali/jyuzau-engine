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

#include "jyuzau/kinematics.hh"
#include "jyuzau/actor.hh"
#include "jyuzau/scene.hh"

#include "p_utils.hh"

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreLogManager.h>

#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

using namespace Jyuzau;

class KinematicsController: public btKinematicCharacterController
{
public:
	KinematicsController(btPairCachingGhostObject *ghostObject, btConvexShape *convexShape, btScalar stepHeight, int upAxis = 1);
};

KinematicsController::KinematicsController(btPairCachingGhostObject *ghostObject, btConvexShape *convexShape, btScalar stepHeight, int upAxis):
	btKinematicCharacterController::btKinematicCharacterController(ghostObject, convexShape, stepHeight, upAxis)
{
}
	
Kinematics::Kinematics(Actor *actor):
	m_actor(actor)
{
	Ogre::Vector3 vec;
	btDynamicsWorld *dynamics;
	btScalar characterHeight, characterWidth, stepHeight;
	btTransform startTransform;
	KinematicsController *c;
	
	m_rigidBody = actor->rigidBody();
	m_scene = actor->scene();
	m_dynamics = m_scene->dynamics();
	m_pairCache = m_scene->broadphase();

	vec = actor->entity()->getBoundingBox().getSize();
	characterWidth = (vec.x / 2);
	characterHeight = (vec.y / 2);
	stepHeight = characterHeight / 50;
	
	m_pairCache->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

	m_ghost = new btPairCachingGhostObject();

	startTransform.setIdentity();

	m_shape = new btCapsuleShape(characterWidth, characterHeight);
	m_ghost->setCollisionShape(m_shape);
	m_ghost->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_CHARACTER_OBJECT);

	c = new KinematicsController(m_ghost, m_shape, stepHeight);
	c->setGravity(std::abs(m_scene->gravity().y));
	c->setFallSpeed(55 * 100);
	c->setMaxJumpHeight(characterHeight / 4);
	c->setJumpSpeed(25 * 100);

	m_actionController = c;
	/* Remove the Actor body from the scene */
	m_actor->getWorldTransform(startTransform);
	m_dynamics->removeRigidBody(m_rigidBody);
	m_rigidBody->setCollisionFlags(m_rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	m_rigidBody->setMotionState(this);
	m_rigidBody->setMassProps(0, btVector3(0, 0, 0));
	m_rigidBody->setActivationState(DISABLE_DEACTIVATION);
	m_dynamics->addRigidBody(m_rigidBody, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::AllFilter & ~(btBroadphaseProxy::DebrisFilter | btBroadphaseProxy::CharacterFilter));
	/* Add the ghost in its place */
	m_ghost->setWorldTransform(startTransform);
	m_dynamics->addCollisionObject(m_ghost, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::AllFilter & ~(btBroadphaseProxy::DebrisFilter | btBroadphaseProxy::CharacterFilter));
	/* Add the character controller */
	m_dynamics->addAction(m_actionController);
}

Kinematics::~Kinematics()
{
	m_dynamics->removeAction(m_actionController);
	m_dynamics->removeCollisionObject(m_ghost);
	m_dynamics->removeRigidBody(m_rigidBody);
	m_rigidBody->forceActivationState(ACTIVE_TAG);
	m_rigidBody->setMotionState(m_actor);
	m_rigidBody->setCollisionFlags(m_rigidBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
	m_rigidBody->setMassProps(m_actor->mass(), btVector3(0, 0, 0));
	m_dynamics->addRigidBody(m_rigidBody);
	
	m_rigidBody = NULL;
	delete m_actionController;
	m_actionController = NULL;
	delete m_shape;
	m_shape = NULL;
	delete m_ghost;
	m_ghost = NULL;
	m_pairCache = NULL;
	m_dynamics = NULL;
	m_scene = NULL;
	m_actor = NULL;
}

/* Invoked by Actor::frameRenderingQueued() to ensure everything is in sync
 * between the Actor's state and the Kinematics instance.
 */
bool
Kinematics::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	btVector3 btVec;
	btTransform transform;

	/* Keep the actor's position in sync with the ghost */
	transform = m_ghost->getWorldTransform();
	btVec = transform.getOrigin();
	m_actor->setPosition(bulletVecToOgre(btVec));
	return true;
}

/* Invoked by Actor::frameRenderingQueued() when the movement velocity is
 * non-zero.
 */
bool
Kinematics::walkDirection(const Ogre::Vector3 &vec)
{
	btVector3 btVec;
	
	btVec = ogreVecToBullet(vec);
	m_actionController->setWalkDirection(btVec);
	return true;
}

/* Invoked by Actor::jump() */
bool
Kinematics::jump(void)
{
	m_actionController->jump();
	return true;
}

/* btMotionState interface */
void
Kinematics::getWorldTransform(btTransform &worldTrans) const
{
	worldTrans = m_ghost->getWorldTransform();
}

void 
Kinematics::setWorldTransform(const btTransform &worldTrans)
{
	m_ghost->setWorldTransform(worldTrans);
	m_actor->setWorldTransform(worldTrans);
}

