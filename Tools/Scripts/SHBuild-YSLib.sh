#!/usr/bin/bash
# (C) 2014 FrankHB.
# Example script for build YSLib using SHBuild.

set -e
SHBuild_ToolDir=$(cd `dirname "$0"`; pwd)
: ${SHBuild_BaseDir:="${SHBuild_ToolDir}/../SHBuild"}
source ${SHBuild_ToolDir}/SHBuild-common.sh
export AR="gcc-ar"
source ${SHBuild_ToolDir}/SHBuild-common-toolchain.sh

: ${SHBuild:="${SHBuild_BaseDir}/SHBuild"}
: ${SHBuild_YSLib_Platform:="MinGW32"}
INCLUDES_YBase="-I../../YBase/include"
INCLUDES_YFramework=" \
	-I../../YFramework/include -I../../YFramework/Android/include \
	-I../../YFramework/DS/include -I../../YFramework/MinGW32/include \
	-I../../3rdparty/include -I../../YBase/include"

SHBuild_EchoVar "SHBuild" "${SHBuild}"
SHBuild_EchoVar "SHBuild_YSLib_Platform" "${SHBuild_YSLib_Platform}"
SHBuild_EchoVar "INCLUDES_YBase" "${INCLUDES_YBase}"
SHBuild_EchoVar "INCLUDES_YFramework" "${INCLUDES_YFramework}"

export LANG=C
SHBuild_Pushd
cd ${SHBuild_BaseDir}

echo Building static libraries ...

SHBOPT="-xid,alternative -xid,data -xid,include -xid,Android"
unset CXXFLAGS_OPT_DBG
unset CXXFLAGS_COMMON
unset CXXFLAGS
unset LDFLAGS
source ${SHBuild_ToolDir}/SHBuild-common-options.sh
LDFLAGS="${LDFLAGS} -Wl,--dn"

SHBuild_EchoVar "SHBOPT" "${SHBOPT}"
SHBuild_EchoVar "CXXFLAGS" "${CXXFLAGS}"
SHBuild_EchoVar "LDFLAGS" "${LDFLAGS}"

${SHBuild} ${SHBOPT} $@ ../../YBase \
	${CXXFLAGS} ${INCLUDES_YBase}
${SHBuild} ${SHBOPT} $@ ../../YFramework \
	${CXXFLAGS} -DFREEIMAGE_LIB ${INCLUDES_YFramework} ${INCLUDES_YBase}

echo Finished building static libraries.

echo Building dynamic libraries ...

SHBOPT="-xd,.shbuild-dll -xid,alternative -xid,data -xid,include -xid,Android \
	-xmode,2"
unset CXXFLAGS_OPT_DBG
unset CXXFLAGS_COMMON
unset CXXFLAGS
unset LDFLAGS
source ${SHBuild_ToolDir}/SHBuild-common-options.sh
export LDFLAGS="${LDFLAGS} -shared -Wl,--dll"
LIBS_YFramework="-L../../YFramework/${SHBuild_YSLib_Platform}/lib \
	-lFreeImage -lfreetype"

SHBuild_EchoVar "SHBOPT" "${SHBOPT}"
SHBuild_EchoVar "CXXFLAGS" "${CXXFLAGS}"
SHBuild_EchoVar "LDFLAGS" "${LDFLAGS}"
SHBuild_EchoVar "LIBS_YFramework" "${LIBS_YFramework}"

${SHBuild} ${SHBOPT} $@ ../../YBase \
	${CXXFLAGS} -DYB_BUILD_DLL ${INCLUDES_YBase}

export LIBS="${LIBS_YFramework} -L.shbuild-dll -lYBase \
	-lgdi32 -limm32 -lShlWapi"

${SHBuild} ${SHBOPT} $@ ../../YFramework \
	${CXXFLAGS} -DYB_BUILD_DLL -DYF_BUILD_DLL \
	-DFREEIMAGE_LIB ${INCLUDES_YFramework} ${INCLUDES_YBase}

echo Finished building dynamic libraries.

SHBuild_Popd
echo Done.

