#!/usr/bin/bash
# (C) 2014 FrankHB.
# Script for build YSLib debug configurations using SHBuild.

set -e
SHBuild_ToolDir=$(cd `dirname "$0"`; pwd)
. $SHBuild_ToolDir/SHBuild-YSLib-common.sh

SHBuild_Pushd
cd $SHBuild_BaseDir

CXXFLAGS_OPT_DBG='-O0 -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC'

if [[ "$SHBuild_NoStatic" == '' ]]; then
	echo Building debug static libraries ...

	SHBOUT=.shbuild-debug
	SHBOPT="-xd,$SHBOUT $SHBOPT_IGN"
	. $SHBuild_ToolDir/SHBuild-common-options.sh
	export LDFLAGS="-Wl,--dn"

	SHBuild_EchoVar SHBOPT "$SHBOPT"
	SHBuild_EchoVar CXXFLAGS "$CXXFLAGS"
	SHBuild_EchoVar LDFLAGS "$LDFLAGS"

	SHBuild_CheckPCH_ "$INCLUDE_PCH" "$SHBOUT/stdinc.h"

	$SHBuild $SHBOPT -xn,${LIBPFX}YBased $@ ../../YBase \
		$CXXFLAGS $INCLUDES_YBase $SHBuild_IncPCH
	$SHBuild $SHBOPT -xn,${LIBPFX}YFrameworkd $@ ../../YFramework \
		$CXXFLAGS -DFREEIMAGE_LIB $INCLUDES_YFramework $INCLUDES_YBase \
		$SHBuild_IncPCH

	echo Finished building debug static libraries.
else
	echo Skipped building debug static libraries.
fi

if [[ "$SHBuild_NoDynamic" == '' ]]; then
	echo Building debug dynamic libraries ...

	SHBOUT=.shbuild-dll-debug
	SHBOPT="-xd,$SHBOUT $SHBOPT_IGN -xmode,2"
	. $SHBuild_ToolDir/SHBuild-common-options.sh
	export LDFLAGS="$LDFLAGS_DYN"

	SHBuild_EchoVar SHBOPT "$SHBOPT"
	SHBuild_EchoVar CXXFLAGS "$CXXFLAGS"
	SHBuild_EchoVar LDFLAGS "$LDFLAGS"

	SHBuild_CheckPCH_ "$INCLUDE_PCH" "$SHBOUT/stdinc.h"

	$SHBuild $SHBOPT -xn,${LIBPFX}YBased $@ ../../YBase \
		$CXXFLAGS $C_CXXFLAGS_PIC -DYB_BUILD_DLL $INCLUDES_YBase \
		$SHBuild_IncPCH

	export LIBS="-L.shbuild-dll-debug -lYBased $LIBS_YFramework"

	$SHBuild $SHBOPT -xn,${LIBPFX}YFrameworkd $@ ../../YFramework \
		$CXXFLAGS $C_CXXFLAGS_PIC -DYB_DLL -DYF_BUILD_DLL \
		-DFREEIMAGE_LIB $INCLUDES_YFramework $INCLUDES_YBase $SHBuild_IncPCH

	echo Finished building debug dynamic libraries.
else
	echo Skipped building debug dynamic libraries.
fi

SHBuild_Popd
echo Done.

