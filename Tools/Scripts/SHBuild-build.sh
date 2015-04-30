#!/usr/bin/env bash
# (C) 2014-2015 FrankHB.
# Build script for SHBuild.

set -e
: ${SHBuild_ToolDir:=$(cd `dirname "$0"`; pwd)}
: ${SHBuild_BaseDir:="$SHBuild_ToolDir/../SHBuild"}
CXXFLAGS_OPT_UseAssert=true
. $SHBuild_ToolDir/SHBuild-bootstrap.sh

SHBuild_Pushd
cd $SHBuild_BaseDir

echo Building ...

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

SHBuild_CheckPCH_ "$INCLUDE_PCH" "stdinc.h"

"$CXX" Main.cpp -oSHBuild $CXXFLAGS $LDFLAGS $SHBuild_IncPCH $INCLUDES $LIBS

SHBuild_Popd
echo Done.

