/* Copyright 2014 Mo McRoberts.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *	  http://www.apache.org/licenses/LICENSE-2.0
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

#include "demoapp.hh"

DemoApp::DemoApp():
	Core::Core(),
	m_trayMgr(NULL),
	m_detailsPanel(NULL)
{
}

DemoApp::~DemoApp()
{
	if(m_trayMgr)
	{
		delete m_trayMgr;
	}
}

void
DemoApp::createFrameListener(void)
{
	Core::createFrameListener();
	
	m_inputContext.mKeyboard = mKeyboard;
	m_inputContext.mMouse = mMouse;
	
	m_trayMgr = new OgreBites::SdkTrayManager("InterfaceName", mWindow, m_inputContext, this);
	m_trayMgr->showFrameStats(OgreBites::TL_TOPLEFT);
	m_trayMgr->hideCursor();
	m_trayMgr->toggleAdvancedFrameStats();

	// Create a params panel for displaying sample details
	Ogre::StringVector items;
	items.push_back("cam.pX");
	items.push_back("cam.pY");
	items.push_back("cam.pZ");
	items.push_back("");
	items.push_back("cam.oW");
	items.push_back("cam.oX");
	items.push_back("cam.oY");
	items.push_back("cam.oZ");
	items.push_back("");
	items.push_back("Filtering");
	items.push_back("Poly Mode");

	m_detailsPanel = m_trayMgr->createParamsPanel(OgreBites::TL_NONE, "DetailsPanel", 200, items);
	m_detailsPanel->setParamValue(9, "Bilinear");
	m_detailsPanel->setParamValue(10, "Solid");
	m_detailsPanel->hide();
}

bool
DemoApp::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	bool r;
	
	r = Core::frameRenderingQueued(evt);
	if(!r)
	{
		return false;
	}
	m_trayMgr->frameRenderingQueued(evt);

	if (!m_trayMgr->isDialogVisible())
	{
		if (m_detailsPanel->isVisible())
		{
			m_detailsPanel->setParamValue(0, Ogre::StringConverter::toString(mCamera->getDerivedPosition().x));
			m_detailsPanel->setParamValue(1, Ogre::StringConverter::toString(mCamera->getDerivedPosition().y));
			m_detailsPanel->setParamValue(2, Ogre::StringConverter::toString(mCamera->getDerivedPosition().z));
			m_detailsPanel->setParamValue(4, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().w));
			m_detailsPanel->setParamValue(5, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().x));
			m_detailsPanel->setParamValue(6, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().y));
			m_detailsPanel->setParamValue(7, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().z));
		}
	}
	return true;
}

/* Bind a set of default keys, send everything else to the camera */
bool
DemoApp::keyPressed( const OIS::KeyEvent &arg )
{
	if (m_trayMgr->isDialogVisible())
	{
		return true;
	}
	if (arg.key == OIS::KC_F)
	{
		m_trayMgr->toggleAdvancedFrameStats();
	}
	else if (arg.key == OIS::KC_G)
	{
		if (m_detailsPanel->getTrayLocation() == OgreBites::TL_NONE)
		{
			m_trayMgr->moveWidgetToTray(m_detailsPanel, OgreBites::TL_TOPRIGHT, 0);
			m_detailsPanel->show();
		}
		else
		{
			m_trayMgr->removeWidgetFromTray(m_detailsPanel);
			m_detailsPanel->hide();
		}
	}
	else if (arg.key == OIS::KC_T)
	{
		Ogre::String newVal;
		Ogre::TextureFilterOptions tfo;
		unsigned int aniso;

		switch (m_detailsPanel->getParamValue(9).asUTF8()[0])
		{
		case 'B':
			newVal = "Trilinear";
			tfo = Ogre::TFO_TRILINEAR;
			aniso = 1;
			break;
		case 'T':
			newVal = "Anisotropic";
			tfo = Ogre::TFO_ANISOTROPIC;
			aniso = 8;
			break;
		case 'A':
			newVal = "None";
			tfo = Ogre::TFO_NONE;
			aniso = 1;
			break;
		default:
			newVal = "Bilinear";
			tfo = Ogre::TFO_BILINEAR;
			aniso = 1;
		}
		Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
		Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
		m_detailsPanel->setParamValue(9, newVal);
	}
	else if (arg.key == OIS::KC_R)
	{
		Ogre::String newVal;
		Ogre::PolygonMode pm;

		switch (mCamera->getPolygonMode())
		{
		case Ogre::PM_SOLID:
			newVal = "Wireframe";
			pm = Ogre::PM_WIREFRAME;
			break;
		case Ogre::PM_WIREFRAME:
			newVal = "Points";
			pm = Ogre::PM_POINTS;
			break;
		default:
			newVal = "Solid";
			pm = Ogre::PM_SOLID;
		}

		mCamera->setPolygonMode(pm);
		m_detailsPanel->setParamValue(10, newVal);
	}
	else if(arg.key == OIS::KC_F5)
	{
		Ogre::TextureManager::getSingleton().reloadAll();
	}
	else if (arg.key == OIS::KC_SYSRQ)
	{
		mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
	}
	return Jyuzau::Core::keyPressed(arg);
}

bool
DemoApp::mouseMoved(const OIS::MouseEvent &arg)
{
	if (m_trayMgr->injectMouseMove(arg)) return true;
	return Jyuzau::Core::mouseMoved(arg);
}

bool
DemoApp::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (m_trayMgr->injectMouseDown(arg, id)) return true;
	return Jyuzau::Core::mousePressed(arg, id);
}

bool
DemoApp::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (m_trayMgr->injectMouseUp(arg, id)) return true;
	return Jyuzau::Core::mouseReleased(arg, id);
}
