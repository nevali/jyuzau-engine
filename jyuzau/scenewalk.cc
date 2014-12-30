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

#include <OIS/OISEvents.h>
#include <OIS/OISInputManager.h>
#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

#include <OgreSceneManager.h>

#include "jyuzau/scenewalk.hh"
#include "jyuzau/scene.hh"
#include "jyuzau/core.hh"

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
	m_scene = Scene::create(m_name);
}

void
SceneWalkState::attachScenes(void)
{
	m_scene->attach(m_sceneManager);
}

bool
SceneWalkState::keyPressed(const OIS::KeyEvent &arg)
{
	return State::keyPressed(arg);
}

bool
SceneWalkState::keyReleased(const OIS::KeyEvent &arg)
{
	return State::keyReleased(arg);
}

bool
SceneWalkState::mouseMoved(const OIS::MouseEvent &arg)
{
	return State::mouseMoved(arg);
}

bool
SceneWalkState::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return State::mousePressed(arg, id);
}

bool
SceneWalkState::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return State::mouseReleased(arg, id);
}
