#!/usr/bin/bash
# (C) 2014 FrankHB.
# SHBuild installation script.

set -e
SHBuild_ToolDir=$(cd `dirname "$0"`/Scripts; pwd)
: ${SHBuild_BaseDir:="$SHBuild_ToolDir/../SHBuild"}
: ${YSLib_BaseDir:="$SHBuild_ToolDir/../.."}
. $SHBuild_ToolDir/SHBuild-common-options.sh # For $LIBPFX and $DSOSFX, etc.

echo Configuring ...
# POSIX paths may be not supported by native Windows programs without
#	conversion.
export COMSPEC=bash
: ${SHBuild_BuildOpt:="-xj,6"}
export SHBuild_BuildOpt
: ${SHBuild_LogOpt:="-xlogfl,128"}
export SHBuild_LogOpt
: ${SHBuild_Opt:="$SHBuild_LogOpt $SHBuild_BuildOpt"}
export SHBuild_Opt
: ${SHBuild_SysRoot="$YSLib_BaseDir/sysroot"}
# TODO: Merge with SHBuild-YSLib-common.sh.
SHBuild_CheckUName
if [[ "$SHBuild_Env_OS" == "Win32" ]]; then
	: ${SHBuild_YSLib_Platform:="MinGW32"}
else
	: ${SHBuild_YSLib_Platform:=$SHBuild_Env_OS}
fi
SHBuild_YSLib_LibArch="$SHBuild_YSLib_Platform/lib-$SHBuild_Env_Arch"
SHBuild_SysRoot_Bin="$SHBuild_SysRoot/usr/bin"
SHBuild_SysRoot_Include="$SHBuild_SysRoot/usr/include"
SHBuild_SysRoot_Lib="$SHBuild_SysRoot/usr/lib"
SHBuild_SysRoot_SHBuild="$SHBuild_SysRoot/.shbuild"
: ${AR:="gcc-ar"}
. $SHBuild_ToolDir/SHBuild-common-toolchain.sh
if [ -z ${SHBuild_UseRelease+x} ]; then
	SHBuild_UseRelease=true
fi
export SHBuild_NoStatic
export SHBuild_NoDynamic
echo Done.

echo Bootstraping ...
SHBuild_Stage1_SHBuild="$SHBuild_BaseDir/SHBuild"

if command -v "$SHBuild_Stage1_SHBuild"; then
	echo Found stage 1 SHBuild "$SHBuild_Stage1_SHBuild", building skipped.	
else
	echo Stage 1 SHBuild not found. Building ...
	$SHBuild_ToolDir/SHBuild-build.sh
	echo Finished building stage 1 SHBuild.
fi

echo Building YSLib libraries ...
if [ x"$SHBuild_UseDebug" != x ]; then
	echo Building debug libraries ...
	$SHBuild_ToolDir/SHBuild-YSLib-debug.sh "$SHBuild_Opt"
	echo Finished building debug libraries.
else
	echo Skipped building debug libraries.
fi
if [ x"$SHBuild_UseRelease" != x ]; then
	echo Building release libraries ...
	$SHBuild_ToolDir/SHBuild-YSLib.sh "$SHBuild_Opt"
	echo Finished building release libraries.
else
	echo Skipped building release libraries.
fi

echo Finished building YSLib libraries.

echo Installing headers and libraries ...
mkdir -p "$SHBuild_SysRoot_Bin"
mkdir -p "$SHBuild_SysRoot_Include"
mkdir -p "$SHBuild_SysRoot_Lib"

SHBuild_InstallDir "$YSLib_BaseDir/3rdparty/include/" \
	"$SHBuild_SysRoot_Include"
SHBuild_InstallDir "$YSLib_BaseDir/YBase/include/" \
	"$SHBuild_SysRoot_Include"
SHBuild_InstallDir "$YSLib_BaseDir/YFramework/include/" \
	"$SHBuild_SysRoot_Include"
SHBuild_InstallDir "$YSLib_BaseDir/YFramework/DS/include/" \
	"$SHBuild_SysRoot_Include"
SHBuild_InstallDir "$YSLib_BaseDir/YFramework/MinGW32/include/" \
	"$SHBuild_SysRoot_Include"

SHBuild_SysRoot_Stage1_SHBuild="$SHBuild_BaseDir/.shbuild"
SHBuild_SysRoot_Stage1_SHBuild_debug="$SHBuild_BaseDir/.shbuild-debug"
SHBuild_SysRoot_Stage1_SHBuild_DLL="$SHBuild_BaseDir/.shbuild-dll"
SHBuild_SysRoot_Stage1_SHBuild_DLL_debug="$SHBuild_BaseDir/.shbuild-dll-debug"

SHBuild_Install_lib_Static()
{
	SHBuild_Install_HardLink "$1/$LIBPFX$3.a" "$2/lib$3.a"
}

