#!/usr/bin/env bash
# (C) 2014-2017 FrankHB.
# Script to build YSLib using SHBuild.

set -e

# Requires:
#	YSLib_BuildDir: The directory of built output.

: ${AR:='gcc-ar'}
SHBuild_ToolDir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
: ${SHBuild:="$YSLib_BuildDir/.stage1/SHBuild"}

AR=$AR SHBuild_Tool_Common=$SHBuild_ToolDir/SHBuild-common.sh \
	SHBuild_Tool_Common_ToolChain=$SHBuild_ToolDir/SHBuild-common-toolchain.sh \
	SHBuild=$SHBuild \
$SHBuild_ToolDir/SHBuild-YSLib-common.sh "$@"

