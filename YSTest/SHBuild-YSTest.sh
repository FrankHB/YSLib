#!/usr/bin/sh
# (C) 2014-2015 FrankHB.
# Build script for YSTest using SHBuild.

set -e
SHBuild_AppBaseDir="`dirname "$0"`/../build"
source SHBuild-BuildApp.sh $@
SrcDir="../YSTest"
SHBuild_BuildApp -xid,Android -xid,DS -xid,DS_ARM7 -xid,DS_ARM9 "$SrcDir" \
	-I\"$SrcDir/include\"
echo Done.

