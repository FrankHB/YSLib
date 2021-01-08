#!/usr/bin/env bash
# (C) 2014-2016, 2020-2021 FrankHB.
# Test script for self-hosting SHBuild.

set -e

: "${SHBuild_ToolDir:=$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)}"
# shellcheck source=./SHBuild-bootstrap.sh
. "$SHBuild_ToolDir/SHBuild-bootstrap.sh"

# XXX: Stage 1 shall be ready before the call. See "%Tools/install-sysroot.sh".
S1_BuildDir="$SHBuild_BuildDir/.stage1"
S1_SHBuild="$S1_BuildDir/SHBuild"
outdir="$SHBuild_BuildDir/.self-host-dll"
# XXX: %SHBuild_Output is internal.
# shellcheck disable=2034
SHBuild_Output="$outdir/SHBuild"

# XXX: Use stage 2 by default, assuming in the same build directory.
: "${SHBuild_LIBDIR_:="$SHBuild_BuildDir/.shbuild-dll"}"
export LIBS="-L\"$SHBuild_LIBDIR_\" -lYFramework -lYBase"
CXXFLAGS="$CXXFLAGS -fwhole-program -DYF_DLL -DYB_DLL"
export LDFLAGS="${LDFLAGS/-Wl,--dn/}"

SHBuild_Pushd .
cd "$SHBuild_BaseDir"

	# XXX: Value of several variables may contain whitespaces.
	# shellcheck disable=2086
"$S1_SHBuild" . "-xd,\"$outdir\"" -xmode,2 "$@" $CXXFLAGS $INCLUDES
# NOTE: The output be same as following commands, except for "SHBuild.a".
#"$S1_SHBuild" . "-xd,\"$outdir\"" "$@" $CXXFLAGS $INCLUDES
#"$LD" "$outdir/SHBuild.a" -o"$SHBuild_Output" $CXXFLAGS $LDFLAGS \
#	$INCLUDES $LIBS

SHBuild_Popd
SHBuild_Puts 'Done.'

