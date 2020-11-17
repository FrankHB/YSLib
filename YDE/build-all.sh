#!/usr/bin/env bash
# (C) 2014-2016, 2018, 2020 FrankHB.
# Script for build all YDE packages using SHBuild-BuildApp.

set -e
# NOTE: %SHBuild can be overriden externally.
: "${SHBuild:="$(which "SHBuild")"}"
[[ "$SHBuild" != '' ]] || echo "ERROR: Variable 'SHBuild' shall not be empty."
: "${NPLA1_ROOT:="$(dirname "$SHBuild")/../share/NPLA1"}"
# NOTE: SHBuild-common.sh should be in $PATH.
# shellcheck source=../Tools/Scripts/SHBuild-common.sh
. "SHBuild-common.sh" # For SHBuild_CheckHostPlatform;

SHBuild_CheckHostPlatform
: "${SHBuild_BuildDir=".$SHBuild_Host_Platform"}"
export SHBuild_BuildDir

: "${SHBuild_ThisDir:="$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)"}"

SHBuild_PkgPathList="$(ls -d "$SHBuild_ThisDir"/*/)"

# TODO: Reuse the configuration.
for SHBuild_PkgPath in $SHBuild_PkgPathList
do
	SHBuild_Pkg=$(basename "$SHBuild_PkgPath")
	SHBuild_Puts "Building package \"$SHBuild_Pkg\" in directory" \
		"\"$(SHBuild_2m "$SHBuild_PkgPath")\" ..."
	(cd "$SHBuild_ThisDir"; "$SHBuild" -xcmd,RunNPLFile \
		"$NPLA1_ROOT/SHBuild-BuildApp.txt" -- "$@" "$SHBuild_PkgPath")
	SHBuild_Puts "Built \"$SHBuild_Pkg\"."
done

SHBuild_Puts 'Done.'

