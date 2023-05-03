#!/usr/bin/env bash
# (C) 2014-2018, 2020, 2022-2023 FrankHB.
# Common source script.

test -z "$INC_SHBuild_common" && INC_SHBuild_common=1 || return 0

: "${SHBuild_CMD:="$COMSPEC"}"
: "${SHBuild_CMD:=cmd}"

trap exit 1 INT

SHBuild_Popd()
{
	# XXX: The error is ignored.
	# shellcheck disable=2164
	popd > /dev/null 2>& 1
}

SHBuild_Pushd()
{
	# XXX: The error is ignored.
	# shellcheck disable=2164
	pushd "$1" > /dev/null 2>& 1
}

SHBuild_Put()
{
	printf '%s' "$*"
}

SHBuild_Puts()
{
	# XXX: %SHBuild_EOL is external.
	# shellcheck disable=2154
	if test -z "$SHBuild_EOL"; then
		# TODO: More precise detection of Windows shell.
		if test -z "$COMSPEC"; then
			eval readonly SHBuild_EOL='\\n'
		else
			eval readonly SHBuild_EOL='\\r\\n'
		fi
	fi
	printf "%s$SHBuild_EOL" "$*"
}


SHBuild_AssertNonempty()
{
	local vval

	eval "vval=\"\${$1}\""
	test -n "$vval" || \
		(SHBuild_Puts "ERROR: Variable \"$1\" should not be empty." >& 2; \
			exit 1)
}

SHBuild_CheckedCall()
{
	if [[ "$1" != */* ]] && (hash "$1" > /dev/null 2>& 1) \
		|| [[ "$1" == */* && ! -d "$1" && -x "$1" ]]; then
		"$@" || exit
	else
		SHBuild_Puts "ERROR: \"$1\" should exist. Failed calling \"$*\"." >& 2
		exit 1
	fi
}

SHBuild_CheckedCallSilent()
{
	if hash "$1" > /dev/null 2>& 1; then
		"$@" > /dev/null || exit
	else
		SHBuild_Puts "ERROR: \"$1\" should exist. Failed calling \"$*\"." >& 2
		exit 1
	fi
}

SHBuild_InitReadonly()
{
	if test -n "$1"; then
		local vval

		eval "vval=\"\${$1}\""
		if test -z "$vval"; then
			local varname=$1
			shift
			eval readonly \""$varname"\"=\"\$\(SHBuild_CheckedCall "$*"\)\"
			eval "vval=\"\${$varname}\""
			SHBuild_Puts \
				"Cached readonly variable \"$varname\" = \"$vval\"." >& 2
		fi
	fi
}


SHBuild_2m()
{
	cygpath -m "$1" 2> /dev/null || SHBuild_Put "$1"
}

SHBuild_2u()
{
	cygpath -au "$1" 2> /dev/null || SHBuild_Put "$1"
}

SHBuild_EchoEscape()
{
	# FIXME: Turn off ANSI escape sequence when $TERM not supported.
	if test -t 1; then
		echo -ne "$1"
	fi
}

SHBuild_EchoVar()
{
	SHBuild_EchoEscape '\033[36m'
	SHBuild_Put "$1"
	SHBuild_EchoEscape '\033[0m'
	SHBuild_Put ' = "'
	SHBuild_EchoEscape '\033[31m'
	SHBuild_Put "$2"
	SHBuild_EchoEscape '\033[0m'
	SHBuild_Puts '"'
}

SHBuild_EchoVar_N()
{
	eval "SHBuild_EchoVar \"\$1\" \"\${${1//./_}}\""
}


SHBuild_CheckUName_Case_()
{
	case "$1" in
	*Darwin*)
		SHBuild_Put OS_X
		;;
	*MSYS* | *MINGW*)
		SHBuild_Put Win32
		;;
	*Linux*)
		SHBuild_Put Linux
		;;
	*)
		SHBuild_Put unknown
	esac
}

SHBuild_CheckUNameM_Case_()
{
	case "$1" in
	x86_64 | i*86-64)
		SHBuild_Put x86_64
		;;
	i*86)
		SHBuild_Put "$1"
		;;
	aarch64)
		SHBuild_Put aarch64
		;;
	*)
		SHBuild_Put unknown
	esac
}

SHBuild_CheckUName_Init_Env_OS_()
{
	SHBuild_InitReadonly SHBuild_Env_uname uname
	SHBuild_AssertNonempty SHBuild_Env_uname
	# XXX: %SHBuild_Env_uname is external.
	# shellcheck disable=2154
	SHBuild_InitReadonly SHBuild_Env_OS SHBuild_CheckUName_Case_ \
		"$SHBuild_Env_uname" > /dev/null
}

SHBuild_CheckUName_Init_Env_Arch_()
{
	SHBuild_InitReadonly SHBuild_Env_uname_m uname -m
	SHBuild_AssertNonempty SHBuild_Env_uname_m
	# XXX: %SHBuild_Env_uname_m is external.
	# shellcheck disable=2154
	SHBuild_InitReadonly SHBuild_Env_Arch SHBuild_CheckUNameM_Case_ \
		"$SHBuild_Env_uname_m" > /dev/null
}

