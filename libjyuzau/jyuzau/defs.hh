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

#ifndef JYUZAU_DEFS_HH_
# define JYUZAU_DEFS_HH_               1

# include <OGRE/OgreString.h>

# define CHAR_CURRENCY_MAX             5
# define CHAR_WEAPON_MAX               10

/* Actor movement defaults */
# define ACTOR_WALK_SPEED              150.0f
# define ACTOR_WALK_ACCEL              10.0f
# define ACTOR_WALK_DECEL              10.0f
# define ACTOR_WALK_DIST               5.0f
# define ACTOR_RUN_FACTOR              10.0f
# define ACTOR_CREEP_FACTOR            0.5f

# define ACTOR_TURN_SPEED              4.0f
# define ACTOR_TURN_STEP               0.25f
# define ACTOR_TURN_ACCEL              10.0f
# define ACTOR_TURN_DECEL              10.0f
# define ACTOR_TURN_ANGLE              0.15f
# define ACTOR_TURN_FACTOR             0.005f

# define ACTOR_CAM_PITCH_SPEED         4.0f
# define ACTOR_CAM_PITCH_STEP          0.25f
# define ACTOR_CAM_PITCH_ACCEL         10.0f
# define ACTOR_CAM_PITCH_DECEL         10.0f
# define ACTOR_CAM_PITCH_ANGLE         0.15f
# define ACTOR_CAM_PITCH_FACTOR        0.005f

# define DYNAMICS_MAX_SUBSTEPS         4

namespace Ogre
{
	class Camera;
	class Viewport;
};

namespace Jyuzau
{
	class Character;

	enum CameraType {
		CT_UNSPEC,
		CT_FIRSTPERSON,
		CT_FOLLOWING,
		/* The following must always be last */
		CT_COUNT
	};
	
	enum MoveSpeed {
		MS_CURRENT,
		MS_CREEP,
		MS_WALK,
		MS_RUN
	};
	
	struct RosterEntry
	{
		Character *character;
		Ogre::String title;
	};
	
	struct StateViewportEntry
	{
		int zorder;
		Ogre::Camera *camera;
		Ogre::Viewport *vp;
	};

	typedef std::pair<Ogre::String, Ogre::String> Attr;
	typedef std::vector<Attr> AttrList;
	typedef AttrList::iterator AttrListIterator;

};

#endif /*!JYUZAU_DEFS_HH*/
