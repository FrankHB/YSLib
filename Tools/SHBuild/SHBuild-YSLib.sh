#!/usr/bin/sh
# (C) 2014 FrankHB.
# Example script for build YSLib using SHBuild.

set -e

# TODO: Run link commands.
# FIXME: Alternative source should be excluded.
./shbuild ../../YBase -O3 -pipe -s -std=c++11 -pedantic-errors \
	-DNDEBUG -DYB_BUILD_DLL \
	-fdata-sections \
	-fexpensive-optimizations \
	-ffunction-sections \
	-flto=jobserver \
	-fomit-frame-pointer \
	-mthreads \
	-Wall -Wextra \
	-Winvalid-pch \
	-Wzero-as-null-pointer-constant \
	-I../../YBase/include
./shbuild ../../YFramework -O3 -pipe -s -std=c++11 \
	-DNDEBUG -DYB_DLL -DYF_BUILD_DLL -DFREEIMAGE_LIB \
	-fdata-sections \
	-fexpensive-optimizations \
	-ffunction-sections \
	-flto=jobserver \
	-fomit-frame-pointer \
	-mthreads \
	-Wall -Wextra \
	-Winvalid-pch \
	-Wzero-as-null-pointer-constant \
	-I../../YFramework/include -I../../YFramework/Android/include \
	-I../../YFramework/DS/include -I../../YFramework/MinGW32/include \
	-I../../3rdparty/include -I../../YBase/include \
	-lYBase

echo Done.

