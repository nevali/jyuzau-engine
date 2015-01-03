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

#ifndef JYUZAU_SPLASH_HH_
# define JYUZAU_SPLASH_HH_             1

# include "jyuzau/state.hh"

namespace Jyuzau
{
	class Scene;
	
	/* Present a splash screen while the next state loads */
	class SplashState: public State
	{
	public:
		SplashState();
	protected:
		virtual void activated(Ogre::RenderWindow *window);
	};
	
};

#endif /*!JYUZAU_SPLASH_HH_*/
