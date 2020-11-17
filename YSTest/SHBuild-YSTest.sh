#!/usr/bin/env bash
# (C) 2014-2016, 2020 FrankHB.
# Build script for YSTest using SHBuild.
# Usage: SHBuild-YSTest.sh CONF [SHBOPT_BASE ...].

set -e
# NOTE: %SHBuild can be overriden externally.
: "${SHBuild:="$(which "SHBuild")"}"
[[ "$SHBuild" != '' ]] || echo "ERROR: Variable 'SHBuild' shall not be empty."
: "${NPLA1_ROOT:="$(dirname "$SHBuild")/../share/NPLA1"}"
# NOTE: Relative location of %SHBuild-common.sh depends on Sysroot.
# shellcheck source=../Tools/Scripts/SHBuild-common.sh
. "$(dirname "$SHBuild")/SHBuild-common.sh"
[[ "$1" != '' ]] || (SHBuild_Puts \
	"ERROR: The configuration name should not be empty." >& 2; exit 1)
SHBuild_CheckHostPlatform
SHBuild_AssertNonempty SHBuild_Host_Platform
: "${SHBuild_BuildDir:=\
"$(dirname "${BASH_SOURCE[0]}")/../build/$SHBuild_Host_Platform"}"
SrcDir=$(cd "$(dirname "$0")"; pwd)
# XXX: Several value of variables here are known having quotes.
# shellcheck disable=2086
(cd "$SHBuild_BuildDir"; "$SHBuild" -xcmd,RunNPLFile \
	"$NPLA1_ROOT/SHBuild-BuildApp.txt" -- "$@" -xid,Android -xid,DS \
	-xid,DS_ARM7 -xid,DS_ARM9 "$SrcDir" -I\"$SrcDir/include\")
SHBuild_Puts 'Done.'

