#!/usr/bin/env bash
# (C) 2014-2015, 2017, 2020-2021 FrankHB.
# Common source script: toolchain configuration.

set -e

: "${SHBuild_ToolDir:=$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)}"
# shellcheck source=./SHBuild-common.sh
. "$SHBuild_ToolDir/SHBuild-common.sh" # for SHBuild_PrepareBuild,
#	SHBuild_AssertNonempty;
SHBuild_PrepareBuild # for SHBuild_Env_TempDir;

# Check the availablity of the compiler by trying to compile the specified
#	source.
#	If the compiler is executable, The result is specified in the output.
# Params: $1 = path of the compiler.
# Params: $2 = the source to compile.
# Params: $3 = optional output on success.
# Params: $4 = optional output on failure.
# Params: $5... = options to compile.
SHBuild_CheckCompiler()
{
	local compile="$1"
	local src="$2"
	SHBuild_AssertNonempty compile
	SHBuild_AssertNonempty src
	# NOTE: The output path cannot be '/dev/null'. See http://sourceforge.net/p/msys2/discussion/general/thread/2d6adff2/?limit=25.
	if [[ "$compile" != */* ]] && (hash "$compile" > /dev/null 2>& 1) \
		|| [[ "$compile" == */* && ! -d "$compile" && -x "$compile" ]]; then
		local success="$3"
		local failure="$4"
		shift 4
		# XXX: %SHBuild_Env_TempDir is external.
		# shellcheck disable=2154
		if echo "$src" | "$compile" "$@" -o"$SHBuild_Env_TempDir/null" - \
			2> /dev/null; then
			SHBuild_Put "$success"
		else
			SHBuild_Put "$failure"
		fi
	else
		SHBuild_Put ""
	fi
}

# Check the availablity of the C compiler.
#	The result is the style supported: either "Clang", "GCC" or invalid.
# Params: $1 = path of the C compiler.
SHBuild_CheckCC()
{
	SHBuild_CheckCompiler "$1" 'int main(void){return __clang__;}' Clang GCC -xc
}

# Check the availablity of the C++ compiler.
#	The result is the style supported: either "Clang++", "G++" or invalid.
# Params: $1 = path of the C++ compiler.
SHBuild_CheckCXX()
{
	SHBuild_CheckCompiler "$1" 'int main(){return __clang__;}' Clang++ G++ -xc++
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

