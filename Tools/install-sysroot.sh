#!/usr/bin/bash
# (C) 2014 FrankHB.
# SHBuild installation script.

set -e
SHBuild_ToolDir=$(cd `dirname "$0"`/Scripts; pwd)
: ${SHBuild_BaseDir:="${SHBuild_ToolDir}/../SHBuild"}
: ${YSLib_BaseDir:="${SHBuild_ToolDir}/../.."}
. ${SHBuild_ToolDir}/SHBuild-common.sh

echo Configuring ...
# POSIX paths may be not supported by native Windows programs without
#	conversion.
export COMSPEC=bash
: ${SHBuild_BuildOpt:="-xj,6"}
export SHBuild_BuildOpt
: ${SHBuild_LogOpt:="-xlogfl,128"}
export SHBuild_LogOpt
: ${SHBuild_Opt:="${SHBuild_LogOpt} ${SHBuild_BuildOpt}"}
export SHBuild_Opt
: ${SHBuild_SysRoot="${YSLib_BaseDir}/sysroot"}
: ${YSLib_Arch="MinGW32"}
SHBuild_SysRoot_Bin="${SHBuild_SysRoot}/usr/bin"
SHBuild_SysRoot_Include="${SHBuild_SysRoot}/usr/include"
SHBuild_SysRoot_Lib="${SHBuild_SysRoot}/usr/lib"
SHBuild_SysRoot_SHBuild="${SHBuild_SysRoot}/.shbuild"
export AR="gcc-ar"
. ${SHBuild_ToolDir}/SHBuild-common-toolchain.sh
echo Done.

echo Bootstraping ...
SHBuild_Stage1_SHBuild="${SHBuild_BaseDir}/SHBuild"

if command -v "$SHBuild_Stage1_SHBuild"; then
	echo Found stage 1 SHBuild "${SHBuild_ToolDir}", building skipped.	
else
	echo Stage 1 SHBuild not found. Building ...
	${SHBuild_ToolDir}/SHBuild-build.sh
	echo Finished building stage 1 SHBuild.
fi

echo Building YSLib libraries ...
if [ x"${SHBuild_UseDebug}" != x ]; then
	echo Building debug libraries ...
	${SHBuild_ToolDir}/SHBuild-YSLib-debug.sh "${SHBuild_Opt}"
	echo Finished building debug libraries.
else
	echo Skipped building debug libraries.
fi
${SHBuild_ToolDir}/SHBuild-YSLib.sh "${SHBuild_Opt}"
echo Finished building YSLib libraries.

echo Installing headers and libraries ...
mkdir -p "${SHBuild_SysRoot_Bin}"
mkdir -p "${SHBuild_SysRoot_Include}"
mkdir -p "${SHBuild_SysRoot_Lib}"

SHBuild_InstallDir "${YSLib_BaseDir}/3rdparty/include/" \
	"${SHBuild_SysRoot_Include}"
SHBuild_InstallDir "${YSLib_BaseDir}/YBase/include/" \
	"${SHBuild_SysRoot_Include}"
SHBuild_InstallDir "${YSLib_BaseDir}/YFramework/include/" \
	"${SHBuild_SysRoot_Include}"
SHBuild_InstallDir "${YSLib_BaseDir}/YFramework/DS/include/" \
	"${SHBuild_SysRoot_Include}"
SHBuild_InstallDir "${YSLib_BaseDir}/YFramework/MinGW32/include/" \
	"${SHBuild_SysRoot_Include}"

SHBuild_SysRoot_Stage1_SHBuild="${SHBuild_BaseDir}/.shbuild"
SHBuild_SysRoot_Stage1_SHBuild_debug="${SHBuild_BaseDir}/.shbuild-debug"
SHBuild_SysRoot_Stage1_SHBuild_DLL="${SHBuild_BaseDir}/.shbuild-dll"
SHBuild_SysRoot_Stage1_SHBuild_DLL_debug="${SHBuild_BaseDir}/.shbuild-dll-debug"

