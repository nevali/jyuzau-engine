/* Copyright 2014-2015 Mo McRoberts.
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

#ifndef JYUZAU_SCENEWALK_HH_
# define JYUZAU_SCENEWALK_HH_          1

# include "jyuzau/state.hh"

namespace OgreBites {

	class SdkCameraMan;
	
};

namespace Jyuzau
{
	class Scene;
	
	/* A simple state engine which allows walking through a loaded scene as
	 * the first player.
	 */
	class SceneWalkState: public State
	{
	public:
		SceneWalkState(Ogre::String name);
		~SceneWalkState();
	protected:
		Ogre::String m_name;
		Scene *m_scene;
		
		virtual void createScenes(void);
		virtual void attachScenes(void);
	};
	
};

#endif /*!JYUZAU_SCENEWALK_HH_*/
