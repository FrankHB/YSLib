#!/usr/bin/bash
# (C) 2014 FrankHB.
# Example script for build YSLib using SHBuild.

: ${SHBuild_ToolDir:=$(cd `dirname "$0"`; pwd)}
: ${SHBuild_BaseDir:="${SHBuild_ToolDir}/../SHBuild"}

SHBuild_PrintUsage()
{
	echo Usage: "$0" [-cCONF] [SHBOPT ...]
	printf 'Build application using SHBuild.\n\n'
	printf 'CONF\n'
	printf '\tThe configuration name. If equals to "debug",'
	printf ' or environment variable SHBuild_Debug is set as non null value,'
	printf ' using debug configuration;'
	printf ' otherwise using release configuration.\n'
	printf '\tThe output directory is determined as ".CONF".\n\n'
	printf 'SHBOPT ...\n'
	printf '\tThe options remained to pass to SHBuild.\n\n'
	exit
}

if [ x"$1" != x ]; then
	if [ x"$1" == x--help ]; then
		SHBuild_PrintUsage
	elif [ x"${1:0:2}" == x-c ]; then
		SHBuild_Conf=${1#-c}
		shift 1
		echo Found configuration \'${SHBuild_Conf}\'.
	fi
fi
if [ x"${SHBuild_Conf}" == x ]; then
	SHBuild_Conf=shbuild
fi
if [ x${SHBuild_Conf} == xdebug ]; then
	echo Debug mode turned on by configuration.
	SHBuild_Debug=${SHBuild_Conf}
fi
export SHBuild_Debug

SHBOPT="-xd,.${SHBuild_Conf} -xmode,2 $@"
source ${SHBuild_ToolDir}/common.sh
if hash gcc-ar > /dev/null; then
	export AR=gcc-ar
fi
source ${SHBuild_ToolDir}/common-toolchain.sh
if [ x${SHBuild_Debug} != x ]; then
	echo Use debug configuration ${SHBuild_Conf}.
	CXXFLAGS_OPT_DBG="-O0 -g"
	unset CXXFLAGS_COMMON
	unset CXXFLAGS
	LDFLAGS=" "
	source ${SHBuild_ToolDir}/common-options.sh
	export SHBuild_YSLib_LibNames='-lYFrameworkd -lYBased'
else
	echo Use release configuration ${SHBuild_Conf}.
	unset CXXFLAGS_OPT_DBG
	unset CXXFLAGS_COMMON
	unset CXXFLAGS
	unset LDFLAGS
	source ${SHBuild_ToolDir}/common-options.sh
	export LDFLAGS="${LDFLAGS} -mwindows"
	export SHBuild_YSLib_LibNames='-lYFramework -lYBase'
fi

SHBuild_EchoVar_N "CXXFLAGS"
SHBuild_EchoVar_N "LDFLAGS"
SHBuild_EchoVar_N "SHBOPT"

