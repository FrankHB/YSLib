#!/usr/bin/env bash
# (C) 2014-2016 FrankHB.
# Build script for SHBuild.

set -e
: ${SHBuild_ToolDir:=$(cd `dirname "$0"`; pwd)}
: ${SHBuild_BaseDir:="$SHBuild_ToolDir/../SHBuild"}
CXXFLAGS_OPT_UseAssert=true
. $SHBuild_ToolDir/SHBuild-bootstrap.sh

: ${SHBuild_Output:=SHBuild}
: ${SHBuild_PCH_stdinc_h:=stdinc.h}

SHBuild_Pushd
cd $SHBuild_BaseDir

SHBuild_Puts Building ...

SHBuild_CheckPCH "$INCLUDE_PCH" "$SHBuild_PCH_stdinc_h"

"$CXX" Main.cpp -o"$SHBuild_Output" $CXXFLAGS $LDFLAGS $SHBuild_IncPCH \
	$INCLUDES $LIBS

SHBuild_Popd
SHBuild_Puts Done.

