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
	JyuzauCore *mApp;
}
- (JyuzauDelegate *) init:(JyuzauCore *) app;
- (void) renderOneFrame:(id) sender;

@property (retain) NSTimer *mTimer;
@property (nonatomic) NSTimeInterval mLastFrameTime;

@end

# endif /*__OBJC__*/

#endif /*!JYUZAU_DELEGATE_HH_*/