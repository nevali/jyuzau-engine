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

#ifndef JYUZAU_CAMERA_HH_
# define JYUZAU_CAMERA_HH_             1

# include <OGRE/OgreCommon.h>
# include <OGRE/OgreString.h>
# include <OGRE/OgreVector3.h>

# include "jyuzau/defs.hh"

namespace Ogre
{
	class Camera;
	class Viewport;
	class RenderTarget;
	class SceneManager;
	class SceneNode;
}

namespace Jyuzau
{
	class Actor;
	
	/* This is a simple wrapper class around an Ogre Camera which deals with
	 * being attached to an Actor, having yaw, pitch and roll nodes, and
	 * having a viewport.
	 */
	struct Camera
	{
		Ogre::Camera *camera;
		Ogre::Viewport *viewport;
		Ogre::SceneNode *node;
		Ogre::SceneNode *yawNode;
		Ogre::SceneNode *pitchNode;
		Ogre::SceneNode *rollNode;
		Actor *actor;
		CameraType cameraType;
		bool limitPitch;
		
		Camera(Ogre::String name, Ogre::SceneManager *sceneManager);
		virtual ~Camera();
		
		virtual Ogre::Viewport *createViewport(Ogre::RenderTarget *target, int zorder = 0);
		virtual void deleteViewport(void);
		
		virtual void matchAspectRatio(void);
		
		virtual void setPosition(const Ogre::Vector3 &vec);
		virtual void setNearClipDistance(Ogre::Real dist);

		virtual void lookAt(const Ogre::Vector3 &vec);
		
		virtual void attach(Ogre::SceneNode *parentNode);
		virtual void detach(void);
		
		virtual void pitch(Ogre::Radian angle);
		
		virtual void yaw(Ogre::Radian angle)
		{
			if(yawNode)
			{
				yawNode->yaw(angle);
			}
		}
		
		virtual void roll(Ogre::Radian angle)
		{
			if(rollNode)
			{
				rollNode->roll(angle);
			}
		}
		
	};
};

#endif /*!JYUZAU_CAMERA_HH_*/

