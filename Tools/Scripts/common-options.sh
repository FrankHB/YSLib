#!/usr/bin/bash
# (C) 2014 FrankHB.
# Example script for build YSLib using SHBuild.

: ${CFLAGS_WARNING:=" \
	-Wall \
	-Wcast-align \
	-Wextra \
	-Winit-self \
	-Winvalid-pch \
	-Wmain \
	-Wmissing-declarations \
	-Wmissing-include-dirs \
	-Wnon-virtual-dtor \
	-Wredundant-decls \
	-Wsign-promo \
	-Wunreachable-code \
	-Wzero-as-null-pointer-constant \
	"}
: ${CXXFLAGS_STD:="-std=c++11"}
: ${CXXFLAGS_WARNING:=" ${CFLAGS_WARNING} \
	-Wctor-dtor-privacy \
	-Wnon-virtual-dtor \
	-Wzero-as-null-pointer-constant \
	"}
: ${CXXFLAGS_COMMON:=" -pipe ${CXXFLAGS_STD} \
	-fdata-sections \
	-ffunction-sections \
	${CXXFLAGS_WARNING} \
	-mthreads \
	-pedantic-errors \
	"}
: ${CXXFLAGS_OPT_DBG:=" \
	-O3 -s \
	-DNDEBUG \
	-fexpensive-optimizations \
	-flto=jobserver \
	-fomit-frame-pointer \
	"}
: ${CXXFLAGS:="${CXXFLAGS_COMMON} ${CXXFLAGS_OPT_DBG}"}
: ${LDFLAGS:="-s -Wl,--gc-sections"}

