#!/usr/bin/env bash
# (C) 2014-2017, 2020-2021, 2023 FrankHB.
# Script for testing.
# Requires: G++/Clang++, Tools/Scripts, YBase source.

set -e

: "${SHBuild_ToolDir:=\
$(cd "$(dirname "${BASH_SOURCE[0]}")/../Tools/Scripts"; pwd)}"

# XXX: Following variables are internal.
# shellcheck disable=2034
CXXFLAGS_OPT_UseAssert=true
# shellcheck disable=2034
SHBuild_Debug=debug

# shellcheck source=../Tools/Scripts/SHBuild-YSLib.sh
. "$SHBuild_ToolDir/SHBuild-YSLib.sh" # for YSLib_BaseDir, SHBuild_GetBuildName,
#	SHBuild_Pushd, SHBuild_S1_InitializePCH, DoLTOCleanup_, DoLTOCleanup_Int_,
#	DoLTOCleanup_AddTarget_, CXXFLAGS, LDFLAGS, INCLUDES, SHBuild_Popd;

# NOTE: Coordinated at build 966.
LIBS="$YSLib_BaseDir/YBase/source/ystdex/cassert.cpp \
$YSLib_BaseDir/YBase/source/ystdex/cstdio.cpp \
$YSLib_BaseDir/YBase/source/ystdex/tree.cpp \
$YSLib_BaseDir/YBase/source/ystdex/hash_table.cpp \
$YSLib_BaseDir/YBase/source/ytest/test.cpp \
"

TestDir="$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)"
Test_BuildDir="$YSLib_BaseDir/build/$(SHBuild_GetBuildName)/.test"
mkdir -p "$Test_BuildDir"
SHBuild_Pushd "$Test_BuildDir"

SHBuild_S1_InitializePCH # for SHBuild_IncPCH.

# XXX: See %Tools/Scripts/SHBuild-common-options.sh.
trap DoLTOCleanup_ EXIT
trap DoLTOCleanup_Int_ INT QUIT TERM

DoLTOCleanup_AddTarget_ .
# XXX: Value of several variables may contain whitespaces.
# shellcheck disable=2086,2154
"$CXX" "$TestDir/YBase.cpp" -oYBase $CXXFLAGS $LDFLAGS $SHBuild_IncPCH \
	$INCLUDES $LIBS "$@"

./YBase

SHBuild_Popd

SHBuild_Puts 'Done.'

