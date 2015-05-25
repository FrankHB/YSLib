﻿== Modified files of FreeImage
Several files are modified based on official FreeImage version.
Each file is licensed the same as the original project.
Modifying are listed below.

= Compatibility caveat
Source and binary compatibility was kept before b456.
Since b456, the "Plugin" struct is renamed (to avoid global namespace pollution), new interface are introduced, and some functions are removed.
Unmodified library code should compile, but wrappers based on old code might not work as before.

= Since build 600:
Based on previous revision b520:
Merged base version 3.17.0 without functions with suffix "U" and real support for new formats.
For compatibility, "mkdir -p Dist" is not added as official makefiles.
Added Win64 support in Makefile.mingw. Now it uses variable "MSYSTEM" to distinguish Win32 and Win64. The value shall be one of "MINGW32" or "MINGW64".

= Since build 520:
Based on previous revision b456:
Merged base version 3.16.0 without functions with suffix "U" and real support for new formats.
FreeImagePlus is not touched.

= Since build 456:
Based on previous revision b431:
Removed plugin functions: "FreeImage_GetFormatFromFIF", "FreeImage_GetFIFMimeType", "FreeImage_GetFIFExtensionList", "FreeImage_GetFIFDescription", "FreeImage_GetFIFRegExpr", "FreeImage_FIFSupportsReading", "FreeImage_FIFSupportsWriting", "FreeImage_FIFSupportsExportBPP", "FreeImage_FIFSupportsExportType", "FreeImage_FIFSupportsICCProfiles", "FreeImage_FIFSupportsNoPixels", "FreeImage_SetPluginEnabled", "FreeImage_IsPluginEnabled".
In "../include/FreeImage.h",:
	Exposed struct "PluginNode" as "FI_PluginNodeRec".
	Added declaration of function "FreeImageEx_GetPluginNodeFromFIF".
	Added macro "nullptr" for C compatibility.
	Renamed struct "Plugin" to "FI_PluginRec".
In "Source/Utilities.h"
	Added typedef declaration of "Plugin" to keep source compatibility.
In "Source/Plugin.h"
	Removed unnecessary class name declaration "Plugin".
	Moved struct "PluginNode" to public header.
	Removed declaration of function "FreeImage_stricmp".
	Removed including "Utilities.h".
	Added typedef declaration of "PluginNode" to keep source compatibility.
In "Source/FreeImage/Plugin.cpp"
	Added definition of function "FreeImageEx_GetPluginNodeFromFIF".
	Simplified implementation using C++11.
	Made "PluginList::AddFakeNode" call "FreeImage_OutputMessageProc" on fail.

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

= Since build 417:
Based on official version 3.15.4:
Removed "#include <memory.h>" in "Source/Utilities.h" to make it conforming to the ISO C/C++ standard.
In "../include/FreeImage.h":
	Replaced all function default argument for pointer type parameters from "NULL" or "0" to "nullptr" to "nullptr", to avoid possible G++ warning [-Wzero-as-null-pointer-constant]. This requires the "nullptr" keyword being supported.
	Removed all spaces at end of lines.
	Saved as UTF-8 + BOM.
Disabled unused plugins and make placeholders in plugin list in "Source/Plugin.h" and "Source/FreeImage/Plugin.cpp".
"Makefile.srcs" is edited properly to exclude unused files.

== Additional source replacement

= Since build 600:
Based on base version 3.17.0:
Source files of libpng are updated to 1.6.17 from 1.6.16.
Get the libpng source from http://sourceforge.net/projects/libpng/files/libpng16/1.6.17/lpng1617.7z .
Source files of libjpeg-turbo are required to replace libjpeg in original FreeImage source distribution.
Get the libjpeg-turbo source from http://sourceforge.net/projects/libjpeg-turbo/files/1.4.0/libjpeg-turbo-1.4.0.tar.gz .
(This version contains the newest release of zlib 1.2.8, no need to change.)

