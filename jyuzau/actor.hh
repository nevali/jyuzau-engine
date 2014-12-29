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

#ifndef JYUZAU_ACTOR_HH_
# define JYUZAU_ACTOR_HH_              1

# include "jyuzau/prop.hh"

namespace Ogre
{
	class Camera;
}

namespace Jyuzau
{
	
	enum CameraType {
		CT_FIRSTPERSON,
		CT_FOLLOWING,
		/* The following must always be last */
		CT_COUNT
	};
	
	enum MoveSpeed {
		MS_CREEP,
		MS_WALK,
		MS_RUN
	};
	
	/* An actor is a kind of prop which can have autonomous behaviours
	 * and cameras attached to it.
	 */
	class Actor: public Prop
	{
	public:
		static Actor *create(Ogre::String name, Scene *scene = NULL);
		
		Actor(Ogre::String name);
		virtual ~Actor();
		
		virtual Ogre::Camera *camera(CameraType type);

		virtual void forward(MoveSpeed speed = MS_WALK);
		virtual void backward(MoveSpeed speed = MS_WALK);
		virtual void turnLeft(MoveSpeed speed = MS_WALK);
		virtual void turnRight(MoveSpeed speed = MS_WALK);
		virtual void strafeLeft(MoveSpeed speed = MS_WALK);
		virtual void strafeRight(MoveSpeed speed = MS_WALK);
		virtual void primaryFire(void);
		virtual void secondaryFire(void);
		virtual void special(void);
		virtual void crouch(void);
		virtual void jump(void);
		virtual void zoom(void);
		virtual void switchWeapon(int index);
		virtual void prevWeapon(void);
		virtual void nextWeapon(void);
	protected:
		Ogre::Camera *m_cameras[CT_COUNT];
		
		virtual Ogre::Camera *createCamera(CameraType type);
	};
};

#endif /*!JYUZAU_ACTOR_HH_*/
