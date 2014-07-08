#!/usr/bin/sh
# (C) 2014 FrankHB.
# Script for installing headers at default location (/usr/include).
# Required rsync.

set -e

pushd .

# XXX: Location related.
cd ..

# FIXME: Possibly header pollution.
rsync -av 3rdparty/include/ /usr/include

rsync -av YBase/include/ /usr/include
rsync -av YFramework/include/ /usr/include
rsync -av YFramework/DS/include/ /usr/include
rsync -av YFramework/MinGW32/include/ /usr/include

