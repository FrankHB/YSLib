#!/usr/bin/env bash
# (C) 2014-2023 FrankHB.
# Common options script to build projects in the shell.

# NOTE: This is mainly for stage 1 SHBuild bootstrap and the test. The options
#	here should be equivalent to %SHBuild-YSLib-common.txt. See the user
#	document for details.

set -e

: "${SHBuild_ToolDir:=$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)}"
# shellcheck source=./SHBuild-common-toolchain.sh
. "$SHBuild_ToolDir/SHBuild-common-toolchain.sh" # for
#	SHBuild-common-toolchain.sh, SHBuild_Debug, SHBuild_Host_OS, CXX,
#	SS_Verbose, SHBuild_Puts, CXXFLAGS, LDFLAGS;

# XXX: %SHBuild_Debug is external.
# shellcheck disable=2154
if test -n "$SHBuild_Debug"; then
	CXXFLAGS_OPT_DBG='-O0 -g -D_GLIBCXX_DEBUG_PEDANTIC'
	LDFLAGS_OPT_DBG=' '
fi

: "${C_CXXFLAGS_GC:=-fdata-sections -ffunction-sections}"

# XXX: %SHBuild_Host_OS is external.
# shellcheck disable=2154
if test "$SHBuild_Host_OS" = OS_X; then
	: "${LDFLAGS_GC:="-Wl,--dead-strip"}"
else
	: "${LDFLAGS_GC:="-Wl,--gc-sections"}"
	#: "${LDFLAGS_GC:="-Wl,--gc-sections -Wl,--print-gc-sections"}"
fi

# NOTE: See %SHBuild_CheckCXX.
if test -n "$SHBuild_CXX_Name" \
	&& ! SHBuild_CXX_TestSimple "$C_CXXFLAGS_GC" "$LDFLAGS_GC"; then \
	C_CXXFLAGS_GC=''
	LDFLAGS_GC=''
fi
# XXX: %SHBuild_Host_OS is external.
# shellcheck disable=2154
if test "$SHBuild_Host_OS" != Win32; then
	: "${C_CXXFLAGS_PIC:=-fPIC -fno-semantic-interposition}"
fi

# NOTE: See https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html#Link-Options,
#	also https://clang.llvm.org/docs/ClangCommandLineReference.html#linker-flags.
: "${LDFLAGS_STRIP:=-s}"

if test "$SHBuild_Host_OS" != Win32; then
	: "${C_CXXFLAGS_EXT="-D_POSIX_C_SOURCE=200809L"}"
fi
: "${C_CXXFLAGS_COMMON:= \
"-pipe $C_CXXFLAGS_GC $C_CXXFLAGS_ARCH -pedantic-errors $C_CXXFLAGS_EXT"}"
: "${C_CXXFLAGS_OPT_LV:=-O3}"

