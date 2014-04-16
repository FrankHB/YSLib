#!/usr/bin/sh
# (C) 2014 FrankHB.
# Script for MSys2 packages update using pacman.

set -e

pacman -S mingw32/mingw-w64-i686-gcc mingw32/mingw-w64-i686-make make

echo Done.

