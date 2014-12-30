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
	actor(NULL)
{
	camera = sceneManager->createCamera(name);
}

Camera::~Camera()
{
	if(viewport)
	{
		viewport->getTarget()->removeViewport(viewport->getZOrder());
	}
	if(rollNode)
	{
		camera->detachFromParent();
		delete rollNode;
	}
	if(pitchNode)
	{
		delete pitchNode;
	}
	if(yawNode)
	{
		delete yawNode;
	}
	if(node)
	{
		delete node;
	}
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
