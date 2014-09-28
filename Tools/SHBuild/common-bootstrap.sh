#!/usr/bin/sh
# (C) 2014 FrankHB.
# Common source script: bootstrap configuration.

echo "Bootstrap beginned."

source ${SHBuild_BaseDir}/common.sh

SHBuild_EchoVar "SHBuild.BaseDir" "${SHBuild_BaseDir}"
SHBuild_EchoVar "SHBuild.Me" "${SHBuild_Me}"

echo "Configuring ..."

SHBuild_BuildDir="$SHBuild_BaseDir/.shbuild"
[[ ${CXX} ]] || CXX="g++"
[[ ${CXXFLAGS} ]]  || CXXFLAGS="-O3 -pipe -DNDEBUG -std=c++11 -Wall"
[[ ${LDFLAGS} ]]  || LDFLAGS="-s -Wl,--dn -Wl,--gc-sections"
INCLUDES=" \
	-I../../YFramework/include -I../../YFramework/Android/include \
	-I../../YFramework/DS/include -I../../YFramework/MinGW32/include \
	-I../../3rdparty/include -I../../YBase/include \
	"

# Coordinated with build 538.
LIBS=" \
	../../YBase/source/ystdex/cassert.cpp \
	../../YBase/source/ystdex/cstdio.cpp \
	../../YBase/source/ystdex/any.cpp \
	../../YBase/source/ystdex/concurrency.cpp \
	../../YFramework/source/CHRLib/chrmap.cpp \
	../../YFramework/source/CHRLib/chrproc.cpp \
	../../YFramework/source/CHRLib/MapEx.cpp \
	../../YFramework/source/YCLib/Debug.cpp \
	../../YFramework/source/YCLib/FileSystem.cpp \
	../../YFramework/source/YCLib/Host.cpp \
	../../YFramework/source/YSLib/Core/yexcept.cpp \
	../../YFramework/source/YSLib/Service/FileSystem.cpp \
	../../YFramework/MinGW32/source/YCLib/MinGW32.cpp \
	../../YFramework/MinGW32/source/YCLib/Consoles.cpp \
	-lShlWapi \
	"

SHBuild_EchoVar "SHBuild.BuildDir" "${SHBuild_BuildDir}"
SHBuild_EchoVar "CXX" "${CXX}"
SHBuild_EchoVar "CXXFLAGS" "${CXXFLAGS}"
SHBuild_EchoVar "LDFLAGS" "${LDFLAGS}"
SHBuild_EchoVar "INCLUDES" "${INCLUDES}"
SHBuild_EchoVar "LIBS" "${LIBS}"

echo "Configuring done."

