#!/usr/bin/env bash
# (C) 2014-2017 FrankHB.
# Common source script.

[[ "$INC_SHBuild_common" == '' ]] && INC_SHBuild_common=1 || return 0

: ${SHBuild_CMD:="$COMSPEC"}
: ${SHBuild_CMD:=cmd}

trap exit 1 INT

SHBuild_Put()
{
	printf '%s' "$*"
}

SHBuild_Puts()
{
	if [[ "$SHBuild_EOL" == '' ]]; then
		# TODO: More precise detection of Windows shell.
		if [[ "$COMSPEC" == '' ]]; then
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
	[[ "$vval" != '' ]] || \
		(SHBuild_Puts ERROR: Variable \""$1"\" should not be empty. && exit 1)
}

SHBuild_CheckedCall()
{
	if hash "$1" > /dev/null 2>& 1; then
		$* || exit $?
	else
		SHBuild_Puts ERROR: \""$1"\" should exist. Failed calling \""$*"\".
		exit 1
	fi
}

SHBuild_CheckedCallSilent()
{
	if hash "$1" > /dev/null 2>& 1; then
		$* > /dev/null || exit $?
	else
		SHBuild_Puts ERROR: \""$1"\" should exist. Failed calling \""$*"\".
		exit 1
	fi
}

SHBuild_InitReadonly()
{
	if [[ "$1" != '' ]]; then
		local vval
		eval "vval=\"\${$1}\""
		if [[ "$vval" == '' ]]; then
			local varname=$1
			shift
			eval readonly \"$varname\"=\"\$\(SHBuild_CheckedCall $*\)\"
			eval "vval=\"\${$varname}\""
			SHBuild_Puts Cached readonly variable \"$varname\" = \"$vval\".
		fi
	fi
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
		SHBuild_Put $1
		;;
	i*86)
		SHBuild_Put $1
		;;
	*)
		SHBuild_Put unknown
	esac
}

# Usage: SHBuild_CheckUName && printf $SHBuild_Env_OS $SHBuild_Env_Arch
SHBuild_CheckUName()
{
	[[ "$SHBuild_OS" == '' ]] || return 0
	SHBuild_InitReadonly SHBuild_Env_uname uname
	SHBuild_AssertNonempty SHBuild_Env_uname
	SHBuild_InitReadonly SHBuild_Env_OS SHBuild_CheckUName_Case_ \
		"$SHBuild_Env_uname" > /dev/null
	[[ "$SHBuild_Arch" == '' ]] || return 0
	SHBuild_InitReadonly SHBuild_Env_uname_m uname -m
	SHBuild_AssertNonempty SHBuild_Env_uname_m
	SHBuild_InitReadonly SHBuild_Env_Arch SHBuild_CheckUNameM_Case_ \
		"$SHBuild_Env_uname_m" > /dev/null
}


SHBuild_2u()
{
	cygpath -au "$1" 2> /dev/null || SHBuild_Put "$1"
}

SHBuild_2w()
{
	cygpath -w "$1" 2> /dev/null || SHBuild_Put "$1"
}

SHBuild_Install()
{
	hash rsync > /dev/null 2>& 1 && rsync -a "$1" "$2" || cp -fr "$1" "$2"
}

SHBuild_InstallDir()
{
	hash rsync > /dev/null 2>& 1 && rsync -a "$1" "$2" || cp -fr "$1" "$2/.."
}

SHBuild_Install_Exe()
{
	SHBuild_Install "$1" "$2" && chmod +x "$2"
}

SHBuild_Install_HardLink()
{
	(rm -f "$2" || true) && \
		($SHBuild_CMD //c "mklink /H `SHBuild_2w "$2"` `SHBuild_2w "$1"`" > \
		/dev/null 2>& 1 || ln -T "$1" "$2" || SHBuild_Install "$1" "$2")
}

SHBuild_Install_HardLink_Exe()
{
	SHBuild_Install_HardLink "$1" "$2" && chmod +x "$2"
}

SHBuild_Install_Link()
{
	(rm -f "$2" || true) && \
		($SHBuild_CMD //c "mklink `SHBuild_2w "$2"` `SHBuild_2w "$1"`" > \
		/dev/null 2>& 1 || ln -s -T "$1" "$2")
}

SHBuild_Pushd()
{
	pushd "$1" > /dev/null 2>& 1
}

SHBuild_Popd()
{
	popd > /dev/null 2>& 1
}

SHBuild_EchoEscape()
{
	# FIXME: Turn off ANSI escape sequence when $TERM not supported.
	if [[ -t 1 ]]; then
		echo -ne $1
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
		mkdir -p "`dirname "$SHBOUT_PCH"`"
		SHBuild_Puts "Building precompiled file \"$SHBOUT_PCH\" ..."
		SHBuild_Install_HardLink "$1" "$2"
		$3 "$1" -o$SHBOUT_PCH
		SHBuild_Puts "Building precompiled file \"$SHBOUT_PCH\" done."
	fi
}

# Check host platform and set value of %SHBuild_Host_Platform if not set.
#	For Win32 platforms, only 'MinGW32' is supported currently.
SHBuild_CheckHostPlatform()
{
	SHBuild_CheckUName
	if [[ "$SHBuild_Env_OS" == 'Win32' ]]; then
		: ${SHBuild_Host_Platform:='MinGW32'}
	else
		: ${SHBuild_Host_Platform:=$SHBuild_Env_OS}
	fi
	SHBuild_AssertNonempty SHBuild_Host_Platform
}

# Link and build precompiled header if %SHBuild_NoPCH is not set to nonnull
#	value, and set internal value %SHBuild_IncPCH to proper command option
#	in GNU toolchain flavor. %CXX is used as the compiler with %CXXFLAGS as
#	flags. Only GNU-compatible command is supported currently.
# Params: $1 = path of header to be copied, $2 = path of header to be included.
SHBuild_CheckPCH()
{
	if [[ $SHBuild_NoPCH == '' ]]; then
		SHBuild_BuildGCH "$1" "$2" "$CXX -xc++-header $CXXFLAGS"
		SHBuild_IncPCH="-include $2"
	else
		SHBuild_Puts Skipped building precompiled file.
		SHBuild_IncPCH=""
	fi
}

