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

#ifndef JYUZAU_SCENEVIEW_HH_
# define JYUZAU_SCENEVIEW_HH_          1

# include "jyuzau/state.hh"

namespace OgreBites {

	class SdkCameraMan;
	
};

namespace Jyuzau
{
	class Scene;
	
	/* A simple state engine which allows flying through a loaded scene */
	class SceneViewState: public State
	{
	public:
		SceneViewState(Ogre::String name);
		~SceneViewState();
	protected:
		Ogre::String m_name;
		Scene *m_scene;
		OgreBites::SdkCameraMan* m_cameraMan;
		
		virtual void createScenes(void);
		virtual void attachScenes(void);
		virtual void createPlayers(void);
		virtual void deletePlayers(void);
		
		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
		virtual bool keyPressed(const OIS::KeyEvent &arg);
		virtual bool keyReleased(const OIS::KeyEvent &arg);
		virtual bool mouseMoved(const OIS::MouseEvent &arg);
		virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
		virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	};
	
};

#endif /*!JYUZAU_SCENEVIEW_HH_*/
