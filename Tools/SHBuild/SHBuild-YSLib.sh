#!/usr/bin/sh
# (C) 2014 FrankHB.
# Example script for build YSLib using SHBuild.

set -e
[[ ${SHBuild_BaseDir} ]] || SHBuild_BaseDir=$(cd `dirname "$0"`; pwd)
[[ ${SHBuild_ToolDir} ]] || SHBuild_ToolDir=${SHBuild_BaseDir}
source ${SHBuild_ToolDir}/common.sh
AR=gcc-ar
source ${SHBuild_ToolDir}/common-toolchain.sh

SHBOPT="-xid,alternative -xid,data -xid,include -xid,Android"

unset CXXFLAGS_OPT_DBG
unset CXXFLAGS_COMMON
unset CXXFLAGS
unset LDFLAGS
source ${SHBuild_ToolDir}/SHBuild-options.sh
LDFLAGS="${LDFLAGS} -Wl,--dn"

SHBuild_EchoVar "SHBOPT" "${SHBOPT}"
SHBuild_EchoVar "CXXFLAGS" "${CXXFLAGS}"
SHBuild_EchoVar "LDFLAGS" "${LDFLAGS}"

SHBuild_Pushd
cd ${SHBuild_BaseDir}

# TODO: Run link commands.
${SHBuild_ToolDir}/SHBuild ${SHBOPT} "$@" ../../YBase \
	${CXXFLAGS} \
	-I../../YBase/include
${SHBuild_ToolDir}/SHBuild ${SHBOPT} "$@" ../../YFramework \
	${CXXFLAGS} \
	-DFREEIMAGE_LIB \
	-I../../YFramework/include -I../../YFramework/Android/include \
	-I../../YFramework/DS/include -I../../YFramework/MinGW32/include \
	-I../../3rdparty/include -I../../YBase/include

SHBuild_Popd
echo Done.

