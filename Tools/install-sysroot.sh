#!/usr/bin/env bash
# (C) 2014-2018, 2020 FrankHB.
# SHBuild installation script.

set -e
SHBuild_ToolDir=$(cd "`dirname "$0"`/Scripts"; pwd)
. "$SHBuild_ToolDir/SHBuild-common.sh" # For SHBuild_Puts,
#	SHBuild_CheckHostPlatform;

# NOTE: Some variables are configurable. If not set or set to empty, the
#	variables can be later set by the scripts being called.

# NOTE: Optional stage 1 path which can later set by
#	"%Scripts/SHBuild-bootstrap.sh" This is required to locate the YSLib
#	repository base directory.
: ${YSLib_BaseDir:="$SHBuild_ToolDir/../.."}

SHBuild_Puts Configuring ...
: ${SHBuild_BuildOpt:="-xj,6"}
export SHBuild_BuildOpt
: ${SHBuild_LogOpt:="-xlogfl,128"}
export SHBuild_LogOpt
: ${SHBuild_Opt:="$SHBuild_LogOpt $SHBuild_BuildOpt"}
export SHBuild_Opt
: ${SHBuild_SysRoot:="$YSLib_BaseDir/sysroot"}
export YSLib_BaseDir=$(cd "$YSLib_BaseDir"; pwd)
SHBuild_CheckHostPlatform
# This directory will be created if not existed by following stage 1 process
#	or by %SHBuild-YSLib-build.txt.
: ${SHBuild_BuildDir:="$YSLib_BaseDir/build/$SHBuild_Host_Platform"}
export YSLib_BuildDir="$SHBuild_BuildDir"
SHBuild_Puts Build directory is \"$YSLib_BuildDir\".
SHBuild_CheckUName
: ${AR:="gcc-ar"}
. "$SHBuild_ToolDir/SHBuild-common-toolchain.sh"
if [[ -z ${SHBuild_UseRelease+x} ]]; then
	SHBuild_UseRelease=true
fi
SHBuild_Puts Done.

SHBuild_Puts Bootstraping ...
# S1 = Stage 1.
S1_BuildDir="$YSLib_BuildDir/.stage1"
mkdir -p $S1_BuildDir
S1_SHBuild="$S1_BuildDir/SHBuild"

if [[ "$SHBuild_Rebuild_S1" == '' ]]; then
	if command -v "$S1_SHBuild" > /dev/null ; then
		SHBuild_Puts Found stage 1 SHBuild \"$S1_SHBuild\", building skipped.
	else
		SHBuild_Puts Stage 1 SHBuild not found.
		SHBuild_Rebuild_S1_=true
	fi
else
	SHBuild_Puts Stage 1 SHBuild would be rebuilt.
	SHBuild_Rebuild_S1_=true
fi
if [[ "$SHBuild_Rebuild_S1_" == 'true' ]]; then
	SHBuild_Puts Building Stage 1 SHBuild ...
	SHBuild_Output="$S1_SHBuild" SHBuild_PCH_stdinc_h="$S1_BuildDir/stdinc.h" \
		$SHBuild_ToolDir/SHBuild-build.sh
	SHBuild_Puts Finished building stage 1 SHBuild.
fi

# Stage 1 SHBuild done. Following code call stage 1 SHBuild by default.
: ${SHBuild:="$S1_SHBuild"}
AR="$AR" SHBuild="$SHBuild" SHBuild_ToolDir="$SHBuild_ToolDir" \
	SHBuild_Common="$SHBuild_ToolDir/SHBuild-YSLib-common.txt" \
	SHBuild_NoStatic="$SHBuild_NoStatic" \
	SHBuild_NoDynamic="$SHBuild_NoDynamic" \
	SHBuild_UseDebug="$SHBuild_UseDebug" \
	SHBuild_UseRelease="$SHBuild_UseRelease" \
	"$SHBuild" -xcmd,RunNPLFile \
	"$SHBuild_ToolDir/SHBuild-YSLib-build.txt" -- $SHBuild_Opt

