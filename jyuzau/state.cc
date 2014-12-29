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

#include <utility>

#include "jyuzau/state.hh"
#include "jyuzau/core.hh"

using namespace Jyuzau;

State::State():
	m_prev(NULL), m_next(NULL)
{
}

State::~State()
{
	Core::getInstance()->removeState(this);
}

void
State::activated(void)
{
}

void
State::deactivated(void)
{
}

bool
State::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	return true;
}

/* OIS event handlers */
bool
State::keyPressed(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_ESCAPE)
	{
		Core::getInstance()->shutdown();
	}
	return true;
}

bool
State::keyReleased(const OIS::KeyEvent &arg)
{
	return true;
}

bool
State::mouseMoved(const OIS::MouseEvent &arg)
{
	return true;
}

bool
State::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}

bool
State::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}

