#!/usr/bin/env bash
# (C) 2015-2017 FrankHB.
# Project generation script: generating Code::Blocks .cbp files using
#	ProjectGenerator.

: ${SHBuild_Bin:="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"}
. $SHBuild_Bin/SHBuild-common.sh

TopLevel="$(hg root 2> /dev/null || (cd "$(git rev-parse --show-cdup)"; pwd))"
TopLevel="$(SHBuild_2u $TopLevel)"
: ${ProjectGenerator:="`which ProjectGenerator`"}

if [[ "$TopLevel" == '' ]]; then
	SHBuild_Puts \
		ERROR: Cannot find top level directory. Make sure hg or git in PATH.
	exit 1;
else
	SHBuild_Puts Found top level directory \"$TopLevel\".
fi

SHBuild_Pushd $TopLevel


SHBuild_GenerateCBP_()
{
	declare -r target="$1"
	shift
	SHBuild_Puts Writing \"$target\" ...
	$ProjectGenerator "$@" > $target
}

SHBuild_GenerateCBP_Wrap_()
{
	if [[ "$3" == '' ]]; then
		SHBuild_GenerateCBP_ "$TopLevel/$2/$2.cbp" "$1" "$2"
	else
		SHBuild_GenerateCBP_ "$TopLevel/$2/$3/$2_$3.cbp" "$1" "$2" "$3"
	fi
}

SHBuild_GenerateCBP_Wrap_ cl YBase
SHBuild_GenerateCBP_Wrap_ cl YBase MinGW32
SHBuild_GenerateCBP_Wrap_ cl YBase DS
SHBuild_GenerateCBP_Wrap_ cl YBase Android
SHBuild_GenerateCBP_Wrap_ cl YFramework
SHBuild_GenerateCBP_Wrap_ cl YFramework MinGW32
SHBuild_GenerateCBP_Wrap_ cl YFramework DS
SHBuild_GenerateCBP_Wrap_ cl YFramework Android
SHBuild_GenerateCBP_ "$TopLevel/YSTest/DS_ARM7/YSTest_ARM7.cbp" c \
	YSTest/DS_ARM7 DS_ARM7
SHBuild_GenerateCBP_ "$TopLevel/YSTest/DS_ARM9/YSTest_ARM9.cbp" c \
	YSTest/DS_ARM9 DS_ARM9
SHBuild_GenerateCBP_Wrap_ c YSTest MinGW32
SHBuild_GenerateCBP_Wrap_ c YSTest DS
SHBuild_GenerateCBP_Wrap_ c YSTest Android

SHBuild_Popd
SHBuild_Puts Done.

