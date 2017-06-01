#!/usr/bin/env bash
# (C) 2014-2017 FrankHB.
# Script for build YSLib using SHBuild.

set -e
SHBuild_ToolDir=$(cd `dirname "$0"`; pwd)
: ${SHBuild_BaseDir:="$SHBuild_ToolDir/../SHBuild"}
: ${YSLib_BaseDir:="$SHBuild_ToolDir/../.."}

. $SHBuild_ToolDir/SHBuild-common.sh
SHBuild_AssertNonempty YSLib_BuildDir

: ${AR:='gcc-ar'}
export AR
. $SHBuild_ToolDir/SHBuild-common-toolchain.sh

mkdir -p "$YSLib_BuildDir"
: ${SHBuild:="$YSLib_BuildDir/.stage1/SHBuild"}
SHBuild_CheckHostPlatform

declare -r SNSC_Impl_PrepareLibBuild_=''
declare -r SNSC_PrepareLibBuild_="$SNSC_Common_ $SNSC_Impl_PrepareLibBuild_"

export LANG=C
unset CXXFLAGS_COMMON
unset CXXFLAGS

SHBuild_Pushd
cd "$YSLib_BuildDir"
declare -r debug_="$1"
shift
SHBuild_CheckUName
export SHBuild_Env_OS
export CXX
export CFLAGS CXXFLAGS
export C_CXXFLAGS_GC LDFLAGS_GC
export C_CXXFLAGS_PIC C_CXXFLAGS_COMMON C_CXXFLAGS_ARCH C_CXXFLAGS_OPT_LV \
	C_CXXFLAGS_WARNING CXXFLAGS_IMPL_WARNING C_CXXFLAGS_COMMON_IMPL_ \
	C_CXXFLAGS_IMPL_WARNING
export CXXFLAGS_IMPL_OPT LDFLAGS_IMPL_OPT
export CFLAGS_STD CFLAGS_WARNING CFLAGS_COMMON
export CXXFLAGS_IMPL_COMMON CXXFLAGS_STD CXXFLAGS_WARNING CXXFLAGS_COMMON \
	CXXFLAGS_OPT_UseAssert=$CXXFLAGS_OPT_UseAssert
export LDFLAGS_OPT_DBG LDFLAGS_DYN_BASE
export LIBS_RPATH LIBPFX DSOSFX EXESFX
export LDFLAGS_DYN_EXTRA LDFLAGS_DYN LDFLAGS
export SHBuild_NoPCH SHBuild
export YSLib_BaseDir
# TODO: Use host freetype in non-Win32 hosted platforms at
#	%SHBuild_YF_SystemLibs @ SHBuild-BuildApp.sh like here.

debug=$debug_ args="$@" \
	SHBuild_Host_Platform=$SHBuild_Host_Platform \
	SHBuild_Env_Arch=$SHBuild_Env_Arch \
	INCLUDES_freetype=$INCLUDES_freetype \
	SHBuild_RunNPLs_ $SNSC_PrepareLibBuild_ \
	'
	$defl! SHBuild_EchoVar_E (env var) SHBuild_EchoVar var
		(eval (string->symbol (SHBuild_SDot_ var)) env);
	$defl! skip-or-build (nskip dynamic) $let
	(
		(debug not? (env-empty? "debug"))
		(lname ++ ($if dynamic "dynamic " "static ") "libraries")
		(env-os env-get "SHBuild_Env_OS")
	)
	(
		$def! YSLib_BaseDir SHBuild_2m env-os (env-get "YSLib_BaseDir");
		$def! YF_SystemLibs
			$if (win32? env-os) "-lgdi32 -limm32" "-lxcb -lpthread";
		$def! DIR_YFramework ++ YSLib_BaseDir "/YFramework";
		$defl! incsub (dir) ++ "-I\"" (SHBuild_2m env-os dir) "\"";
		$def! INCLUDES_YBase SHBuild_TrimOptions_
			(incsub (++ YSLib_BaseDir "/YBase/include"));
		$def! INCLUDES_YFramework SHBuild_TrimOptions_ (++
			(foldr1 ++ "" (map1
				($lambda (n) ++ (incsub (++ DIR_YFramework n "/include")) " ")
				(list "" "/Android" "/DS" "/Win32")))
			(incsub (++ YSLib_BaseDir "/3rdparty/include")) " "
			(env-get "INCLUDES_freetype") " "
			INCLUDES_YBase
		);
		$def! LIBS_YFramework SHBuild_TrimOptions_
			(++ " -L\"" (SHBuild_2m env-os (++ DIR_YFramework "/"
			(env-get "SHBuild_Host_Platform")
			"/lib-" (env-get "SHBuild_Env_Arch")))
			"\" -lFreeImage -lfreetype " YF_SystemLibs);
		for-each-ltr SHBuild_EchoVar_N (list "SHBuild" "SHBuild_Host_Platform");
		for-each-ltr ($lambda (var) SHBuild_EchoVar_E
			(() get-current-environment) var)
			(list "INCLUDES_YBase" "INCLUDES_YFramework" "LIBS_YFramework");
		$if debug ($redef! lname ++ "debug " lname);
		$if nskip
		(
			$let
			(
				(shbuild env-get "SHBuild")
				(outdir ++ ".shbuild" ($if dynamic "-dll" "")
					($if debug "-debug" ""))
			)
			(
				putss "Building " lname " ...";
				build-with-conf-opt outdir env-os debug dynamic
					"-xid,alternative -xid,data -xid,include -xid,Android"
					($lambda (CXX CXXFLAGS SHBOPT LIBPFX)
					(
						$def! repo-base env-get "YSLib_BaseDir";
						$if (env-empty? "SHBuild_NoPCH")
						(
							SHBuild_BuildGCH (++ repo-base
								"/YBase/include/stdinc.h")
								(++ outdir "/stdinc.h")
								(++ CXX " -xc++-header " CXXFLAGS);
							$def! SHBuild_IncPCH ++ "-include " outdir
								"/stdinc.h"
						)
						(
							puts "Skipped building precompiled file.";
							$def! SHBuild_IncPCH ""
						);
						$defl! libdname (lib) ++ lib ($if debug "d" "");
						$def! DFLAG_B_YB $if dynamic "-DYB_BUILD_DLL" "";
						$def! args env-get "args";
						$defl! build-lib (lib-name opt-str)
							system-check (++ shbuild " " SHBOPT " -xn," LIBPFX
							(libdname lib-name) " " args " " repo-base "/"
							lib-name " " CXXFLAGS " " opt-str " "
							SHBuild_IncPCH);
						build-lib "YBase" (++ DFLAG_B_YB " " INCLUDES_YBase);
						$if dynamic (env-set "LIBS" (++ "-L" outdir " -l"
							(libdname "YBase") " " LIBS_YFramework));
						build-lib "YFramework" (++ ($if debug
							($if dynamic "-DYB_DLL" "") DFLAG_B_YB) " "
							($if dynamic "-DYF_BUILD_DLL" "")
							" -DFREEIMAGE_LIB " (SHBuild_TrimOptions_
							INCLUDES_YFramework))
					)
				);
				putss "Finished building " lname "."
			)
		)
		(putss "Skipped " lname ".")
	);
	skip-or-build (env-empty? "SHBuild_NoStatic") #f;
	skip-or-build (env-empty? "SHBuild_NoDynamic") #t;
	'
SHBuild_Popd
SHBuild_Puts Done.

