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