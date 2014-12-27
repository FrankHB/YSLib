#!/usr/bin/bash
# (C) 2014 FrankHB.
# Script for build YSLib release configurations using SHBuild.

set -e
SHBuild_ToolDir=$(cd `dirname "$0"`; pwd)
. $SHBuild_ToolDir/SHBuild-YSLib-common.sh

SHBuild_Pushd
cd $SHBuild_BaseDir

unset CXXFLAGS_OPT_DBG

if [ x"$SHBuild_NoStatic" == x ]; then
	echo Building static libraries ...

	SHBOPT="$SHBOPT_IGN"
	unset LDFLAGS
	. $SHBuild_ToolDir/SHBuild-common-options.sh
	LDFLAGS="$LDFLAGS -Wl,--dn"

	SHBuild_EchoVar "SHBOPT" "$SHBOPT"
	SHBuild_EchoVar "CXXFLAGS" "$CXXFLAGS"
	SHBuild_EchoVar "LDFLAGS" "$LDFLAGS"

	$SHBuild $SHBOPT -xn,${LIBPFX}YBase $@ ../../YBase \
		$CXXFLAGS $INCLUDES_YBase
	$SHBuild $SHBOPT -xn,${LIBPFX}YFramework $@ ../../YFramework \
		$CXXFLAGS -DFREEIMAGE_LIB $INCLUDES_YFramework $INCLUDES_YBase

	echo Finished building static libraries.
else
	echo Skipped building static libraries.
fi

if [ x"$SHBuild_NoDynamic" == x ]; then
	echo Building dynamic libraries ...

	SHBOPT="-xd,.shbuild-dll $SHBOPT_IGN -xmode,2"
	unset LDFLAGS
	. $SHBuild_ToolDir/SHBuild-common-options.sh
	export LDFLAGS="$LDFLAGS $LDFLAGS_DYN"

	SHBuild_EchoVar "SHBOPT" "$SHBOPT"
	SHBuild_EchoVar "CXXFLAGS" "$CXXFLAGS"
	SHBuild_EchoVar "LDFLAGS" "$LDFLAGS"

	$SHBuild $SHBOPT -xn,${LIBPFX}YBase $@ ../../YBase \
		$CXXFLAGS $C_CXXFLAGS_PIC -DYB_BUILD_DLL $INCLUDES_YBase

	export LIBS="-L.shbuild-dll -lYBase $LIBS_YFramework"

	$SHBuild $SHBOPT -xn,${LIBPFX}YFramework $@ ../../YFramework \
		$CXXFLAGS $C_CXXFLAGS_PIC -DYB_BUILD_DLL -DYF_BUILD_DLL \
		-DFREEIMAGE_LIB $INCLUDES_YFramework $INCLUDES_YBase

	echo Finished building dynamic libraries.
else
	echo Skipped building dynamic libraries.
fi

SHBuild_Popd
echo Done.

