#!/usr/bin/bash
# (C) 2014 FrankHB.
# SHBuild installation script.

set -e
SHBuild_ToolDir=$(cd `dirname "$0"`; pwd)
: ${SHBuild_BaseDir:="${SHBuild_ToolDir}/../SHBuild"}
: ${YSLib_BaseDir:="${SHBuild_ToolDir}/../.."}
source ${SHBuild_ToolDir}/common.sh

echo Configuring ...
# POSIX paths may be not supported by native Windows programs without
#	conversion.
export COMSPEC=bash
: ${SHBuild_BuildOpt:="-xj,8"}
export SHBuild_BuildOpt
: ${SHBuild_LogOpt:="-xlogfl,192"}
export SHBuild_LogOpt
: ${SHBuild_Opt:="${SHBuild_LogOpt} ${SHBuild_BuildOpt}"}
export SHBuild_Opt
: ${SHBuild_SysRoot="${YSLib_BaseDir}/sysroot"}
SHBuild_SysRoot_Bin="${SHBuild_SysRoot}/usr/bin"
SHBuild_SysRoot_Include="${SHBuild_SysRoot}/usr/include"
SHBuild_SysRoot_Lib="${SHBuild_SysRoot}/usr/lib"
SHBuild_SysRoot_SHBuild="${SHBuild_SysRoot}/.shbuild"
export AR="gcc-ar"
source ${SHBuild_ToolDir}/common-toolchain.sh
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
SHBuild_SysRoot_Stage1_SHBuild_DLL="${SHBuild_BaseDir}/.shbuild-dll"

SHBuild_Install "${SHBuild_SysRoot_Stage1_SHBuild}/YBase.a" \
	"${SHBuild_SysRoot_Lib}/libYBase.a"
SHBuild_Install "${SHBuild_SysRoot_Stage1_SHBuild}/YFramework.a" \
	"${SHBuild_SysRoot_Lib}/libYFramework.a"
SHBuild_Install "${SHBuild_SysRoot_Stage1_SHBuild_DLL}/YBase.dll" \
	"${SHBuild_SysRoot_Bin}/YBase.dll"
SHBuild_Install "${SHBuild_SysRoot_Stage1_SHBuild_DLL}/YFramework.dll" \
	"${SHBuild_SysRoot_Bin}/YFramework.dll"
SHBuild_Install_Link "${SHBuild_SysRoot_Bin}/YBase.dll" \
	"${SHBuild_SysRoot_Lib}/YBase.dll.a"
SHBuild_Install_Link "${SHBuild_SysRoot_Bin}/YFramework.dll" \
	"${SHBuild_SysRoot_Lib}/YFramework.dll.a"

echo Finished installing headers and libraries.

export INCLUDES="${INCLUDES} -I${SHBuild_SysRoot_Include}"
export LIBS="${LIBS} -L`SHBuild_2w ${SHBuild_SysRoot_Lib}` -lYFramework -lYBase"
source ${SHBuild_ToolDir}/common-options.sh

echo Building Stage 2 SHBuild ...
${SHBuild_BaseDir}/SHBuild ${SHBuild_BaseDir} \
	"-xd,${SHBuild_SysRoot_SHBuild}" -xmode,2 ${SHBuild_Opt} ${CXXFLAGS} \
	${INCLUDES}
echo Finished building Stage 2 SHBuild.

echo Installing Stage 2 SHBuild ...
SHBuild_Install_Exe "${SHBuild_SysRoot_SHBuild}/SHBuild.exe" \
	"${SHBuild_SysRoot_Bin}/SHBuild.exe"
echo Finished installing Stage 2 SHBuild.

echo Done.

