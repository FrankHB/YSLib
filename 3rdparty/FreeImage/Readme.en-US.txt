== Modified files of FreeImage
Several files are modified based on official FreeImage version.
Each file is licensed the same as the original project.
Modifying are listed below.

= Since build 417:
Based on official version 3.15.4:
Removed "#include <memory.h>" in "Source/Utilities.h" to make it conforming to the ISO C/C++ standard.
In "../include/FreeImage.h":
  Replaced all function default argument for pointer type parameters from "NULL" or "0" to "nullptr" to "nullptr", to avoid possible G++ warning [-Wzero-as-null-pointer-constant]. This requires the "nullptr" keyword being supported.
  Removed all spaces at end of lines.
  Saved as UTF-8 + BOM.
Disabled unused plugins and make placeholders in plugin list in "Source/Plugin.h" and "Source/FreeImage/Plugin.cpp".
"Makefile.srcs" is edited properly to exclude unused files.

= Since build 431:
Based on previous revision b417:
In "../include/FreeImage.h", "Source/FreeImage/Plugin.cpp", "Source/FreeImage/GetType.cpp" and "Source/FreeImageToolkit/JPEGTransform.cpp":
Removed functions only supported in Win32: "FreeImage_LoadU", "FreeImage_SaveU", "FreeImage_GetFileTypeU", "FreeImage_GetFIFFromFilenameU", "FreeImage_JPEGTransformU", "FreeImage_JPEGCropU".
Confirmed for all modified above:
  Removed all spaces at end of lines.
  Saved as UTF-8 + BOM.
  All EOL markers are normalized as CR+LF.
Note: "Source/FreeImageToolkit/JPEGTransform.cpp" is excluded in the source list. The file is reserved for future use.
Additional source of libjpeg-turbo-1.3.0 is now used to replace libjpeg-8d in "Source/LibJPEG".
"Makefile.srcs" is edited properly to exclude unused files and to add libjepg-turbo source files.
"Makefile.mingw" is modified to handle assembly files with NASM.

== Additional source replacement

= Since build 431:
Based on official version 3.15.4:
Source files of libjpeg-turbo are required to replace libjpeg in original FreeImage source distribution.
Get the libjpeg-turbo source from http://sourceforge.net/projects/libjpeg-turbo/files/1.3.0/libjpeg-turbo-1.3.0.tar.gz .
See "release/License.rtf" for the license of libjpeg-turbo, which is in BSD-style and should be compatible with the other part of the YSLib project.
Source files of zlib are updated to 1.2.8 from 1.2.7.
Get the zlib source from http://sourceforge.net/projects/libpng/files/zlib/1.2.8/zlib128.zip .
Source files of libpng are updated to 1.6.3 from 1.5.13.
Get the libpng source from http://sourceforge.net/projects/libpng/files/libpng16/older-releases/1.6.3/lpng163.7z .

= Since build 452:
Based on previous revision b431:
Source files of libpng are updated to 1.6.6 from 1.6.3.
Get the libpng source from http://sourceforge.net/projects/libpng/files/libpng16/1.6.6/lpng166.7z .

== FreeImage build instructions
FreeImage source files are required.
Get the source from http://sourceforge.net/projects/freeimage/files/Source%20Distribution/3.15.4/FreeImage3154.zip .
Note that the content of header file "FreeImage.h" is actually equivalent with the original file. It is not need to replace the original file for building the library.
The output files of different platform will be put in the same directory "dist". Make sure it is cleaned before each build.

= Patching
Use additional source above to replace specific parts of FreeImage source by directly copying(and overwriting existed files) to "Source/LibJPEG", "Source/ZLib" and "Source/LibPNG" respectively.
Copy all files except this document in this directory to the source directory and overwrite original files before building the library.

= DS
Currently only building on Windows is supported.
MSYS and devkitARM are required.
The environment variable "DEVKITARM" should be set properly like "/C/devkitPro", to make sure the toolchain files can be found.
The file "Makefile.ds" should have been copied to the source directory.
Run "make -f Makefile.ds -j" to build the static library.
Run "make -f Makefile.ds clean" to delete all build files.

= MinGW32
MinGW-w64 GCC toolchain, the "cp" and "rm" command are required by the official or modified MinGW makefile. (MSYS should be OK.)
NASM is required for compiling libjpeg-turbo SIMD source files.
The environment variable "PATH" should be set properly like "C:\Windows\System32;C:\MinGW32\bin;C:\msys\bin;C:\Program Files (x86)\NASM", to make sure the toolchain files can be found.
The modified file "Makefile.mingw" should have been copied to the source directory and overwritten the original one.
Run "mingw32-make FREEIMAGE_LIBRARY_TYPE=STATIC -f Makefile.mingw -j" to build static library for MinGW32.
Run "mingw32-make -f Makefile.mingw clean" to delete all build files. Note the official "clean.bat" is not enough.

