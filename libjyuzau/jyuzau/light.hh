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

#ifndef JYUZAU_LIGHT_HH_
# define JYUZAU_LIGHT_HH_              1

# include "jyuzau/node.hh"

# include <OGRE/OgreVector3.h>
# include <OGRE/OgreColourValue.h>
# include <OGRE/OgreSceneManager.h>
# include <OGRE/OgreString.h>

namespace Jyuzau
{
	class Scene;
	
	class Light: public Node
	{
	public:
		Light(const Light &object);
		Light(Ogre::String name, State *state, Ogre::String kind = "light");
		virtual ~Light();
		
		virtual Loadable *clone(void) const;
		virtual Ogre::Light *light(void) const;
	protected:
		Ogre::Light *m_light;

		virtual bool complete(void) const;

		virtual bool attachToSceneNode(Scene *scene, Ogre::SceneNode *parentNode, Ogre::String id);
		virtual void detach(void);
		virtual bool loadDocument(Ogre::String path);
	};
};

#endif /*!JYUZAU_LIGHT_HH_*/
