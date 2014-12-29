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

#ifndef DEMOAPP_HH_
# define DEMOAPP_HH_

# include "jyuzau.hh"

# include <OGRE/SdkTrays.h>

class DemoApp: public Jyuzau::Core, public OgreBites::SdkTrayListener
{
public:
	DemoApp();
	virtual ~DemoApp();
protected:
	OgreBites::InputContext m_inputContext;
	OgreBites::SdkTrayManager *m_trayMgr;
	OgreBites::ParamsPanel *m_detailsPanel;
	
	virtual void setupResources(void);
	
	virtual void createFrameListener(void);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	
	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
};

#endif /*!DEMOAPP_HH_*/
