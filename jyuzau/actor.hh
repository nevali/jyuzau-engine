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

#ifndef JYUZAU_ACTOR_HH_
# define JYUZAU_ACTOR_HH_              1

# include "loadable.hh"

namespace Jyuzau
{
	
	/* An actor is a kind of prop which can have autonomous behaviours
	 * and cameras attached to it.
	 */
	class Actor: public Prop
	{
	public:
		static Actor *create(Ogre::String name, Scene *scene = NULL);
		
		Actor(Ogre::String name);
		virtual ~Actor();
	};
};

#endif /*!JYUZAU_ACTOR_HH_*/