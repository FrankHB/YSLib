#!/usr/bin/env bash
# (C) 2014-2017 FrankHB.
# Script to build YSLib using SHBuild.

set -ae

# Requires:
#	YSLib_BuildDir: The directory of built output.

: ${AR:='gcc-ar'}
SHBuild_ToolDir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
: ${SHBuild:="$YSLib_BuildDir/.stage1/SHBuild"}

. "$SHBuild_ToolDir/SHBuild-common.sh"
. "$SHBuild_ToolDir/SHBuild-common-toolchain.sh"
SHBuild_AssertNonempty YSLib_BuildDir

SHBuild_Pushd
cd "$YSLib_BuildDir"
declare -r debug="$1"
shift

args="$@" SHBuild_Common="$SHBuild_ToolDir/SHBuild-YSLib-common.txt" \
	$SHBuild -xcmd,RunNPLFile "$SHBuild_ToolDir/SHBuild-YSLib-build.txt"
SHBuild_Popd
SHBuild_Puts Done.