# NOTE: The compiler should be specified earlier than this line to
#	automatically determine if these values should be used.
SHBuild_Get_C_CXXFLAGS_WARNING()
{
	# NOTE: The minimal versions are GCC 2.95.3 and Clang 3.0.0.
	local opts_cc=''
	local w_deprecated=-Wdeprecated
	local w_deprecated_declarations=-Wdeprecated-declarations
	local w_double_promotion=-Wdouble-promotion
	local w_extra=-Wextra
	local w_format='-Wformat=2 -Wno-format-nonliteral'
	local w_float_equal=-Wfloat-equal
	local w_invalid_pch=-Winvalid-pch
	local w_missing_include_dirs=-Wmissing-include-dirs
	local w_sign_conversion=-Wsign-conversion
	# XXX: This is for G++ only, but there is always some (partial) fallback for
	#	supported compilers.
	local w_suggest_attribute="-Wsuggest-attribute=const \
-Wsuggest-attribute=noreturn \
-Wsuggest-attribute=pure"

	if test "$SHBuild_CXX_Name" = Clang++; then
		if ((SHBuild_CXX_Version < 30800)); then
			w_double_promotion=''
		fi
		w_suggest_attribute=-Wmissing-noreturn
	elif test "$SHBuild_CXX_Name" = G++; then
		local w_logical_op=-Wlogical-op
		local w_trampolines=-Wtrampolines

		# XXX: '-Wfloat-equal' is documented in GCC 3.0.4. Actually it is
		#	introduced earlier, see https://gcc.gnu.org/git/?p=gcc.git;a=commit;h=b843d2101e5b335f9ae87d5dea12372aca5fb5da.
		if ((SHBuild_CXX_Version < 30000)); then
			w_float_equal=''
		fi
		# XXX: '-Wno-deprecated-declarations' is documented in GCC 3.1.0.
		if ((SHBuild_CXX_Version < 30100)); then
			w_deprecated_declarations=''
		fi
		# XXX: '-W' seems working since GCC 2.5.
		if ((SHBuild_CXX_Version < 30400)); then
			w_extra=-W
			w_format=-Wformat
			w_invalid_pch=''
		fi
		if ((SHBuild_CXX_Version < 40000)); then
			w_missing_include_dirs=''
		fi
		if ((SHBuild_CXX_Version < 40300)); then
			w_logical_op=''
			w_sign_conversion=''
		fi
		# XXX: '-Wno-deprecated' is documented in GCC 4.4.0 for warnings. This
		#	was only for C/Obj-C++ previously.
		if ((SHBuild_CXX_Version < 40400)); then
			w_deprecated=''
		fi
		# XXX: This is not documented in GCC 5.1, but the alternative is
		#	already used since 4.6. See the default value initialized above.
		if ((SHBuild_CXX_Version < 40600)); then
			w_suggest_attribute=-Wmissing-noreturn
			w_double_promotion=''
			w_trampolines=''
		fi
		opts_cc="$w_logical_op $w_trampolines"
	fi
	echo "-Wall \
-Wcast-align \
$w_deprecated \
$w_deprecated_declarations \
$w_double_promotion \
$w_extra \
$w_float_equal \
$w_format \
$w_invalid_pch \
-Wmissing-declarations \
$w_missing_include_dirs \
-Wmultichar \
-Wredundant-decls \
$w_sign_conversion \
$w_suggest_attribute \
-Wshadow \
$opts_cc"
}

: "${C_CXXFLAGS_WARNING:="$(SHBuild_Get_C_CXXFLAGS_WARNING)"}"

if "$CXX" -dumpspecs 2>& 1 | grep mthreads: > /dev/null; then
	CXXFLAGS_IMPL_COMMON_THRD_=-mthreads
	LDFLAGS_IMPL_COMMON_THRD_=-mthreads
elif SHBuild_CXX_TestSimple -Werror -mthreads; then
	if SHBuild_CXX_TestSimple -c -Werror -mthreads; then
		CXXFLAGS_IMPL_COMMON_THRD_=-mthreads
	else
		CXXFLAGS_IMPL_COMMON_THRD_=-D_MT
	fi
	LDFLAGS_IMPL_COMMON_THRD_=-mthreads
elif "$CXX" -dumpspecs 2>& 1 | grep no-pthread: > /dev/null; then
	CXXFLAGS_IMPL_COMMON_THRD_=''
	LDFLAGS_IMPL_COMMON_THRD_=''
elif SHBuild_CXX_TestSimple -pthread; then
	CXXFLAGS_IMPL_COMMON_THRD_=-pthread
	LDFLAGS_IMPL_COMMON_THRD_=-pthread
else
	CXXFLAGS_IMPL_COMMON_THRD_=''
	LDFLAGS_IMPL_COMMON_THRD_=''
fi
: "${C_CXXFLAGS_IMPL_WARNING:=""}"
: "${CXXFLAGS_IMPL_WARNING:=""}"
if test "$SHBuild_CXX_Name" = Clang++; then
	# NOTE: The minimal versions are GCC 2.95.3 and Clang 3.0.0.
	# XXX: This is needed for conformance, so not in %CXXFLAGS_IMPL_OPT. See
	#	also $2023-05 @ doc/Workflow.txt.
	: "${C_CXXFLAGS_COMMON_IMPL_:=-fno-merge-all-constants}"
	# XXX: This is the only choice to support LTO on MinGW32 now.
	LDFLAGS_IMPL_USE_LLD_=true
	#: "${CXXFLAGS_IMPL_OPT:=-flto}"
	#: "${LDFLAGS_IMPL_OPT:=-flto}"
	# XXX: LTO is disabled by default for compatibility to the prebuilt
	#	binaries (by G++). If enabled, '-flto=auto' is available since Clang++
	#	13.0.
