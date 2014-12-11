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
	${SHBuild_CMD} //c "mklink /H `SHBuild_2w "$2"` `SHBuild_2w "$1"`" > \
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
	${SHBuild_CMD} //c "mklink `SHBuild_2w "$2"` `SHBuild_2w "$1"`" > \
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

