#!/usr/bin/env bash
# (C) 2014-2016 FrankHB.
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

SHBuild_EchoVar SHBuild "$SHBuild"
SHBuild_EchoVar SHBuild_Host_Platform "$SHBuild_Host_Platform"
SHBuild_EchoVar INCLUDES_YBase "$INCLUDES_YBase"
SHBuild_EchoVar INCLUDES_YFramework "$INCLUDES_YFramework"
SHBuild_EchoVar LIBS_YFramework "$LIBS_YFramework"

export LANG=C
unset CXXFLAGS_COMMON
unset CXXFLAGS

