#!/usr/bin/env bash
# (C) 2020 FrankHB.
# Warpper script for YSLib packages using SHBuild.

: "${SHBuild_Bin:=$(cd "$(dirname "$0")" && pwd)}"
: "${SHBuild:="$SHBuild_Bin/SHBuild"}"

"$SHBuild" -xcmd,RunNPLFile "$SHBuild_Bin/../var/NPLA1/SHBuild-BuildApp.txt" \
	"$@"
# TODO: Package.

