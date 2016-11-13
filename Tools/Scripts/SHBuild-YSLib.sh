#!/usr/bin/env bash
# (C) 2014-2016 FrankHB.
# Script for build YSLib release configurations using SHBuild.

set -e
SHBuild_ToolDir=$(cd `dirname "$0"`; pwd)
. $SHBuild_ToolDir/SHBuild-YSLib-common.sh

SHBuild_Pushd
cd "$YSLib_BuildDir"

unset CXXFLAGS_OPT_DBG

if [[ "$SHBuild_NoStatic" == '' ]]; then
	SHBuild_Puts Building static libraries ...

	SHBOUT=.shbuild
	SHBOPT="$SHBOPT_IGN"
	unset LDFLAGS
	. $SHBuild_ToolDir/SHBuild-common-options.sh
	LDFLAGS="$LDFLAGS -Wl,--dn"

	SHBuild_EchoVar SHBOPT "$SHBOPT"
	SHBuild_EchoVar CXXFLAGS "$CXXFLAGS"
	SHBuild_EchoVar LDFLAGS "$LDFLAGS"

	SHBuild_CheckPCH_ "$INCLUDE_PCH" "$SHBOUT/stdinc.h"

	$SHBuild $SHBOPT -xn,${LIBPFX}YBase $@ "$YSLib_BaseDir/YBase" $CXXFLAGS \
		$INCLUDES_YBase $SHBuild_IncPCH
	$SHBuild $SHBOPT -xn,${LIBPFX}YFramework $@ "$YSLib_BaseDir/YFramework" \
		$CXXFLAGS -DFREEIMAGE_LIB $INCLUDES_YFramework $INCLUDES_YBase \
		$SHBuild_IncPCH

	SHBuild_Puts Finished building static libraries.
else
	SHBuild_Puts Skipped building static libraries.
fi

if [[ "$SHBuild_NoDynamic" == '' ]]; then
	SHBuild_Puts Building dynamic libraries ...

	SHBOUT=.shbuild-dll
	SHBOPT="-xd,$SHBOUT $SHBOPT_IGN -xmode,2"
	unset LDFLAGS
	. $SHBuild_ToolDir/SHBuild-common-options.sh
	export LDFLAGS="$LDFLAGS $LDFLAGS_DYN"

	SHBuild_EchoVar SHBOPT "$SHBOPT"
	SHBuild_EchoVar CXXFLAGS "$CXXFLAGS"
	SHBuild_EchoVar LDFLAGS "$LDFLAGS"

	SHBuild_CheckPCH_ "$INCLUDE_PCH" "$SHBOUT/stdinc.h"

	$SHBuild $SHBOPT -xn,${LIBPFX}YBase $@ "$YSLib_BaseDir/YBase" \
		$CXXFLAGS -DYB_BUILD_DLL $INCLUDES_YBase $SHBuild_IncPCH

	export LIBS="-L.shbuild-dll -lYBase $LIBS_YFramework"

	$SHBuild $SHBOPT -xn,${LIBPFX}YFramework $@ "$YSLib_BaseDir/YFramework" \
		$CXXFLAGS -DYB_BUILD_DLL -DYF_BUILD_DLL \
		-DFREEIMAGE_LIB $INCLUDES_YFramework $INCLUDES_YBase $SHBuild_IncPCH

	SHBuild_Puts Finished building dynamic libraries.
else
	SHBuild_Puts Skipped building dynamic libraries.
fi

SHBuild_Popd
SHBuild_Puts Done.

