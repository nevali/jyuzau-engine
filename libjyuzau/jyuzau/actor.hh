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

#ifndef JYUZAU_ACTOR_HH_
# define JYUZAU_ACTOR_HH_              1

# include "jyuzau/prop.hh"
# include "jyuzau/defs.hh"

# include <OGRE/OgreFrameListener.h>

class btPairCachingGhostObject;
class btKinematicCharacterController;
class btBroadphaseInterface;

namespace Jyuzau
{
	class Camera;
	class Character;
	class Kinematics;
	
	/* An actor is a kind of prop which can have autonomous behaviours
	 * and cameras attached to it.
	 */
	class Actor: public Prop, public Ogre::FrameListener
	{
		friend class Character;
	public:
		Actor(const Actor &object);
		Actor(Ogre::String name, State *state, Ogre::String kind = "actor");
		virtual ~Actor();
		
		virtual Loadable *clone(void) const;
		
		virtual Character *character(void) const;
		
		virtual Camera *createCamera(CameraType type);
		virtual void resetActiveCameras(void);
		virtual void setActiveCamera(Camera *cam);
		
		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

		/* Basic movement primitives */
		virtual void setSpeed(MoveSpeed speed);
		virtual void beginRun(void);
		virtual void endRun(void);
		
		virtual void forward(MoveSpeed speed = MS_CURRENT);
		virtual void backward(MoveSpeed speed = MS_CURRENT);
		virtual void turnLeft(MoveSpeed speed = MS_CURRENT);
		virtual void turnRight(MoveSpeed speed = MS_CURRENT);
		virtual void turnLeftRight(int distance);
		virtual void strafeLeft(MoveSpeed speed = MS_CURRENT);
		virtual void strafeRight(MoveSpeed speed = MS_CURRENT);
		virtual void crouch(void);
		virtual void jump(void);

		/* Smoothed keyboard movement */
		virtual void beginForward(MoveSpeed speed = MS_CURRENT);
		virtual void endForward(void);
		virtual void beginBackward(MoveSpeed speed = MS_CURRENT);
		virtual void endBackward(void);
		virtual void beginTurnLeft();
		virtual void endTurnLeft(void);
		virtual void beginTurnRight();
		virtual void endTurnRight(void);
		virtual void beginStrafeLeft(MoveSpeed speed = MS_CURRENT);
		virtual void endStrafeLeft(void);
		virtual void beginStrafeRight(MoveSpeed speed = MS_CURRENT);
		virtual void endStrafeRight(void);
		
		/* Basic looking */
		virtual void lookUp(void);
		virtual void lookDown(void);
		virtual void lookUpDown(int distance);
		virtual void resetCamera(void);
		virtual void zoom(void);
		
		/* Smoothed keyboard looking */
		virtual void beginLookUp(void);
		virtual void endLookUp(void);
		virtual void beginLookDown(void);
		virtual void endLookDown(void);

		/* Weapons */
		virtual void primaryFire(void);
		virtual void secondaryFire(void);
		virtual void special(void);
		virtual void switchWeapon(int index);
		virtual void prevWeapon(void);
		virtual void nextWeapon(void);
	protected:
		double m_health;
		Character *m_character;
		unsigned m_level;
		Camera *m_cameras[CT_COUNT];

		/* Movement */
		bool m_forward, m_backward, m_left, m_right;
		MoveSpeed m_speed;
		Ogre::Vector3 m_velocity;
		Ogre::Real m_topSpeed;
		Ogre::Real m_moveAccel;
		Ogre::Real m_moveDecel;
		Ogre::Real m_moveRunFactor;
		Ogre::Real m_moveCreepFactor;
		Ogre::Real m_moveDistance;
		
		/* Turn (rotation) velocity */
		bool m_clockwise, m_cclockwise;
		Ogre::Real m_rotVelocity;
		Ogre::Real m_rotSpeed;
		Ogre::Real m_rotAccel;
		Ogre::Real m_rotDecel;
		Ogre::Real m_rotStep;
		Ogre::Real m_rotAngle;
		Ogre::Real m_rotFactor;
		
		/* Look up/down (camera pitch) velocity */
		bool m_lookUp, m_lookDown;
		Ogre::Real m_camPitchVelocity;
		Ogre::Real m_camPitchSpeed;
		Ogre::Real m_camPitchAccel;
		Ogre::Real m_camPitchDecel;
		Ogre::Real m_camPitchStep;
		Ogre::Real m_camPitchAngle;
		Ogre::Real m_camPitchFactor;
		
		/* Physics */
		Kinematics *m_kinematics;

		virtual void characterAttached(void);
		virtual void characterDetached(void);
		
		void accelerateXYMovement(Ogre::Vector3 &velocity, bool f, bool b, bool l, bool r, Ogre::Real topSpeed, Ogre::Real accelFactor, Ogre::Real decelFactor, Ogre::Real elapsed);
		void accelerateRotation(Ogre::Real &velocity, bool back, bool forward, Ogre::Real topSpeed, Ogre::Real step, Ogre::Real accelFactor, Ogre::Real decelFactor, Ogre::Real elapsed);
	};
};

#endif /*!JYUZAU_ACTOR_HH_*/
