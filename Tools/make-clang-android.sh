#!/usr/bin/env sh
# (C) 2014-2016 FrankHB.
# Script for make standalone Android Clang toolchain based on NDK r9d x86_64.

set -e

BASE_DIRECTORY=`dirname $0`
BASE_DIRECTORY=`(cd $BASE_DIRECTORY; pwd)`

# Prerequisition: bash, tar, etc. Set $NDK to the Android NDK root directory.
# Make sure '/tmp' is writable and large enough (at least ~300MiB).

$NDK/build/tools/make-standalone-toolchain.sh \
	--package-dir=$BASE_DIRECTORY \
	--platform=android-9 \
	--toolchain=arm-linux-androideabi-5 \
	--llvm-version=3.7 \
	--system=windows-x86_64 \
	--verbose

echo Done.

