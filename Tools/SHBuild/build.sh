#!/usr/bin/sh
# (C) 2014 FrankHB.
# Build script for SHBuild.

set -e
SHBuild_BaseDir=`dirname $0`
SHBuild_BaseDir=`(cd ${SHBuild_BaseDir}; pwd)`
SHBuild_Me=`basename $0`
source ${SHBuild_BaseDir}/common-bootstrap.sh

SHBuild_Pushd
cd ${SHBuild_BaseDir}

${CXX} main.cpp -oSHBuild ${CXXFLAGS} ${LDFLAGS} ${INCLUDES} ${LIBS}

SHBuild_Popd
echo Done.

