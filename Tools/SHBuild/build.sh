#!/usr/bin/sh
# (C) 2014 FrankHB.
# Build script for SHBuild.

set -e
[[ ${SHBuild_BaseDir} ]] || SHBuild_BaseDir=$(cd `dirname "$0"`; pwd)
[[ ${SHBuild_ToolDir} ]] || SHBuild_ToolDir=${SHBuild_BaseDir}
source ${SHBuild_ToolDir}/common-bootstrap.sh

SHBuild_Pushd
cd ${SHBuild_BaseDir}

${CXX} main.cpp -oSHBuild ${CXXFLAGS} ${LDFLAGS} ${INCLUDES} ${LIBS}

SHBuild_Popd
echo Done.