# Initialize %SHBuild_Env_OS and %SHBuild_Env_Arch if necessary. The
#	initialization is done by shell calls to 'uname'.
SHBuild_CheckUName()
{
	# XXX: %SHBuild_Env_OS is external.
	# shellcheck disable=2154
	test -n "$SHBuild_Env_OS" || SHBuild_CheckUName_Init_Env_OS_
	# XXX: %SHBuild_Env_Arch is external.
	# shellcheck disable=2154
	test -n "$SHBuild_Env_Arch" || SHBuild_CheckUName_Init_Env_Arch_
}

# Get temporary directory name by checking "$TMPDIR", "$TEMP", "$TMP" and
#	"/tmp".
SHBuild_GetTempDir()
{
	# NOTE: The type and the permission of the file indicated by the path is not
	#	checked by design.
	if test -n "$TMPDIR"; then
		SHBuild_Put "$TMPDIR"
	elif test -n "$TEMP"; then
		SHBuild_Put "$TEMP"
	elif test -n "$TMP"; then
		SHBuild_Put "$TMP"
	else
		SHBuild_2m '/tmp'
	fi
}


# Check host platform and set value of platform string based on input parameters
#	indicating the operating system and architecture (as in host and target
#	triplets in common build systems). See user documentation
#	Tools/Scripts.zh-CN.md for details.
# Params: $1 = variable of OS like $SHBuild_Env_OS,
#	$2 = variable of OS like $SHBuild_Env_Arch.
SHBuild_Platform_Detect()
{
	local result

	if test "$1" = Win32; then
		# XXX: See '/etc/msystem' distributed by MSYS2.
		if test "$MSYSTEM" = MINGW64; then
			result=MinGW64
		elif test "$MSYSTEM" = MINGW32; then
			result=MinGW32
		elif test "$MSYSTEM" = CLANG64; then
			result=MinGW_Clang64
		elif test "$MSYSTEM" = CLANG32; then
			result=MinGW_Clang32
		elif test "$MSYSTEM" = CLANGARM64; then
			result=MinGW_ClangARM64
		elif test "$MSYSTEM" = UCRT64; then
			result=MinGW_UCRT64
		elif test "$2" = x86_64; then
			result=MinGW64
		else
			result=MinGW32
		fi
	else
		result="$1"
	fi
	SHBuild_AssertNonempty result
	echo "$result"
}

SHBuild_PrepareBuild_Init_Host_Arch_()
{
	if test "$SHBuild_Host_OS" = Win32; then
		# XXX: See '/etc/msystem' distributed by MSYS2.
		if test "$MSYSTEM" = MINGW64; then
			SHBuild_Host_Arch=x86_64
		elif test "$MSYSTEM" = MINGW32; then
			SHBuild_Host_Arch=i686
		elif test "$MSYSTEM" = CLANG64; then
			SHBuild_Host_Arch=x86_64
		elif test "$MSYSTEM" = CLANG32; then
			SHBuild_Host_Arch=i686
		elif test "$MSYSTEM" = CLANGARM64; then
			SHBuild_Host_Arch=aarch64
		elif test "$MSYSTEM" = UCRT64; then
			SHBuild_Host_Arch=x86_64
		else
			SHBuild_Host_Arch="$SHBuild_Env_Arch"
		fi
	else
		SHBuild_Host_Arch="$SHBuild_Env_Arch"
	fi
}

# Prepare the environment for build. Initialize the temporary directory. Check
#	system environment variables. Variables for the build system are initialized
#	if necessary.
SHBuild_PrepareBuild()
{
	: "${SHBuild_Env_TempDir:=$(SHBuild_GetTempDir)}"
	SHBuild_CheckUName
	: "${SHBuild_Host_OS:="$SHBuild_Env_OS"}"
	test -n "$SHBuild_Host_Arch" || SHBuild_PrepareBuild_Init_Host_Arch_
	# XXX: Every initializaed variable shall be nonempty. Just avoid the
	#	assertions here for efficiency.
}

SHBuild_GetBuildName()
{
	SHBuild_AssertNonempty SHBuild_Host_Arch
	SHBuild_AssertNonempty SHBuild_Host_OS
	SHBuild_Platform_Detect "$SHBuild_Host_OS" "$SHBuild_Host_Arch"
}


# Link and build GNU precompiled header.
# Params: $1 = path of header to be copied, $2 = path of header to be included,
#	$3 = tool to build header.
SHBuild_BuildGCH()
{
	declare -r SHBOUT_PCH="$2.gch"
	if [[ -s "$SHBOUT_PCH" && -r "$SHBOUT_PCH" ]]; then
		# FIXME: Update necessarily.
		SHBuild_Puts "PCH file exists, skipped building."
	else
		mkdir -p "$(dirname "$SHBOUT_PCH")"
		SHBuild_Puts "Building precompiled file \"$SHBOUT_PCH\" ..."
		SHBuild_Install_HardLink "$1" "$2"
		$3 "$1" -o"$SHBOUT_PCH"
		SHBuild_Puts "Building precompiled file \"$SHBOUT_PCH\" done."
	fi
}

