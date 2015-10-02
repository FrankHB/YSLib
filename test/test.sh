#!/usr/bin/env bash
# (C) 2014-2015 FrankHB.
# Script for testing.
# Requires: G++/Clang++, Tools/Scripts, YBase source.

set -e
: ${TestDir:=$(cd `dirname "$0"`; pwd)}
: ${SHBuild_ToolDir:=$(cd `dirname "$0"`/../Tools/Scripts; pwd)}

CXXFLAGS_OPT_UseAssert=true
SHBuild_Debug=debug
SHBuild_NoAdjustSubsystem=true

: ${AR:='gcc-ar'}
. $SHBuild_ToolDir/SHBuild-BuildApp.sh

if !(echo "$CXX" | grep clang++ > /dev/null) \
	&& echo "$CXX" | grep g++ > /dev/null; then
	CXXFLAGS="$CXXFLAGS -Wno-double-promotion"
fi

INCLUDE_PCH='../YBase/include/stdinc.h'
INCLUDES=" \
	-I../YFramework/include -I../YFramework/Android/include \
	-I../YFramework/DS/include -I../YFramework/Win32/include \
	-I../3rdparty/include -I../YBase/include \
	"

LIBS=" \
	../YBase/source/ystdex/cstdio.cpp \
	../YBase/source/ytest/test.cpp \
	"

pushd $TestDir

# TODO: Merge with SHBuild-YSLib-common.sh?
SHBuild_CheckPCH_()
{
	if [[ $SHBuild_NoPCH == '' ]]; then
		SHBuild_BuildGCH "$1" "$2" "$CXX -xc++-header $CXXFLAGS"
		SHBuild_IncPCH="-include $2"
	else
		echo Skipped building precompiled file.
		SHBuild_IncPCH=""
	fi
}

SHBuild_CheckPCH_ "$INCLUDE_PCH" "$TestDir/stdinc.h"

"$CXX" YBase.cpp -oYBase $CXXFLAGS $LDFLAGS $SHBuild_IncPCH $INCLUDES $LIBS "$@"

./YBase

popd

echo Done.

