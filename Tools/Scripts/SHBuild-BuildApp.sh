#!/usr/bin/env bash
# (C) 2014-2018, 2020 FrankHB.
# Script for build YSLib applications using SHBuild.

: "${SHBuild_Bin:=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)}"
# XXX: The error is ignored.
# shellcheck disable=2164
: "${SHBuild_AppBaseDir=$(cd "$(dirname "$0")"; pwd)}"
: "${SHBuild:="$SHBuild_Bin/SHBuild"}"
# shellcheck source=./SHBuild-common.sh
. "$SHBuild_Bin/SHBuild-common.sh"

SHBuild_PrintUsage()
{
	SHBuild_Puts 'Usage: '"$0"' [-cCONF] [SHBOPT_BASE ...]'
	SHBuild_Puts 'Build application using SHBuild.'
	SHBuild_Puts ''
	SHBuild_Puts 'CONF'
	printf '\tThe configuration name. If begins with "debug",'
	printf ' or environment variable SHBuild_Debug is set as non null value,'
	printf ' using debug configuration;'
	printf ' otherwise using release configuration.'
	printf ' If ends with "static",'
	printf ' or environment variable SHBuild_Static is set as non null value,'
	printf ' using debug configuration;'
	SHBuild_Puts ' otherwise using release configuration.'
	printf '\t'
	SHBuild_Puts 'The output directory is determined as ".CONF".'
	SHBuild_Puts ''
	SHBuild_Puts 'SHBOPT_BASE ...'
	printf '\t'
	SHBuild_Puts 'The base options remained to pass to SHBuild.'
	SHBuild_Puts ''
	SHBuild_Puts \
		'There are several other environment variables to control the build.'
	SHBuild_Puts ''
	SHBuild_Puts 'SHBuild_NoAdjustSubsystem'
	printf '\tIf set to non null, no adjustment for linker arguments would be'
	printf ' performed for MinGW. Otherwise, "-mwindows" would be added in '
	SHBuild_Puts ' linker command line in release modes.'
	SHBuild_Puts ''
	exit
}

if [[ "$1" != '' ]]; then
	if [[ "$1" == --help ]]; then
		SHBuild_PrintUsage
	elif [[ "${1:0:2}" == -c ]]; then
		SHBuild_Conf=${1#-c}
		shift 1
		SHBuild_Puts "Found configuration '$SHBuild_Conf'."
	fi
fi
if [[ "$SHBuild_Conf" == '' ]]; then
	SHBuild_Conf=shbuild
fi
if [[ "$SHBuild_Conf" =~ debug.* ]]; then
	SHBuild_Puts 'Debug mode turned on by configuration.'
	SHBuild_Debug=$SHBuild_Conf
fi
if [[ "$SHBuild_Conf" =~ .*static ]]; then
	SHBuild_Puts 'Static mode turned on by configuration.'
	SHBuild_Static=$SHBuild_Conf
fi

# XXX: %SHBuild_BuildPrefix is external.
# shellcheck disable=2154
SHBuild_Dest="$SHBuild_BuildPrefix.$SHBuild_Conf"
# XXX: The value of %SHBOPT is known having quotes.
# shellcheck disable=2089
SHBOPT="-xd,\"$SHBuild_Dest\" -xid,include -xmode,2 $*"

# shellcheck source=./SHBuild-common-toolchain.sh
. "$SHBuild_Bin/SHBuild-common-toolchain.sh"
AR=$(SHBuild_GetAR_ "$CXX" "$AR")
export AR
if [[ "$SHBuild_Debug" != '' ]]; then
	SHBuild_Puts 'Use debug configuration '"$SHBuild_Conf"'.'
	CXXFLAGS_OPT_DBG='-O0 -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC'
	unset CXXFLAGS_COMMON
	unset CXXFLAGS
	LDFLAGS_OPT_DBG=' '
	# shellcheck source=./SHBuild-common-options.sh
	. "$SHBuild_Bin/SHBuild-common-options.sh"
	SHBuild_YSLib_LibNames='-lYFrameworkd -lYBased'
else
	SHBuild_Puts 'Use release configuration '"$SHBuild_Conf"'.'
	unset CXXFLAGS_OPT_DBG
	unset CXXFLAGS_COMMON
	unset CXXFLAGS
	unset LDFLAGS
	# shellcheck source=./SHBuild-common-options.sh
	. "$SHBuild_Bin/SHBuild-common-options.sh"
	# XXX: %SHBuild_NoAdjustSubsystem is external.
	# shellcheck disable=2154
	if [[ "$SHBuild_Env_OS" == 'Win32' \
		&& "$SHBuild_NoAdjustSubsystem" == '' ]]; then
		LDFLAGS="$LDFLAGS -mwindows"
		SHBuild_Puts "Added \"-mwindows\" to LDFLAGS."
	fi
	SHBuild_YSLib_LibNames='-lYFramework -lYBase'
fi
export LDFLAGS

# TODO: Merge with logic in %SHBuild-YSLib-build.txt.
SHBuild_CheckHostPlatform
: "${SHBuild_YSLib_Platform:="$SHBuild_Host_Platform"}"
if [[ "$SHBuild_Env_OS" == 'Win32' ]]; then
	SHBuild_YF_SystemLibs='-lgdi32 -limm32'
else
	SHBuild_YF_SystemLibs='-Wl,-dy -lxcb -lpthread'
	SHBuild_YF_CFlags_freetype="$(pkg-config --cflags freetype2 2> /dev/null)"
	# XXX: The default value of %SHBuild_YF_CFlags_freetype is known having
	#	quotes.
	# shellcheck disable=2089
	: "${SHBuild_YF_CFlags_freetype:="-I\"/$SHBuild_Bin/../include\""}"
