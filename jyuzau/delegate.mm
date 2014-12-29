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

/* Provide an application delegate implementation which can be used with
 * a JyuzauCore instance.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "jyuzau/delegate.hh"

@implementation JyuzauDelegate

@synthesize mTimer;
@dynamic mLastFrameTime;

- (JyuzauDelegate *) init:(Jyuzau::Core *) app
{
	[super init];
	mApp = app;
	return self;
}

- (void) dealloc
{
	if(mTimer)
	{
		[mTimer invalidate];
		mTimer = nil;
	}
    [super dealloc];
}

- (NSTimeInterval) mLastFrameTime
{
	return mLastFrameTime;
}

- (void) setLastFrameTime:(NSTimeInterval) frameInterval
{
	/* Frame interval defines how many display frames must pass between each
	   time the display link fires. The display link will only fire 30 times
	   a second when the frame internal is two on a display that refreshes 60
	   times a second. The default frame interval setting of one will fire 60
	   times a second when the display refreshes at 60 times a second. A frame
	   interval setting of less than one results in undefined behavior.
	*/
	if (frameInterval >= 1)
	{
		mLastFrameTime = frameInterval;
	}
}

- (void) applicationDidFinishLaunching:(NSNotification *) application
{
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	mLastFrameTime = 1;
	mTimer = nil;
	bool r;
	
	try
	{
		r = mApp->init();
	}
	catch(Ogre::Exception& e)
	{
		NSLog(@"Jyuzau: Core initialisation threw exception: %s", e.getFullDescription().c_str());
		r = false;
	}
	if(!r)
	{
		NSLog(@"Jyuzau: Core initialisation failed, terminating application");
		[NSApp performSelector:@selector(terminate:) withObject:nil afterDelay:0.0];
		return;
	}
	mTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)(1.0f / 60.0f) * mLastFrameTime
		target:self
		selector:@selector(render:)
		userInfo:nil
		repeats:YES];
	[pool release];
}

- (void) render:(id) sender
{
	if(!mApp->render((Ogre::Real) [mTimer timeInterval]))
	{
		NSLog(@"Jyuzau: Core has shut down, terminating application");
		[mTimer invalidate];
		mTimer = nil;
		[NSApp performSelector:@selector(terminate:) withObject:nil afterDelay:0.0];
	}
}

@end
