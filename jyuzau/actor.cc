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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreLogManager.h>

#include "jyuzau/actor.hh"
#include "jyuzau/character.hh"
#include "jyuzau/camera.hh"

using namespace Jyuzau;

Actor *
Actor::create(Ogre::String name, Scene *scene)
{
	Actor *p;
	
	p = new Actor(name);
	if(!p->load())
	{
		delete p;
		return NULL;
	}
	if(scene)
	{
		if(!p->attach(scene))
		{
			delete p;
			return NULL;
		}
	}
	return p;
}

Actor *
Actor::create(Ogre::String name, Ogre::SceneManager *sceneManager)
{
	Actor *p;
	
	p = new Actor(name);
	if(!p->load())
	{
		delete p;
		return NULL;
	}
	if(sceneManager)
	{
		if(!p->attach(sceneManager))
		{
			delete p;
			return NULL;
		}
	}
	return p;
}

Actor::Actor(Ogre::String name):
	Prop::Prop(name, "actor"),
	m_character(NULL),
	m_health(100.0f),
	m_level(1),
	m_forward(false), m_backward(false), m_left(false), m_right(false),
	m_clockwise(false), m_cclockwise(false),
	m_lookUp(false), m_lookDown(false),
	m_speed(MS_WALK),
	m_topSpeed(150.0f),
	m_velocity(Ogre::Vector3::ZERO),
	m_rotVelocity(0),
	m_camPitchVelocity(0)
{
	resetActiveCameras();
}

Actor::~Actor()
{
}

Character *
Actor::character(void)
{
	return m_character;
}

void
Actor::characterAttached(void)
{
	m_level = m_character->level();
}

void
Actor::characterDetached(void)
{
}

Camera *
Actor::createCamera(CameraType type)
{
	Camera *cam;
	
	if(type >= CT_COUNT)
	{
		return NULL;
	}
	if(!m_node)
	{
		/* Can't create a camera if we don't yet have a node */
		return NULL;
	}
	cam = new Camera(m_group + "::camera[" + std::to_string((int) type) + "]", m_node->getCreator());
	cam->cameraType = type;
	cam->attach(m_node);
	if(type == CT_FIRSTPERSON)
	{
		cam->limitPitch = true;
	}
	cam->node->translate(0, 75, 150);
	/* XXX adjust position, orientation, etc. */
	setActiveCamera(cam);
	return cam;
}

void
Actor::resetActiveCameras(void)
{
	memset(m_cameras, 0, sizeof(m_cameras));
}

void
Actor::setActiveCamera(Camera *cam)
{
	if(cam->cameraType >= CT_COUNT)
	{
		return;
	}
	m_cameras[cam->cameraType] = cam;
}

