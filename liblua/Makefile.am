ACLOCAL_AMFLAGS = -I ../m4

AM_CPPFLAGS = @AM_CPPFLAGS@ -I${srcdir}/upstream/src

bin_PROGRAMS = lua luac

lib_LTLIBRARIES = liblua.la

include_HEADERS = \
	upstream/src/lua.h upstream/src/luaconf.h upstream/src/lualib.h \
	upstream/src/lauxlib.h upstream/src/lua.hpp

lua_SOURCES = upstream/src/lua.c
lua_LDADD = liblua.la

luac_SOURCES = upstream/src/luac.c
luac_LDADD = liblua.la

man1_MANS = upstream/doc/lua.1 upstream/doc/luac.1

liblua_la_SOURCES = \
	upstream/src/lapi.c upstream/src/lcode.c upstream/src/lctype.c \
	upstream/src/ldebug.c upstream/src/ldo.c upstream/src/ldump.c \
	upstream/src/lfunc.c upstream/src/lgc.c upstream/src/llex.c \
	upstream/src/lmem.c upstream/src/lobject.c upstream/src/lopcodes.c \
	upstream/src/lparser.c upstream/src/lstate.c upstream/src/lstring.c \
	upstream/src/ltable.c upstream/src/ltm.c upstream/src/lundump.c \
	upstream/src/lvm.c upstream/src/lzio.c \
	\
	upstream/src/lauxlib.c upstream/src/lbaselib.c upstream/src/lbitlib.c \
	upstream/src/lcorolib.c upstream/src/ldblib.c upstream/src/liolib.c \
	upstream/src/lmathlib.c upstream/src/loslib.c upstream/src/lstrlib.c \
	upstream/src/ltablib.c upstream/src/loadlib.c upstream/src/linit.c

liblua_la_LDFLAGS = -avoid-version
