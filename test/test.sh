#!/usr/bin/env bash
# (C) 2014-2016 FrankHB.
# Script for testing.
# Requires: G++/Clang++, Tools/Scripts, YBase source.

set -e
: ${TestDir:=$(cd `dirname "$0"`; pwd)}
: ${SHBuild_ToolDir:=$(cd `dirname "$0"`/../Tools/Scripts; pwd)}
: ${YSLib_BaseDir:="$SHBuild_ToolDir/../.."}
YSLib_BaseDir=$(cd "$YSLib_BaseDir" && pwd)

CXXFLAGS_OPT_UseAssert=true
SHBuild_Debug=debug
SHBuild_NoAdjustSubsystem=true

: ${AR:='gcc-ar'}
. "$SHBuild_ToolDir/SHBuild-BuildApp.sh"

if !(echo "$CXX" | grep clang++ > /dev/null) \
	&& echo "$CXX" | grep g++ > /dev/null; then
	CXXFLAGS="$CXXFLAGS -Wno-double-promotion"
fi

INCLUDE_PCH="$YSLib_BaseDir/YBase/include/stdinc.h"
INCLUDES=" \
	-I$YSLib_BaseDir/YFramework/include \
	-I$YSLib_BaseDir/YFramework/Android/include \
	-I$YSLib_BaseDir/YFramework/DS/include \
	-I$YSLib_BaseDir/YFramework/Win32/include \
	-I$YSLib_BaseDir/3rdparty/include \
	-I$YSLib_BaseDir/YBase/include \
	"

LIBS=" \
	$YSLib_BaseDir/YBase/source/ystdex/cassert.cpp \
	$YSLib_BaseDir/YBase/source/ystdex/cstdio.cpp \
	$YSLib_BaseDir/YBase/source/ytest/test.cpp \
	"

SHBuild_CheckHostPlatform
Test_BuildDir="$YSLib_BaseDir/build/$SHBuild_Host_Platform/.test"
mkdir -p $Test_BuildDir
SHBuild_Pushd $Test_BuildDir

SHBuild_CheckPCH "$INCLUDE_PCH" "stdinc.h"

"$CXX" $TestDir/YBase.cpp -oYBase$EXESFX $CXXFLAGS $LDFLAGS $SHBuild_IncPCH \
	$INCLUDES $LIBS "$@"

./YBase

SHBuild_Popd

echo Done.

