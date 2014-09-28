#!/usr/bin/sh
# (C) 2014 FrankHB.
# Test script for self-hosting SHBuild.

set -e
SHBuild_BaseDir=`dirname $0`
SHBuild_BaseDir=`(cd ${SHBuild_BaseDir}; pwd)`
SHBuild_Me=`basename $0`
source ${SHBuild_BaseDir}/common-bootstrap.sh

SHBuild_Pushd
cd ${SHBuild_BaseDir}

./shbuild . ${CXXFLAGS} ${INCLUDES}
${CXX} ${SHBuild_BuildDir}/SHBuild.a -o${SHBuild_BuildDir}/SHBuild \
	${CXXFLAGS} ${LDFLAGS} ${INCLUDES} ${LIBS}

SHBuild_Popd
echo Done.

