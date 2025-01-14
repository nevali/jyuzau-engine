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

#include <Cocoa/Cocoa.h>

#include "jyuzau/main.hh"
#include "jyuzau/delegate.hh"

/* Implement main() for Cocoa applications */
int
Jyuzau::main(int argc, char **argv, Jyuzau::Core *app)
{
	id delegate;
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	delegate = [[JyuzauDelegate alloc] init:app];
	[[NSApplication sharedApplication] setDelegate:delegate];
	int retVal = NSApplicationMain(argc, (const char **) argv);

	[pool release];
	
	return retVal;
}
