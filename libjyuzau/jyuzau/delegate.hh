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

#ifndef JYUZAU_DELEGATE_HH_
# define JYUZAU_DELEGATE_HH_           1

# include "core.hh"

# include <Cocoa/Cocoa.h>

# ifdef __OBJC__

@interface JyuzauDelegate : NSObject <NSApplicationDelegate>
{
	NSTimer *mTimer;
	NSDate *mDate;
	NSTimeInterval mLastFrameTime;
	Jyuzau::Core *mApp;
}
- (JyuzauDelegate *) init:(Jyuzau::Core *) app;
- (void) render:(id) sender;

@property (retain) NSTimer *mTimer;
@property (nonatomic) NSTimeInterval mLastFrameTime;

@end

# endif /*__OBJC__*/

#endif /*!JYUZAU_DELEGATE_HH_*/