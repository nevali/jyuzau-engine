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

#ifndef JYUZAU_LIGHT_HH_
# define JYUZAU_LIGHT_HH_              1

# include <OGRE/OgreVector3.h>
# include <OGRE/OgreColourValue.h>
# include <OGRE/OgreSceneManager.h>
# include <OGRE/OgreString.h>

# include "jyuzau/loadable.hh"

namespace Jyuzau
{
	class Scene;
	
	class Light: public Loadable
	{
	public:
		static Light *create(Ogre::String name, Scene *scene = NULL, Ogre::Vector3 pos = Ogre::Vector3::ZERO);
	
		Light(Ogre::String name, Ogre::String kind = "light");
		virtual ~Light();
		
		virtual bool attach(Scene *scene, Ogre::String name, Ogre::Vector3 pos);
		virtual bool attach(Ogre::SceneManager *manager, Ogre::String name, Ogre::Vector3 pos);
		virtual bool detach(void);
	protected:
		Ogre::Light *m_light;

		virtual bool load(void);
		virtual bool loadDocument(Ogre::String path);
	};
};

#endif /*!JYUZAU_LIGHT_HH_*/
