#!/usr/bin/env bash
# (C) 2014-2016, 2020 FrankHB.
# Test script for self-hosting SHBuild.

set -e
SHBuild_ToolDir=$(cd `dirname "$0"`; pwd)
: ${SHBuild_BaseDir:="$SHBuild_ToolDir/../SHBuild"}
. "$SHBuild_ToolDir/SHBuild-bootstrap.sh"
SHBuild_CheckHostPlatform
SHBuild_AssertNonempty SHBuild_Host_Platform

# XXX: Stage 1 shall be ready before the call. See "%Tools/install-sysroot.sh".
SHBuild_BuildDir="$SHBuild_ToolDir/../../build/$SHBuild_Host_Platform"
S1_BuildDir="$SHBuild_BuildDir/.stage1"
S1_SHBuild="$S1_BuildDir/SHBuild"
outdir="$SHBuild_BuildDir/.shbuild-dll"
SHBuild_Output="$outdir/SHBuild"

# XXX: Use stage 2 by default.
: ${SHBuild_LIBDIR_:=\
"$SHBuild_ToolDir/../../build/$SHBuild_Host_Platform/.shbuild-dll"}
export LIBS="-L\"$SHBuild_LIBDIR_\" -lYFramework -lYBase"
CXXFLAGS="$CXXFLAGS -fwhole-program -DYF_DLL -DYB_DLL"
export LDFLAGS="${LDFLAGS/-Wl,--dn/}"

SHBuild_Pushd
cd "$SHBuild_BaseDir"

"$S1_SHBuild" . "-xd,\"$outdir\"" -xmode,2 "$@" $CXXFLAGS $INCLUDES
# NOTE: The output be same as following commands, except for "SHBuild.a".
#"$S1_SHBuild" . "-xd,\"$outdir\"" "$@" $CXXFLAGS $INCLUDES
#"$LD" "$outdir/SHBuild.a" -o"$SHBuild_Output" $CXXFLAGS $LDFLAGS \
#	$INCLUDES $LIBS

SHBuild_Popd
SHBuild_Puts Done.

