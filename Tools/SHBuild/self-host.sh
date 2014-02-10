#!/usr/bin/sh
# (C) 2014 FrankHB.
# Test script for self-hosting SHBuild.

set -e

CXXFLAGS="-O3 -pipe -s -DNDEBUG -DYB_DLL -DYF_DLL -std=c++11 -Wall"
INCLUDES="-I/include"
LIBS="-lYFramework -lYBase"
ARGS="${CXXFLAGS} ${INCLUDES} ${LIBS}"

./shbuild . ${ARGS}
g++ .shbuild/SHBuild.a ${ARGS} -o .shbuild/SHBuild

echo Done.

