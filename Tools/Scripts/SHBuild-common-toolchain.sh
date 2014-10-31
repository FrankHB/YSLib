#!/usr/bin/bash
# (C) 2014 FrankHB.
# Common source script: toolchain configuration.

: ${CC:="gcc"}
export CC
: ${CXX:="g++"}
export CXX
: ${AR:="ar"}
export AR
: ${ARFLAGS:="rcs"}
export ARFLAGS
: ${LD:=${CXX}}
export LD