bool
Actor::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	Ogre::Vector3 accel;
	Ogre::Real taccel;
	Ogre::Quaternion orientation;
	Ogre::Real squared;
	Ogre::Real minVelocity = std::numeric_limits<Ogre::Real>::epsilon();
	Ogre::Real topSpeed = (m_speed == MS_RUN ? m_topSpeed * 20 : (m_speed == MS_CREEP ? m_topSpeed / 5 : m_topSpeed));
	
	if(!m_node)
	{
		return false;
	}
	/* Process movement based upon m_forward, m_backward, m_left, m_right and
	 * m_speed
	 */
	orientation = m_node->getOrientation();
	accel = Ogre::Vector3::ZERO;
	if(m_forward)
	{
		accel -= orientation.zAxis();
	}
	if(m_backward)
	{
		accel += orientation.zAxis();
	}
	if(m_left)
	{
		accel -= orientation.xAxis();
	}
	if(m_right)
	{
		accel += orientation.xAxis();
	}
	if(accel.squaredLength() != 0)
	{
		accel.normalise();
		m_velocity += accel * topSpeed * evt.timeSinceLastFrame * 10;
	}
	else
	{
		m_velocity -= m_velocity * evt.timeSinceLastFrame * 10;
	}
	squared = m_velocity.squaredLength();
	if(squared > topSpeed * topSpeed)
	{
		m_velocity.normalise();
		m_velocity *= topSpeed;
	}
	else if(squared < minVelocity * minVelocity)
	{
		m_velocity = Ogre::Vector3::ZERO;
	}
	if(m_velocity != Ogre::Vector3::ZERO)
	{
		m_node->translate(m_velocity * evt.timeSinceLastFrame);
	}
	/* Process rotation */
	taccel = 0.0f;
	topSpeed = 4.0f;
	if(m_clockwise)
	{
		taccel -= 0.25f;
	}
	if(m_cclockwise)
	{
		taccel += 0.25f;
	}
	if(taccel != 0.0f)
	{
		m_rotVelocity += taccel * topSpeed * evt.timeSinceLastFrame * 10;
	}
	else
	{
		m_rotVelocity -= m_rotVelocity * evt.timeSinceLastFrame * 10;
	}
	if(std::abs(m_rotVelocity) > topSpeed)
	{
		m_rotVelocity = topSpeed * (m_rotVelocity < 0.0f ? -1.0f : 1.0f);
	}
	else if(std::abs(m_rotVelocity) < minVelocity)
	{
		m_rotVelocity = 0.0f;
	}
	if(m_rotVelocity != 0.0f)
	{
		m_node->yaw((Ogre::Radian) (m_rotVelocity * evt.timeSinceLastFrame));
	}
	/* Process camera pitching */
	taccel = 0.0f;
	topSpeed = 4.0f;
	if(m_lookUp)
	{
		taccel += 0.25f;
	}
	if(m_lookDown)
	{
		taccel -= 0.25f;
	}
	if(taccel != 0.0f)
	{
		m_camPitchVelocity += taccel * topSpeed * evt.timeSinceLastFrame * 10;
	}
	else
	{
		m_camPitchVelocity -= m_camPitchVelocity * evt.timeSinceLastFrame * 10;
	}
	if(std::abs(m_rotVelocity) > topSpeed)
	{
		m_camPitchVelocity = topSpeed * (m_camPitchVelocity < 0.0f ? -1.0f : 1.0f);
	}
	else if(std::abs(m_camPitchVelocity) < minVelocity)
	{
		m_camPitchVelocity = 0.0f;
	}
	if(m_camPitchVelocity != 0.0f)
	{
		if(m_cameras[CT_FIRSTPERSON])
		{
			m_cameras[CT_FIRSTPERSON]->pitch((Ogre::Radian) (m_camPitchVelocity * evt.timeSinceLastFrame));
		}
	}
	
	return true;
}

void
Actor::forward(MoveSpeed speed)
{
	if(!m_node)
	{
		return;
	}
	m_node->translate(0, 0, -CONTROL_WALK_DISTANCE, Ogre::Node::TS_LOCAL);
}

void
Actor::beginForward(MoveSpeed speed)
{
	if(!m_node)
	{
		return;
	}
	m_forward = true;
	m_speed = speed;
}

void
Actor::endForward(void)
{
	if(!m_node)
	{
		return;
	}
	m_forward = false;
}

void
Actor::backward(MoveSpeed speed)
{
	if(!m_node)
	{
		return;
	}
	m_node->translate(0, 0, CONTROL_WALK_DISTANCE, Ogre::Node::TS_LOCAL);
}

void
Actor::beginBackward(MoveSpeed speed)
{
	if(!m_node)
	{
		return;
	}
	m_backward = true;
	m_speed = speed;
}

void
Actor::endBackward(void)
{
	if(!m_node)
	{
		return;
	}
	m_backward = false;
}

void
Actor::turnLeft(MoveSpeed speed)
{
	if(!m_node)
	{
		return;
	}
	m_node->yaw((Ogre::Radian) CONTROL_YAW_ANGLE);
}

void
Actor::beginTurnLeft(void)
{
	if(!m_node)
	{
		return;
	}
	m_cclockwise = true;
}

