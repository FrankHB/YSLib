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
	-Wredundant-decls \
	-Wsign-promo \
	-Wunreachable-code \
	"}

# NOTE: The compiler should be specified earlier than this line to
#	automatically determine if these values should be used.
if echo ${CXX} | grep clang++ > /dev/null; then
	CXXFLAGS_IMPL_WARNING=" \
		-Wno-deprecated-register \
		-Wno-mismatched-tags \
		"
	CXXFLAGS_IMPL_COMMON=""
	CXXFLAGS_IMPL_OPT="-flto"
elif echo ${CXX} | grep g++ > /dev/null; then
	CXXFLAGS_IMPL_WARNING=" \
		-Wzero-as-null-pointer-constant \
		"
	CXXFLAGS_IMPL_COMMON="-mthreads"
	CXXFLAGS_IMPL_OPT="-s -fexpensive-optimizations -flto=jobserver"
fi

: ${CXXFLAGS_STD:="-std=c++11"}
: ${CXXFLAGS_WARNING:=" ${CFLAGS_WARNING} \
	-Wctor-dtor-privacy \
	-Wnon-virtual-dtor \
	${CXXFLAGS_IMPL_WARNING} \
	"}
: ${CXXFLAGS_COMMON:=" -pipe ${CXXFLAGS_STD} \
	-fdata-sections \
	-ffunction-sections \
	${CXXFLAGS_WARNING} \
	${CXXFLAGS_IMPL_COMMON} \
	-pedantic-errors \
	"}
: ${CXXFLAGS_OPT_DBG:=" \
	-O3 -DNDEBUG \
	${CXXFLAGS_IMPL_OPT} \
	-fomit-frame-pointer \
	"}
: ${CXXFLAGS:="${CXXFLAGS_COMMON} ${CXXFLAGS_OPT_DBG}"}

: ${LDFLAGS:="-s -Wl,--gc-sections"}