elif test "$SHBuild_CXX_Name" = G++; then
	# XXX: See $2023-05 @ doc/Workflow.txt.
	LTO_=-flto=auto
	f_new_inheriting_ctors_=-fnew-inheriting-ctors
	f_no_strong_eval_order_=-fno-strong-eval-order
	f_no_enforce_eh_specs_=-fno-enforce-eh-specs
	if ((SHBuild_CXX_Version < 30000)); then
		f_no_enforce_eh_specs_=''
	fi
	if ((SHBuild_CXX_Version < 70100)); then
		f_new_inheriting_ctors_=''
		f_no_strong_eval_order_=''
	fi
	# XXX: Despite the documentation, MinGW GCC 10 does not work with
	#	'-flto=auto'. See also
	#	https://gitlab.kitware.com/cmake/cmake/-/merge_requests/7559.
	if ((SHBuild_CXX_Version < 100100)) || (test "$SHBuild_Host_OS" = Win32 \
		&& ((SHBuild_CXX_Version < 110000))); then
		LTO_=-flto=jobserver
	fi
	# NOTE: The minimal version is 2.95.3, so '-fexpensive-optimizations' is
	#	always supported.
	: "${CXXFLAGS_IMPL_OPT:="-fexpensive-optimizations \
$LTO_ \
$f_new_inheriting_ctors_ \
$f_no_enforce_eh_specs_ \
$f_no_strong_eval_order_"}"
	# XXX: See $2023-05 @ doc/Workflow.txt.
	# XXX: The call of %SHBuild_CheckCXX should have initialized
	#	%SHBuild_Env_TempDir.
	if test "$SHBuild_Host_OS" = Win32 && test -n $LTO_; then
		: "${LDFLAGS_IMPL_OPT:=-fexpensive-optimizations $LTO_ -save-temps \
-dumpdir "$SHBuild_Env_TempDir"}"
		# NOTE: 'lto_wrapper_args' is not cleanup here. It may be in '$TEMP'
		#	directory needing no cleanup (e.g. directly compiling and linking
		#	with G++ < 11) regardless of the dumping directory or the current
		#	working directory; otherwise, it can be in the output directory
		#	which needs the interaction in the call site (if necessary).
		# XXX: Assume no other './*res' can be here. For G++ < 11, the pattern
		#	can usually just be './*.res', though.
		# XXX: Assume no spaces in the filenames matching '*' when being
		#	evaluated in %DoLTOCleanup_.
		# XXX: %LTO_CLEANUP_ is internal.
		# shellcheck disable=2034
		LTO_CLEANUP_='rm -f ./*res ./*.ltrans*'
	else
		: "${LDFLAGS_IMPL_OPT:=-fexpensive-optimizations $LTO_}"
	fi
fi

# NOTE: The following internal stage 1 LTO cleanup functions are intended to be
#	used for cleanup to work around specific implementation problems. Legend of
#	typical usage:
#	trap DoLTOCleanup_ EXIT
#	trap DoLTOCleanup_Int_ INT QUIT TERM
# XXX: Notice the 'trap' on 'EXIT' does not requires additional call on normal
#	exit. It may not work in shells other than GNU bash.

DoLTOCleanup_()
{
	local err=$?

	trap - EXIT
	set +e
	# XXX: %SS_Verbose is external.
	# shellcheck disable=2154
	if test -n "$SS_Verbose"; then
		SHBuild_Puts "INFO: Local error is $err."
	fi
	if test -n "$LTO_CLEANUP_"; then
		if test -n "$SS_Verbose"; then
			SHBuild_Puts \
"INFO: Calling cleanup command: $LTO_CLEANUP_ 2> /dev/null."
		fi
		eval "$LTO_CLEANUP_ 2> /dev/null"
	fi
	exit $err
}

DoLTOCleanup_AddTarget_()
{
	if test -n "$LTO_CLEANUP_"; then
		# XXX: Since the string is to be evaluated, use single quotes to prevent
		#	the unexpected '$' injection.
		# XXX: Assume no spaces in the filenames matching '*' when being
		#	evaluated in %DoLTOCleanup_.
		LTO_CLEANUP_="$LTO_CLEANUP_ '$(dirname "$1")'/*lto_wrapper_args"
	fi
}

