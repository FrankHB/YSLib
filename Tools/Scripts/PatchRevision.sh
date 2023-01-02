#!/usr/bin/env bash
# (C) 2014-2016, 2020, 2022-2023 FrankHB.
# Revision patching script: patching source revisions using RevisionPatcher.
# Required: hg / git, sed.

set -e
# NOTE: Relative location of %SHBuild-common.sh is depends on both in stage 1
#	and Sysroot.
# shellcheck source=./SHBuild-common.sh
. "$(dirname "${BASH_SOURCE[0]}")/SHBuild-common.sh"

SHBuild_CheckedCallSilent hg root
SHBuild_CheckedCallSilent SHBuild_Put x | sed 's/x/x/g'

: "${PatchBegin:="1"}"
: "${PatchEnd:="30"}"
: "${RevisionPatcher:="$(which RevisionPatcher)"}"
SHBuild_CheckedCallSilent SHBuild_Put x | "$RevisionPatcher"

root_=''
use_hg_=''
use_git_=''

test_hg_()
{
	if [[ $root_ == '' ]] && hash hg > /dev/null 2>& 1; then
		root_="$(SHBuild_2u "$(hg root 2> /dev/null)")"
		if [[ $root_ != '' ]]; then
			use_hg_=hg
		fi
	fi
}

test_git_()
{
	if [[ $root_ == '' ]] && hash git > /dev/null 2>& 1; then
		root_="$(SHBuild_2u "$(git rev-parse --show-toplevel 2> /dev/null)")"
		if [[ root_ != '' ]]; then
			use_git_=git
		fi
	fi
}

if [[ $PatchHg != '' ]]; then
	test_hg_
elif [[ $PatchGit != '' ]]; then
	test_git_
else
	test_hg_
	test_git_
fi

SHBuild_Pushd "$root_"

if [[ $use_hg_ != '' ]]; then
	SHBuild_Puts "Using hg."
	hg status --color=none -amn0 | xargs -0 hg diff --color=none > bak.patch
elif [[ $use_git_ != '' ]]; then
	SHBuild_Puts "Using git."
	git diff --diff-filter=d --color=never HEAD > bak.patch
else
	SHBuild_Puts "ERROR: either 'hg' or 'git' should exist." >& 2
	exit 1
fi

# XXX: This requires Bash 4.x.
mapfile -t Versions < <(< bak.patch $RevisionPatcher)

SHBuild_Puts Patch area = range [$PatchBegin, $PatchEnd].
file=''
for var in "${Versions[@]}"
do
	if [[ $file == '' ]]; then
		file=$var
	else
		sed -b -i \
"$PatchBegin,${PatchEnd}s/version r[0-9][^ \\r\\n]*/version r$var/g" "$file"
		SHBuild_Puts "\"$file\" processed using revision number \"$var\"."
		file=''
	fi
done

SHBuild_Popd
SHBuild_Puts Done.

