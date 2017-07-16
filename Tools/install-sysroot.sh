#!/usr/bin/env bash
# (C) 2014-2017 FrankHB.
# SHBuild installation script.

set -e
SHBuild_ToolDir=$(cd `dirname "$0"`/Scripts; pwd)
: ${SHBuild_BaseDir:="$SHBuild_ToolDir/../SHBuild"}
: ${YSLib_BaseDir:="$SHBuild_ToolDir/../.."}
YSLib_BaseDir=$(cd $YSLib_BaseDir; pwd)
. $SHBuild_ToolDir/SHBuild-common-options.sh # For $LIBPFX and $DSOSFX, etc.

SHBuild_Puts Configuring ...
: ${SHBuild_BuildOpt:="-xj,6"}
export SHBuild_BuildOpt
: ${SHBuild_LogOpt:="-xlogfl,128"}
export SHBuild_LogOpt
: ${SHBuild_Opt:="$SHBuild_LogOpt $SHBuild_BuildOpt"}
export SHBuild_Opt
: ${SHBuild_SysRoot="$YSLib_BaseDir/sysroot"}

SHBuild_CheckHostPlatform

export YSLib_BaseDir
# This directory will be created if not existed by following stage 1 process
#	or by %SHBuild-YSLib-build.txt.
export YSLib_BuildDir="$YSLib_BaseDir/build/$SHBuild_Host_Platform"
YSLib_DataDir="$YSLib_BaseDir/Data"

SHBuild_Puts Build directory is \"$YSLib_BuildDir\".

# SR = SHBuild SysRoot.
SR_Prefix="$SHBuild_SysRoot/usr"
SR_Bin="$SR_Prefix/bin"
SR_Include="$SR_Prefix/include"
SR_Lib="$SR_Prefix/lib"
SR_Share="$SR_Prefix/share"
SR_SHBuild="$SHBuild_SysRoot/.shbuild"

# BD = YSLib Base Directory.
BD_3rdparty="$YSLib_BaseDir/3rdparty"
BD_3rdparty_freetype_inc="$BD_3rdparty/freetype/include"
BD_YFramework="$YSLib_BaseDir/YFramework"

# TODO: Merge with SHBuild-YSLib-common.sh.
SHBuild_CheckUName
if [[ "$SHBuild_Env_OS" == "Win32" ]]; then
	: ${SHBuild_YSLib_Platform:="MinGW32"}
	: ${INCLUDES_freetype="-I\"`SHBuild_2w $BD_3rdparty_freetype_inc`\""}
	SR_DSO_Dest="$SR_Bin"
	SR_DSO_Imp=".a"
else
	: ${SHBuild_YSLib_Platform:=$SHBuild_Env_OS}
	# TODO: Merge with SHBuild-BuildApp.sh?
	INCLUDES_freetype="`pkg-config --cflags-only-I freetype2 2> /dev/null \
		|| true`"
	: ${INCLUDES_freetype:="-I/usr/include"}
	SR_DSO_Dest="$SR_Lib"
	SR_DSO_Imp=""
fi
export INCLUDES_freetype

SHBuild_YSLib_LibArch="$SHBuild_YSLib_Platform/lib-$SHBuild_Env_Arch"
BD_YFramework_Lib="$BD_YFramework/$SHBuild_YSLib_LibArch"

: ${AR:="gcc-ar"}
. $SHBuild_ToolDir/SHBuild-common-toolchain.sh
if [[ -z ${SHBuild_UseRelease+x} ]]; then
	SHBuild_UseRelease=true
fi
export SHBuild_NoStatic
export SHBuild_NoDynamic
SHBuild_Puts Done.

SHBuild_Puts Bootstraping ...

# S1 = Stage 1.
SHBuild_S1_BuildDir="$YSLib_BuildDir/.stage1"
mkdir -p $SHBuild_S1_BuildDir
SHBuild_S1_SHBuild="$SHBuild_S1_BuildDir/SHBuild"
export SHBuild_PCH_stdinc_h="$SHBuild_S1_BuildDir/stdinc.h"

if [[ "$SHBuild_Rebuild_S1" == '' ]]; then
	if command -v "$SHBuild_S1_SHBuild" > /dev/null ; then
		SHBuild_Puts Found stage 1 SHBuild \"$SHBuild_S1_SHBuild\", \
			building skipped.
	else
		SHBuild_Puts Stage 1 SHBuild not found.
		SHBuild_Rebuild_S1_=true
	fi
else
	SHBuild_Puts Stage 1 SHBuild would be rebuilt.
	SHBuild_Rebuild_S1_=true
fi
if [[ "$SHBuild_Rebuild_S1_" == 'true' ]]; then
	SHBuild_Puts Building Stage 1 SHBuild ...
	SHBuild_Output="$SHBuild_S1_SHBuild" $SHBuild_ToolDir/SHBuild-build.sh
	SHBuild_Puts Finished building stage 1 SHBuild.
