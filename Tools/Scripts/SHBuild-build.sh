#!/usr/bin/env bash
# (C) 2014-2018 FrankHB.
# Build script for SHBuild.

set -e
: ${SHBuild_ToolDir:=$(cd `dirname "$0"`; pwd)}
: ${SHBuild_BaseDir:="$SHBuild_ToolDir/../SHBuild"}
# XXX: Some options for stage 1 SHBuild are fixed. Provide more separated
#	options in future?
CXXFLAGS_OPT_UseAssert=true
C_CXXFLAGS_GC=''
LD_CXXFLAGS_GC=''
C_CXXFLAGS_STRIP=''
LD_CXXFLAGS_STRIP=''
CXXFLAGS_OPT_DBG='-Og -g'
. $SHBuild_ToolDir/SHBuild-bootstrap.sh

: ${SHBuild_Output:=SHBuild}
: ${SHBuild_PCH_stdinc_h:=stdinc.h}

SHBuild_Pushd
cd $SHBuild_BaseDir

SHBuild_Puts Building ...

# Precompiled header is not used here because it does not work well with
#	external %CXXFLAGS_OPT_DBG. It is also not used frequently like in stage 2.
#	Even it is needed, it should be better separated with the stage 2 option.
#	When needed, uncomment the following command.
#	SHBuild_CheckPCH "$INCLUDE_PCH" "$SHBuild_PCH_stdinc_h"

# Note '-fwhole-program' should not be used because there
#	do exist multiple translation units when linking with YSLib source,
#	otherwise there would be unresolved reference to names with external
#	linkage which had been optimized away.
"$CXX" Main.cpp -o"$SHBuild_Output" $CXXFLAGS $LDFLAGS $SHBuild_IncPCH \
	$INCLUDES $LIBS

SHBuild_Popd
SHBuild_Puts Done.