if [ x"${SHBuild_UseDebug}" != x ]; then
	SHBuild_Install_HardLink "${SHBuild_SysRoot_Stage1_SHBuild_debug}/YBase.a" \
		"${SHBuild_SysRoot_Lib}/libYBased.a"
	SHBuild_Install_HardLink \
		"${SHBuild_SysRoot_Stage1_SHBuild_debug}/YFramework.a" \
		"${SHBuild_SysRoot_Lib}/libYFrameworkd.a"
	SHBuild_Install_HardLink \
		"${SHBuild_SysRoot_Stage1_SHBuild_DLL_debug}/YBased.dll" \
		"${SHBuild_SysRoot_Bin}/YBased.dll"
	SHBuild_Install_HardLink \
		"${SHBuild_SysRoot_Stage1_SHBuild_DLL_debug}/YFrameworkd.dll" \
		"${SHBuild_SysRoot_Bin}/YFrameworkd.dll"
	SHBuild_Install_Link "${SHBuild_SysRoot_Bin}/YBased.dll" \
		"${SHBuild_SysRoot_Lib}/YBased.dll.a"
	SHBuild_Install_Link "${SHBuild_SysRoot_Bin}/YFrameworkd.dll" \
		"${SHBuild_SysRoot_Lib}/YFrameworkd.dll.a"
else
	echo Skipped installing debug libraries.
fi
SHBuild_Install_HardLink "${SHBuild_SysRoot_Stage1_SHBuild}/YBase.a" \
	"${SHBuild_SysRoot_Lib}/libYBase.a"
SHBuild_Install_HardLink "${SHBuild_SysRoot_Stage1_SHBuild}/YFramework.a" \
	"${SHBuild_SysRoot_Lib}/libYFramework.a"
SHBuild_Install_HardLink "${SHBuild_SysRoot_Stage1_SHBuild_DLL}/YBase.dll" \
	"${SHBuild_SysRoot_Bin}/YBase.dll"
SHBuild_Install_HardLink \
	"${SHBuild_SysRoot_Stage1_SHBuild_DLL}/YFramework.dll" \
	"${SHBuild_SysRoot_Bin}/YFramework.dll"
SHBuild_Install_Link "${SHBuild_SysRoot_Bin}/YBase.dll" \
	"${SHBuild_SysRoot_Lib}/YBase.dll.a"
SHBuild_Install_Link "${SHBuild_SysRoot_Bin}/YFramework.dll" \
	"${SHBuild_SysRoot_Lib}/YFramework.dll.a"

# 3rd party libraries.
SHBuild_Install_HardLink \
	"${YSLib_BaseDir}/YFramework/${YSLib_Arch}/lib/libfreetype.a" \
	"${SHBuild_SysRoot_Lib}/libfreetype.a"
SHBuild_Install_HardLink \
	"${YSLib_BaseDir}/YFramework/${YSLib_Arch}/lib/libFreeImage.a" \
	"${SHBuild_SysRoot_Lib}/libFreeImage.a"

echo Finished installing headers and libraries.

export INCLUDES="${INCLUDES} -I${SHBuild_SysRoot_Include}"
export LIBS="${LIBS} -L`SHBuild_2w ${SHBuild_SysRoot_Lib}` -lYFramework -lYBase"
. ${SHBuild_ToolDir}/SHBuild-common-options.sh
export LDFLAGS

echo Building Stage 2 SHBuild ...
${SHBuild_BaseDir}/SHBuild ${SHBuild_BaseDir} \
	"-xd,${SHBuild_SysRoot_SHBuild}" -xmode,2 ${SHBuild_Opt} ${CXXFLAGS} \
	-fno-lto -fwhole-program ${INCLUDES}
echo Finished building Stage 2 SHBuild.

echo Installing Stage 2 SHBuild ...
SHBuild_Install_HardLink_Exe "${SHBuild_SysRoot_SHBuild}/SHBuild.exe" \
	"${SHBuild_SysRoot_Bin}/SHBuild.exe"
SHBuild_Install_Exe "${SHBuild_ToolDir}/SHBuild-common.sh" \
	"${SHBuild_SysRoot_Bin}/SHBuild-common.sh"
SHBuild_Install_Exe "${SHBuild_ToolDir}/SHBuild-common-options.sh" \
	"${SHBuild_SysRoot_Bin}/SHBuild-common-options.sh"
SHBuild_Install_Exe "${SHBuild_ToolDir}/SHBuild-common-toolchain.sh" \
	"${SHBuild_SysRoot_Bin}/SHBuild-common-toolchain.sh"
SHBuild_Install_Exe "${SHBuild_ToolDir}/SHBuild-BuildApp.sh" \
	"${SHBuild_SysRoot_Bin}/SHBuild-BuildApp.sh"
echo Finished installing Stage 2 SHBuild.

echo Done.

