#!/usr/bin/sh
# (C) 2014 FrankHB.
# Common source script.

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
	echo -ne "\033[36m"
	echo -n "$1"
	echo -ne "\033[0m"
	echo -n " = \""
	echo -ne "\033[31m"
	echo -n "$2"
	echo -e "\033[0m\""
}

