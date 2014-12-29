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

#ifndef JYUZAU_STATE_HH_
# define JYUZAU_STATE_HH_              1

# include <OGRE/OgreFrameListener.h>

# include <OIS/OISEvents.h>
# include <OIS/OISKeyboard.h>
# include <OIS/OISMouse.h>

namespace Jyuzau
{

	/* The State class encapsulates the game logic at any given point,
	 * including cut-scenes, menus, and so on.
	 *
	 * States can be stacked (i.e., pushed and popped), as well as
	 * replaced when needed, via Jyuzau::Core methods.
	 */
	class State
	{
		friend class Core;
	public:
		State();
		virtual ~State();
	protected:
		State *m_prev, *m_next;
		
		virtual void activated(void);
		virtual void deactivated(void);
		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
		virtual bool keyPressed(const OIS::KeyEvent &arg);
		virtual bool keyReleased(const OIS::KeyEvent &arg);
		virtual bool mouseMoved(const OIS::MouseEvent &arg);
		virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
		virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	};

};

#endif /*!JYUZAU_STATE_HH_*/