= Since build 520:
Based on base version 3.16.0:
Source files of libpng are updated to 1.6.12 from 1.6.10.
Get the libpng source from http://sourceforge.net/projects/libpng/files/libpng16/1.6.12/lpng1612.7z .
Source files of libjpeg-turbo are required to replace libjpeg in original FreeImage source distribution.
Get the libjpeg-turbo source from http://sourceforge.net/projects/libjpeg-turbo/files/1.3.1/libjpeg-turbo-1.3.1.tar.gz .
(This version contains the newest release of zlib 1.2.8, no need to change.)

= Since build 452:
Based on previous revision b431:
Source files of libpng are updated to 1.6.6 from 1.6.3.
Get the libpng source from http://sourceforge.net/projects/libpng/files/libpng16/older-releases/1.6.6/lpng166.7z .

= Since build 431:
Based on official version 3.15.4:
Source files of libjpeg-turbo are required to replace libjpeg in original FreeImage source distribution.
Get the libjpeg-turbo source from http://sourceforge.net/projects/libjpeg-turbo/files/1.3.0/libjpeg-turbo-1.3.0.tar.gz .
See "release/License.rtf" for the license of libjpeg-turbo, which is in BSD-style and should be compatible with the other part of the YSLib project.
Source files of zlib are updated to 1.2.8 from 1.2.7.
Get the zlib source from http://sourceforge.net/projects/libpng/files/zlib/1.2.8/zlib128.zip .
Source files of libpng are updated to 1.6.3 from 1.5.13.
Get the libpng source from http://sourceforge.net/projects/libpng/files/libpng16/older-releases/1.6.3/lpng163.7z .

== FreeImage build instructions
FreeImage source files are required.
Get the source from http://sourceforge.net/projects/freeimage/files/Source%20Distribution .
The output files of different platform will be put in the same directory "Dist". Make sure it is cleaned before each build.
See following sections before build.

= Patching
Use additional source above to replace specific parts of FreeImage source by directly copying(and overwriting existed files) to "Source/LibJPEG", "Source/ZLib" and "Source/LibPNG" respectively.
Copy all files except this document in this directory to the source directory and overwrite original files before building the library.
It is now required to replace the original header file "FreeImage.h" in "Source" directory by header in "3rdparty/include" for building the library, though the content of header file "FreeImage.h" is actually equivalent with the original file before b456.

= DS
Currently only building on Windows is tested.
MSYS and devkitARM are required.
The environment variable "DEVKITARM" should be set properly like "/C/devkitPro", to make sure the toolchain files can be found.
The file "Makefile.ds" should have been copied to the source directory.
Run "make -f Makefile.ds -j" to build the static library.
Run "make -f Makefile.ds clean" to delete all build files.

= MinGW32
MinGW-w64 GCC toolchain, the "cp" and "rm" command are required by the official or modified MinGW makefile. (MSYS should be OK.)
Assembler is required for compiling libjpeg-turbo SIMD source files. This is called by variable NASM. It now defaults to "yasm", since "nasm" from MSYS2 does not work.
The environment variable "MSYSTEM" shall be set to "MINGW32", e.g. run "set MSYSTEM=MINGW32" at "cmd".
The environment variable "PATH" should be set properly like "C:\msys\mingw32\bin;C:\msys\usr\bin;C:\Windows\System32", to make sure the toolchain files can be found.
The modified file "Makefile.mingw" should have been copied to the source directory and overwritten the original one.
Run "mingw32-make FREEIMAGE_LIBRARY_TYPE=STATIC -f Makefile.mingw -j" to build static library for MinGW32.
Run "mingw32-make -f Makefile.mingw clean" to delete all build files. Note the official "clean.bat" is not enough.

= Linux
GNU toolchain is needed.
NASM is required for compiling libjpeg-turbo SIMD source files.
Run "make -f Makefile.gnu libfreeimage.a -j" to build static library.
Run "make -f Makefile.gnu clean" to delete all build files.
For hosted Linux environment, "-f Makefile.gnu" can be omitted. Also "libfreeimage.a" can be omitted to build both static and shared library, although only static library is used by YFramework.

