== Modified files
Several files are modified based on FreeImage 3.15.4.
Each file is licensed the same as the original project.
List of modifying:
Removed "#include <memory.h>" in "Source/Utilities.h" to make it conforming to the ISO C/C++ standard.
In "../include/FreeImage.h":
  Replaced all function default argument for pointer type parameters from "NULL" or "0" to "nullptr" to "nullptr", to avoid possible G++ warning [-Wzero-as-null-pointer-constant]. This requires the "nullptr" keyword being supported.
  Removed all spaces at end of lines.
  Saved as UTF-8 + BOM.
Disabled unused plugins and make placeholders in plugin list in "Source/Plugin.h" and "Source/FreeImage/Plugin.cpp".

== FreeImage build instructions
FreeImage source files are required.
Get the source from http://sourceforge.net/projects/freeimage/files/Source%20Distribution/ .
Copy all files except this document in this directory to the source directory and overwrite original files before building the library.
Note that the content of header file "FreeImage.h" is actually equivalent with the original file. It is not need to replace the original file for building the library.
The output files of different platform will be put in the same directory "dist". Make sure it is cleaned before each build.

= DS
Currently only building on Windows is supported.
MSYS and devkitARM are required.
The environment variable "DEVKITARM" should be set properly like "/C/devkitPro", to make sure the toolchain files can be found.
The file "Makefile.ds" should have been copied to the source directory.
Run "make -f Makefile.ds" to build the static library.
Run "make -f Makefile.ds clean" to delete all build files.

= MinGW32
MinGW-w64 GCC toolchain, the "cp" and "rm" command are required by the official makefile. (MSYS should be OK.)
The environment variable "PATH" should be set properly like "C:\Windows\System32;C:\MinGW32\bin;C:\msys\bin", to make sure the toolchain files can be found.
Run "mingw32-make FREEIMAGE_LIBRARY_TYPE=STATIC -f Makefile.mingw" using official makefile to build static library for MinGW32.
Run "mingw32-make -f Makefile.mingw clean" to delete all build files. Note the official "clean.bat" is not enough.

