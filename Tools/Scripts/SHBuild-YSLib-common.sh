#!/usr/bin/env bash
# (C) 2014-2017 FrankHB.
# Script for build YSLib using SHBuild.

set -ae
SHBuild_ToolDir=$(cd `dirname "$0"`; pwd)

# Requires:
#	SHBuild_Tool_Common: The path %SHBuild-common.sh.
#	SHBuild_Tool_Common_ToolChain: The path of %SHBuild-common-toolchain.sh.
#	AR: Archiver, should override default value in %SHBuild-common-toolchain.sh
#		if LTO is enabled, usually 'gcc-ar'.
#	SHBuild: The path of %SHBuild command. It is usually stage 1 here.

. $SHBuild_Tool_Common
SHBuild_AssertNonempty SHBuild_Tool_Common_ToolChain
. $SHBuild_Tool_Common_ToolChain
SHBuild_AssertNonempty SHBuild

SHBuild_Pushd
cd "$YSLib_BuildDir"
declare -r debug="$1"
shift
# TODO: Use host freetype in non-Win32 hosted platforms at
#	%SHBuild_YF_SystemLibs @ SHBuild-BuildApp.sh like here.
# Require when not skipped: YSLib_BaseDir = path to YSLib base directory.
args="$@" SHBuild_RunNPLs_ $SNSC_Common_ '
	$env-de! AR "gcc-ar";
	$def! YSLib_BuildDir env-get "YSLib_BuildDir";
	$assert-nonempty YSLib_BuildDir;
	SHBuild_EnsureDirectory_ YSLib_BuildDir;
	$env-de! SHBuild_Env_OS ($set-system-var! SHBuild_Env_uname "uname";
		SHBuild_CheckUName_Case_ SHBuild_Env_uname);
	$env-de! SHBuild_Env_Arch ($set-system-var! SHBuild_Env_uname_m "uname -m";
		SHBuild_CheckUNameM_Case_ SHBuild_Env_uname_m);
	$defl! SHBuild_EchoVar_E (env var) SHBuild_EchoVar var
		(eval (string->symbol (SHBuild_SDot_ var)) env);
	$defl! skip-or-build (nskip dynamic) $let
	(
		(debug not? (env-empty? "debug"))
		(lname ++ ($if dynamic "dynamic " "static ") "libraries")
	)
	(
		$def! YSLib_BaseDir env-get "YSLib_BaseDir";
		$assert-nonempty YSLib_BaseDir;
		$if debug ($redef! lname ++ "debug " lname);
		$if nskip ($let*
		(
			(env-os SHBuild_Env_OS)
			(SHBuild_Host_Platform $if (win32? env-os) "MinGW32" env-os)
			(repo-base SHBuild_2m env-os YSLib_BaseDir)
			(YF_SystemLibs
				$if (win32? env-os) "-lgdi32 -limm32" "-lxcb -lpthread")
			(DIR_YFramework ++ repo-base "/YFramework")
			(LIBS_YFramework SHBuild_TrimOptions_ (++ " -L\"" (SHBuild_2m env-os
				(++ DIR_YFramework "/" SHBuild_Host_Platform "/lib-"
				SHBuild_Env_Arch)) "\" -lFreeImage -lfreetype " YF_SystemLibs))
		)
		(
			$defl! incsub (dir) ++ "-I\"" (SHBuild_2m env-os dir) "\"";
			$def! INCLUDES_YBase SHBuild_TrimOptions_
				(incsub (++ repo-base "/YBase/include"));
			$def! INCLUDES_YFramework SHBuild_TrimOptions_ (++
				(foldr1 ++ "" (map1 ($lambda (n) ++ (incsub (++ DIR_YFramework n
					"/include")) " ") (list "" "/Android" "/DS" "/Win32")))
				(incsub (++ repo-base "/3rdparty/include")) " "
				(env-get "INCLUDES_freetype") " " INCLUDES_YBase);
			for-each-ltr SHBuild_EchoVar_N (list "SHBuild");
			for-each-ltr ($lambda (var) SHBuild_EchoVar_E
				(() get-current-environment) var)
				(list "SHBuild_Host_Platform" "INCLUDES_YBase"
				"INCLUDES_YFramework" "LIBS_YFramework");
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
						$if (env-empty? "SHBuild_NoPCH")
						(
							SHBuild_BuildGCH
								(++ repo-base "/YBase/include/stdinc.h")
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
						$defl! build-lib (lib-name opt-str)
							system-check (++ shbuild " " SHBOPT " -xn," LIBPFX
							(libdname lib-name) " " (env-get "args") " "
							repo-base "/" lib-name " " CXXFLAGS " " opt-str " "
							SHBuild_IncPCH);
						build-lib "YBase" (++ DFLAG_B_YB " " INCLUDES_YBase);
						$if dynamic (env-set "LIBS" (++ "-L" outdir " -l"
							(libdname "YBase") " " LIBS_YFramework));
						build-lib "YFramework"
							(++ ($if debug ($if dynamic "-DYB_DLL" "")
							DFLAG_B_YB) " " ($if dynamic "-DYF_BUILD_DLL" "")
							" -DFREEIMAGE_LIB "
							(SHBuild_TrimOptions_ INCLUDES_YFramework))
					)
				);
				putss "Finished building " lname "."
			)
		))
		(putss "Skipped " lname ".")
	);
	skip-or-build (env-empty? "SHBuild_NoStatic") #f;
	skip-or-build (env-empty? "SHBuild_NoDynamic") #t;
	'
SHBuild_Popd
SHBuild_Puts Done.

