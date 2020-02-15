#!/usr/bin/env bash
# (C) 2014-2016, 2020 FrankHB.
# Test script for self-hosting SHBuild.

set -e
SHBuild_ToolDir=$(cd `dirname "$0"`; pwd)
: ${SHBuild_BaseDir:="$SHBuild_ToolDir/../SHBuild"}
. $SHBuild_ToolDir/SHBuild-bootstrap.sh
SHBuild_CheckHostPlatform
SHBuild_AssertNonempty SHBuild_Host_Platform

# XXX: Stage 1 shall be ready before the call. See "%Tools/install-sysroot.sh".
SHBuild_BuildDir="$SHBuild_ToolDir/../../build/$SHBuild_Host_Platform"
S1_BuildDir="$SHBuild_BuildDir/.stage1"
S1_SHBuild="$S1_BuildDir/SHBuild"
outdir="$SHBuild_BuildDir/.shbuild"
SHBuild_Output="$outdir/SHBuild.exe"

SHBuild_Pushd
cd "$SHBuild_BaseDir"

"$S1_SHBuild" . "-xd,\"$outdir\"" "$@" $CXXFLAGS $INCLUDES
# XXX: MinGW will expect 'WinMain' if the linked archive has a 'main' function.
if [[ "$SHBuild_Env_OS" == 'Win32' ]]; then
	"$CXX" -o"$SHBuild_Output" $CXXFLAGS $LDFLAGS \
		$INCLUDES "$outdir/SHBuild/Main.cpp.o" $LIBS
else
	"$CXX" -o"$SHBuild_Output" $CXXFLAGS $LDFLAGS \
		$INCLUDES "$outdir/SHBuild.a" $LIBS
fi

SHBuild_Popd
SHBuild_Puts Done.

