#!/usr/bin/env bash
# (C) 2014-2018, 2020-2021 FrankHB.
# SHBuild installation script.

set -e

: "${SHBuild_ToolDir:=$(cd "$(dirname "${BASH_SOURCE[0]}")/Scripts"; pwd)}"
# XXX: Use 'shell -x -P SCRIPTDIR'.
# shellcheck source=./Scripts/SHBuild-common.sh
. "$SHBuild_ToolDir/SHBuild-common.sh" # for SHBuild_Puts, SHBuild_PrepareBuild,
#	SHBuild_GetBuildName;

# NOTE: Some variables are configurable. If not set or set to empty, the
#	variables can be later set by the scripts being called. Variable specific to
#	Sysroot is not used, so a clean bootstrap does not need to unset Sysroot
#	variables configured by the user (e.g. %NPLA1_ROOT) in the environment (if
#	any).

# NOTE: Optional stage 1 path which can later be set by
#	"%Scripts/SHBuild-bootstrap.sh". This is required to locate the YSLib
#	repository base directory.
: "${YSLib_BaseDir:="$SHBuild_ToolDir/../.."}"

SHBuild_Puts 'Configuring ...'
: "${SHBuild_BuildOpt:="-xj,6"}"
export SHBuild_BuildOpt
: "${SHBuild_LogOpt:="-xlogfl,128"}"
export SHBuild_LogOpt
: "${SHBuild_Opt:="$SHBuild_LogOpt $SHBuild_BuildOpt"}"
export SHBuild_Opt
: "${SHBuild_SysRoot:="$YSLib_BaseDir/sysroot"}"
YSLib_BaseDir=$(cd "$YSLib_BaseDir"; pwd)
export YSLib_BaseDir
# NOTE: This is necessary before the call to %SHBuild_GetBuildName.
SHBuild_PrepareBuild
# NOTE: The default value is same to %SHBuild_BuildDir in
#	%SHBuild-bootstrap.sh. The directory will be created if not existed by
#	the following stage 1 process or by %SHBuild-YSLib-build.txt.
: "${SHBuild_BuildDir:="$YSLib_BaseDir/build/$(SHBuild_GetBuildName)"}"
export SHBuild_BuildDir
SHBuild_Puts "Build directory is \"$SHBuild_BuildDir\"."
if [[ -z ${SHBuild_UseRelease+x} ]]; then
	SHBuild_UseRelease=true
fi
SHBuild_Puts 'Done.'

SHBuild_Puts 'Bootstraping ...'
# S1 = Stage 1.
S1_BuildDir="$SHBuild_BuildDir/.stage1"
mkdir -p "$S1_BuildDir"
S1_SHBuild="$S1_BuildDir/SHBuild"

# XXX: %SHBuild_Rebuild_S1 is external, and not same to %SHBuild_Rebuild_S1_.
# shellcheck disable=2154
if [[ "$SHBuild_Rebuild_S1" == '' ]]; then
	if command -v "$S1_SHBuild" > /dev/null ; then
		SHBuild_Puts \
			"Found stage 1 SHBuild \"$S1_SHBuild\", building skipped."
	else
		SHBuild_Puts 'Stage 1 SHBuild not found.'
		SHBuild_Rebuild_S1_=true
	fi
else
	SHBuild_Puts 'Stage 1 SHBuild would be rebuilt.'
	SHBuild_Rebuild_S1_=true
fi
if [[ "$SHBuild_Rebuild_S1_" == 'true' ]]; then
	SHBuild_Puts 'Building Stage 1 SHBuild ...'
	SHBuild_Output="$S1_SHBuild" SHBuild_PCH_stdinc_h="$S1_BuildDir/stdinc.h" \
		"$SHBuild_ToolDir/SHBuild-build.sh"
	SHBuild_Puts 'Finished building stage 1 SHBuild.'
fi

# Stage 1 SHBuild done. Following code call stage 1 SHBuild by default.
: "${SHBuild:="$S1_SHBuild"}"
SHBuild_AssertNonempty SHBuild
# XXX: Variable %SHBuild_Opt can have whitespaces.
# XXX: Variables here are assigned locally and guaranteed to be expanded to the
#	same values to avoid 'export' pollution.
# shellcheck disable=2086,2097,2098,2154
SHBuild="$SHBuild" SHBuild_Epoch=0 SHBuild_ToolDir="$SHBuild_ToolDir" \
	SHBuild_NoStatic="$SHBuild_NoStatic" \
	SHBuild_NoDynamic="$SHBuild_NoDynamic" \
	SHBuild_UseDebug="$SHBuild_UseDebug" \
	SHBuild_UseRelease="$SHBuild_UseRelease" \
	"$SHBuild" -xcmd,RunNPLFile \
	"$SHBuild_ToolDir/SHBuild-YSLib-build.txt" -- $SHBuild_Opt

