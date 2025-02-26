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

#ifndef JYUZAU_CONTROLLER_HH_
# define JYUZAU_CONTROLLER_HH_         1

# include "jyuzau/defs.hh"

# include <OIS/OISEvents.h>
# include <OIS/OISInputManager.h>
# include <OIS/OISKeyboard.h>
# include <OIS/OISMouse.h>

namespace Jyuzau
{

	class Core;
	class Actor;

	/* The Controller maps keyboard, mouse and joypad events to playable
	 * actors' actions.
	 */
	class Controller: public OIS::KeyListener, public OIS::MouseListener
	{
	public:
		Controller();
		virtual ~Controller();
		
		virtual void bind(Actor *actor);
		virtual void unbindAll(void);
		
		/* Event injection */
		virtual bool keyPressed(const OIS::KeyEvent &arg);
		virtual bool keyReleased(const OIS::KeyEvent &arg);
		virtual bool mouseMoved(const OIS::MouseEvent &arg);
		virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
		virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	protected:
		std::vector<Actor *> m_actors;
	};

};

#endif /*!JYUZAU_CONTROLLER_HH_*/
