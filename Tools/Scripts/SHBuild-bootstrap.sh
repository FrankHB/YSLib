#!/usr/bin/env bash
# (C) 2014-2017 FrankHB.
# Common source script: bootstrap configuration.

SHBuild_ToolDir=$(cd `dirname "$0"`; pwd)
. $SHBuild_ToolDir/SHBuild-common.sh
SHBuild_Puts Bootstrap beginned.
SHBuild_CheckUName
: ${SHBuild_BaseDir:="$SHBuild_ToolDir/../SHBuild"}
: ${YSLib_BaseDir:="$SHBuild_ToolDir/../.."}
SHBuild_BaseDir=$(cd "$SHBuild_BaseDir" && pwd)
YSLib_BaseDir=$(cd "$YSLib_BaseDir" && pwd)

SHBuild_EchoVar_N 'SHBuild.BaseDir'
SHBuild_EchoVar_N 'SHBuild.ToolDir'

SHBuild_Puts Configuring ...

SHBuild_BuildDir="$SHBuild_BaseDir/.shbuild"

. $SHBuild_ToolDir/SHBuild-common-toolchain.sh
. $SHBuild_ToolDir/SHBuild-common-options.sh

INCLUDE_PCH="$YSLib_BaseDir/YBase/include/stdinc.h"
INCLUDES=" \
	-I$YSLib_BaseDir/YFramework/include \
	-I$YSLib_BaseDir/YFramework/Android/include \
	-I$YSLib_BaseDir/YFramework/DS/include \
	-I$YSLib_BaseDir/YFramework/Win32/include \
	-I$YSLib_BaseDir/3rdparty/include \
	-I$YSLib_BaseDir/3rdparty/freetype/include \
	-I$YSLib_BaseDir/YBase/include \
	"

# Coordinated with build 758.
LIBS=" \
	$YSLib_BaseDir/YBase/source/ystdex/cassert.cpp \
	$YSLib_BaseDir/YBase/source/ystdex/cstdio.cpp \
	$YSLib_BaseDir/YBase/source/ystdex/base.cpp \
	$YSLib_BaseDir/YBase/source/ystdex/exception.cpp \
	$YSLib_BaseDir/YBase/source/ystdex/any.cpp \
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
	$YSLib_BaseDir/YFramework/source/NPL/Dependency.cpp \
	"
if [[ "$SHBuild_Env_OS" == 'Win32' ]]; then
	LIBS="$LIBS \
		$YSLib_BaseDir/YFramework/Win32/source/YCLib/MinGW32.cpp \
		$YSLib_BaseDir/YFramework/Win32/source/YCLib/Consoles.cpp \
		$YSLib_BaseDir/YFramework/Win32/source/YCLib/NLS.cpp \
		"
fi

export CXXFLAGS
export LDFLAGS

SHBuild_EchoVar_N 'SHBuild.BuildDir'
SHBuild_EchoVar_N 'CXX'
SHBuild_EchoVar_N 'CXXFLAGS'
SHBuild_EchoVar_N 'LDFLAGS'
SHBuild_EchoVar_N 'INCLUDES'
SHBuild_EchoVar_N 'LIBS'

SHBuild_Puts Configuring done.

