#!/usr/bin/env bash
# (C) 2014-2020 FrankHB.
# Common source script: bootstrap configuration.

set -e
: "${SHBuild_ToolDir:=$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)}"
# shellcheck source=./SHBuild-common.sh
. "$SHBuild_ToolDir/SHBuild-common.sh" # for SHBuild_Puts, SHBuild_PrepareBuild,
#	SHBuild_EchoVar_N.
SHBuild_Puts "Bootstrap beginned."
SHBuild_PrepareBuild
: "${SHBuild_BaseDir:="$SHBuild_ToolDir/../SHBuild"}"
: "${YSLib_BaseDir:="$SHBuild_ToolDir/../.."}"
SHBuild_BaseDir=$(cd "$SHBuild_BaseDir"; pwd)
YSLib_BaseDir=$(cd "$YSLib_BaseDir"; pwd)

SHBuild_EchoVar_N 'SHBuild.BaseDir'
SHBuild_EchoVar_N 'SHBuild.ToolDir'
SHBuild_EchoVar_N 'SHBuild.Host.Arch'
SHBuild_EchoVar_N 'SHBuild.Host.OS'

SHBuild_Puts "Configuring ..."

# shellcheck source=./SHBuild-common-toolchain.sh
. "$SHBuild_ToolDir/SHBuild-common-toolchain.sh"
# shellcheck source=./SHBuild-common-options.sh
. "$SHBuild_ToolDir/SHBuild-common-options.sh"

# XXX: %INCLUDE_PCH and %INCLUDES are internal.
# shellcheck disable=2034
INCLUDE_PCH="$YSLib_BaseDir/YBase/include/stdinc.h"
# shellcheck disable=2034
INCLUDES="-I$YSLib_BaseDir/YFramework/include \
-I$YSLib_BaseDir/YFramework/Android/include \
-I$YSLib_BaseDir/YFramework/DS/include \
-I$YSLib_BaseDir/YFramework/Win32/include \
-I$YSLib_BaseDir/3rdparty/include \
-I$YSLib_BaseDir/3rdparty/freetype/include \
-I$YSLib_BaseDir/YBase/include \
"

# Coordinated at build 882.
LIBS="$YSLib_BaseDir/YBase/source/ystdex/base.cpp \
$YSLib_BaseDir/YBase/source/ystdex/exception.cpp \
$YSLib_BaseDir/YBase/source/ystdex/cassert.cpp \
$YSLib_BaseDir/YBase/source/ystdex/cstdio.cpp \
$YSLib_BaseDir/YBase/source/ystdex/any.cpp \
$YSLib_BaseDir/YBase/source/ystdex/memory_resource.cpp \
$YSLib_BaseDir/YBase/source/ystdex/node_base.cpp \
$YSLib_BaseDir/YBase/source/ystdex/tree.cpp \
$YSLib_BaseDir/YBase/source/ystdex/concurrency.cpp \
$YSLib_BaseDir/YFramework/source/CHRLib/chrmap.cpp \
$YSLib_BaseDir/YFramework/source/CHRLib/CharacterProcessing.cpp \
$YSLib_BaseDir/YFramework/source/CHRLib/MappingEx.cpp \
$YSLib_BaseDir/YFramework/source/YCLib/NativeAPI.cpp \
$YSLib_BaseDir/YFramework/source/YCLib/Debug.cpp \
$YSLib_BaseDir/YFramework/source/YCLib/FileIO.cpp \
$YSLib_BaseDir/YFramework/source/YCLib/FileSystem.cpp \
$YSLib_BaseDir/YFramework/source/YCLib/Host.cpp \
$YSLib_BaseDir/YFramework/source/YCLib/YCommon.cpp \
$YSLib_BaseDir/YFramework/source/YCLib/MemoryMapping.cpp \
$YSLib_BaseDir/YFramework/source/YSLib/Core/YCoreUtilities.cpp \
$YSLib_BaseDir/YFramework/source/YSLib/Core/YException.cpp \
$YSLib_BaseDir/YFramework/source/YSLib/Core/YObject.cpp \
$YSLib_BaseDir/YFramework/source/YSLib/Core/ValueNode.cpp \
$YSLib_BaseDir/YFramework/source/YSLib/Service/FileSystem.cpp \
$YSLib_BaseDir/YFramework/source/YSLib/Service/File.cpp \
$YSLib_BaseDir/YFramework/source/YSLib/Service/TextFile.cpp \
$YSLib_BaseDir/YFramework/source/NPL/Lexical.cpp \
$YSLib_BaseDir/YFramework/source/NPL/SContext.cpp \
$YSLib_BaseDir/YFramework/source/NPL/NPLA.cpp \
$YSLib_BaseDir/YFramework/source/NPL/NPLA1.cpp \
$YSLib_BaseDir/YFramework/source/NPL/NPLA1Internals.cpp \
$YSLib_BaseDir/YFramework/source/NPL/NPLA1Forms.cpp \
$YSLib_BaseDir/YFramework/source/NPL/Dependency.cpp"
# XXX: %SHBuild_Host_OS is external.
# shellcheck disable=2154
if [[ "$SHBuild_Host_OS" == 'Win32' ]]; then
	LIBS="$LIBS \
$YSLib_BaseDir/YFramework/Win32/source/YCLib/MinGW32.cpp \
$YSLib_BaseDir/YFramework/Win32/source/YCLib/Consoles.cpp \
$YSLib_BaseDir/YFramework/Win32/source/YCLib/NLS.cpp \
$YSLib_BaseDir/YFramework/Win32/source/YCLib/Registry.cpp"
fi

export CXXFLAGS
export LDFLAGS

: "${SHBuild_BuildDir:="$SHBuild_ToolDir/../../build/$(SHBuild_GetBuildName)"}"

SHBuild_EchoVar_N 'SHBuild.BuildDir'
SHBuild_EchoVar_N 'CXX'
SHBuild_EchoVar_N 'CXXFLAGS'
SHBuild_EchoVar_N 'LDFLAGS'
SHBuild_EchoVar_N 'INCLUDES'
SHBuild_EchoVar_N 'LIBS'

SHBuild_Puts "Configuring done."

