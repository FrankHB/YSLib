#!/usr/bin/env bash
# (C) 2014-2016, 2020 FrankHB.
# Build script for YSTest using SHBuild.

set -e
# shellcheck source=../Tools/Scripts/SHBuild-common.sh
. SHBuild-common.sh
SHBuild_CheckHostPlatform
SHBuild_AssertNonempty SHBuild_Host_Platform
# XXX: %SHBuild_BuildDir is external.
# shellcheck disable=2154
if [[ "$SHBuild_BuildDir" != '' ]]; then
	: "${SHBuild_AppBaseDir:="$SHBuild_BuildDir"}"
else
	: "${SHBuild_AppBaseDir:="$(dirname "$0")/../build/$SHBuild_Host_Platform"}"
fi
mkdir -p "$SHBuild_AppBaseDir"
SHBuild_AppBaseDir="$(cd "$SHBuild_AppBaseDir"; pwd)"
: "${SHBuild_SystemPrefix:=$(SHBuild_GetSystemPrefix "$SHBuild_Host_Platform")}"
# XXX: %SHBuild_Sysroot is external.
# shellcheck disable=2154
if [[ "$SHBuild_SysRoot" != '' ]]; then
	export SHBuild_Bin="$SHBuild_SysRoot$SHBuild_SystemPrefix/bin"
fi
# shellcheck source=../Tools/Scripts/SHBuild-BuildApp.sh
. SHBuild-BuildApp.sh "$@"
SrcDir=$(cd "$(dirname "$0")"; pwd)
# XXX: Several value of variables here are known having quotes.
# shellcheck disable=2086
SHBuild_BuildApp -xid,Android -xid,DS -xid,DS_ARM7 -xid,DS_ARM9 "$SrcDir" \
	-I\"$SrcDir/include\"
echo Done.

