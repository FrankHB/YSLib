#!/usr/bin/sh
# (C) 2014 FrankHB.
# Build script for SHBuild.

set -e

g++ main.cpp -O3 -pipe -s -DNDEBUG -DYB_DLL -DYF_DLL -std=c++11 -Wall -I/include -L/lib -lYFramework -lYBase -o SHBuild

echo Done.

