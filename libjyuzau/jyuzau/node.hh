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

#ifndef JYUZAU_NODE_HH_
# define JYUZAU_NODE_HH_               1

# include "jyuzau/loadable.hh"

# include <OGRE/OgreSceneManager.h>

namespace Jyuzau
{
	class Scene;
	
	/* A Node is a base class for objects which can be attached to scenes
	 * and can have child objects attached, including areas, props, actors,
	 * and so forth.
	 *
	 * A Node on its own does not interact with the dynamics world, only
	 * subclasses do.
	 */
	class Node: public Loadable
	{
	public:
		Node(const Node &object);
		Node(Ogre::String className, State *state, Ogre::String kind, bool subdir);
		~Node();
		
		virtual Scene *scene(void) const;
		virtual Ogre::SceneNode *sceneNode(void) const;
		
		virtual bool attachToScene(Scene *scene, Ogre::String id = "");
		virtual bool attachToScene(Node *scene, Ogre::String id = "");

		virtual void setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z);
		virtual void setPosition(const Ogre::Vector3 &vec);
		virtual void setOrientation(const Ogre::Quaternion &quaternion);

		virtual void scale(const Ogre::Vector3 &vec);
		virtual void translate(const Ogre::Vector3 &vec);
		virtual void yaw(const Ogre::Radian &angle);
		virtual void pitch(const Ogre::Radian &angle);
		virtual void roll(const Ogre::Radian &angle);
	protected:
		Scene *m_scene;
		Ogre::SceneNode *m_node;
		Ogre::Vector3 m_position;
		Ogre::Vector3 m_scale;
		Ogre::Quaternion m_orientation;
		
		virtual void detach(void);
		virtual bool attachToSceneNode(Scene *scene, Ogre::SceneNode *parentNode, Ogre::String id);
		virtual bool createNode(Ogre::SceneNode *parentNode, Ogre::String id);
	};
};

#endif /*!JYUZAU_NODE_HH_*/
