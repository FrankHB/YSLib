#!/usr/bin/env bash
# (C) 2014-2016 FrankHB.
# Revision patching script: patching source revisions using RevisionPatcher.
# Required: hg, sed.

: ${SHBuild_Bin:="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"}
. $SHBuild_Bin/SHBuild-common.sh

SHBuild_CheckedCallSilent hg root
SHBuild_CheckedCallSilent SHBuild_Put x | sed 's/x/x/g'

: ${PatchBegin:="1"}
: ${PatchEnd:="30"}
: ${RevisionPatcher:="`which RevisionPatcher`"}
SHBuild_CheckedCallSilent SHBuild_Put x | "$RevisionPatcher"

SHBuild_Pushd $(SHBuild_2u `hg root`)

hg status --color=none -amn0 | xargs -0 hg diff --color=none > bak.patch

Versions=(`cat bak.patch | $RevisionPatcher`)

SHBuild_Puts Patch area = range [$PatchBegin, $PatchEnd].
file=''
for var in "${Versions[@]}"
do
	if [[ $file == '' ]]; then
		file=$var
	else
		sed -b -i \
"$PatchBegin,${PatchEnd}s/version r[0-9][^ \\r\\n]*/version r$var/g" "$file"
		SHBuild_Puts \"$file\" processed using revision number \"$var\".
		file=''
	fi
done

SHBuild_Popd
SHBuild_Puts Done.

