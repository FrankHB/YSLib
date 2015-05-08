#!/usr/bin/env bash
# (C) 2014-2015 FrankHB.
# Common options script for build YSLib using SHBuild.

: ${SHBuild_ToolDir:="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"}
: ${SHBuild_AppBaseDir=$(cd `dirname "$0"`; pwd)}
. $SHBuild_ToolDir/SHBuild-common.sh
SHBuild_CheckUName

: ${C_CXXFLAGS_GC:='-fdata-sections -ffunction-sections'}

if [[ "$SHBuild_Env_OS" == 'OS_X' ]]; then
	: ${LDFLAGS_GC:="-Wl,--dead-strip"}
else
	: ${LDFLAGS_GC:="-Wl,--gc-sections"}
	#: ${LDFLAGS_GC:="-Wl,--gc-sections -Wl,--print-gc-sections"}
fi

# See http://sourceforge.net/p/msys2/discussion/general/thread/2d6adff2/?limit=25 .
if [[ "$SHBuild_Env_OS" != 'Win32' ]]; then
	if [[ "$CXX" != '' ]] && !(echo 'int main(){}' | "$CXX" -xc++ -o/tmp/null \
		$C_CXXFLAGS_GC $LDFLAGS_GC -; 2>& 1 > /dev/null); then
		C_CXXFLAGS_GC=''
		LDFLAGS_GC=''
	fi
	: ${C_CXXFLAGS_PIC:='-fPIC'}
fi

: ${C_CXXFLAGS_COMMON:="-pipe $C_CXXFLAGS_GC $C_CXXFLAGS_ARCH -pedantic-errors"}
: ${C_CXXFLAGS_OPT_LV:='-O3'}
: ${C_CXXFLAGS_WARNING:=" \
	-Wall \
	-Wcast-align \
	-Wdeprecated \
	-Wdeprecated-declarations \
	-Wextra \
	-Wfloat-equal \
	-Wformat=2 \
	-Winvalid-pch \
	-Wmissing-declarations \
	-Wmissing-include-dirs \
	-Wmultichar \
	-Wno-format-nonliteral \
	-Wpacked \
	-Wredundant-decls \
	-Wshadow \
	-Wsign-conversion \
	"}

# TODO: BSD etc.
if "$CXX" -dumpspecs 2>& 1 | grep mthreads: > /dev/null; then
	CXXFLAGS_IMPL_COMMON_THRD_='-mthreads'
elif [[ "$SHBuild_Env_OS" == 'Linux' || "$SHBuild_Env_OS" == 'OS_X' ]]; then
	CXXFLAGS_IMPL_COMMON_THRD_='-pthread'
fi

# NOTE: The compiler should be specified earlier than this line to
#	automatically determine if these values should be used.
if echo "$CXX" | grep clang++ > /dev/null; then
	: ${CXXFLAGS_IMPL_WARNING:=" \
		-Wno-deprecated-register \
		-Wno-mismatched-tags \
		-Wshorten-64-to-32 \
		-Wweak-vtables \
		"}
#	: ${CXXFLAGS_IMPL_OPT:='-flto'}
	: ${LDFLAGS_IMPL_OPT:="$CXXFLAGS_IMPL_OPT"}
elif echo "$CXX" | grep g++ > /dev/null; then
	: ${C_CXXFLAGS_IMPL_WARNING:=" \
		-Wdouble-promotion \
		-Wlogical-op \
		-Wtrampolines \
		"}
	: ${CXXFLAGS_IMPL_WARNING:=" \
		-Wconditionally-supported \
		-Wstrict-null-sentinel \
		-Wzero-as-null-pointer-constant \
		"}
	: ${CXXFLAGS_IMPL_OPT:='-s -fexpensive-optimizations -flto=jobserver'}
	: ${LDFLAGS_IMPL_OPT:='-s -fexpensive-optimizations -flto'}
fi

: ${CFLAGS_STD:='-std=c11'}
: ${CFLAGS_WARNING:=" \
	$C_CXXFLAGS_WARNING \
	$C_CXXFLAGS_IMPL_WARNING \
	"}
: ${CFLAGS_COMMON:="$C_CXXFLAGS_COMMON $CFLAGS_STD $CFLAGS_WARNING"}

: ${CXXFLAGS_IMPL_COMMON:="$CXXFLAGS_IMPL_COMMON_THRD_"}

: ${CXXFLAGS_STD:='-std=c++11'}
: ${CXXFLAGS_WARNING:=" $CFLAGS_WARNING \
	-Wctor-dtor-privacy \
	-Wnon-virtual-dtor \
	-Woverloaded-virtual \
	-Wsign-promo \
	$C_CXXFLAGS_IMPL_WARNING \
	$CXXFLAGS_IMPL_WARNING \
	"}
: ${CXXFLAGS_COMMON:=" $CXXFLAGS_STD \
	$C_CXXFLAGS_COMMON \
	$CXXFLAGS_WARNING \
	$CXXFLAGS_IMPL_COMMON \
	"}
if [[ "$CXXFLAGS_OPT_UseAssert" == '' ]]; then
	: ${CXXFLAGS_OPT_DBG:=" \
		$C_CXXFLAGS_OPT_LV -DNDEBUG \
		$CXXFLAGS_IMPL_OPT \
		-fomit-frame-pointer \
		"}
else
	: ${CXXFLAGS_OPT_DBG:=" \
		$C_CXXFLAGS_OPT_LV \
		$CXXFLAGS_IMPL_OPT \
		-fomit-frame-pointer \
		"}
fi

# XXX: Rename %CXXFLAGS_OPT_DBG to CFLAGS_OPT_DBG or C_CXXFLAGS_OPT_DBG?
: ${CFLAGS:="$CFLAGS_COMMON $CXXFLAGS_OPT_DBG"}
CFLAGS="${CFLAGS//	/ }"

: ${CXXFLAGS:="$CXXFLAGS_COMMON $CXXFLAGS_OPT_DBG"}
CXXFLAGS="${CXXFLAGS//	/ }"

: ${LDFLAGS_OPT_DBG:="$LDFLAGS_IMPL_OPT $LDFLAGS_GC"}

if [[ "$SHBuild_Env_OS" == 'Win32' ]]; then
	: ${LDFLAGS_DYN_BASE:="-shared -Wl,--dll"}
	: ${DSOSFX:='.dll'}
	: ${EXESFX:='.exe'}
else
	: ${LDFLAGS_DYN_BASE:='-shared'}
	: ${LIBS_RPATH:="-Wl,-rpath,'\$ORIGIN:\$ORIGIN/../lib'"}
	: ${LIBPFX:='lib'}
	: ${DSOSFX:='.so'}
fi

: ${LDFLAGS_DYN_EXTRA:="-Wl,--no-undefined \
	-Wl,--dynamic-list-data,--dynamic-list-cpp-new,--dynamic-list-cpp-typeinfo"}

: ${LDFLAGS_DYN:="$LDFLAGS_DYN_BASE $LDFLAGS_DYN_EXTRA"}

: ${LDFLAGS:="$LDFLAGS_OPT_DBG"}
LDFLAGS="${LDFLAGS//	/ }"

