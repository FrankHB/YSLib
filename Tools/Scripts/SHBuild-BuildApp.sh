#!/usr/bin/env bash
# (C) 2014-2016 FrankHB.
# Script for build YSLib applications using SHBuild.

: ${SHBuild_Bin:="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"}
: ${SHBuild_AppBaseDir=$(cd `dirname "$0"`; pwd)}
. $SHBuild_Bin/SHBuild-common.sh

SHBuild_PrintUsage()
{
	SHBuild_Puts Usage: "$0" [-cCONF] [SHBOPT_BASE ...]
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
		SHBuild_Puts Found configuration \'$SHBuild_Conf\'.
	fi
fi
if [[ "$SHBuild_Conf" == '' ]]; then
	SHBuild_Conf=shbuild
fi
if [[ "$SHBuild_Conf" =~ debug.* ]]; then
	SHBuild_Puts Debug mode turned on by configuration.
	SHBuild_Debug=$SHBuild_Conf
fi
if [[ "$SHBuild_Conf" =~ .*static ]]; then
	SHBuild_Puts Static mode turned on by configuration.
	SHBuild_Static=$SHBuild_Conf
fi
export SHBuild_Debug
export SHBuild_Static

SHBuild_Dest="$SHBuild_BuildPrefix.$SHBuild_Conf"
SHBOPT="-xd,$SHBuild_Dest -xid,include -xmode,2 $@"
. $SHBuild_Bin/SHBuild-common.sh
if hash gcc-ar > /dev/null; then
	: ${AR:='gcc-ar'}
elif hash ar > /dev/null; then
	: ${AR:='ar'}
fi
export AR
. $SHBuild_Bin/SHBuild-common-toolchain.sh
if [[ "$SHBuild_Debug" != '' ]]; then
	SHBuild_Puts Use debug configuration $SHBuild_Conf.
	CXXFLAGS_OPT_DBG='-O0 -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC'
	unset CXXFLAGS_COMMON
	unset CXXFLAGS
	LDFLAGS_OPT_DBG=' '
	. $SHBuild_Bin/SHBuild-common-options.sh
	export SHBuild_YSLib_LibNames='-lYFrameworkd -lYBased'
else
	SHBuild_Puts Use release configuration $SHBuild_Conf.
	unset CXXFLAGS_OPT_DBG
	unset CXXFLAGS_COMMON
	unset CXXFLAGS
	unset LDFLAGS
	. $SHBuild_Bin/SHBuild-common-options.sh
	if [[ "$SHBuild_Env_OS" == 'Win32' \
		&& "$SHBuild_NoAdjustSubsystem" == '' ]]; then
		LDFLAGS="$LDFLAGS -mwindows"
		SHBuild_Puts Added \"-mwindows\" to LDFLAGS.
	fi
	export SHBuild_YSLib_LibNames='-lYFramework -lYBase'
fi
export LDFLAGS

# TODO: Merge with logic in %SHBuild-YSLib-build.txt.
SHBuild_CheckUName
if [[ "$SHBuild_Env_OS" == 'Win32' ]]; then
	: ${SHBuild_YSLib_Platform:='MinGW32'}
	SHBuild_YF_SystemLibs='-lgdi32 -limm32'
	: ${SHBuild_YF_Libs_freetype:='-lfreetype'}
else
	: ${SHBuild_YSLib_Platform:=$SHBuild_Env_OS}
	SHBuild_YF_SystemLibs='-Wl,-dy -lxcb -lpthread'
	SHBuild_YF_CFlags_freetype="`pkg-config --cflags freetype2 2> /dev/null`"
	: ${SHBuild_YF_CFlags_freetype:='-I/usr/include'}
	SHBuild_YF_Libs_freetype=" \
		-Wl,-dy `pkg-config --libs freetype2 2> /dev/null`"
	: ${SHBuild_YF_Libs_freetype:='-Wl,-dy -lfreetype'}
fi

LIBS="$LIBS_RPATH -L\"`SHBuild_2w "$SHBuild_Bin/../lib"`\""

if [[ "$SHBuild_Static" == '' ]]; then
	export SHBuild_YSLib_Flags="-DYF_DLL -DYB_DLL \
		$SHBuild_YF_CFlags_freetype -I\"$SHBuild_Bin/../include\""
	export LIBS="$LIBS $SHBuild_YSLib_LibNames"
else
	export SHBuild_YSLib_Flags="$SHBuild_YF_CFlags_freetype \
		-I$SHBuild_Bin/../include"
	export SHBuild_YSLib_LibNames="$SHBuild_YSLib_LibNames \
		-lFreeImage $SHBuild_YF_Libs_freetype -L/usr/lib $SHBuild_YF_SystemLibs"
	export LIBS="$LIBS -Wl,-dn $SHBuild_YSLib_LibNames"
fi
export SHBuild_CFLAGS="$CFLAGS"
export SHBuild_CXXFLAGS="$CXXFLAGS"

SHBuild_BuildApp()
{
	SHBuild_AssertNonempty SHBuild_AppBaseDir
	SHBuild_Puts Found application base directory \"$SHBuild_AppBaseDir\".
	(cd $SHBuild_AppBaseDir; "$SHBuild_Bin/SHBuild" $SHBOPT $@ \
		$SHBuild_YSLib_Flags)
}

SHBuild_EchoVar_N 'CXXFLAGS'
SHBuild_EchoVar_N 'LDFLAGS'
SHBuild_EchoVar_N 'SHBOPT'

