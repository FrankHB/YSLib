#!/usr/bin/env bash
# (C) 2014-2015, 2017, 2020 FrankHB.
# Common source script: toolchain configuration.

set -e
: "${SHBuild_ToolDir:=$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)}"
# shellcheck source=./SHBuild-common.sh
. "$SHBuild_ToolDir/SHBuild-common.sh" # for SHBuild_PrepareBuild,
#	SHBuild_GetTempDir.
SHBuild_PrepareBuild

# Check the availablity of the C compiler.
#	The result is the style supported: either "Clang", "GCC" or invalid.
# Params: $1 = path of the C compiler.
SHBuild_CheckCC()
{
	# NOTE: The output path cannot be '/dev/null'. See http://sourceforge.net/p/msys2/discussion/general/thread/2d6adff2/?limit=25.
	SHBuild_InitReadonly SHBuild_Env_TempDir SHBuild_GetTempDir_
	if hash "$1" 2> /dev/null; then
		# XXX: %SHBuild_Env_TempDir is external.
		# shellcheck disable=2154
		if echo 'int main(void){return __clang__;}' | "$1" \
			-xc -o"$SHBuild_Env_TempDir/null" - 2> /dev/null; then
			SHBuild_Put "Clang"
		else
			SHBuild_Put "GCC"
		fi
	else
		SHBuild_Put ""
	fi
}

# Check the availablity of the C++ compiler.
#	The result is the style supported: either "Clang++", "G++" or invalid.
# Params: $1 = path of the C++ compiler.
SHBuild_CheckCXX()
{
	# NOTE: As %SHBuild_CheckCC.
	SHBuild_InitReadonly SHBuild_Env_TempDir SHBuild_GetTempDir
	if hash "$1" 2> /dev/null; then
		if echo 'int main(){return __clang__;}' | "$1" \
			-xc++ -o"$SHBuild_Env_TempDir/null" - 2> /dev/null; then
			SHBuild_Put "Clang++"
		else
			SHBuild_Put "G++"
		fi
	else
		SHBuild_Put ""
	fi
}

SHBuild_GetAR_()
{
	local SHBuild_CXX_Style_
	SHBuild_CXX_Style_=$(SHBuild_CheckCXX "$1")
	local ar="$2"
	if [[ $SHBuild_CXX_Style_ == Clang++ ]]; then
		if hash llvm-ar 2> /dev/null; then
			ar=llvm-ar
		fi
	elif [[ $SHBuild_CXX_Style_ == G++ ]]; then
		if hash gcc-ar 2> /dev/null; then
			ar=gcc-ar
		fi
	fi
	SHBuild_Put "$ar"
}

: "${CC:=gcc}"
export CC
: "${CXX:=g++}"
export CXX
: "${AR:=$(SHBuild_GetAR_ "$CXX" ar)}"
export AR
: "${ARFLAGS:=rcs}"
export ARFLAGS
: "${LD:="$CXX"}"
export LD

