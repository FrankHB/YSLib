#!/usr/bin/bash
# (C) 2014 FrankHB.
# SHBuild installation script.

set -e
SHBuild_ToolDir=$(cd `dirname "$0"`/Scripts; pwd)
: ${SHBuild_BaseDir:="$SHBuild_ToolDir/../SHBuild"}
: ${YSLib_BaseDir:="$SHBuild_ToolDir/../.."}
. $SHBuild_ToolDir/SHBuild-common-options.sh # For $LIBPFX and $DSOSFX, etc.

echo Configuring ...
: ${SHBuild_BuildOpt:="-xj,6"}
export SHBuild_BuildOpt
: ${SHBuild_LogOpt:="-xlogfl,128"}
export SHBuild_LogOpt
: ${SHBuild_Opt:="$SHBuild_LogOpt $SHBuild_BuildOpt"}
export SHBuild_Opt
: ${SHBuild_SysRoot="$YSLib_BaseDir/sysroot"}

# SR = SHBuild SysRoot.
SR_Bin="$SHBuild_SysRoot/usr/bin"
SR_Include="$SHBuild_SysRoot/usr/include"
SR_Lib="$SHBuild_SysRoot/usr/lib"
SR_SHBuild="$SHBuild_SysRoot/.shbuild"

# BD = YSLib Base Directory.
BD_3rdparty="$YSLib_BaseDir/3rdparty"
BD_3rdparty_freetype_inc="$BD_3rdparty/freetype/include"
BD_YFramework="$YSLib_BaseDir/YFramework"

# TODO: Merge with SHBuild-YSLib-common.sh.
SHBuild_CheckUName
if [[ "$SHBuild_Env_OS" == "Win32" ]]; then
	: ${SHBuild_YSLib_Platform:="MinGW32"}
	: ${INCLUDES_freetype="-I$BD_3rdparty_freetype_inc"}
	SR_DSO_Dest="$SR_Bin"
	SR_DSO_Imp=".a"
else
	: ${SHBuild_YSLib_Platform:=$SHBuild_Env_OS}
	# TODO: Merge with SHBuild-BuildApp.sh?
	INCLUDES_freetype="`pkg-config --cflags-only-I freetype2 2> /dev/null`"
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
echo Done.

echo Bootstraping ...

# S1 = Stage 1.
SHBuild_S1_SHBuild="$SHBuild_BaseDir/SHBuild"

if command -v "$SHBuild_S1_SHBuild"; then
	echo Found stage 1 SHBuild "$SHBuild_S1_SHBuild", building skipped.	
else
	echo Stage 1 SHBuild not found. Building ...
	$SHBuild_ToolDir/SHBuild-build.sh
	echo Finished building stage 1 SHBuild.
fi

echo Building YSLib libraries ...
if [[ "$SHBuild_UseDebug" != '' ]]; then
	echo Building debug libraries ...
	$SHBuild_ToolDir/SHBuild-YSLib-debug.sh "$SHBuild_Opt"
	echo Finished building debug libraries.
else
	echo Skipped building debug libraries.
fi
if [[ "$SHBuild_UseRelease" != '' ]]; then
	echo Building release libraries ...
	$SHBuild_ToolDir/SHBuild-YSLib.sh "$SHBuild_Opt"
	echo Finished building release libraries.
else
	echo Skipped building release libraries.
fi

echo Finished building YSLib libraries.

# Installation functions.
SHB_InstLibS()
{
	SHBuild_Install_HardLink "$1/$LIBPFX$3.a" "$2/lib$3.a"
}

SHB_InstLibD()
{
	local target="$LIBPFX$2$DSOSFX"
	local dest="$SR_DSO_Dest/$target"

	SHBuild_Install_HardLink "$1/$target" "$dest"
	if [[ "$SR_DSO_Imp" != "" ]]; then
		SHBuild_Install_Link "$dest" "$SR_Lib/$target$SR_DSO_Imp"
	fi
}

SHB_InstInc()
{
	SHBuild_InstallDir "$1" "$SR_Include"
}

SHB_InstTool()
{
	SHBuild_Install_Exe "$SHBuild_ToolDir/$1" "$SR_Bin/$1"
}

echo Installing headers and libraries ...
mkdir -p "$SR_Bin"
mkdir -p "$SR_Include"
mkdir -p "$SR_Lib"

if [[ "$SHBuild_No3rd" == '' ]]; then
	if [[ "$INCLUDES_freetype" != '' ]]; then
		SHB_InstInc "$BD_3rdparty_freetype_inc/"
	fi
	SHB_InstInc "$BD_3rdparty/include/"
