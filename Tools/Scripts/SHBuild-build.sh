#!/usr/bin/env bash
# (C) 2014-2018, 2020-2023 FrankHB.
# Build script for SHBuild.

set -e

: "${SHBuild_ToolDir:=$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)}"
# XXX: Some options for stage 1 SHBuild are fixed. Provide more separated
#	options in future?
# XXX: Following variables are internal.
# shellcheck disable=2034
CXXFLAGS_OPT_UseAssert=true
# shellcheck disable=2034
C_CXXFLAGS_GC=' '
# shellcheck disable=2034
LDFLAGS_GC=' '
# shellcheck disable=2034
LDFLAGS_STRIP=' '
# NOTE: '-Og -g' is not supported. See also $2020-09 @ %Documentation::Workflow.
# shellcheck disable=2034
# XXX: This avoids clash between '-Og' with error 'Optimization level must be
#	between 0 and 3' caused by options passed to the linker from Clang++ driver.
#	Otherwise, there needs an additional version check.
CXXFLAGS_OPT_DBG='-O0 -g'
# XXX: Anyway LTO is not make it easy to debug as the intentional '-Og' or '-O0'
#	here, so simply disable it. See also
#	https://bugs.llvm.org/show_bug.cgi?id=38305.
# shellcheck disable=2034
LDFLAGS_IMPL_OPT=' '
# shellcheck source=./SHBuild-bootstrap.sh
. "$SHBuild_ToolDir/SHBuild-bootstrap.sh" # for SHBuild_Host_OS,
#	SHBuild_Host_Arch, SHBuild_Pushd, SHBuild_BaseDir, SHBuild_Puts, CXXFLAGS,
#	DoLTOCleanup_, DoLTOCleanup_Int_, DoLTOCleanup_AddTarget_, INCLUDES,
#	LDFLAGS, LIBS, SHBuild_Popd;

: "${SHBuild_Output:=SHBuild}"

# XXX: After MSYS2 enabling ASLR by default, x86_64 binutils with g++ is buggy.
#	See https://www.msys2.org/news/#2021-01-31-aslr-enabled-by-default,
#	https://github.com/msys2/MINGW-packages/issues/6986,
#	https://github.com/msys2/MINGW-packages/issues/7023,
#	and https://sourceware.org/bugzilla/show_bug.cgi?id=26659. Here is a
#	workaround to the issue.
# XXX: This is now fixed by https://github.com/msys2/MINGW-packages/pull/8259.
# TODO: Detect precise version?
# XXX: There are some other bugs not resolved for ld.bfd on PE targets:
#	https://sourceware.org/bugzilla/show_bug.cgi?id=11539,
#	https://sourceware.org/bugzilla/show_bug.cgi?id=19803.
if test "$SHBuild_Host_OS" = Win32 && test "$SHBuild_Host_Arch" = x86_64 \
	&& test "$SHBuild_CXX_Name" = G++; then
#	LDFLAGS_WKRD_="$(SHBuild_CheckCompiler "$CXX" \
#		'int main(){}' -Wl,--default-image-base-low '' \
#		-xc++ -Wl,--default-image-base-low)"
	# XXX: The linker (ld) from mingw-w64-x86_64-binutils 2.38-3 versions does
	#	not work otherwise. And mingw-w64-x86_64-lld-14.0.4-1 does not work with
	#	LTO here.
	LDFLAGS_WKRD_='-fuse-ld=lld'
else
	LDFLAGS_WKRD_=
fi

SHBuild_Pushd .
cd "$SHBuild_BaseDir"

SHBuild_Puts 'Building ...'

# Precompiled header is not used here because it does not work well with
#	external %CXXFLAGS_OPT_DBG. It is also not used frequently like in stage 2.
#	Even it is needed, it should be better separated with the stage 2 option.
#	When needed, uncomment the following command (where
#	%SHBuild_S1_InitializePCH is from %SHBuild_ToolDir/SHBuild-bootstrap.sh),
#	to provide %SHBuild_IncPCH.
# SHBuild_S1_InitializePCH

# Note '-fwhole-program' should not be used because there
#	do exist multiple translation units when linking with YSLib source,
#	otherwise there would be unresolved reference to names with external
#	linkage which had been optimized away.

# XXX: See %Tools/Scripts/SHBuild-common-options.sh.
trap DoLTOCleanup_ EXIT
trap DoLTOCleanup_Int_ INT QUIT TERM

Call_()
{
	# XXX: %SS_Verbose is external.
	# shellcheck disable=2154
	if test -n "$SS_Verbose"; then
		# shellcheck disable=2086
		SHBuild_Puts "$@"
	fi
	"$@"
}

DoLTOCleanup_AddTarget_ "$SHBuild_Output"
# XXX: Assume GNU parallel, not parallel from moreutils, if '--version' is
#	supported.
# XXX: %SS_NoParallel is external.
# shellcheck disable=2154
if test -z "$SS_NoParallel" && hash parallel 2> /dev/null \
	&& hash grep 2> /dev/null && parallel --will-cite --version \
	> /dev/null 2> /dev/null; \
	then
	SHBuild_Puts 'Using parallel.'
	if test -n "$SS_Verbose"; then
		SS_Verbose_t_=-t
	fi
	SHBuild_S1_Dir_="$(dirname "$SHBuild_Output")"
	SHBuild_Puts "Build directory is \"$SHBuild_S1_Dir_\"."
	if test -t 1; then
		if test "$SHBuild_CXX_Name" = G++ \
			&& ((SHBuild_CXX_Version >= 40900)) \
			|| (test "$SHBuild_CXX_Name" = Clang++ \
			&& ((SHBuild_CXX_Version >= 30300))); then
			CXXFLAGS_COLOR_=-fdiagnostics-color=always
		elif (test "$SHBuild_CXX_Name" = Clang++ \
			&& ((SHBuild_CXX_Version >= 30200))); then
			CXXFLAGS_COLOR_=-fcolor-diagnostics
		fi
	fi
	# XXX: Value of several variables may contain whitespaces.
	# shellcheck disable=2086
	parallel --will-cite $SS_Verbose_t_ "$CXX" -c {} \
-o"$SHBuild_S1_Dir_/{#}.o" $CXXFLAGS $CXXFLAGS_COLOR_ $SHBuild_IncPCH \
$INCLUDES ::: Main.cpp $SRCS
	# shellcheck disable=2086
	Call_ "$CXX" -o"$SHBuild_Output" $CXXFLAGS_COLOR_ $LDFLAGS $LDFLAGS_WKRD_ \
$SHBuild_IncPCH $SHBuild_S1_Dir_/*.o $LIBS
else
	SHBuild_Puts 'Not using parallel.'

	# XXX: Ditto.
	# shellcheck disable=2086
	Call_ "$CXX" Main.cpp -o"$SHBuild_Output" $CXXFLAGS $LDFLAGS \
$LDFLAGS_WKRD_ $SHBuild_IncPCH $INCLUDES $SRCS $LIBS
fi

SHBuild_Popd
SHBuild_Puts 'Done.'