DoLTOCleanup_Int_()
{
	SHBuild_Puts "INFO: Interrupted."
	trap - EXIT
	set +e
	false
	DoLTOCleanup_
}

# XXX: %SHBuild_Host_OS is external.
# shellcheck disable=2154
if test "$SHBuild_Host_OS" = Win32 && test -z "$LDFLAGS_IMPL_USE_LLD_"; then
	# XXX: Workarond for LTO bug on MinGW. See
	#	https://sourceware.org/bugzilla/show_bug.cgi?id=12762.
	: "${LDFLAGS_WKRD_:="-Wl,-allow-multiple-definition"}"
else
	: "${LDFLAGS_WKRD_:=""}"
fi

: "${CFLAGS_STD:=-std=c11}"
: "${CFLAGS_WARNING:="$C_CXXFLAGS_WARNING $C_CXXFLAGS_IMPL_WARNING"}"

# XXX: This is an optimization since the dynamic loading is explicitly
#	unsupported as specified in
#	https://frankhb.github.io/YSLib-book/Development.zh-CN.html, as well a
#	workaround for the buggy out-of-line definition of %std::type_info::before
#	which having the bug in https://gcc.gnu.org/bugzilla/show_bug.cgi?id=103240.
# XXX: %SHBuild_Host_OS is external.
# shellcheck disable=2154
if test "$SHBuild_Host_OS" != Win32; then
	# XXX: '-D__GXX_TYPEINFO_EQUALITY_INLINE=1' is required for platform
	#	%MinGW64. '-D__GXX_MERGED_TYPEINFO_NAMES=1' is also an optimization.
	: "${CXXFLAGS_IMPL_COMMON:="$CXXFLAGS_IMPL_COMMON_THRD_ \
		-U__GXX_TYPEINFO_EQUALITY_INLINE -D__GXX_TYPEINFO_EQUALITY_INLINE=1 \
		-U__GXX_MERGED_TYPEINFO_NAMES -D__GXX_MERGED_TYPEINFO_NAMES=1"}"
else
	# XXX: This is required for platform %MinGW32. Both '1' would cause stage 1
	#	SHBuild fail.
	: "${CXXFLAGS_IMPL_COMMON:="$CXXFLAGS_IMPL_COMMON_THRD_ \
		-U__GXX_TYPEINFO_EQUALITY_INLINE -D__GXX_TYPEINFO_EQUALITY_INLINE=1 \
		-U__GXX_MERGED_TYPEINFO_NAMES -D__GXX_MERGED_TYPEINFO_NAMES=0"}"
fi
: "${CXXFLAGS_STD:=-std=c++11}"