void
Actor::endTurnLeft(void)
{
	if(!m_node)
	{
		return;
	}
	m_cclockwise = false;
}

void
Actor::turnRight(MoveSpeed speed)
{
	if(!m_node)
	{
		return;
	}
	m_node->yaw((Ogre::Radian) -CONTROL_YAW_ANGLE);
}

void
Actor::beginTurnRight(void)
{
	if(!m_node)
	{
		return;
	}
	m_clockwise = true;
}

void
Actor::endTurnRight(void)
{
	if(!m_node)
	{
		return;
	}
	m_clockwise = false;
}

void
Actor::turnLeftRight(int distance)
{
	int i;
	Ogre::Radian angle;
	
	if(!m_node)
	{
		return;
	}
	angle = (Ogre::Radian) -distance * CONTROL_YAW_FACTOR;
	m_node->yaw(angle);
}

void
Actor::strafeLeft(MoveSpeed speed)
{
	if(!m_node)
	{
		return;
	}
	m_node->translate(-CONTROL_WALK_DISTANCE, 0, 0);
}

void
Actor::beginStrafeLeft(MoveSpeed speed)
{
	if(!m_node)
	{
		return;
	}
	m_left = true;
	m_speed = speed;
}

void
Actor::endStrafeLeft(void)
{
	if(!m_node)
	{
		return;
	}
	m_left = false;
}


void
Actor::strafeRight(MoveSpeed speed)
{
	if(!m_node)
	{
		return;
	}
	m_node->translate(CONTROL_WALK_DISTANCE, 0, 0);
}

void
Actor::beginStrafeRight(MoveSpeed speed)
{
	if(!m_node)
	{
		return;
	}
	m_right = true;
	m_speed = speed;
}

void
Actor::endStrafeRight(void)
{
	if(!m_node)
	{
		return;
	}
	m_right = false;
}

void
Actor::lookUp(void)
{
	if(!m_node)
	{
		return;
	}
	if(m_cameras[CT_FIRSTPERSON])
	{
		m_cameras[CT_FIRSTPERSON]->pitch((Ogre::Radian) -CONTROL_PITCH_ANGLE);
	}
}

void
Actor::beginLookUp(void)
{
	if(!m_node)
	{
		return;
	}
	m_lookUp = true;
}

void
Actor::endLookUp(void)
{
	if(!m_node)
	{
		return;
	}
	m_lookUp = false;
}

void
Actor::lookDown(void)
{
	if(!m_node)
	{
		return;
	}
	if(m_cameras[CT_FIRSTPERSON])
	{
		m_cameras[CT_FIRSTPERSON]->pitch((Ogre::Radian) CONTROL_PITCH_ANGLE);
	}
}

void
Actor::beginLookDown(void)
{
	if(!m_node)
	{
		return;
	}
	m_lookDown = true;
}

void
Actor::endLookDown(void)
{
	if(!m_node)
	{
		return;
	}
	m_lookDown = false;
}


void
Actor::lookUpDown(int distance)
{
	int i;
	Ogre::Radian angle;
	
	if(!m_node)
	{
		return;
	}
	angle = (Ogre::Radian) distance * CONTROL_PITCH_FACTOR;
	if(m_cameras[CT_FIRSTPERSON])
	{
		m_cameras[CT_FIRSTPERSON]->pitch(angle);
	}
}

void
Actor::resetCamera(void)
{
	if(!m_node)
	{
		return;
	}
	if(m_cameras[CT_FIRSTPERSON])
	{
		m_cameras[CT_FIRSTPERSON]->pitchNode->resetOrientation();
	}
}

void
Actor::primaryFire(void)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::secondaryFire(void)
{
	if(!m_node)
	{
		return;
	}
	
}

void
Actor::special(void)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::crouch(void)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::jump(void)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::zoom(void)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::switchWeapon(int index)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::prevWeapon(void)
{
	if(!m_node)
	{
		return;
	}
}

void
Actor::nextWeapon(void)
{
	if(!m_node)
	{
		return;
	}
}
