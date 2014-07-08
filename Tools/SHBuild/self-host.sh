#!/usr/bin/sh
# (C) 2014 FrankHB.
# Test script for self-hosting SHBuild.

set -e

# See "build.sh".

CXXFLAGS="-O3 -pipe -DNDEBUG -std=c++11 -Wall"

LDFLAGS="-s -Wl,--dn -Wl,--gc-sections"

INCLUDES=" \
	-I../../YFramework/include -I../../YFramework/Android/include \
	-I../../YFramework/DS/include -I../../YFramework/MinGW32/include \
	-I../../3rdparty/include -I../../YBase/include \
	"

# Coordinated with build 515.
LIBS=" \
	../../YBase/source/ystdex/cstdio.cpp \
	../../YBase/source/ystdex/any.cpp \
	../../YFramework/source/CHRLib/chrproc.cpp \
	../../YFramework/source/CHRLib/MapEx.cpp \
	../../YFramework/source/YCLib/FileSystem.cpp \
	../../YFramework/source/YCLib/Host.cpp \
	../../YFramework/source/YSLib/Core/yexcept.cpp \
	../../YFramework/source/YSLib/Service/FileSystem.cpp \
	../../YFramework/MinGW32/source/YCLib/MinGW32.cpp \
	-lShlWapi \
	"

./shbuild . ${CXXFLAGS} ${INCLUDES}
g++ .shbuild/SHBuild.a -o.shbuild/SHBuild \
	${CXXFLAGS} ${LDFLAGS} ${INCLUDES} ${LIBS}

echo Done.

