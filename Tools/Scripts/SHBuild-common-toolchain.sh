#!/usr/bin/env bash
# (C) 2014-2015, 2017 FrankHB.
# Common source script: toolchain configuration.

: ${CC:='gcc'}
export CC
: ${CXX:='g++'}
export CXX
: ${AR:='ar'}
export AR
: ${ARFLAGS:='rcs'}
export ARFLAGS
: ${LD:="$CXX"}
export LD

SHBuild_RunNPLs_()
{
	declare -r SHBuild_RunNPLs_cmd_=$@
	$SHBuild -xcmd,RunNPL "$SHBuild_RunNPLs_cmd_"
}

# XXX: 'SHBuild_2*' depend on 'cygpath' optionally.
# XXX: 'SHBuild_CheckUName_*' depend on 'uname'.
# SNSC=SHBuild NPL String Components
# declare -r SNSC_Common_
SNSC_Common_='
	$def! $redef! $def!;
	$defl! rmatch? (x r) regex-match? x (string->regex r);
	$defl! putss (.xs) puts (apply ++ xs);
	$defl! cmd-error-msg_ (cmd) ++ "Failed to call command: " cmd ".";
	$defl! system-ok (cmd) eqv? (system cmd) 0;
	$defl! system-check (cmd) $unless (system-ok cmd)
		(SHBuild_RaiseError_ (cmd-error-msg_ cmd));
	$defv! $set-if-empty! (var .vexpr) env $if (string-empty? (eval var env))
		(eval (list $set! env var vexpr) env);
	$defv! $env-de! (var .vexpr) env
		$let ((t env-get (symbol->string var)))
			eval (list $def! var ($if (string-empty? t) (list vexpr) t)) env;
	$defv! $set-system-var! (var cmd) env $unless ($binds1? env var)
	(
		$let ((res system-get (eval cmd env))) $if (eqv? (first (rest (res))) 0)
			(eval (list $set! env var (list first (list system-get cmd))) env)
			(SHBuild_RaiseError_ (cmd-error-msg_ cmd))
	);
	$defv! $assert-nonempty (var) env $unless
		($and? (eval (list $binds1? env var) env)
			(not? (string-empty? (eval (list var) env))))
		(SHBuild_RaiseError_ (++ "Variable " (SHBuild_QuoteS_
			(symbol->string var)) " should not be empty."));
	$defl! SHBuild_CheckUName_Case_ (x) $cond
		((rmatch? x ".*Darwin.*") "OS_X")
		(($or? (rmatch? x ".*MSYS.*") (rmatch? x ".*MINGW.*")) "Win32")
		((rmatch? x ".*Linux.*") "Linux")
		(#t "unknown");
	$defl! SHBuild_CheckUNameM_Case_ (x) $cond
		((rmatch? x "x86_64|i.*86-64") x)
		((rmatch? x "i.*86") x)
		(#t "unknown");
	$defl! SHBuild_GetLangFlags_
		(var flags-pic flags-common flags-impl flags-dbg)
		$let ((val env-get var))
			SHBuild_TrimOptions_ ($if (string-empty? val)
				(++ flags-pic " " flags-common " " flags-impl " " flags-dbg)
				val);
	$defl! compile-ok (src compile out opt err-out)
	(
		$unless (string-empty? (env-get "SHELL"))
			(string<- src (++ "\"" src "\""));
		system-ok (++ "echo " src " | \"" compile "\" -xc++ -o" out " " opt
			" - 2> " err-out)
	);
	$defl! get-thread-option (cxx err-out)
	(
		"TODO", "Impl without pthread?";
		$defl! dmp-test (name) (system-ok (++ "\"" cxx
			"\" -dumpspecs 2>& 1 | grep " name ": > " err-out));
		$if (dmp-test "no-pthread")
			($if (dmp-test "mthreads") "-mthreads" "") "-pthread"
	);
	$defl! win32? (env-os) eqv? env-os "Win32";
	$defl! get-nul-dev (env-os) $if (win32? env-os) "NUL" "/dev/null";
	$defl! get-tmp-dir (env-os) $if (win32? env-os) "%TEMP%" "/tmp";
	$defl! get-tmp-nul (env-os) ++ (get-tmp-dir env-os) "/null";
	$defl! system-or-puts (env-os cmd pth)
	(
		$def! res system-get (++ cmd " \"" pth "\" 2> " (get-nul-dev env-os));
		$if (eqv? (first (rest res)) 0) (first res) pth
	);
	$defl! SHBuild_2m (env-os pth) system-or-puts env-os "cygpath -m" pth;
	$defl! SHBuild_2u (env-os pth) system-or-puts env-os "cygpath -au" pth;
	$defl! SHBuild_2w (env-os pth) system-or-puts env-os "cygpath -w" pth;
	$defl! compile-ok-silent (env-os src compile opt)
		compile-ok src compile (get-tmp-nul env-os) opt (get-nul-dev env-os);
	$defl! get-thread-option-silent (env-os cxx)
		get-thread-option cxx (get-nul-dev env-os);
	$defl! build-with-conf-opt (outdir env-os debug dynamic SHBOPT_IGN do-build)
	(
		$def! CXXFLAGS_OPT_DBG
			$if debug ("-O0 -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC") "";
		$env-de! CXX "";
		"TODO", "Add compiler sanity check.";
		$env-de! C_CXXFLAGS_GC "-fdata-sections -ffunction-sections";
		$env-de! LDFLAGS_GC
			($if (eqv? env-os "OS_X") "-Wl,--dead-strip" "-Wl,--gc-sections");
		"#", "-Wl,--print-gc-sections";
		$unless (compile-ok-silent env-os "int main(){}" CXX
			(++ C_CXXFLAGS_GC " " LDFLAGS_GC))
		(
			$redef! C_CXXFLAGS_GC "",
			$redef! LDFLAGS_GC ""
		);
		$env-de! C_CXXFLAGS_PIC ($if (win32? env-os) "" "-fPIC");
		$env-de! C_CXXFLAGS_COMMON (++ "-pipe "
			C_CXXFLAGS_GC " " (env-get "C_CXXFLAGS_ARCH") " -pedantic-errors");
		$env-de! C_CXXFLAGS_OPT_LV "-O3";
		$env-de! C_CXXFLAGS_WARNING "-Wall
			-Wcast-align
			-Wdeprecated
			-Wdeprecated-declarations
			-Wextra
			-Wfloat-equal
			-Wformat=2
			-Winvalid-pch
			-Wmissing-declarations
			-Wmissing-include-dirs
			-Wmultichar
			-Wno-format-nonliteral
			-Wredundant-decls
			-Wshadow
			-Wsign-conversion";
		$def! cxxflags_impl_common_thrd_ get-thread-option-silent env-os CXX;
		$env-de! CXXFLAGS_IMPL_WARNING "";
		$env-de! C_CXXFLAGS_COMMON_IMPL_ "";
		$env-de! CXXFLAGS_IMPL_OPT "";
		$env-de! C_CXXFLAGS_IMPL_WARNING "";
		$env-de! LDFLAGS_IMPL_OPT "";
		$cond
		(
			(string-contains-ci? CXX "clang++")
			($set-if-empty! C_CXXFLAGS_COMMON_IMPL_
				"-fno-merge-all-constants")
			($set-if-empty! CXXFLAGS_IMPL_WARNING
				"-Wno-deprecated-register
				-Wno-mismatched-tags
				-Wno-missing-braces
				-Wshorten-64-to-32
				-Wweak-vtables")
			("#", "CXXFLAGS_IMPL_OPT -flto")
			($set-if-empty! LDFLAGS_IMPL_OPT CXXFLAGS_IMPL_OPT)
		)
		(
			(string-contains-ci? CXX "g++")
			($set-if-empty! C_CXXFLAGS_IMPL_WARNING
				"-Wdouble-promotion
				-Wlogical-op
				-Wtrampolines")
			($set-if-empty! CXXFLAGS_IMPL_WARNING
				"-Wconditionally-supported
				-Wno-noexcept-type
				-Wstrict-null-sentinel
				-Wzero-as-null-pointer-constant")
			($set-if-empty! CXXFLAGS_IMPL_OPT
				"-s -fexpensive-optimizations -flto=jobserver")
			($set-if-empty! LDFLAGS_IMPL_OPT
				"-s -fexpensive-optimizations -flto")
		);
		$env-de! CFLAGS_STD "-std=c11";
		$env-de! CFLAGS_WARNING
			(++ C_CXXFLAGS_WARNING " " C_CXXFLAGS_IMPL_WARNING);
		$env-de! CFLAGS_COMMON
			(++ C_CXXFLAGS_COMMON " " CFLAGS_STD " " CFLAGS_WARNING);
		$env-de! CXXFLAGS_IMPL_COMMON cxxflags_impl_common_thrd_;
		$env-de! CXXFLAGS_STD "-std=c++11";
		$env-de! CXXFLAGS_WARNING (++ CFLAGS_WARNING
			" -Wctor-dtor-privacy
			-Wnon-virtual-dtor
			-Woverloaded-virtual
			-Wsign-promo "
			CXXFLAGS_IMPL_WARNING);
		$env-de! CXXFLAGS_COMMON (++ CXXFLAGS_STD " "
			C_CXXFLAGS_COMMON " " CXXFLAGS_WARNING " " CXXFLAGS_IMPL_COMMON);
		$set-if-empty! CXXFLAGS_OPT_DBG (++ C_CXXFLAGS_OPT_LV
			(
				$if (env-empty? "CXXFLAGS_OPT_UseAssert")
					" -DNDEBUG "
					" "
			)
			CXXFLAGS_IMPL_OPT " -fomit-frame-pointer");
		"XXX", "Rename %CXXFLAGS_OPT_DBG -> CFLAGS_OPT_DBG/C_CXXFLAGS_OPT_DBG?";
		$defl! get-lang-flags (var flags-common)
		(
			SHBuild_GetLangFlags_ var C_CXXFLAGS_PIC flags-common
				C_CXXFLAGS_COMMON_IMPL_ CXXFLAGS_OPT_DBG
		);
		$def! CFLAGS get-lang-flags "CFLAGS" CFLAGS_COMMON;
		$def! CXXFLAGS get-lang-flags "CXXFLAGS" CXXFLAGS_COMMON;
		$env-de! LDFLAGS_OPT_DBG
			(++ LDFLAGS_IMPL_OPT " " LDFLAGS_GC);
		$def! LDFLAGS_DYN_BASE env-get "LDFLAGS_DYN_BASE";
		$def! LIBS_RPATH env-get "LIBS_RPATH";
		$def! LIBPFX env-get "LIBPFX";
		$def! DSOSFX env-get "DSOSFX";
		$def! EXESFX env-get "EXESFX";
		$if (win32? env-os)
		(
			$set-if-empty! LDFLAGS_DYN_BASE "-shared -Wl,--dll";
			$set-if-empty! DSOSFX ".dll";
			$set-if-empty! EXESFX ".exe"
		)
		(
			$set-if-empty! LDFLAGS_DYN_BASE "-shared";
			$set-if-empty! LIBS_RPATH (++ "-Wl,-rpath,"
				(SHBuild_QuoteS_ "\\$ORIGIN:\\$ORIGIN/../lib"));
			$set-if-empty! LIBPFX "lib";
			$set-if-empty! DSOSFX ".so";
		);
		$env-de! LDFLAGS_DYN_EXTRA (++ "-Wl,--no-undefined"
			" -Wl,--dynamic-list-data,--dynamic-list-cpp-new,"
			"--dynamic-list-cpp-typeinfo");
		$env-de! LDFLAGS_DYN (++ LDFLAGS_DYN_BASE " " LDFLAGS_DYN_EXTRA);
		"#", "Forced setting %LDFLAGS for debug configurations?";
		$def! LDFLAGS $if debug
			(++ C_CXXFLAGS_PIC " " cxxflags_impl_common_thrd_)
			(++ C_CXXFLAGS_PIC " "
				cxxflags_impl_common_thrd_ " " LDFLAGS_OPT_DBG);
		$redef! LDFLAGS SHBuild_TrimOptions_ (++ (SHBuild_TrimOptions_ LDFLAGS)
			($if dynamic (++ " " LDFLAGS_DYN) " -Wl,--dn"));
		env-set "LDFLAGS" LDFLAGS;
		$def! SHBOPT ++ "-xd," outdir " " SHBOPT_IGN;
		$if dynamic ($redef! SHBOPT ++ SHBOPT " -xmode,2");
		$defl! echo-var (var) SHBuild_EchoVar var (value-of var);
		echo-var "SHBOPT";
		echo-var "CXXFLAGS";
		echo-var "LDFLAGS";
		do-build CXX CXXFLAGS SHBOPT LIBPFX
	);
	'