fi

SHB_BuildYSLib_()
{
	local L_AR="$AR"
	local L_SHBuild="$SHBuild"
	: ${L_AR:='gcc-ar'}
	: ${L_SHBuild:="$SHBuild_S1_BuildDir/SHBuild"}
	SHBuild_Pushd
	cd "$YSLib_BuildDir"
	debug="$1" AR="$L_AR" SHBuild="$L_SHBuild" \
		SHBuild_Common="$SHBuild_ToolDir/SHBuild-YSLib-common.txt" \
		"$L_SHBuild" -xcmd,RunNPLFile \
		"$SHBuild_ToolDir/SHBuild-YSLib-build.txt" -- $SHBuild_Opt
	SHBuild_Popd
	SHBuild_Puts Done.
}

SHBuild_Puts Building YSLib libraries ...
if [[ "$SHBuild_UseDebug" != '' ]]; then
	SHBuild_Puts Building debug libraries ...
	SHB_BuildYSLib_ true
	SHBuild_Puts Finished building debug libraries.
else
	SHBuild_Puts Skipped building debug libraries.
fi
if [[ "$SHBuild_UseRelease" != '' ]]; then
	SHBuild_Puts Building release libraries ...
	SHB_BuildYSLib_ ''
	SHBuild_Puts Finished building release libraries.
else
	SHBuild_Puts Skipped building release libraries.
fi
SHBuild_Puts Finished building YSLib libraries.

# Installation functions.
SHB_InstLibS()
{
	# TODO: Error handling.
	"$SHBuild_S1_SHBuild" -xcmd,InstallHardLink "$2/lib$3.a" "$1/$LIBPFX$3.a"
}

SHB_InstLibD()
{
	declare -r target="$LIBPFX$2$DSOSFX"
	declare -r dest="$SR_DSO_Dest/$target"
	# TODO: Error handling.
	"$SHBuild_S1_SHBuild" -xcmd,InstallHardLink "$dest" "$1/$target"
	if [[ "$SR_DSO_Imp" != "" ]]; then
		SHBuild_Install_Link "$dest" "$SR_Lib/$target$SR_DSO_Imp"
	fi
}

SHB_InstInc()
{
	# TODO: Error handling.
	"$SHBuild_S1_SHBuild" -xcmd,InstallDirectory "$SR_Include" "$1"
}

SHB_InstTool()
{
	# TODO: Error handling.
	"$SHBuild_S1_SHBuild" -xcmd,InstallExecutable "$SR_Bin/$1" \
		"$SHBuild_ToolDir/$1"
}

SHB_EnsureDirectory()
{
	# TODO: Error handling.
	"$SHBuild_S1_SHBuild" -xcmd,EnsureDirectory "$1"
}

SHBuild_Puts Installing headers and libraries ...
SHB_EnsureDirectory "$SR_Bin"
SHB_EnsureDirectory "$SR_Include"
SHB_EnsureDirectory "$SR_Lib"
SHB_EnsureDirectory "$SR_Share"

if [[ "$SHBuild_No3rd" == '' ]]; then
	if [[ "$INCLUDES_freetype" != '' ]]; then
		SHB_InstInc "$BD_3rdparty_freetype_inc/"
	fi
	SHB_InstInc "$BD_3rdparty/include/"
fi
SHB_InstInc "$YSLib_BaseDir/YBase/include/"
SHB_InstInc "$BD_YFramework/include/"
SHB_InstInc "$BD_YFramework/DS/include/"
SHB_InstInc "$BD_YFramework/Win32/include/"

SR_S1_SHBuild="$YSLib_BuildDir/.shbuild"
SR_S1_SHBuild_debug="$YSLib_BuildDir/.shbuild-debug"
SR_S1_SHBuild_DLL="$YSLib_BuildDir/.shbuild-dll"
SR_S1_SHBuild_DLL_debug="$YSLib_BuildDir/.shbuild-dll-debug"

if [[ "$SHBuild_UseDebug" != '' ]]; then
	if [[ "$SHBuild_NoStatic" == '' ]]; then
		SHB_InstLibS "$SR_S1_SHBuild_debug" "$SR_Lib" YBased
		SHB_InstLibS "$SR_S1_SHBuild_debug" "$SR_Lib" YFrameworkd
	else
		SHBuild_Puts Skipped installing debug static libraries.
	fi
	if [[ "$SHBuild_NoDynamic" == '' ]]; then
		SHB_InstLibD "$SR_S1_SHBuild_DLL_debug" YBased
		SHB_InstLibD "$SR_S1_SHBuild_DLL_debug" YFrameworkd
	else
		SHBuild_Puts Skipped installing debug dynamic libraries.
	fi
