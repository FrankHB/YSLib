#!/usr/bin/bash
# (C) 2014 FrankHB.
# Common source script.

[ x"$INC_SHBuild_common" == x ] && INC_SHBuild_common=1 || return 0

: ${SHBuild_CMD:="$COMSPEC"}
: ${SHBuild_CMD:=cmd}

trap exit 1 INT

SHBuild_AssertNonempty()
{
	local vval
	eval "vval=\"\${$1}\""
	[ x"$vval" != x ] || \
		(echo ERROR: Variable \""$1"\" should not be empty. && exit 1)
}

SHBuild_CheckedCall()
{
	if hash "$1" > /dev/null 2>& 1; then
		$* || exit $?
	else
		echo ERROR: \""$1"\" should be exist. Failed calling \""$*"\".
		exit 1
	fi
}

SHBuild_InitReadonly()
{
	if [ x"$1" != x ]; then
		local vval
		eval "vval=\"\${$1}\""
		if [ x"$vval" == x ]; then
			local varname=$1
			shift
			eval readonly \"$varname\"=\"\$\(SHBuild_CheckedCall $*\)\"
			eval "vval=\"\${$varname}\""
			echo Cached readonly variable \"$varname\" = \"$vval\".
		fi
	fi
}


SHBuild_CheckUName_Case_()
{
	case "$1" in
	*Darwin*)
		echo OS_X
		;;
	*MSYS* | *MINGW*)
		echo Win32
		;;
	*Linux*)
		echo Linux
		;;
	*)
		echo unknown
	esac
}

SHBuild_CheckUNameM_Case_()
{
	case "$1" in
	x86_64 | i*86-64)
		echo $1
		;;
	i*86)
		echo $1
		;;
	*)
		echo unknown
	esac
}

# Usage: SHBuild_CheckUName && echo $SHBuild_Env_OS $SHBuild_Env_Arch
SHBuild_CheckUName()
{
	[ x"$SHBuild_OS" == x ] || return 0
	SHBuild_InitReadonly "SHBuild_Env_uname" uname
	SHBuild_AssertNonempty SHBuild_Env_uname
	SHBuild_InitReadonly "SHBuild_Env_OS" SHBuild_CheckUName_Case_ \
		"$SHBuild_Env_uname" > /dev/null
	[ x"$SHBuild_Arch" == x ] || return 0
	SHBuild_InitReadonly "SHBuild_Env_uname_m" uname -m
	SHBuild_AssertNonempty SHBuild_Env_uname_m
	SHBuild_InitReadonly "SHBuild_Env_Arch" SHBuild_CheckUNameM_Case_ \
		"$SHBuild_Env_uname_m" > /dev/null
}


SHBuild_2u()
{
	cygpath -au "$1" || echo "$1"
}

SHBuild_2w()
{
	cygpath -w "$1" || echo "$1"
}

SHBuild_Install()
{
	if hash rsync > /dev/null 2>& 1; then
		rsync -a "$1" "$2"
	else
		cp -fr "$1" "$2"
	fi
}

SHBuild_InstallDir()
{
	if hash rsync > /dev/null 2>& 1; then
		rsync -a "$1" "$2"
	else
		cp -fr "$1" "$2/.."
	fi
}

SHBuild_Install_Exe()
{
	SHBuild_Install "$1" "$2"
	chmod +x "$2"
}

SHBuild_Install_HardLink()
{
	rm -f "$2" || true
	$SHBuild_CMD //c "mklink /H `SHBuild_2w "$2"` `SHBuild_2w "$1"`" > \
		/dev/null 2>& 1 || ln -T "$1" "$2" || SHBuild_Install "$1" "$2"
}

SHBuild_Install_HardLink_Exe()
{
	SHBuild_Install_HardLink "$1" "$2"
	chmod +x "$2"
}

SHBuild_Install_Link()
{
	rm -f "$2" || true
	$SHBuild_CMD //c "mklink `SHBuild_2w "$2"` `SHBuild_2w "$1"`" > \
		/dev/null 2>& 1 || ln -s -T "$1" "$2"
}

SHBuild_Pushd()
{
	pushd . > /dev/null 2>& 1
}

SHBuild_Popd()
{
	popd > /dev/null 2>& 1
}

SHBuild_EchoEscape()
{
	# FIXME: Turn off ANSI escape sequence when $TERM not supported.
	if [ -t 1 ]; then
		echo -ne $1
	fi
}

SHBuild_EchoVar()
{
	SHBuild_EchoEscape '\033[36m'
	echo -n "$1"
	SHBuild_EchoEscape '\033[0m'
	echo -n ' = "'
	SHBuild_EchoEscape '\033[31m'
	echo -n "$2"
	SHBuild_EchoEscape '\033[0m'
	echo '"'
}

SHBuild_EchoVar_N()
{
	eval "SHBuild_EchoVar \"\$1\" \"\${${1//./_}}\""
}

