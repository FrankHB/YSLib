#!/usr/bin/bash
# (C) 2014 FrankHB.
# Script for build YSLib release configurations using SHBuild.

set -e
SHBuild_ToolDir=$(cd `dirname "$0"`; pwd)
. ${SHBuild_ToolDir}/SHBuild-YSLib-common.sh

SHBuild_Pushd
cd ${SHBuild_BaseDir}

unset CXXFLAGS_OPT_DBG

if [ x"${SHBuild_NoStatic}" == x ]; then
	echo Building static libraries ...

	SHBOPT="${SHBOPT_IGN}"
	unset LDFLAGS
	. ${SHBuild_ToolDir}/SHBuild-common-options.sh
	LDFLAGS="${LDFLAGS} -Wl,--dn"

	SHBuild_EchoVar "SHBOPT" "${SHBOPT}"
	SHBuild_EchoVar "CXXFLAGS" "${CXXFLAGS}"
	SHBuild_EchoVar "LDFLAGS" "${LDFLAGS}"

	${SHBuild} ${SHBOPT} $@ ../../YBase \
		${CXXFLAGS} ${INCLUDES_YBase}
	${SHBuild} ${SHBOPT} $@ ../../YFramework \
		${CXXFLAGS} -DFREEIMAGE_LIB ${INCLUDES_YFramework} ${INCLUDES_YBase}

	echo Finished building static libraries.
else
	echo Skipped building static libraries.
fi

if [ x"${SHBuild_NoDynamic}" == x ]; then
	echo Building dynamic libraries ...

	SHBOPT="-xd,.shbuild-dll ${SHBOPT_IGN} -xmode,2"
	unset LDFLAGS
	. ${SHBuild_ToolDir}/SHBuild-common-options.sh
	export LDFLAGS="${LDFLAGS} -shared -Wl,--dll"
	LIBS_YFramework="-L../../YFramework/${SHBuild_YSLib_Platform}/lib \
		-lFreeImage -lfreetype"

	SHBuild_EchoVar "SHBOPT" "${SHBOPT}"
	SHBuild_EchoVar "CXXFLAGS" "${CXXFLAGS}"
	SHBuild_EchoVar "LDFLAGS" "${LDFLAGS}"
	SHBuild_EchoVar "LIBS_YFramework" "${LIBS_YFramework}"

	${SHBuild} ${SHBOPT} $@ ../../YBase \
		${CXXFLAGS} -DYB_BUILD_DLL ${INCLUDES_YBase}

	export LIBS="${LIBS_YFramework} -L.shbuild-dll -lYBase -lgdi32 -limm32"

	${SHBuild} ${SHBOPT} $@ ../../YFramework \
		${CXXFLAGS} -DYB_BUILD_DLL -DYF_BUILD_DLL \
		-DFREEIMAGE_LIB ${INCLUDES_YFramework} ${INCLUDES_YBase}

	echo Finished building dynamic libraries.
else
	echo Skipped building dynamic libraries.
fi

SHBuild_Popd
echo Done.

