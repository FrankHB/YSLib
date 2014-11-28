#!/usr/bin/bash
# (C) 2014 FrankHB.
# Test script for self-hosting SHBuild.

set -e
SHBuild_ToolDir=$(cd `dirname "$0"`; pwd)
: ${SHBuild_BaseDir:="${SHBuild_ToolDir}/../SHBuild"}
. ${SHBuild_ToolDir}/SHBuild-bootstrap.sh

SHBuild_Pushd
cd ${SHBuild_BaseDir}

${SHBuild_BaseDir}/shbuild . $@ ${CXXFLAGS} ${INCLUDES}
${LD} ${SHBuild_BuildDir}/SHBuild.a -o${SHBuild_BuildDir}/SHBuild \
	${CXXFLAGS} ${LDFLAGS} ${INCLUDES} ${LIBS}

SHBuild_Popd
echo Done.

