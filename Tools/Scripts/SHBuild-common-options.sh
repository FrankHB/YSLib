#!/usr/bin/bash
# (C) 2014 FrankHB.
# Common options script for build YSLib using SHBuild.

: ${C_CXXFLAGS_GC:="-fdata-sections -ffunction-sections"}

: ${LDFLAGS_GC:="-Wl,--gc-sections"}
#: ${LDFLAGS_GC:="-Wl,--gc-sections -Wl,--print-gc-sections"}

# See http://sourceforge.net/p/msys2/discussion/general/thread/2d6adff2/?limit=25 .
if !(uname | grep MINGW > /dev/null || uname | grep MSYS > /dev/null); then
	if !(echo "int main(){}" | "$CXX" -xc++ -o/tmp/null $C_CXXFLAGS_GC \
		$LDFLAGS_GC -; 2>& 1 > /dev/null); then
		C_CXXFLAGS_GC=""
		LDFLAGS_GC=""
	fi
fi

: ${C_CXXFLAGS_COMMON:="-pipe $C_CXXFLAGS_GC -pedantic-errors"}
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

: ${CFLAGS_COMMON:="$C_CXXFLAGS_COMMON $CFLAGS_STD $CFLAGS_WARNING"}

# TODO: BSD etc.
if "$CXX" -dumpspecs 2>& 1 | grep mthreads: > /dev/null; then
	CXXFLAGS_IMPL_COMMON_THRD_="-mthreads"
elif uname | grep Linux > /dev/null; then
	CXXFLAGS_IMPL_COMMON_THRD_="-pthread"
elif uname | grep Darwin > /dev/null; then
	CXXFLAGS_IMPL_COMMON_THRD_="-pthread"
fi

# NOTE: The compiler should be specified earlier than this line to
#	automatically determine if these values should be used.
if echo "$CXX" | grep clang++ > /dev/null; then
	: ${CXXFLAGS_IMPL_WARNING:=" \
		-Wno-deprecated-register \
		-Wno-mismatched-tags \
		"}
#	: ${CXXFLAGS_IMPL_OPT:="-flto"}
	: ${LDFLAGS_IMPL_OPT:="$CXXFLAGS_IMPL_OPT"}
elif echo "$CXX" | grep g++ > /dev/null; then
	: ${CXXFLAGS_IMPL_WARNING:=" \
		-Wzero-as-null-pointer-constant \
		"}
	: ${CXXFLAGS_IMPL_OPT:="-s -fexpensive-optimizations -flto=jobserver"}
	: ${LDFLAGS_IMPL_OPT:="-s -fexpensive-optimizations -flto"}
fi
: ${CXXFLAGS_IMPL_COMMON:="$CXXFLAGS_IMPL_COMMON_THRD_"}

: ${CXXFLAGS_STD:="-std=c++11"}
: ${CXXFLAGS_WARNING:=" $CFLAGS_WARNING \
	-Wctor-dtor-privacy \
	-Wnon-virtual-dtor \
	$CXXFLAGS_IMPL_WARNING \
	"}
: ${CXXFLAGS_COMMON:=" $CXXFLAGS_STD \
	$C_CXXFLAGS_COMMON \
	$CXXFLAGS_WARNING \
	$CXXFLAGS_IMPL_COMMON \
	"}
if [ x"$CXXFLAGS_OPT_UseAssert" == x ]; then
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

: ${CXXFLAGS:="$CXXFLAGS_COMMON $CXXFLAGS_OPT_DBG"}

: ${LDFLAGS_OPT_DBG:="$LDFLAGS_IMPL_OPT $LDFLAGS_GC"}

: ${LDFLAGS:="$LDFLAGS_OPT_DBG"}

