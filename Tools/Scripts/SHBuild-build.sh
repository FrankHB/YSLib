#!/usr/bin/env bash
# (C) 2014-2018, 2020-2021 FrankHB.
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
# NOTE: '-Og -g' is not supported. See $2020-09 @ %Documentation::Workflow.
# shellcheck disable=2034
CXXFLAGS_OPT_DBG='-O0 -g'
# XXX: This avoids clash between '-Og' with error 'Optimization level must be
#	between 0 and 3' caused by options passed to the linker from Clang++ driver.
#	See also https://bugs.llvm.org/show_bug.cgi?id=38305. Anyway LTO is not make
#	it easy to debug as the intentional '-Og' or '-O0' here, so simply disable
#	it.
# shellcheck disable=2034
LDFLAGS_IMPL_OPT=' '
# shellcheck source=./SHBuild-bootstrap.sh
. "$SHBuild_ToolDir/SHBuild-bootstrap.sh" # for SHBuild_Pushd, SHBuild_BaseDir,
#	SHBuild_Puts, CXXFLAGS, LDFLAGS, INCLUDES, LIBS, SHBuild_Popd.

: "${SHBuild_Output:=SHBuild}"

SHBuild_Pushd .
cd "$SHBuild_BaseDir"

SHBuild_Puts Building ...

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
# XXX: %SHBuild_Verbose_ is external.
# shellcheck disable=2154
if [[ "$SHBuild_Verbose_" != '' ]]; then
	# XXX: Value of several variables may contain whitespaces.
	# shellcheck disable=2086
	SHBuild_Puts "$CXX" Main.cpp -o"$SHBuild_Output" $CXXFLAGS $LDFLAGS \
		$SHBuild_IncPCH $INCLUDES $LIBS
fi
# XXX: Value of several variables may contain whitespaces.
# shellcheck disable=2086
"$CXX" Main.cpp -o"$SHBuild_Output" $CXXFLAGS $LDFLAGS $SHBuild_IncPCH \
	$INCLUDES $LIBS

SHBuild_Popd
SHBuild_Puts Done.

