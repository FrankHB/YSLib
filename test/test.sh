#!/usr/bin/env bash
# (C) 2014-2017, 2020 FrankHB.
# Script for testing.
# Requires: G++/Clang++, Tools/Scripts, YBase source.

set -e
: "${SHBuild_ToolDir:=\
$(cd "$(dirname "${BASH_SOURCE[0]}")/../Tools/Scripts"; pwd)}"
: "${YSLib_BaseDir:="$SHBuild_ToolDir/../.."}"
YSLib_BaseDir=$(cd "$YSLib_BaseDir"; pwd)

# XXX: Following variables are internal.
# shellcheck disable=2034
CXXFLAGS_OPT_UseAssert=true
# shellcheck disable=2034
SHBuild_Debug=debug
# shellcheck disable=2034
SHBuild_NoAdjustSubsystem=true

# shellcheck source=../Tools/Scripts/SHBuild-common-options.sh
. "$SHBuild_ToolDir/SHBuild-common-options.sh" # for SHBuild_PrepareBuild,
#	SHBuild_GetBuildName, SHBuild_Pushd, SHBuild_Popd, SHBuild_CheckPCH,
#	SHBuild_Puts;

INCLUDE_PCH="$YSLib_BaseDir/YBase/include/stdinc.h"
INCLUDES="-I$YSLib_BaseDir/YFramework/include \
-I$YSLib_BaseDir/YFramework/Android/include \
-I$YSLib_BaseDir/YFramework/DS/include \
-I$YSLib_BaseDir/YFramework/Win32/include \
-I$YSLib_BaseDir/3rdparty/include \
-I$YSLib_BaseDir/YBase/include \
"

LIBS="$YSLib_BaseDir/YBase/source/ystdex/cassert.cpp \
$YSLib_BaseDir/YBase/source/ystdex/cstdio.cpp \
$YSLib_BaseDir/YBase/source/ytest/test.cpp \
"

SHBuild_PrepareBuild
TestDir="$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)"
Test_BuildDir="$YSLib_BaseDir/build/$(SHBuild_GetBuildName)/.test"
mkdir -p "$Test_BuildDir"
SHBuild_Pushd "$Test_BuildDir"

SHBuild_CheckPCH "$INCLUDE_PCH" "stdinc.h"

# XXX: Value of several variables may contain whitespaces.
# shellcheck disable=2086,2154
"$CXX" "$TestDir/YBase.cpp" -oYBase$EXESFX $CXXFLAGS $LDFLAGS $SHBuild_IncPCH \
	$INCLUDES $LIBS "$@"

./YBase

SHBuild_Popd

SHBuild_Puts 'Done.'

