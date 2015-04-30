#!/usr/bin/env sh
# (C) 2014-2015 FrankHB.
# Script for installing headers at default location (/usr/include).
# Requires: rsync.

set -e

cd `dirname $0`/..

# FIXME: Possibly header pollution.
rsync -av 3rdparty/include/ /usr/include

rsync -av YBase/include/ /usr/include
rsync -av YFramework/include/ /usr/include
rsync -av YFramework/DS/include/ /usr/include
rsync -av YFramework/MinGW32/include/ /usr/include

