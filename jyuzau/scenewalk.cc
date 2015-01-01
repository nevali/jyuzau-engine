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

#include "jyuzau/scenewalk.hh"
#include "jyuzau/scene.hh"
#include "jyuzau/core.hh"

#include <OIS/OISEvents.h>
#include <OIS/OISInputManager.h>
#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

#include <OGRE/OgreSceneManager.h>

using namespace Jyuzau;

SceneWalkState::SceneWalkState(Ogre::String name):
	State::State(),
	m_scene(NULL),
	m_name(name)
{
}

SceneWalkState::~SceneWalkState()
{
	if(m_scene)
	{
		delete m_scene;
	}
}

void
SceneWalkState::createScenes()
{
	m_scene = dynamic_cast<Scene *>(factory("scene", m_name));
}

void
SceneWalkState::attachScenes(void)
{
	m_scene->attach(m_sceneManager);
}
