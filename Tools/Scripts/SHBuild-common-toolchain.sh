#!/usr/bin/env bash
# (C) 2014-2015, 2017, 2020-2021, 2023 FrankHB.
# Common source script: toolchain configuration.

set -e

: "${SHBuild_ToolDir:=$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)}"
# shellcheck source=./SHBuild-common.sh
. "$SHBuild_ToolDir/SHBuild-common.sh" # for SHBuild_PrepareBuild,
#	SHBuild_AssertNonempty;
SHBuild_PrepareBuild # for SHBuild_Env_TempDir, SHBuild_Host_OS;

# Check the availablity of the compiler by trying to compile the specified
#	source.
#	If the compiler is executable, The result is specified in the output.
# Params: $1 = path of the compiler.
# Params: $2 = the source to compile.
# Params: $3 = optional output on success.
# Params: $4 = optional output on failure.
# Params: $5... = options to compile, including '-' indicating the source.
SHBuild_CheckCompiler()
{
	local compile="$1"
	local src="$2"

	SHBuild_AssertNonempty compile
	SHBuild_AssertNonempty src
	if [[ "$compile" != */* ]] && (hash "$compile" > /dev/null 2>& 1) \
		|| [[ "$compile" == */* && ! -d "$compile" && -x "$compile" ]]; then
		local success="$3"
		local failure="$4"

		shift 4
		# NOTE: The output path cannot be '/dev/null'. See http://sourceforge.net/p/msys2/discussion/general/thread/2d6adff2/?limit=25.
		# XXX: %SHBuild_Env_TempDir is external.
		# shellcheck disable=2154
		if echo "$src" | "$compile" "$@" -o"$SHBuild_Env_TempDir/null" \
			2> /dev/null; then
			SHBuild_Put "$success"
		else
			SHBuild_Put "$failure"
		fi
	fi
}

# Check the availablity of the C compiler.
#	The result is the style supported: either "Clang", "GCC" or invalid.
# Params: $1 = path of the C compiler.
SHBuild_CheckCC()
{
	if test "$(SHBuild_CheckCompiler "$1" \
		'int main(void){return __clang__;}' Clang GCC -xc -)" = Clang; then
		SHBuild_Put 'Clang'
	else
		SHBuild_CheckCompiler "$1" 'int main(void){return __GNUC__;}' GCC '' \
			-xc++ -
	fi
}

# Check the availablity of the C++ compiler.
#	The result is the name supported: either "Clang++", "G++" or invalid.
# Params: $1 = path of the C++ compiler.
SHBuild_CheckCXX()
{
	if test "$(SHBuild_CheckCompiler "$1" \
		'int main(){return __clang__;}' Clang++ G++ -xc++ -)" = Clang++; then
		SHBuild_Put 'Clang++'
	else
		SHBuild_CheckCompiler "$1" 'int main(){return __GNUG__;}' G++ '' \
			-xc++ -
	fi
}

# Try to compile C source program using "$CC" and specified optons.
# Params: $1 = the source to compile.
# Params: $2... = options to compile.
SHBuild_CC_Test()
{
	local prog="$1"

	shift
	test -n "$(SHBuild_CheckCompiler "$CC" "$prog" x '' -pipe -xc - "$@")"
}

# Test the availablity of the C++ compiler using "$CC" with minimal
#	well-formed program.
# Params: options.
SHBuild_CC_TestSimple()
{
	SHBuild_CC_Test 'int main(void){return 0;}' "$@"
}

# Try to compile C++ source program using "$CXX" and specified optons.
# Params: $1 = the source to compile.
# Params: $2... = options to compile.
SHBuild_CXX_Test()
{
	local prog="$1"

	shift
	test -n "$(SHBuild_CheckCompiler "$CXX" "$prog" x '' -pipe -xc++ - "$@")"
}

# Test the availablity of the C++ compiler using "$CXX"  with minimal
#	well-formed program.
# Params: options.
SHBuild_CXX_TestSimple()
{
	SHBuild_CXX_Test 'int main(){}' "$@"
}

