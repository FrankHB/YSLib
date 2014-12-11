#!/usr/bin/bash
# (C) 2014 FrankHB.
# Script for build YSLib applications using SHBuild.

: ${SHBuild_Bin:="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"}
: ${SHBuild_AppBaseDir=$(cd `dirname "$0"`; pwd)}

SHBuild_PrintUsage()
{
	echo Usage: "$0" [-cCONF] [SHBOPT ...]
	printf 'Build application using SHBuild.\n\n'
	printf 'CONF\n'
	printf '\tThe configuration name. If begins with "debug",'
	printf ' or environment variable SHBuild_Debug is set as non null value,'
	printf ' using debug configuration;'
	printf ' otherwise using release configuration.'
	printf ' If ends with "static",'
	printf ' or environment variable SHBuild_Static is set as non null value,'
	printf ' using debug configuration;'
	printf ' otherwise using release configuration.\n'
	printf '\tThe output directory is determined as ".CONF".\n\n'
	printf 'SHBOPT ...\n'
	printf '\tThe options remained to pass to SHBuild.\n\n'
	printf 'There are several other environment variables to control the build.'
	printf '\n\n'
	printf 'SHBuild_NoAdjustSubsystem\n'
	printf '\tIf set to non null, no adjustment for linker arguments would be'
	printf ' performed for MinGW. Otherwise, "-mwindows" would be added in '
	printf ' linker command line in release modes.\n'
	printf '\n'
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
if [[ x"${SHBuild_Conf}" =~ xdebug.* ]]; then
	echo Debug mode turned on by configuration.
	SHBuild_Debug=${SHBuild_Conf}
fi
if [[ x"${SHBuild_Conf}" =~ x.*static ]]; then
	echo Static mode turned on by configuration.
	SHBuild_Static=${SHBuild_Conf}
fi
export SHBuild_Debug
export SHBuild_Static

SHBOPT="-xd,.${SHBuild_Conf} -xmode,2 $@"
. ${SHBuild_Bin}/SHBuild-common.sh
if hash gcc-ar > /dev/null; then
	export AR=gcc-ar
fi
. ${SHBuild_Bin}/SHBuild-common-toolchain.sh
if [ x${SHBuild_Debug} != x ]; then
	echo Use debug configuration ${SHBuild_Conf}.
	CXXFLAGS_OPT_DBG="-O0 -g"
	unset CXXFLAGS_COMMON
	unset CXXFLAGS
	LDFLAGS=" "
	source ${SHBuild_Bin}/SHBuild-common-options.sh
	export SHBuild_YSLib_LibNames='-lYFrameworkd -lYBased'
else
	echo Use release configuration ${SHBuild_Conf}.
	unset CXXFLAGS_OPT_DBG
	unset CXXFLAGS_COMMON
	unset CXXFLAGS
	unset LDFLAGS
	source ${SHBuild_Bin}/SHBuild-common-options.sh
	if [ x"${SHBuild_NoAdjustSubsystem}" == x ]; then
		LDFLAGS="${LDFLAGS} -mwindows"
		echo Added \"-mwindows\" to LDFLAGS.
	fi
	export SHBuild_YSLib_LibNames='-lYFramework -lYBase'
fi
export LDFLAGS
if [ x"${SHBuild_Static}" == x ]; then
	export SHBuild_YSLib_Flags="${CXXFLAGS} -DYF_DLL -DYB_DLL \
		-I${SHBuild_Bin}/../include"
	export LIBS="-L`SHBuild_2w "${SHBuild_Bin}/../lib"` \
		${SHBuild_YSLib_LibNames}"
else
	export SHBuild_YSLib_Flags="${CXXFLAGS} \
		-I${SHBuild_Bin}/../include"
	export SHBuild_YSLib_LibNames="${SHBuild_YSLib_LibNames} \
		-lFreeImage -lfreetype -L/usr/lib -lgdi32 -limm32"
	export LIBS="-L`SHBuild_2w "${SHBuild_Bin}/../lib"` -Wl,-dn \
		${SHBuild_YSLib_LibNames}"
fi

SHBuild_BuildApp()
{
	(cd ${SHBuild_AppBaseDir}; "${SHBuild_Bin}/SHBuild" ${SHBOPT} -xid,include \
		$@ ${SHBuild_YSLib_Flags})
}

SHBuild_EchoVar_N "CXXFLAGS"
SHBuild_EchoVar_N "LDFLAGS"
SHBuild_EchoVar_N "SHBOPT"

