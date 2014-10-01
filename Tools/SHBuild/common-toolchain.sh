#!/usr/bin/sh
# (C) 2014 FrankHB.
# Common source script: toolchain configuration.

[[ ${CC} ]] || CC="gcc"
[[ ${CXX} ]] || CXX="g++"
[[ ${AR} ]] || AR="ar"
[[ ${ARFLAGS} ]] || ARFLAGS="rcs"
[[ ${LD} ]] || LD=${CXX}
[[ ${CXXFLAGS} ]]  || CXXFLAGS="-O3 -pipe -DNDEBUG -std=c++11 -Wall"
[[ ${LDFLAGS} ]]  || LDFLAGS="-s -Wl,--dn -Wl,--gc-sections"