fi
SHB_InstInc "$YSLib_BaseDir/YBase/include/"
SHB_InstInc "$BD_YFramework/include/"
SHB_InstInc "$BD_YFramework/DS/include/"
SHB_InstInc "$BD_YFramework/MinGW32/include/"

SR_S1_SHBuild="$SHBuild_BaseDir/.shbuild"
SR_S1_SHBuild_debug="$SHBuild_BaseDir/.shbuild-debug"
SR_S1_SHBuild_DLL="$SHBuild_BaseDir/.shbuild-dll"
SR_S1_SHBuild_DLL_debug="$SHBuild_BaseDir/.shbuild-dll-debug"

if [[ "$SHBuild_UseDebug" != '' ]]; then
	if [[ "$SHBuild_NoStatic" == '' ]]; then
		SHB_InstLibS "$SR_S1_SHBuild_debug" "$SR_Lib" YBased
		SHB_InstLibS "$SR_S1_SHBuild_debug" "$SR_Lib" YFrameworkd
	else
		echo Skipped installing debug static libraries.		
	fi
	if [[ "$SHBuild_NoDynamic" == '' ]]; then
		SHB_InstLibD "$SR_S1_SHBuild_DLL_debug" YBased
		SHB_InstLibD "$SR_S1_SHBuild_DLL_debug" YFrameworkd
	else
		echo Skipped installing debug dynamic libraries.
	fi
else
	echo Skipped installing debug libraries.
fi
if [[ "$SHBuild_UseRelease" != '' ]]; then
	if [[ "$SHBuild_NoStatic" == '' ]]; then
		SHB_InstLibS "$SR_S1_SHBuild" "$SR_Lib" YBase
		SHB_InstLibS "$SR_S1_SHBuild" "$SR_Lib" YFramework
	else
		echo Skipped installing release static libraries.
	fi
	if [[ "$SHBuild_NoDynamic" == '' ]]; then
		SHB_InstLibD "$SR_S1_SHBuild_DLL" YBase
		SHB_InstLibD "$SR_S1_SHBuild_DLL" YFramework
	else
		echo Skipped installing release dynamic libraries.
	fi
else
	echo Skipped installing release libraries.
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
	echo Skipped installing 3rd party libraries.
fi

echo Finished installing headers and libraries.

export INCLUDES="$INCLUDES -I$SR_Include"
export LIBS="$LIBS $LIBS_RPATH -L\"`SHBuild_2w $SR_Lib`\" -lYFramework \
	-lYBase"
. $SHBuild_ToolDir/SHBuild-common-options.sh
export LDFLAGS

echo Building Stage 2 SHBuild ...
$SHBuild_S1_SHBuild $SHBuild_BaseDir "-xd,$SR_SHBuild" -xmode,2 \
	$SHBuild_Opt $CXXFLAGS $INCLUDES
echo Finished building Stage 2 SHBuild.

echo Installing Stage 2 SHBuild ...
SHBuild_Install_HardLink_Exe "$SR_SHBuild/SHBuild.exe" "$SR_Bin/SHBuild$EXESFX"
SHB_InstTool "SHBuild-common.sh"
SHB_InstTool "SHBuild-common-options.sh"
SHB_InstTool "SHBuild-common-toolchain.sh"
SHB_InstTool "SHBuild-BuildApp.sh"
echo Finished installing stage 2 SHBuild.

if [[ "$SHBuild_NoDev" == '' ]]; then
	echo Building other development tools using stage2 SHBuild ...
	SHBuild_S2_SHBuild="$SR_Bin/SHBuild"
	$SHBuild_S2_SHBuild $SHBuild_BaseDir/../RevisionPatcher "-xd,$SR_SHBuild" \
		-xmode,2 $SHBuild_Opt $CXXFLAGS $INCLUDES
	echo Installing other development tools ...
	SHBuild_Install_HardLink_Exe "$SR_SHBuild/RevisionPatcher.exe" \
		"$SR_Bin/RevisionPatcher$EXESFX"
	# XXX: Version of Windows? Extract as a function?
	if [[ "$SHBuild_Env_OS" == "Win32" ]]; then
		SHBuild_Install_HardLink_Exe "$SHBuild_BaseDir/../FixUAC.manifest" \
			"$SR_Bin/RevisionPatcher$EXESFX.manifest"
	fi
	echo Finished installing other development tools.
else
	echo Skipped building and installing other development tools.
fi

echo Done.

