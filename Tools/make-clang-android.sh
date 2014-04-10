#!/usr/bin/sh
# (C) 2014 FrankHB.
# Script for make standalone Android Clang toolchain based on NDK r9d x86_64.

set -e

BASE_DIRECTORY=`dirname $0`
BASE_DIRECTORY=`(cd ${BASE_DIRECTORY}; pwd)`

${NDK}/build/tools/make-standalone-toolchain.sh \
	--package-dir=${BASE_DIRECTORY} \
	--platform=android-9 \
	--toolchain=arm-linux-androideabi-4.8 \
	--llvm-version=3.4 \
	--system=windows-x86_64

echo Done.

