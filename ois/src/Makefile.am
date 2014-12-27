INCLUDES = $(STLPORT_CFLAGS) -I$(top_srcdir)/includes $(CFLAGS) -I/usr/X11R6/include

AM_CXXFLAGS = @AM_CXXFLAGS@ -x objective-c++

.mm.lo:
	$(LTCXXCOMPILE) -c -o $@ $<

lib_LTLIBRARIES=libOIS.la

if OIS_BACKEND_MAC

OIS_BACKEND = \
	mac/CocoaInputManager.mm \
	mac/CocoaJoyStick.mm \
	mac/CocoaKeyboard.mm \
	mac/CocoaMouse.mm \
	mac/MacHelpers.cpp \
	mac/MacHIDManager.cpp \
	mac/MacInputManager.cpp \
	mac/MacJoyStick.cpp \
	mac/MacKeyboard.cpp \
	mac/MacMouse.cpp

endif

if OIS_BACKEND_LINUX

OIS_BACKEND = \
	linux/EventHelpers.cpp \
	linux/LinuxInputManager.cpp \
	linux/LinuxJoyStickEvents.cpp \
	linux/LinuxForceFeedback.cpp \
	linux/LinuxKeyboard.cpp \
	linux/LinuxMouse.cpp

libOIS_la_LIBADD = $(STLPORT_LIBS) -L/usr/X11R6/lib -lX11

endif


libOIS_la_SOURCES = OISInputManager.cpp \
	OISObject.cpp \
	OISEffect.cpp \
	OISJoyStick.cpp \
	OISKeyboard.cpp \
	OISForceFeedback.cpp \
	OISException.cpp \
	$(OIS_BACKEND)

libOIS_la_LDFLAGS = -release @PACKAGE_VERSION@
#libOIS_la_LDFLAGS = -version-info $(shell echo "@PACKAGE_VERSION@" | tr '.' ':')

#eof "$Id: Makefile.am,v 1.15.2.1 2008/02/14 03:33:36 pjcast Exp $"
