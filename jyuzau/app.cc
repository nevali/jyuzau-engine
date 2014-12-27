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

#include "jyuzau.hh"

/* Implement main() for non-Cocoa Unix platforms */
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE && OGRE_PLATFORM != OGRE_PLATFORM_WIN32
int
Jyuzau::main(int argc, char **argv, Jyuzau::Core *app)
{
	try
	{
		app->go();
	}
	catch(Ogre::Exception& e)
	{
		std::cerr << "An exception has occurred: " <<
e.getFullDescription().c_str() << std::endl;
	}
}
#endif

/* Implement main() for Windows platforms */
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT
Jyuzau::WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nShowCmd, Jyuzau::Core *app)
{
	try
	{
		app->go();
	}
	catch(Ogre::Exception& e)
	{
		MessageBox(NULL, e.getFullDescription().c_str(), "Jyuzau: Exception", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}	
}
#endif
