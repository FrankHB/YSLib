#!/usr/bin/env bash
# (C) 2014-2015 FrankHB.
# Common source script: bootstrap configuration.

echo Bootstrap beginned.

SHBuild_ToolDir=$(cd `dirname "$0"`; pwd)
. $SHBuild_ToolDir/SHBuild-common.sh
SHBuild_CheckUName
: ${SHBuild_BaseDir:="$SHBuild_ToolDir/../SHBuild"}

SHBuild_EchoVar_N 'SHBuild.BaseDir'
SHBuild_EchoVar_N 'SHBuild.ToolDir'

echo Configuring ...

SHBuild_BuildDir="$SHBuild_BaseDir/.shbuild"

: ${AR:='gcc-ar'}
. $SHBuild_ToolDir/SHBuild-common-toolchain.sh

. $SHBuild_ToolDir/SHBuild-common-options.sh

INCLUDE_PCH='../../YBase/include/stdinc.h'
INCLUDES=" \
	-I../../YFramework/include -I../../YFramework/Android/include \
	-I../../YFramework/DS/include -I../../YFramework/MinGW32/include \
	-I../../3rdparty/include -I../../3rdparty/freetype/include \
	-I../../YBase/include \
	"

# Coordinated with build 623.
LIBS=" \
	../../YBase/source/ystdex/cassert.cpp \
	../../YBase/source/ystdex/cstdio.cpp \
	../../YBase/source/ystdex/base.cpp \
	../../YBase/source/ystdex/any.cpp \
	../../YBase/source/ystdex/concurrency.cpp \
	../../YFramework/source/CHRLib/chrmap.cpp \
	../../YFramework/source/CHRLib/CharacterProcessing.cpp \
	../../YFramework/source/CHRLib/MappingEx.cpp \
	../../YFramework/source/YCLib/Debug.cpp \
	../../YFramework/source/YCLib/FileIO.cpp \
	../../YFramework/source/YCLib/FileSystem.cpp \
	../../YFramework/source/YCLib/Host.cpp \
	../../YFramework/source/YCLib/ycommon.cpp \
	../../YFramework/source/YSLib/Core/YCoreUtilities.cpp \
	../../YFramework/source/YSLib/Core/YException.cpp \
	../../YFramework/source/YSLib/Core/ValueNode.cpp \
	../../YFramework/source/YSLib/Service/FileSystem.cpp \
	../../YFramework/source/YSLib/Service/File.cpp \
	../../YFramework/source/YSLib/Service/TextFile.cpp \
	../../YFramework/source/NPL/Lexical.cpp \
	../../YFramework/source/NPL/SContext.cpp \
	../../YFramework/source/NPL/Dependency.cpp \
	"
if [[ "$SHBuild_Env_OS" == 'Win32' ]]; then
	LIBS="$LIBS \
		../../YFramework/MinGW32/source/YCLib/MinGW32.cpp \
		../../YFramework/MinGW32/source/YCLib/Consoles.cpp \
		"
fi

# Note '-fwhole-program' should not be used because there
#	does exsit multiple translation units when linking with YSLib source,
#	otherwise there would be unresolved reference to names with external
#	linkage which had been optimized away.
export CXXFLAGS
export LDFLAGS

SHBuild_EchoVar_N 'SHBuild.BuildDir'
SHBuild_EchoVar_N 'CXX'
SHBuild_EchoVar_N 'CXXFLAGS'
SHBuild_EchoVar_N 'AR'
SHBuild_EchoVar_N 'LD'
SHBuild_EchoVar_N 'LDFLAGS'
SHBuild_EchoVar_N 'INCLUDES'
SHBuild_EchoVar_N 'LIBS'

echo Configuring done.

