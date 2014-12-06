#!/usr/bin/bash
# (C) 2014 FrankHB.
# Example script for build YSLib using SHBuild.

: ${C_CXXFLAGS_COMMON:=" \
	-pipe \
	-fdata-sections \
	-ffunction-sections \
	-pedantic-errors \
	"}
: ${C_CXXFLAGS_OPT_LV:="-O3"}

: ${CFLAGS_STD:="-std=c11"}
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

: ${CFLAGS_COMMON:="${C_CXXFLAGS_COMMON} ${CFLAGS_STD} ${CFLAGS_WARNING}"}

# NOTE: The compiler should be specified earlier than this line to
#	automatically determine if these values should be used.
if echo ${CXX} | grep clang++ > /dev/null; then
	: ${CXXFLAGS_IMPL_WARNING:=" \
		-Wno-deprecated-register \
		-Wno-mismatched-tags \
		"}
	: ${CXXFLAGS_IMPL_COMMON:=""}
	: ${CXXFLAGS_IMPL_OPT:="-flto"}
	: ${LDFLAGS_IMPL_OPT:="${CXXFLAGS_IMPL_OPT}"}
elif echo ${CXX} | grep g++ > /dev/null; then
	: ${CXXFLAGS_IMPL_WARNING:=" \
		-Wzero-as-null-pointer-constant \
		"}
	: ${CXXFLAGS_IMPL_COMMON:="-mthreads"}
	: ${CXXFLAGS_IMPL_OPT:="-s -fexpensive-optimizations -flto=jobserver"}
	: ${LDFLAGS_IMPL_OPT:="-s -fexpensive-optimizations -flto"}
fi

: ${CXXFLAGS_STD:="-std=c++11"}
: ${CXXFLAGS_WARNING:=" ${CFLAGS_WARNING} \
	-Wctor-dtor-privacy \
	-Wnon-virtual-dtor \
	${CXXFLAGS_IMPL_WARNING} \
	"}
: ${CXXFLAGS_COMMON:=" ${CXXFLAGS_STD} \
	${C_CXXFLAGS_COMMON} \
	${CXXFLAGS_WARNING} \
	${CXXFLAGS_IMPL_COMMON} \
	"}
if [ x"$CXXFLAGS_OPT_UseAssert" == x ]; then
	: ${CXXFLAGS_OPT_DBG:=" \
		${C_CXXFLAGS_OPT_LV} -DNDEBUG \
		${CXXFLAGS_IMPL_OPT} \
		-fomit-frame-pointer \
		"}
else
	: ${CXXFLAGS_OPT_DBG:=" \
		${C_CXXFLAGS_OPT_LV} \
		${CXXFLAGS_IMPL_OPT} \
		-fomit-frame-pointer \
		"}
fi

# XXX: Rename %CXXFLAGS_OPT_DBG to CFLAGS_OPT_DBG or C_CXXFLAGS_OPT_DBG?
: ${CFLAGS:="${CFLAGS_COMMON} ${CXXFLAGS_OPT_DBG}"}

: ${CXXFLAGS:="${CXXFLAGS_COMMON} ${CXXFLAGS_OPT_DBG}"}

: ${LDFLAGS_OPT_DBG:="-s -Wl,--gc-sections"}

: ${LDFLAGS:="${LDFLAGS_OPT_DBG}"}

