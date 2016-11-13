#!/usr/bin/env sh
# (C) 2014-2016 FrankHB.
# Build script for YSTest using SHBuild.

set -e
source SHBuild-common.sh
SHBuild_CheckHostPlatform
SHBuild_AssertNonempty SHBuild_Host_Platform
SHBuild_AppBaseDir=$(cd `dirname "$0"`/../build/$SHBuild_Host_Platform; pwd)
source SHBuild-BuildApp.sh $@
SrcDir=$(cd `dirname "$0"`; pwd)
SHBuild_BuildApp -xid,Android -xid,DS -xid,DS_ARM7 -xid,DS_ARM9 "$SrcDir" \
	-I\"$SrcDir/include\"
echo Done.

