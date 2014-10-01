#!/usr/bin/sh
# (C) 2014 FrankHB.
# Example script for build YSLib using SHBuild.

set -e
SHBuild_BaseDir=`dirname $0`
SHBuild_BaseDir=`(cd ${SHBuild_BaseDir}; pwd)`
SHBuild_Me=`basename $0`
source ${SHBuild_BaseDir}/common.sh

SHBOPT="-xid,alternative -xid,data -xid,include -xid,Android"

COMMON_CXXFLAGS=" \
	-O3 -pipe -s -std=c++11 \
	-DNDEBUG \
	-Wall \
	-Wcast-align \
	-Wextra \
	-Winit-self \
	-Winvalid-pch \
	-Wmain \
	-Wmissing-declarations \
	-Wmissing-include-dirs \
	-Wnon-virtual-dtor \
	-Wredundant-decls \
	-Wunreachable-code \
	-Wzero-as-null-pointer-constant \
	-fdata-sections \
	-fexpensive-optimizations \
	-ffunction-sections \
	-flto=jobserver \
	-fomit-frame-pointer \
	-mthreads \
	-pedantic-errors \
	"

SHBuild_EchoVar "SHBOPT" "${SHBOPT}"
SHBuild_EchoVar "COMMON_CXXFLAGS" "${COMMON_CXXFLAGS}"

SHBuild_Pushd
cd ${SHBuild_BaseDir}

# TODO: Run link commands.
AR=gcc-ar ./shbuild ${SHBOPT} "$@" ../../YBase \
	${COMMON_CXXFLAGS} \
	-I../../YBase/include
AR=gcc-ar ./shbuild ${SHBOPT} "$@" ../../YFramework \
	${COMMON_CXXFLAGS} \
	-DFREEIMAGE_LIB \
	-I../../YFramework/include -I../../YFramework/Android/include \
	-I../../YFramework/DS/include -I../../YFramework/MinGW32/include \
	-I../../3rdparty/include -I../../YBase/include

SHBuild_Popd
echo Done.