if [[ "$SHBuild_Host_OS" == Win32 && "$MSYSTEM" =~ CLANG(64|32|ARM64) ]]; then
	: "${CC:=clang}"
	: "${CXX:=clang++}"
else
	: "${CC:=gcc}"
	: "${CXX:=g++}"
fi
export CC
export CXX

SHBuild_InitReadonly SHBuild_CXX_Name SHBuild_CheckCXX "$CXX"
SHBuild_AssertNonempty SHBuild_CXX_Name

# Get the version of C compiler using "$CC" and pipe.
#	The result is the version string if supported or "".
SHBuild_CC_GetVersion()
{
	local name
	local ver

	name=$(SHBuild_CheckCC "$CC")
	if test "$name" = Clang; then
		ver=\
'__clang_major__ * 10000L + __clang_minor__ * 100L + __clang_patchlevel__'
	elif test "$name" = GCC; then
		ver='__GNUC__ * 10000L + __GNUC_MINOR__ * 100L + __GNUC_PATCHLEVEL__'
	fi

	local saved_IFS=IFS

	IFS=''
	if SHBuild_CC_Test "
#include <stdio.h>

int
main(void)
{
	printf(\"%ld\n\", $ver);
	return 0;
}
" && test -x "$SHBuild_Env_TempDir/null"; then
		SHBuild_Put "$("$SHBuild_Env_TempDir/null")"
	fi
	IFS="$saved_IFS"
}

# Get the version of C++ compiler using "$CXX" and pipe.
#	The result is the version string if supported or "".
SHBuild_CXX_GetVersion()
{
	local ver

	# XXX: %SHBuild_CXX_Name is external, usually set by %SHBuild_InitReadonly.
	# shellcheck disable=2154
	if test "$SHBuild_CXX_Name" = Clang++; then
		ver=\
'__clang_major__ * 10000L + __clang_minor__ * 100L + __clang_patchlevel__'
	elif test "$SHBuild_CXX_Name" = G++; then
		ver='__GNUG__ * 10000L + __GNUC_MINOR__ * 100L + __GNUC_PATCHLEVEL__'
	fi

	# XXX: This seems a bug of ShellCheck.
	# shellcheck disable=2034
	local saved_IFS=IFS

	IFS=''
	if SHBuild_CXX_Test "
#include <cstdio>

int
main()
{
	std::printf(\"%ld\n\", $ver);
}
" && test -x "$SHBuild_Env_TempDir/null"; then
		SHBuild_Put "$("$SHBuild_Env_TempDir/null")"
	fi
	IFS="$saved_IFS"
}

SHBuild_GetAR_()
{
	local ar=ar

	# XXX: %SHBuild_CXX_Name is external, usually set by %SHBuild_InitReadonly.
	# shellcheck disable=2154
	if test "$SHBuild_CXX_Name" = Clang++; then
		if hash llvm-ar 2> /dev/null; then
			ar=llvm-ar
		fi
	elif test "$SHBuild_CXX_Name" = G++; then
		if hash gcc-ar 2> /dev/null; then
			ar=gcc-ar
		fi
	fi
	SHBuild_Put "$ar"
}

: "${AR:=$(SHBuild_GetAR_)}"
export AR
: "${ARFLAGS:=rcs}"
export ARFLAGS
: "${LD:="$CXX"}"
export LD

SHBuild_InitReadonly SHBuild_CXX_Version SHBuild_CXX_GetVersion
SHBuild_AssertNonempty SHBuild_CXX_Version

SHBuild_Put "C++ compiler identification is $SHBuild_CXX_Name "
# XXX: %SHBuild_CXX_Version is set by %SHBuild_InitReadonly.
# shellcheck disable=2154
SHBuild_Put "$((SHBuild_CXX_Version / 10000))"
SHBuild_Put ".$((SHBuild_CXX_Version % 10000 / 100))"
SHBuild_Puts ".$((SHBuild_CXX_Version % 100))."

