#!/usr/bin/bash
# (C) 2014 FrankHB.
# Build script for SHBuild.

set -e
: ${SHBuild_ToolDir:=$(cd `dirname "$0"`; pwd)}
: ${SHBuild_BaseDir:="${SHBuild_ToolDir}/../SHBuild"}
. ${SHBuild_ToolDir}/SHBuild-bootstrap.sh

SHBuild_Pushd
cd ${SHBuild_BaseDir}

echo Building ...
${CXX} main.cpp -oSHBuild ${CXXFLAGS} ${LDFLAGS} ${INCLUDES} ${LIBS}

SHBuild_Popd
echo Done.