fi
if [[ -f "$SHBuild_Bin/../lib/libfreetype.a" ]]; then
	: "${SHBuild_YF_Libs_freetype:=-lfreetype}"
else
	: "${SHBuild_YF_Libs_freetype:= \
		"$(pkg-config --libs freetype2 2> /dev/null)"}"
	: "${SHBuild_YF_Libs_freetype:=-lfreetype}"
	SHBuild_YF_Libs_freetype="-Wl,-dy $SHBuild_YF_Libs_freetype"
fi

LIBS="$LIBS_RPATH -L\"$(SHBuild_2w "$SHBuild_Bin/../lib")\""

if [[ "$SHBuild_Static" == '' ]]; then
	SHBuild_YSLib_Flags="-DYF_DLL -DYB_DLL \
		$SHBuild_YF_CFlags_freetype -I\"$SHBuild_Bin/../include\""
	export LIBS="$LIBS $SHBuild_YSLib_LibNames"
else
	# XXX: Use 'g++' even when %CXX is 'clang++' to work around LTO issue for
	#	static executables in Linux.
	if SHBuild_Put "$CXX" | grep clang++ > /dev/null; then
		LD=g++
	fi
	SHBuild_YSLib_Flags="$SHBuild_YF_CFlags_freetype \
		-I\"$SHBuild_Bin/../include\""
	SHBuild_YSLib_LibNames="$SHBuild_YSLib_LibNames \
		-lFreeImage $SHBuild_YF_Libs_freetype -L\"$SHBuild_Bin/../lib\" \
		$SHBuild_YF_SystemLibs"
	export LIBS="$LIBS -Wl,-dn $SHBuild_YSLib_LibNames"
fi
export SHBuild_CFLAGS="$CFLAGS"
export SHBuild_CXXFLAGS="$CXXFLAGS"

SHBuild_BuildApp()
{
	SHBuild_AssertNonempty SHBuild_AppBaseDir
	SHBuild_Puts "Found application base directory \"$SHBuild_AppBaseDir\"."
	# XXX: The error is ignored. The value of %SHBOPT can have multiple parts
	#	with some quotes in each argument.
	# shellcheck disable=2086,2090,2164
	(cd "$SHBuild_AppBaseDir"; "$SHBuild" $SHBOPT "$@" $SHBuild_YSLib_Flags)
}

SHBuild_EchoVar_N 'CXXFLAGS'
SHBuild_EchoVar_N 'LDFLAGS'
SHBuild_EchoVar_N 'SHBOPT'