SHBuild_Get_CXXFLAGS_WARNING()
{
	# NOTE: The minimal version is 2.95.3.
	local opts_cxx
	local w_deprecated=''
	local w_no_mismatched_tags=-Wno-mismatched-tags
	# XXX: This is a workaround for https://bugs.llvm.org/show_bug.cgi?id=21629.
	#	Although it is supported in G++ (documented in 3.0.4) and newer versions
	#	of Clang++, this is only enabled for affected versions of Clang++.
	local w_no_missing_braces=''
	local w_no_noexcept_type=-Wno-noexcept-type
	local w_zero_as_null_pointer_constant=-Wzero-as-null-pointer-constant

	if test "$SHBuild_CXX_Name" = Clang++; then
		local w_no_deprecated_register=-Wno-deprecated-register

		if ((SHBuild_CXX_Version < 30400)); then
			w_no_deprecated_register=''
		fi
		# XXX: Clang++ supports '-Wnoexcept-types' since 5.0. Clang++ >= 4.0
		#	warns against '-Wno-noexcept-type', though Clang++ 3.0 not.
		if ((SHBuild_CXX_Version < 50000)); then
			w_no_noexcept_type=''
			w_zero_as_null_pointer_constant=''
		fi
		# XXX: See above.
		if ((SHBuild_CXX_Version < 60000)); then
			w_no_missing_braces=-Wno-missing-braces
		fi
		opts_cxx="$w_no_deprecated_register -Wshorten-64-to-32 -Wweak-vtables"
	elif test "$SHBuild_CXX_Name" = G++; then
		local w_conditionally_supported=-Wconditionally-supported
		local w_strict_null_sentinal=-Wstrict-null-sentinel
		local w_suggest_final='-Wsuggest-final-methods -Wsuggest-final-types'

		if ((SHBuild_CXX_Version < 40100)); then
			w_strict_null_sentinal=''
		fi
		# XXX: '-Wno-deprecated' is documented in GCC 4.4.0 for warnings. This
		#	was only for C/Obj-C++ previously.
		if ((SHBuild_CXX_Version < 40400)); then
			w_deprecated=-Wdeprecated
		fi
		if ((SHBuild_CXX_Version < 40900)); then
			w_conditionally_supported=''
		fi
		if ((SHBuild_CXX_Version < 50100)); then
			w_suggest_final=''
		fi
		# XXX: G++ supports '-Wmismatch-tags' since 9.0. Although
		#	'-Wno-mismatched-tags' is just fine for previous versions, ignore
		#	it here.
		if ((SHBuild_CXX_Version < 90000)); then
			w_no_mismatched_tags=''
		fi
		opts_cxx="$w_conditionally_supported \
$w_strict_null_sentinal \
$w_suggest_final"
	fi
	echo "$C_CXXFLAGS_WARNING \
$C_CXXFLAGS_IMPL_WARNING \
-Wctor-dtor-privacy \
$w_deprecated \
$w_no_mismatched_tags \
$w_no_missing_braces \
$w_no_noexcept_type \
-Wnon-virtual-dtor \
-Woverloaded-virtual \
-Wsign-promo \
$w_zero_as_null_pointer_constant \
$opts_cxx \
$CXXFLAGS_IMPL_WARNING"
}

: "${CXXFLAGS_WARNING:="$(SHBuild_Get_CXXFLAGS_WARNING)"}"
# XXX: %CXXFLAGS_OPT_UseAssert is external.
# shellcheck disable=2154
if test -z "$CXXFLAGS_OPT_UseAssert"; then
	: "${CXXFLAGS_OPT_DBG:=" \
$C_CXXFLAGS_OPT_LV -DNDEBUG \
$CXXFLAGS_IMPL_OPT \
-fomit-frame-pointer"}"
else
	: "${CXXFLAGS_OPT_DBG:=" \
$C_CXXFLAGS_OPT_LV \
$CXXFLAGS_IMPL_OPT \
-fomit-frame-pointer"}"
fi

# XXX: See $2023-05 @ doc/Workflow.txt.
if test "$SHBuild_Host_OS" = Win32 \
	&& SHBuild_CXX_TestSimple -Wa,-mbig-obj "$CXXFLAGS_OPT_DBG"; then
	C_CXXFLAGS_COMMON_IMPL_="$C_CXXFLAGS_COMMON_IMPL_ -Wa,-mbig-obj"
fi

# XXX: Rename %CXXFLAGS_OPT_DBG to CFLAGS_OPT_DBG or C_CXXFLAGS_OPT_DBG?
: "${CFLAGS:="$CFLAGS_STD $C_CXXFLAGS_PIC $C_CXXFLAGS_COMMON $CFLAGS_WARNING \
$C_CXXFLAGS_COMMON_IMPL_ $CXXFLAGS_OPT_DBG"}"
CFLAGS="${CFLAGS//	/ }"

: "${CXXFLAGS:="$CXXFLAGS_STD $C_CXXFLAGS_PIC $C_CXXFLAGS_COMMON \
$CXXFLAGS_WARNING $CXXFLAGS_IMPL_COMMON $C_CXXFLAGS_COMMON_IMPL_ \
$CXXFLAGS_OPT_DBG"}"
CXXFLAGS="${CXXFLAGS//	/ }"

: "${LDFLAGS_OPT_DBG:="$LDFLAGS_STRIP $LDFLAGS_IMPL_OPT $LDFLAGS_GC"}"

: "${LDFLAGS:="$C_CXXFLAGS_PIC \
$LDFLAGS_IMPL_COMMON_THRD_ \
$LDFLAGS_OPT_DBG \
$LDFLAGS_WKRD_"}"
if test -n "$LDFLAGS_IMPL_USE_LLD_"; then
	LDFLAGS="-fuse-ld=lld $LDFLAGS"
fi
LDFLAGS="${LDFLAGS//	/ }"

