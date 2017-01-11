#!/usr/bin/env bash
# (C) 2014-2017 FrankHB.
# Script for build YSLib using SHBuild.

set -e
SHBuild_ToolDir=$(cd `dirname "$0"`; pwd)
: ${SHBuild_BaseDir:="$SHBuild_ToolDir/../SHBuild"}
: ${YSLib_BaseDir:="$SHBuild_ToolDir/../.."}

. $SHBuild_ToolDir/SHBuild-common.sh
SHBuild_AssertNonempty YSLib_BuildDir

: ${AR:='gcc-ar'}
export AR
. $SHBuild_ToolDir/SHBuild-common-toolchain.sh

mkdir -p "$YSLib_BuildDir"
: ${SHBuild:="$YSLib_BuildDir/.stage1/SHBuild"}
SHBuild_CheckHostPlatform
if [[ "$SHBuild_Env_OS" == 'Win32' ]]; then
	SHBuild_YF_SystemLibs='-lgdi32 -limm32'
else
	SHBuild_YF_SystemLibs='-lxcb -lpthread'
fi

DIR_YFramework="$YSLib_BaseDir/YFramework"

INCLUDE_PCH="$YSLib_BaseDir/YBase/include/stdinc.h"
INCLUDES_YBase="-I\"$YSLib_BaseDir/YBase/include\""
INCLUDES_YFramework=" \
	-I$DIR_YFramework/include \
	-I$DIR_YFramework/Android/include \
	-I$DIR_YFramework/DS/include \
	-I$DIR_YFramework/Win32/include \
	-I$YSLib_BaseDir/3rdparty/include $INCLUDES_freetype \
	-I$YSLib_BaseDir/YBase/include"
SHBOPT_IGN="-xid,alternative -xid,data -xid,include -xid,Android"

LIBS_YFramework=" -L\"`SHBuild_2w \
	\"$DIR_YFramework/$SHBuild_Host_Platform/lib-$SHBuild_Env_Arch\"`\" \
	-lFreeImage -lfreetype $SHBuild_YF_SystemLibs"

SHBuild=$SHBuild SHBuild_Host_Platform=$SHBuild_Host_Platform \
	INCLUDES_YBase=$INCLUDES_YBase INCLUDES_YFramework=$INCLUDES_YFramework \
	LIBS_YFramework=$LIBS_YFramework $SHBuild -xcmd,RunNPL '
	SHBuild_EchoVar_N "SHBuild";
	SHBuild_EchoVar_N "SHBuild_Host_Platform";
	SHBuild_EchoVar_N "INCLUDES_YBase";
	SHBuild_EchoVar_N "INCLUDES_YFramework";
	SHBuild_EchoVar_N "LIBS_YFramework"'

# Params: $1 = skip, $2 = debug, $3 = dynamic, $4... = additional build options.
SHBuild_YSLib_Build_()
{
	declare -r debug="$2"
	declare -r dynamic="$3"
	local lname='libraries'

	if [[ "$dynamic" != '' ]]; then
		lname="dynamic $lname"
	else
		lname="static $lname"
	fi
	if [[ "$debug" != '' ]]; then
		lname="debug $lname"
	fi
	if [[ "$1" != '' ]]; then
		SHBuild_Puts Skipped $lname.
	else
		local outdir=.shbuild

		shift 3
		if [[ "$debug" != '' ]]; then
			CXXFLAGS_OPT_DBG='-O0 -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC'
		else
			unset CXXFLAGS_OPT_DBG
			unset LDFLAGS
		fi
		SHBuild_Puts Building $lname ...
		. $SHBuild_ToolDir/SHBuild-common-options.sh
		if [[ "$debug" != '' ]]; then
			LDFLAGS="$C_CXXFLAGS_PIC $CXXFLAGS_IMPL_COMMON_THRD_"
		fi
		if [[ "$dynamic" != '' ]]; then
			export LDFLAGS="$LDFLAGS $LDFLAGS_DYN"
			outdir=$outdir-dll
		else
			export LDFLAGS="$LDFLAGS -Wl,--dn"
		fi
		if [[ "$debug" != '' ]]; then
			outdir=$outdir-debug
		fi
		SHBOPT="-xd,$outdir $SHBOPT_IGN"
		if [[ "$dynamic" != '' ]]; then
			SHBOPT="$SHBOPT -xmode,2"
		fi
		SHBOPT=$SHBOPT CXXFLAGS=$CXXFLAGS $SHBuild -xcmd,RunNPL '
			SHBuild_EchoVar_N "SHBOPT";
			SHBuild_EchoVar_N "CXXFLAGS";
			SHBuild_EchoVar_N "LDFLAGS"'
		if [[ $SHBuild_NoPCH == '' ]]; then
			SHBuild_Arg1="$INCLUDE_PCH" SHBuild_Arg2="$outdir/stdinc.h" \
				$SHBuild -xcmd,RunNPL \
'SHBuild_BuildGCH (env-get "SHBuild_Arg1") (env-get "SHBuild_Arg2")
(++ (env-get "CXX") " -xc++-header " (env-get "CXXFLAGS"))'
			SHBuild_IncPCH="-include $outdir/stdinc.h"
		else
			SHBuild_Puts Skipped building precompiled file.
			SHBuild_IncPCH=""
		fi
		if [[ "$dynamic" != '' ]]; then
			declare -r DFLAG_YB=-DYB_DLL
			declare -r DFLAG_B_YB=-DYB_BUILD_DLL
			declare -r DFLAG_B_YF=-DYF_BUILD_DLL
		else
			declare -r DFLAG_YB=
			declare -r DFLAG_B_YB=
			declare -r DFLAG_B_YF=
		fi
		if [[ "$debug" != '' ]]; then
			declare -r DFLAG_X_YB=$DFLAG_YB
			declare -r libd=d
		else
			declare -r DFLAG_X_YB=$DFLAG_B_YB
			declare -r libd=
		fi
		$SHBuild $SHBOPT -xn,${LIBPFX}YBase$libd $@ "$YSLib_BaseDir/YBase" \
			$CXXFLAGS $DFLAG_B_YB $INCLUDES_YBase $SHBuild_IncPCH
		if [[ "$dynamic" != '' ]]; then
			export LIBS="-L$outdir -lYBase$libd $LIBS_YFramework"
		fi
		$SHBuild $SHBOPT -xn,${LIBPFX}YFramework$libd $@ \
			"$YSLib_BaseDir/YFramework" $CXXFLAGS $DFLAG_X_YB $DFLAG_B_YF \
			-DFREEIMAGE_LIB $INCLUDES_YFramework $INCLUDES_YBase $SHBuild_IncPCH
		SHBuild_Puts Finished building $lname.
	fi
}

export LANG=C
unset CXXFLAGS_COMMON
unset CXXFLAGS

