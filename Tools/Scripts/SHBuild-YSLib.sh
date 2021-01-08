#!/usr/bin/env bash
# (C) 2014-2021 FrankHB.
# Shared stage 1 script for YSLib building.

[[ "$INC_SHBuild_YSLib" == '' ]] && INC_SHBuild_YSLib=1 || return 0

: "${SHBuild_ToolDir:=$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)}"
: "${YSLib_BaseDir:="$SHBuild_ToolDir/../.."}"
YSLib_BaseDir=$(cd "$YSLib_BaseDir"; pwd)

: "${SHBuild_PCH_stdinc_h:=stdinc.h}"

# shellcheck source=./SHBuild-common-options.sh
. "$SHBuild_ToolDir/SHBuild-common-options.sh"

# XXX: %INCLUDE_PCH and %INCLUDES are internal.
# shellcheck disable=2034
INCLUDE_PCH="$YSLib_BaseDir/YBase/include/stdinc.h"
# shellcheck disable=2034
INCLUDES="-I$YSLib_BaseDir/YFramework/include \
-I$YSLib_BaseDir/YFramework/Android/include \
-I$YSLib_BaseDir/YFramework/DS/include \
-I$YSLib_BaseDir/YFramework/Win32/include \
-I$YSLib_BaseDir/3rdparty/include \
-I$YSLib_BaseDir/3rdparty/freetype/include \
-I$YSLib_BaseDir/YBase/include \
"

SHBuild_S1_InitializePCH()
{
	SHBuild_CheckPCH "$INCLUDE_PCH" "$SHBuild_PCH_stdinc_h"
}

