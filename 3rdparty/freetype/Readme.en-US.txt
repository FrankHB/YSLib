== FreeType build instructions
FreeType2 source files are needed.
Get the source from http://freetype.org .
Be careful with EOL. If "mingw32-make" is used to build for Windows, make sure the ".zip" source is used.
Note that the build command might have to be run more than once because the fisrt run might only generate "ftmodule.h" and there would be no actual library output.

= Since build 563:
Dependencies of FreeType interal headers are removed if external build of FreeType2 headers and libraries are used.

= Since build 562:
A check added in implementation of module "YSLib::Adaptor::Font" to enable the hack only when macro "FT_CONFIG_OPTION_OLD_INTERNALS" is defined for version 2.5.0 or above. This macro is not supported since 2.5.0, so both older versions and new versions of external build is supported.
Though untested, now all versions with compatible APIs should work, at least for versions since 2.3.12 because no new APIs are used since then.

= Patching
Since build 420, the following files from version 2.4.11 should replace files from newer versions(since 2.4.12) respectively:
"include/freetype/config/ftoption.h",
"src/sfnt/ttmtx.c".
NOTE: The include path has been modified since 2.5.1. No "freetype/" would be in the destination header path.
WARNING: The patched library has a bug of leaking TrueType font resource on destruction of object with type "YSLib::Drawing::Typeface" until build 425. However, this fix is not compatible with previous versions of FreeType2, until build 552.
Direct fix on FreeType source is not planned. See module "YSLib::Adaptor::Font" of YFramework for the client-side workaround.

= DS
Currently only building on Windows is tested.
Msys and devkitARM are required.
The environment variable "DEVKITARM" should be set properly like "/C/devkitPro", to make sure the toolchain files can be found.
Copy the directory "builds" to the freetype source directory, change current working directory to "builds" and run the script "build-ds" to build the static library.
The script "build-ds.sh" is for UNIX shell. Alternatively, use "build-ds.cmd" for Windows command shell.
The script will clean "objs" directory in the freetype source directory, then copy necessary files, call "make -j -f buildlib.mk".
The output is "objs/ds/libfreetype.a" in the freetype source directory.

= MinGW32
Toolchain compatible to i686-pc-mingw32 is required. Only MinGW-w64 GCC has been tested.
The environment variable "PATH" should be set properly like "C:\msys\mingw32\bin;C:\Windows\System32", to make sure the toolchain files can be found.
Note that if "PATH" is too long or UNIX shell(e.g. from MSys) can be found in the PATH, mingw32-make would probably fail.
Copy the file "builds/ds/modules.cfg" to "objs" in the freetype source directory.
Then run "mingw32-make"(add "-j" to build concurrently) with proper options using official makefile to build static library for MinGW32.
Currently, the YSLib uses:

	mingw32-make CFLAGS="-c -O3 -Wall -fomit-frame-pointer -DNDEBUG"

The output is "objs/freetype.a" in the freetype source directory.
There is a known bug in build system of version 2.5.1 and 2.5.2 for Win32. Make junction "builds/windows" back to "build/win32" as older versions to workaround.

= Android
Currently only building on Windows is tested.
Msys and android standalone toolchain compatible to arm-linux-androideabi is required.
The environment variable "PATH" should be set properly to make sure the toolchain files can be found.
Copy the file "builds/ds/modules.cfg" to "objs" in the freetype source directory.
Run the configure script with "--host=arm-linux-androideabi" in the shell.
Then run "make"(add "-j" to build concurrently) with proper options using official makefile to build static library for Android.
Currently, the YSLib uses:

	./configure --host=arm-linux-androideabi --disable-shared --with-zlib=no --with-bzip2=no --with-png=no --with-harfbuzz=no CFLAGS="-O3 -Wall -Wl,--gc-sections -s -fdata-sections -ffunction-sections -fomit-frame-pointer -DNDEBUG"
	make

The output is "objs/.libs/libfreetype.a" in the freetype source directory.

