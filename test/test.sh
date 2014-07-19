#!/usr/bin/sh
# (C) 2014 FrankHB.
# Script for testing.
# Requires: G++, YBase.

set -e

CXXFLAGS="-O3 -pipe -DNDEBUG -std=c++11 -Wall"

LDFLAGS="-s -Wl,--dn -Wl,--gc-sections"

INCLUDES=" \
	-I../YFramework/include -I../YFramework/Android/include \
	-I../YFramework/DS/include -I../YFramework/MinGW32/include \
	-I../3rdparty/include -I../YBase/include \
	"

LIBS=" \
	../YBase/source/ytest/test.cpp \
	"

g++ YBase.cpp -oYBase ${CXXFLAGS} ${LDFLAGS} ${INCLUDES} ${LIBS}

./YBase

echo Done.

