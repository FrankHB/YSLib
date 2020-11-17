#!/usr/bin/env bash
# (C) 2014-2016, 2020 FrankHB.
# Test script for self-hosting SHBuild.

set -e
: "${SHBuild_ToolDir:=$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)}"
: "${SHBuild_BaseDir:="$SHBuild_ToolDir/../SHBuild"}"
# shellcheck source=./SHBuild-bootstrap.sh
. "$SHBuild_ToolDir/SHBuild-bootstrap.sh"

# XXX: Stage 1 shall be ready before the call. See "%Tools/install-sysroot.sh".
S1_BuildDir="$SHBuild_BuildDir/.stage1"
S1_SHBuild="$S1_BuildDir/SHBuild"
outdir="$SHBuild_BuildDir/.self-host"
SHBuild_Output="$outdir/SHBuild.exe"

SHBuild_Pushd .
cd "$SHBuild_BaseDir"

# XXX: Disable LTO for MinGW32 to work around memory overflow with
#	i686-w64-mingw32-g++.
if [[ "$SHBuild_Env_OS" == 'Win32' && "$SHBuild_Env_Arch" == 'i686' ]]; then
	NO_LTO_=-fno-lto
else
	NO_LTO_=
fi
# XXX: Value of several variables may contain whitespaces.
# shellcheck disable=2086
"$S1_SHBuild" . "-xd,\"$outdir\"" "$@" $CXXFLAGS $NO_LTO_ $INCLUDES
# XXX: MinGW will expect 'WinMain' if the linked archive has a 'main' function.
if [[ "$SHBuild_Env_OS" == 'Win32' ]]; then
	# XXX: Value of several variables may contain whitespaces.
	# shellcheck disable=2086
	"$CXX" -o"$SHBuild_Output" $CXXFLAGS $LDFLAGS \
		$INCLUDES "$outdir/SHBuild/Main.cpp.o" $NO_LTO_ $LIBS
else
	# XXX: Value of several variables may contain whitespaces.
	# shellcheck disable=2086
	"$CXX" -o"$SHBuild_Output" $CXXFLAGS $LDFLAGS \
		$INCLUDES "$outdir/SHBuild.a" $LIBS
fi

SHBuild_Popd
SHBuild_Puts 'Done.'

