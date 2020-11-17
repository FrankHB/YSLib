#!/usr/bin/env bash
# (C) 2020 FrankHB.
# Wrapper script for YSLib packages using SHBuild.

set -e
# NOTE: Relative location of scripts depends on Sysroot.
: "${SHBuild:="$(dirname "${BASH_SOURCE[0]}")/SHBuild"}"
[[ "$SHBuild" != '' ]] || echo "ERROR: Variable 'SHBuild' shall not be empty."
: "${NPLA1_ROOT:="$(dirname "$SHBuild")/../share/NPLA1"}"

"$SHBuild" -xcmd,RunNPLFile "$NPLA1_ROOT/SHBuild-BuildApp.txt" -- "$@"
# TODO: Package.

