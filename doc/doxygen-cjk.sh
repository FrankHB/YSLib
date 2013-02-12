#!/bin/bash
doxygen -w latex header.tex0 footer.tex0 stylesheet.tex0 ../Doxyfile
cat header.tex0  | awk '{print;}
    /\\usepackage{doxygen}/{print("\\usepackage{CJK}")} /\\begin{document}/{print("\\begin{CJK}{UTF8}{gbsn}")}' > header.tex
sed -ie "s/YSTest/Reference Manual/" header.tex
cat footer.tex0 | awk '{print;}/\\printindex/{print("\\end{CJK}")}' > footer.tex
rm stylesheet.tex0
pushd ..
doxygen Doxyfile
popd
pushd latex
make
popd
rm header.tex* footer.tex*

