#!/usr/bin/env bash
# (C) 2020-2021, 2023 FrankHB.
# Wrapper script for building YSLib packages using SHBuild.

set -e

# NOTE: Relative location of scripts depends on Sysroot.
: "${SHBuild:="$(dirname "${BASH_SOURCE[0]}")/SHBuild"}"
test -n "$SHBuild" || echo "ERROR: Variable 'SHBuild' shall not be empty."
: "${NPLA1_ROOT:="$(dirname "$SHBuild")/../share/NPLA1"}"

"$SHBuild" -xcmd,RunNPLFile "$NPLA1_ROOT/SHBuild-BuildApp.txt" -- "$@"
# TODO: Package.