else
	SHBuild_Puts Skipped installing debug libraries.
fi
if [[ "$SHBuild_UseRelease" != '' ]]; then
	if [[ "$SHBuild_NoStatic" == '' ]]; then
		SHB_InstLibS "$SR_S1_SHBuild" "$SR_Lib" YBase
		SHB_InstLibS "$SR_S1_SHBuild" "$SR_Lib" YFramework
	else
		SHBuild_Puts Skipped installing release static libraries.
	fi
	if [[ "$SHBuild_NoDynamic" == '' ]]; then
		SHB_InstLibD "$SR_S1_SHBuild_DLL" YBase
		SHB_InstLibD "$SR_S1_SHBuild_DLL" YFramework
	else
		SHBuild_Puts Skipped installing release dynamic libraries.
	fi
else
	SHBuild_Puts Skipped installing release libraries.
fi

# 3rd party libraries.
if [[ "$SHBuild_No3rd" == '' ]]; then
	if [[ "$INCLUDES_freetype" != '' ]]; then
		# TODO: Check file existence even if optional in some cases.
		SHBuild_Install_HardLink "$BD_YFramework_Lib/libfreetype.a" \
			"$SR_Lib/libfreetype.a" 2> /dev/null || true
	fi
	SHBuild_Install_HardLink "$BD_YFramework_Lib/libFreeImage.a" \
		"$SR_Lib/libFreeImage.a"
else
	SHBuild_Puts Skipped installing 3rd party libraries.
fi

SHBuild_Puts Finished installing headers and libraries.

export INCLUDES="$INCLUDES -I$SR_Include"
export LIBS="$LIBS $LIBS_RPATH -L\"`SHBuild_2w $SR_Lib`\" -lYFramework \
	-lYBase"
. $SHBuild_ToolDir/SHBuild-common-options.sh
export LDFLAGS

SHBuild_Puts Building Stage 2 SHBuild ...
"$SHBuild_S1_SHBuild" $SHBuild_BaseDir "-xd,$SR_SHBuild" -xmode,2 \
	$SHBuild_Opt $CXXFLAGS $INCLUDES
SHBuild_Puts Finished building Stage 2 SHBuild.

SHBuild_Puts Installing Stage 2 SHBuild ...
SHBuild_Install_HardLink_Exe "$SR_SHBuild/SHBuild.exe" "$SR_Bin/SHBuild$EXESFX"
SHB_InstTool "SHBuild-common.sh"
SHB_InstTool "SHBuild-common-options.sh"
SHB_InstTool "SHBuild-common-toolchain.sh"
SHB_InstTool "SHBuild-BuildApp.sh"
SHBuild_Puts Finished installing stage 2 SHBuild.

if [[ "$SHBuild_NoDev" == '' ]]; then
	SHBuild_Puts Building other development tools using stage2 SHBuild ...
	SHBuild_S2_SHBuild="$SR_Bin/SHBuild"
	$SHBuild_S2_SHBuild $SHBuild_BaseDir/../RevisionPatcher "-xd,$SR_SHBuild" \
		-xmode,2 $SHBuild_Opt $CXXFLAGS $INCLUDES
	$SHBuild_S2_SHBuild $SHBuild_BaseDir/../SXML2XML "-xd,$SR_SHBuild" \
		-xmode,2 $SHBuild_Opt $CXXFLAGS $INCLUDES
	$SHBuild_S2_SHBuild $SHBuild_BaseDir/../ProjectGenerator "-xd,$SR_SHBuild" \
		-xmode,2 $SHBuild_Opt $CXXFLAGS $INCLUDES
	SHBuild_Puts Installing other development tools ...
	SHBuild_Install_HardLink_Exe "$SR_SHBuild/RevisionPatcher.exe" \
		"$SR_Bin/RevisionPatcher$EXESFX"
	SHBuild_Install_HardLink_Exe "$SR_SHBuild/SXML2XML.exe" \
		"$SR_Bin/SXML2XML$EXESFX"
	SHBuild_Install_HardLink_Exe "$SR_SHBuild/ProjectGenerator.exe" \
		"$SR_Bin/ProjectGenerator$EXESFX"
	# XXX: Version of Windows? Extract as a function?
	if [[ "$SHBuild_Env_OS" == "Win32" ]]; then
		SHBuild_Install "$YSLib_DataDir/FixUAC.manifest" \
			"$SR_Share/FixUAC.manifest"
		SHBuild_Install_HardLink "$SR_Share/FixUAC.manifest" \
			"$SR_Bin/RevisionPatcher$EXESFX.manifest"
	fi
	SHBuild_Puts Finished installing other development tools.
else
	SHBuild_Puts Skipped building and installing other development tools.
fi

SHBuild_Puts Done.

