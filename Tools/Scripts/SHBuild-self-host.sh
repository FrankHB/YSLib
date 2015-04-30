#!/usr/bin/env bash
# (C) 2014-2015 FrankHB.
# Test script for self-hosting SHBuild.

set -e
SHBuild_ToolDir=$(cd `dirname "$0"`; pwd)
: ${SHBuild_BaseDir:="$SHBuild_ToolDir/../SHBuild"}
. $SHBuild_ToolDir/SHBuild-bootstrap.sh
SHBuild_BuildDir="$SHBuild_ToolDir/../../build/.shbuild"

SHBuild_Pushd
cd $SHBuild_BaseDir

$SHBuild_BaseDir/shbuild . "-xd,$SHBuild_BuildDir" $@ $CXXFLAGS $INCLUDES
"$LD" $SHBuild_BuildDir/SHBuild.a -o$SHBuild_BuildDir/SHBuild \
	$CXXFLAGS $LDFLAGS $INCLUDES $LIBS

SHBuild_Popd
echo Done.

