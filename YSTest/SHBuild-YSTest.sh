#!/usr/bin/env bash
# (C) 2014-2016, 2020 FrankHB.
# Build script for YSTest using SHBuild.

set -e
# shellcheck source=../Tools/Scripts/SHBuild-common.sh
. SHBuild-common.sh
SHBuild_CheckHostPlatform
SHBuild_AssertNonempty SHBuild_Host_Platform
: "${SHBuild_AppBaseDir:="$(dirname "$0")/../build/$SHBuild_Host_Platform"}"
mkdir -p "$SHBuild_AppBaseDir"
SHBuild_AppBaseDir="$(cd "$SHBuild_AppBaseDir"; pwd)"
# shellcheck source=../Tools/Scripts/SHBuild-BuildApp.sh
. SHBuild-BuildApp.sh "$@"
SrcDir=$(cd "$(dirname "$0")"; pwd)
# XXX: Several value of variables here are known having quotes.
# shellcheck disable=2086
SHBuild_BuildApp -xid,Android -xid,DS -xid,DS_ARM7 -xid,DS_ARM9 "$SrcDir" \
	-I\"$SrcDir/include\"
echo Done.

