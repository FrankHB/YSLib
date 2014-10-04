#!/usr/bin/sh
# (C) 2014 FrankHB.
# Common source script: toolchain configuration.

[[ ${CC} ]] || export CC="gcc"
[[ ${CXX} ]] || export CXX="g++"
[[ ${AR} ]] || export AR="ar"
[[ ${ARFLAGS} ]] || export ARFLAGS="rcs"
[[ ${LD} ]] || export LD=${CXX}

