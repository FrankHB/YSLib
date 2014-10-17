#!/usr/bin/bash
# (C) 2014 FrankHB.
# Common source script.

: ${SHBuild_CMD:=${COMSPEC}}
: ${SHBuild_CMD:=cmd}

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
	if hash rsync > /dev/null; then
		rsync -a "$1" "$2"
	else
		cp -fr "$1" "$2"
	fi
}

SHBuild_InstallDir()
{
	if command -v rsync > /dev/null; then
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

SHBuild_Install_Link()
{
	rm -f "$2" || true
	${SHBuild_CMD} //c "mklink `SHBuild_2w "$2"` `SHBuild_2w "$1"`">/dev/null \
		2>&1 || ln -s -T "$1" "$2"
}

SHBuild_Pushd()
{
	pushd .>/dev/null 2>/dev/null
}

SHBuild_Popd()
{
	popd>/dev/null 2>/dev/null
}

SHBuild_EchoVar()
{
	# FIXME: Turn off ANSI escape sequence when not supported.
	echo -ne '\033[36m'
	echo -n "$1"
	echo -ne '\033[0m'
	echo -n ' = "'
	echo -ne '\033[31m'
	echo -n "$2"
	echo -e '\033[0m"'
}

SHBuild_EchoVar_N()
{
	eval "SHBuild_EchoVar \"\$1\" \"\${${1//./_}}\""
}

