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
# include "jyuzau/defs.hh"

namespace Jyuzau
{
	class Camera;
	class Character;

	/* An actor is a kind of prop which can have autonomous behaviours
	 * and cameras attached to it.
	 */
	class Actor: public Prop
	{
		friend class Character;
	public:
		static Actor *create(Ogre::String name, Scene *scene = NULL);
		static Actor *create(Ogre::String name, Ogre::SceneManager *sceneManager);
		
		Actor(Ogre::String name);
		virtual ~Actor();
		
		virtual Camera *createCamera(CameraType type);
		virtual void resetActiveCameras(void);
		virtual void setActiveCamera(Camera *cam);
		
		virtual Character *character(void);

		virtual void forward(MoveSpeed speed = MS_WALK);
		virtual void backward(MoveSpeed speed = MS_WALK);
		virtual void turnLeft(MoveSpeed speed = MS_WALK);
		virtual void turnRight(MoveSpeed speed = MS_WALK);
		virtual void turnLeftRight(int distance);
		virtual void strafeLeft(MoveSpeed speed = MS_WALK);
		virtual void strafeRight(MoveSpeed speed = MS_WALK);
		virtual void lookUp(void);
		virtual void lookDown(void);
		virtual void lookUpDown(int distance);
		virtual void resetCamera(void);
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
		double m_health;
		Character *m_character;
		unsigned m_level;
		Camera *m_cameras[CT_COUNT];
		
		virtual void characterAttached(void);
		virtual void characterDetached(void);
	};
};

#endif /*!JYUZAU_ACTOR_HH_*/