SHBuild_Install_lib_Dynamic()
{
	local target="$LIBPFX$3$DSOSFX"
	local dest="$2/$target"

	SHBuild_Install_HardLink "$1/$target" "$dest"
	SHBuild_Install_Link "$dest" "$4/$target.a"
}

if [ x"$SHBuild_UseDebug" != x ]; then
	if [ x"$SHBuild_NoStatic" == x ]; then
		SHBuild_Install_lib_Static "$SHBuild_SysRoot_Stage1_SHBuild_debug" \
			"$SHBuild_SysRoot_Lib" YBased
		SHBuild_Install_lib_Static "$SHBuild_SysRoot_Stage1_SHBuild_debug" \
			"$SHBuild_SysRoot_Lib" YFrameworkd
	else
		echo Skipped installing debug static libraries.		
	fi
	if [ x"$SHBuild_NoDynamic" == x ]; then
		SHBuild_Install_lib_Dynamic \
			"$SHBuild_SysRoot_Stage1_SHBuild_DLL_debug" "$SHBuild_SysRoot_Bin" \
			YBased "$SHBuild_SysRoot_Lib"
		SHBuild_Install_lib_Dynamic \
			"$SHBuild_SysRoot_Stage1_SHBuild_DLL_debug" "$SHBuild_SysRoot_Bin" \
			YFrameworkd "$SHBuild_SysRoot_Lib"
	else
		echo Skipped installing debug dynamic libraries.
	fi
else
	echo Skipped installing debug libraries.
fi
if [ x"$SHBuild_UseRelease" != x ]; then

	if [ x"$SHBuild_NoStatic" == x ]; then
		SHBuild_Install_lib_Static "$SHBuild_SysRoot_Stage1_SHBuild" \
			"$SHBuild_SysRoot_Lib" YBase
		SHBuild_Install_lib_Static "$SHBuild_SysRoot_Stage1_SHBuild" \
			"$SHBuild_SysRoot_Lib" YFramework
	else
		echo Skipped installing release static libraries.
	fi
	if [ x"$SHBuild_NoDynamic" == x ]; then
		SHBuild_Install_lib_Dynamic \
			"$SHBuild_SysRoot_Stage1_SHBuild_DLL" "$SHBuild_SysRoot_Bin" \
			YBase "$SHBuild_SysRoot_Lib"
		SHBuild_Install_lib_Dynamic \
			"$SHBuild_SysRoot_Stage1_SHBuild_DLL" "$SHBuild_SysRoot_Bin" \
			YFramework "$SHBuild_SysRoot_Lib"
	else
		echo Skipped installing release dynamic libraries.
	fi
else
	echo Skipped installing release libraries.
fi

# 3rd party libraries.
if [ x"$SHBuild_No3rd" == x ]; then
	SHBuild_Install_HardLink \
		"$YSLib_BaseDir/YFramework/$SHBuild_YSLib_LibArch/libfreetype.a" \
		"$SHBuild_SysRoot_Lib/libfreetype.a"
	SHBuild_Install_HardLink \
		"$YSLib_BaseDir/YFramework/$SHBuild_YSLib_LibArch/libFreeImage.a" \
		"$SHBuild_SysRoot_Lib/libFreeImage.a"
else
	echo Skipped installing 3rd party libraries.
fi

echo Finished installing headers and libraries.

export INCLUDES="$INCLUDES -I$SHBuild_SysRoot_Include"
export LIBS="$LIBS -L`SHBuild_2w $SHBuild_SysRoot_Lib` -lYFramework -lYBase"
. $SHBuild_ToolDir/SHBuild-common-options.sh
export LDFLAGS

echo Building Stage 2 SHBuild ...
$SHBuild_BaseDir/SHBuild $SHBuild_BaseDir \
	"-xd,$SHBuild_SysRoot_SHBuild" -xmode,2 $SHBuild_Opt $CXXFLAGS \
	-fno-lto -fwhole-program $INCLUDES
echo Finished building Stage 2 SHBuild.

echo Installing Stage 2 SHBuild ...
SHBuild_Install_HardLink_Exe "$SHBuild_SysRoot_SHBuild/SHBuild.exe" \
	"$SHBuild_SysRoot_Bin/SHBuild.exe"
SHBuild_Install_Exe "$SHBuild_ToolDir/SHBuild-common.sh" \
	"$SHBuild_SysRoot_Bin/SHBuild-common.sh"
SHBuild_Install_Exe "$SHBuild_ToolDir/SHBuild-common-options.sh" \
	"$SHBuild_SysRoot_Bin/SHBuild-common-options.sh"
SHBuild_Install_Exe "$SHBuild_ToolDir/SHBuild-common-toolchain.sh" \
	"$SHBuild_SysRoot_Bin/SHBuild-common-toolchain.sh"
SHBuild_Install_Exe "$SHBuild_ToolDir/SHBuild-BuildApp.sh" \
	"$SHBuild_SysRoot_Bin/SHBuild-BuildApp.sh"
echo Finished installing Stage 2 SHBuild.

echo Done.