# Link and build precompiled header if %SHBuild_NoPCH is not set to nonnull
#	value, and set internal value %SHBuild_IncPCH to proper command option
#	in GNU toolchain flavor. %CXX is used as the compiler with %CXXFLAGS as
#	flags. Only GNU-compatible command is supported currently.
# Params: $1 = path of header to be copied, $2 = path of header to be included.
SHBuild_CheckPCH()
{
	# XXX: %SHBuild_NoPCH is external.
	# shellcheck disable=2154
	if test -z "$SHBuild_NoPCH"; then
		SHBuild_BuildGCH "$1" "$2" "$CXX -xc++-header $CXXFLAGS"
		# XXX: %SHBuild_NoPCH is internal.
		# shellcheck disable=2034
		SHBuild_IncPCH="-include $2"
	else
		SHBuild_Puts Skipped building precompiled file.
		# XXX: %SHBuild_NoPCH is internal.
		# shellcheck disable=2034
		SHBuild_IncPCH=""
	fi
}


# NOTE: The following functions are currently not used directly by the YSLib
#	building process and all can be implemented by NPLA1. However, they can be
#	still useful to manually maintain the installation from the shell,
#	especially when 'SHBuild' is not available from a known location
#	(e.g. in $PATH).

SHBuild_2w()
{
	cygpath -w "$1" 2> /dev/null || SHBuild_Put "$1"
}

SHBuild_Install()
{
	# XXX: Try run 'cp' if 'rsync' fails.
	# shellcheck disable=2015
	hash rsync > /dev/null 2>& 1 && rsync -a "$1" "$2" || cp -fr "$1" "$2"
}

SHBuild_InstallDir()
{
	# XXX: Try run 'cp' if 'rsync' fails.
	# shellcheck disable=2015
	hash rsync > /dev/null 2>& 1 && rsync -a "$1" "$2" || cp -fr "$1" "$2/.."
}

SHBuild_Install_Exe()
{
	SHBuild_Install "$1" "$2" && chmod +x "$2"
}

SHBuild_Install_HardLink()
{
	(rm -f "$2" || true) && \
		($SHBuild_CMD //c "mklink /H $(SHBuild_2w "$2") $(SHBuild_2w "$1")" > \
		/dev/null 2>& 1 || ln -T "$1" "$2" > /dev/null 2>& 1 \
		|| SHBuild_Install "$1" "$2")
}

SHBuild_Install_HardLink_Exe()
{
	SHBuild_Install_HardLink "$1" "$2" && chmod +x "$2"
}

SHBuild_Install_Link()
{
	(rm -f "$2" || true) && \
		($SHBuild_CMD //c "mklink $(SHBuild_2w "$2") $(SHBuild_2w "$1")" \
		> /dev/null 2>& 1 || ln -s -T "$1" "$2")
}

# Convert the platform string to the system prefix.
# Params: $1 = platform string, typically from the result of
#	%SHBuild_Platform_Detect.
SHBuild_GetSystemPrefix()
{
	# XXX: See '/etc/msystem' distributed by MSYS2.
	if test "$1" = MinGW64; then
		echo '/mingw64'
	elif test "$1" = MinGW32; then
		echo '/mingw32'
	elif test "$1" = MinGW_Clang64; then
		echo '/clang64'
	elif test "$1" = MinGW_Clang32; then
		echo '/clang32'
	elif test "$1" = MinGW_ClangARM64; then
		echo '/clangarm64'
	elif test "$1" = MinGW_UCRT64; then
		echo '/ucrt64'
	else
		echo '/usr'
	fi
}

# Prepare stage 2 environment.
# Params: $1 = default value for %SHBuild_SysRoot if not set.
SHBuild_S2_Prepare()
{
	# NOTE: %SHBuild_SysRoot in stage 2 shall be initialized.
	: "${SHBuild_SysRoot:="$1"}"
	SHBuild_AssertNonempty SHBuild_SysRoot
	mkdir -p "$SHBuild_SysRoot"
	# shellcheck disable=2164
	SHBuild_SysRoot=$(cd "$SHBuild_SysRoot"; pwd)
	SHBuild_PrepareBuild
	# NOTE: See also %SHBuild-YSLib-build.txt.
	: "${SHBuild_SystemPrefix:=\
$(SHBuild_GetSystemPrefix "$(SHBuild_GetBuildName)")}"
	SHBuild_InitReadonly SR_Prefix \
		SHBuild_Put "$SHBuild_SysRoot$SHBuild_SystemPrefix"
}

# Prepare stage 2 environment for build.
# Params: $1 = default value for %SHBuild_SysRoot if not set.
SHBuild_S2_Prepare_Build()
{
	SHBuild_S2_Prepare "$1"
	# XXX: %SHBuild is to be exported, assuming stage 2 layout. This should not
	#	be used in building and installing Sysroot which requires a working
	#	%SHBuild out of the tree of '$SHBuild_SysRoot'.
	SHBuild_AssertNonempty SHBuild_SysRoot
	# XXX: %SR_Prefix is internal.
	# shellcheck disable=2154
	export SHBuild="$SR_Prefix/bin/SHBuild"
}

