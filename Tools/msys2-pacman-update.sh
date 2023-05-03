#!/usr/bin/env sh
# (C) 2014-2015 FrankHB.
# Script for MSYS2 packages update using pacman.

echo Updating necessary tools for native hosted environment ...
# NOTE: 'mingw-w64-i686-binutils-git' is needed for LTO plugin support,
#	'mingw-w64-i686-binutils' is not enough for LTO-enabled building.
# 'make' is needed for LTO wrapper and build process for other platforms.
pacman -Syu --needed \
	mingw32/mingw-w64-i686-binutils \
	mingw32/mingw-w64-i686-gcc \
	msys/make
echo Done.

# echo Updating necessary tools for other platforms ...
# echo Done.

echo Updating optional tools ...
# NOTE: 'mingw-w64-i686-nasm' is only used to build FreeImage with
#	libjpeg-turbo.
# NOTE: 'rsync' is not necessary for building, but recommended for installing.
pacman -S --needed \
	mingw32/mingw-w64-i686-gdb \
	mingw32/mingw-w64-i686-nasm \
	msys/base-devel \
	msys/rsync
# XXX: Install freetype2?
echo Done.

