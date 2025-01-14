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

#include "../demoapp.hh"
#include "jyuzau/sceneview.hh"

class SceneViewApp: public DemoApp
{
public:
	SceneViewApp(): 
		DemoApp()
	{
		m_caption = "Jyuzau Scene Viewer";
	}
	
protected:
	Jyuzau::State *viewState;
	
	virtual void createInitialState(void)
	{
		viewState = new Jyuzau::SceneViewState("demo");
		pushState(viewState);
	}
};

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	
INT WINAPI
	WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR strCmdLine, INT nShowCmd)
{
	SceneViewApp app;
	
	return Jyuzau::WinMain(hInst, hPrevInst, strCmdLine, nShowCmd, &app);
}

#else

int
main(int argc, char **argv)
{
	SceneViewApp app;

	return Jyuzau::main(argc, argv, &app);
}

#endif

#ifdef __cplusplus
}
#endif
