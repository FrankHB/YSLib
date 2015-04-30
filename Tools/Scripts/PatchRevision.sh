#!/usr/bin/env bash
# (C) 2014-2015 FrankHB.
# Revision patching script: patching source revisions using RevisionPatcher.

: ${SHBuild_Bin:="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"}
. $SHBuild_Bin/SHBuild-common.sh

SHBuild_CheckedCallSilent hg root
SHBuild_CheckedCallSilent echo x | sed 's/x/x/g'

: ${PatchBegin:="1"}
: ${PatchEnd:="30"}
: ${RevisionPatcher:="`which RevisionPatcher`"}
SHBuild_CheckedCallSilent echo x | "$RevisionPatcher"

SHBuild_Pushd $(SHBuild_2u `hg root`)

hg status --color=none -amn0 | xargs -0 hg diff --color=none > bak.patch

Versions=(`cat bak.patch | $RevisionPatcher`)

echo Patch area = range [$PatchBegin, $PatchEnd].
file=''
for var in "${Versions[@]}"
do
	if [[ $file == '' ]]; then
		file=$var
	else
		sed -b -i \
			"$PatchBegin,${PatchEnd}s/version r[0-9][^ \\r\\n]*/version r$var/g" \
			"$file"
		echo \"$file\" processed using revision number \"$var\".
		file=''
	fi
done

SHBuild_Popd
echo Done.

