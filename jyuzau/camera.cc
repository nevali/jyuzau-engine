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

#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreRenderTarget.h>
#include <OGRE/OgreCamera.h>

#include "jyuzau/camera.hh"

using namespace Jyuzau;

Camera::Camera(Ogre::String name, Ogre::SceneManager *sceneManager):
	viewport(NULL),
	node(NULL),
	yawNode(NULL),
	pitchNode(NULL),
	rollNode(NULL),
	actor(NULL),
	cameraType(CT_UNSPEC),
	limitPitch(false)
{
	camera = sceneManager->createCamera(name);
}

Camera::~Camera()
{
	deleteViewport();
	detach();
	camera->getSceneManager()->destroyCamera(camera);
}

Ogre::Viewport *
Camera::createViewport(Ogre::RenderTarget *target, int zorder)
{
	if(viewport)
	{
		viewport->getTarget()->removeViewport(viewport->getZOrder());
	}
	viewport = target->addViewport(camera, zorder);
	return viewport;
}

void
Camera::deleteViewport(void)
{
	if(viewport)
	{
		viewport->getTarget()->removeViewport(viewport->getZOrder());
	}
	viewport = NULL;
}

void
Camera::matchAspectRatio(void)
{
	if(viewport)
	{
		camera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));
	}
}

void
Camera::setPosition(const Ogre::Vector3 &vec)
{
	camera->setPosition(vec);
}

void
Camera::lookAt(const Ogre::Vector3 &vec)
{
	camera->lookAt(vec);
}

void
Camera::setNearClipDistance(Ogre::Real dist)
{
	camera->setNearClipDistance(dist);
}

void
Camera::attach(Ogre::SceneNode *parentNode)
{
	if(node)
	{
		detach();
	}
	node = parentNode->createChildSceneNode();
	yawNode = node->createChildSceneNode();
	pitchNode = yawNode->createChildSceneNode();
	rollNode = pitchNode->createChildSceneNode();
	rollNode->attachObject(camera);
}

void
Camera::detach(void)
{
	if(rollNode)
	{
		camera->detachFromParent();
		delete rollNode;
		rollNode = NULL;
	}
	if(pitchNode)
	{
		delete pitchNode;
		pitchNode = NULL;
	}
	if(yawNode)
	{
		delete yawNode;
		yawNode = NULL;
	}
	if(node)
	{
		delete node;
		node = NULL;
	}
}

void
Camera::pitch(Ogre::Radian angle)
{
	Ogre::Real pitchAngle;
	Ogre::Real sign;
	
	if(!pitchNode)
	{
		return;
	}
	pitchNode->pitch(angle);
	if(limitPitch)
	{
		/* Limit pitch angle to between -90 and +90 (i.e., you can't look)
		 * behind you by pitching.
		 */
		pitchAngle = (2 * Ogre::Degree(Ogre::Math::ACos(pitchNode->getOrientation().w)).valueDegrees());
		
		sign = pitchNode->getOrientation().x;
		
		if(pitchAngle > 90.0f)
		{
			if(sign > 0)
			{
				pitchNode->setOrientation(
					Ogre::Quaternion(
						Ogre::Math::Sqrt(0.5f), Ogre::Math::Sqrt(0.5f), 0, 0
				));
			}
			else if (sign < 0)
			{
				pitchNode->setOrientation(
					Ogre::Quaternion(
						Ogre::Math::Sqrt(0.5f), -Ogre::Math::Sqrt(0.5f), 0, 0
				));
			}
		}
	}
}
