== FreeType build instructions
FreeType2 source files are needed.
Get the source from http://freetype.org .

= Patching
Since build b420, the following files from version 2.4.11 should replace files from newer versions(since 2.4.12) respectively:
 "include/freetype/config/ftoption.h",
 "src/sfnt/ttmtx.c".
WARNING: The patched library has a bug of leaking TrueType font resource on releasing.
Currently it is not planned to be solved by modifying source code of FreeType. See module "YSLib::Adaptor::Font" of YFramework for the client-side workaround.
There is a known bug in build system of version 2.5.1 for Win32. Make junction "builds/windows" back to "build/win32" as older versions to workaround.

= DS
Currently only building on Windows is supported.
Msys and devkitARM are required.
The environment variable "DEVKITARM" should be set properly like "/C/devkitPro", to make sure the toolchain files can be found.
Copy the directory "build" to the freetype source directory, change current working directory to "builds" and run "build-ds.cmd" to build the static library.
The script will clean "objs" directory in the freetype source directory, then copy necessary files, call "make -j -f buildlib.mk".
The output is "objs/ds/libfreetype.a" in the freetype source directory.

= MinGW32
Toolchain compatible to i686-pc-mingw32 is required. Only MinGW-w64 GCC has been tested.
The environment variable "PATH" should be set properly like "C:\Windows\System32;C:\MinGW32\bin", to make sure the toolchain files can be found.
Note that if "PATH" is too long or UNIX shell(e.g. from MSys) can be found in the PATH, mingw32-make would probably fail.
Copy the file "builds/ds/modules.cfg" to "objs" in the freetype source directory.
Then run "mingw32-make"(add "-j" to build concurrently) using official makefile to build static library for MinGW32.
The output is "objs/freetype.a" in the freetype source directory.

