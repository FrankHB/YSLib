#!/usr/bin/bash
# (C) 2014 FrankHB.
# Script for build YSLib debug configurations using SHBuild.

set -e
SHBuild_ToolDir=$(cd `dirname "$0"`; pwd)
. $SHBuild_ToolDir/SHBuild-YSLib-common.sh

SHBuild_Pushd
cd $SHBuild_BaseDir

CXXFLAGS_OPT_DBG="-O0 -g"

if [ x"$SHBuild_NoStatic" == x ]; then
	echo Building debug static libraries ...

	SHBOPT="-xd,.shbuild-debug $SHBOPT_IGN"
	. $SHBuild_ToolDir/SHBuild-common-options.sh
	export LDFLAGS="-Wl,--dn"

	SHBuild_EchoVar "SHBOPT" "$SHBOPT"
	SHBuild_EchoVar "CXXFLAGS" "$CXXFLAGS"
	SHBuild_EchoVar "LDFLAGS" "$LDFLAGS"

	$SHBuild $SHBOPT -xn,${LIBPFX}YBased $@ ../../YBase \
		$CXXFLAGS $INCLUDES_YBase
	$SHBuild $SHBOPT -xn,${LIBPFX}YFrameworkd $@ ../../YFramework \
		$CXXFLAGS -DFREEIMAGE_LIB $INCLUDES_YFramework $INCLUDES_YBase

	echo Finished building debug static libraries.
else
	echo Skipped building debug static libraries.
fi

if [ x"$SHBuild_NoDynamic" == x ]; then
	echo Building debug dynamic libraries ...

	SHBOPT="-xd,.shbuild-dll-debug $SHBOPT_IGN -xmode,2"
	LDFLAGS="-Wl,--dn"
	. $SHBuild_ToolDir/SHBuild-common-options.sh
	export LDFLAGS="$LDFLAGS $LDFLAGS_DYN"

	SHBuild_EchoVar "SHBOPT" "$SHBOPT"
	SHBuild_EchoVar "CXXFLAGS" "$CXXFLAGS"
	SHBuild_EchoVar "LDFLAGS" "$LDFLAGS"

	$SHBuild $SHBOPT -xn,${LIBPFX}YBased $@ ../../YBase \
		$CXXFLAGS $C_CXXFLAGS_PIC -DYB_BUILD_DLL $INCLUDES_YBase

	export LIBS="-L.shbuild-dll-debug -Wl,--dy -lYBased -Wl,--dn \
		$LIBS_YFramework"

	$SHBuild $SHBOPT -xn,${LIBPFX}YFrameworkd $@ ../../YFramework \
		$CXXFLAGS $C_CXXFLAGS_PIC -DYB_DLL -DYF_BUILD_DLL \
		-DFREEIMAGE_LIB $INCLUDES_YFramework $INCLUDES_YBase

	echo Finished building debug dynamic libraries.
else
	echo Skipped building debug dynamic libraries.
fi

SHBuild_Popd
echo Done.

