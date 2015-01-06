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

#include "jyuzau/controller.hh"
#include "jyuzau/actor.hh"

#include <OGRE/OgreLogManager.h>

using namespace Jyuzau;

Controller::Controller():
	m_actors()
{
}

Controller::~Controller()
{
}

void
Controller::bind(Actor *actor)
{
	m_actors.push_back(actor);
}

void
Controller::unbindAll(void)
{
	m_actors.clear();
}

/* For the time being, we'll apply static bindings. Later, they will be
 * configurable:
 *
 * Forward          [Up]
 * Backward         [Down]
 * Turn left        [Left] or [Mouse-x]
 * Turn right       [Right] or [Mouse-x]
 * Look up          [Z] or [Mouse-y]
 * Look down        [A] or [Mouse-y]
 * Run              [Alt]
 * Fire             [Ctrl] or [Mouse-1]
 * Zoom             [Shift] or [Mouse-2]
 * Action           [E]
 * Secondary fire   [G]
 * Special          [F]
 * Strafe left      [,]
 * Strafe right     [.]
 * Crouch           [C]
 * Jump             [Space]
 * Reset view       [X]
 * Previous weapon  [[]
 * Next weapon      []]
 * Weapon 1         [1]
 * Weapon 2         [2]
 * Weapon 3         [3] (etc.)
 *
 * The following bindings also exist, but aren't handled by the Controller:
 *
 * Inventory        [Tab] or [I]
 * Map              [M]
 * Pause menu       [Esc]
 * Console          [§] or [`]
 */

bool
Controller::keyPressed(const OIS::KeyEvent &arg)
{
	if(m_actors.size() > 0)
	{
		/* Player 1 */
		switch(arg.key)
		{
			case OIS::KC_UP:
				m_actors[0]->beginForward();
				return true;
			case OIS::KC_DOWN:
				m_actors[0]->beginBackward();
				return true;
			case OIS::KC_LEFT:
				m_actors[0]->beginTurnLeft();
				return true;
			case OIS::KC_RIGHT:
				m_actors[0]->beginTurnRight();
				return true;
			case OIS::KC_Z:
				m_actors[0]->beginLookUp();
				return true;
			case OIS::KC_A:
				m_actors[0]->beginLookDown();
				return true;
			case OIS::KC_COMMA:
				m_actors[0]->beginStrafeLeft();
				return true;
			case OIS::KC_PERIOD:
				m_actors[0]->beginStrafeRight();
				return true;
			case OIS::KC_X:
				m_actors[0]->resetCamera();
				return true;
			case OIS::KC_LSHIFT:
			case OIS::KC_RSHIFT:
				m_actors[0]->beginRun();
				break;
			default:
				/* No-op */;
		}
	}
	return false;
}

bool
Controller::keyReleased(const OIS::KeyEvent &arg)
{
	if(m_actors.size() > 0)
	{
		/* Player 1 */
		switch(arg.key)
		{
			case OIS::KC_UP:
				m_actors[0]->endForward();
				return true;
			case OIS::KC_DOWN:
				m_actors[0]->endBackward();
				return true;
			case OIS::KC_LEFT:
				m_actors[0]->endTurnLeft();
				return true;
			case OIS::KC_RIGHT:
				m_actors[0]->endTurnRight();
				return true;
			case OIS::KC_Z:
				m_actors[0]->endLookUp();
				return true;
			case OIS::KC_A:
				m_actors[0]->endLookDown();
				return true;
			case OIS::KC_COMMA:
				m_actors[0]->endStrafeLeft();
				return true;
			case OIS::KC_PERIOD:
				m_actors[0]->endStrafeRight();
				return true;
			case OIS::KC_LSHIFT:
			case OIS::KC_RSHIFT:
				m_actors[0]->endRun();
				break;
			case OIS::KC_SPACE:
				m_actors[0]->jump();
				break;
			default:
				/* No-op */;
		}
	}
	return false;
}

bool
Controller::mouseMoved(const OIS::MouseEvent &arg)
{
	if(arg.state.X.rel > 50 || arg.state.X.rel < -50 ||
		arg.state.Y.rel > 50 || arg.state.Y.rel < -50)
	{
		/* Drop events which are out of range to prevent wild movement */
		return false;
	}
	if(m_actors.size() > 0)
	{
		/* Player 1 */
		m_actors[0]->turnLeftRight(arg.state.X.rel);
		m_actors[0]->lookUpDown(arg.state.Y.rel);
		return true;
	}
	return false;
}

bool
Controller::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return false;
}

bool
Controller::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return false;
}

