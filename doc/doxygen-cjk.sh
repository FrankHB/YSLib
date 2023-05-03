#!/usr/bin/env bash
# (C) 2013, 2021 FrankHB.
# Doxygen script for LaTeX with CJK contents (expermental, untested).
# Requires: Doxygen. LaTeX enabled in Doxyfile. External LaTeX packages.
#	Arch Linux packages texlive-most texlive-lang should work.

set -e

: "${SHBuild_ToolDir:=\
$(cd "$(dirname "${BASH_SOURCE[0]}")/../Tools/Scripts"; pwd)}"
: "${YSLib_BaseDir:="$SHBuild_ToolDir/../.."}"
YSLib_BaseDir=$(cd "$YSLib_BaseDir"; pwd)
YSLib_DocDir=$(cd "$YSLib_BaseDir/doc"; pwd)

# shellcheck source=../Tools/Scripts/SHBuild-common.sh
. "$SHBuild_ToolDir/SHBuild-common.sh" # for SHBuild_Pushd, SHBuild_Popd,

(cd "$YSLib_BaseDir" && doxygen -w latex "$YSLib_DocDir/header.tex0" \
"$YSLib_DocDir/footer.tex0" "$YSLib_DocDir/stylesheet.tex0" Doxyfile)

SHBuild_Pushd "$YSLib_DocDir"
< header.tex0 awk '{print;}
/\\usepackage{doxygen}/{print("\\usepackage{CJK}")} /\\begin{document}/{print("\\begin{CJK}{UTF8}{gbsn}")}' \
> header.tex
sed -ie "s/Your title here/YSTest Reference Manual/" header.tex
< footer.tex0 awk '{print;}/\\printindex/{print("\\end{CJK}")}' > footer.tex
rm stylesheet.tex0
(cd "$YSLib_BaseDir" && doxygen Doxyfile)
(cd latex && make)
rm header.tex* footer.tex*
SHBuild_Popd

