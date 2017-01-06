#!/usr/bin/env bash
# (C) 2014-2017 FrankHB.
# Script for build YSLib debug configurations using SHBuild.

set -e
SHBuild_ToolDir=$(cd `dirname "$0"`; pwd)
. $SHBuild_ToolDir/SHBuild-YSLib-common.sh

SHBuild_Pushd
cd "$YSLib_BuildDir"

SHBuild_YSLib_Build_ "$SHBuild_NoStatic" true '' $@
SHBuild_YSLib_Build_ "$SHBuild_NoDynamic" true true $@

SHBuild_Popd
SHBuild_Puts Done.

