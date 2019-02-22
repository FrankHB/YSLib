== Modified files
Several files are modified based on official FreeType version.
Each file is licensed as same as the original project.
Incremental modifications are listed below.

= Since build 826:
Based on official version 2.9.1.
The modifications are same to previous version.

= Since build 700:
Based on official version 2.6.3:
In "src/sfnt/ttmtx.c":
	Ported old 2.4.11 implementation with "FT_CONFIG_OPTION_OLD_INTERNALS" conditional inclusion of the static function "tt_face_get_metrics".
In "src/smooth/ftgrays.c":
	Used "FT_CONFIG_OPTION_OLD_INTERNALS" as a hack to check if there is a DS build to work around stack overflow.
NOTE: The macro "FT_RENDER_POOL_SIZE" is not modified in "include/freetype/config/ftoption.h". It is not fit here because it also change behavior of other modules. This file in "devel" directory in the source repository also has comments about it should be greater than 4096, which can work but is still considerably too large on platform DS with a stack in ".dtcm" section of only 32KiB specified by devkitARM linker script by default.

== Additional source replacement

= Since build 826:
Source file modification is now based on FreeType 2.9.1.
The modifications are same to previous version.

= Since build 700:
Source file modification is now based on FreeType 2.6.3.
Configuration for platform DS has been revised to make it work with "include/freetype/config/ftoption.h" of FreeType 2.6.3.
There is no need to replace files from old versions (e.g. 2.4.11) manually any longer.
See also "Modified files of FreeType" section above.

= Since build 563:
Dependencies of FreeType interal headers are removed if external build of FreeType2 headers and libraries are used.

= Since build 562:
A check is added in implementation of module "YSLib::Adaptor::Font" to enable the hack only when macro "FT_CONFIG_OPTION_OLD_INTERNALS" is defined for version 2.5.0 or above. This macro is not supported since 2.5.0, so both older versions and new versions of external build is supported.
Though untested, now all versions with compatible APIs should work, at least for versions since 2.3.12 because no new APIs are used since then.

= Since build 420:
The following files from version 2.4.11 should replace files from newer versions(since 2.4.12) respectively:
"include/freetype/config/ftoption.h",
"src/sfnt/ttmtx.c".

= Since build 292
Contentes of source directory "src/cache" are no longer needed to be replaced since version 2.4.9.
The bug of FreeType has been tested fixed.

= Since build 224
Contentes of source directory "src/cache" of 2.4.5 are replaced of those in version 2.4.4 to avoid wrong results in 'FontCache::GetAscender' with unknown reason.

== Custom configuration
The configuration file "modules.cfg" in "build" will make the built library only containing with needed modules by YFramework, with a much smaller binary image size than the default build.
All versions are tested with platform DS of YFramework official build (with same options for every configurations), initially with 'CFLAGS ?= -c -g -O3 -Wall' in makefile.

= Since build 826
Option '-DNDEBUG' is added to DS build script "config.mk".
The following 'make' command line is used with following options for platform MinGW32:
	CFLAGS="-c -pipe -fdata-sections -ffunction-sections -Wl,--gc-sections -Wall -O3 -DNDEBUG -s"

= Since build 700
Based on previous revision b459 and official version of "modules.cfg" in the FreeType 2.6.3 source tree, one effective line is appended removed compared with the official version:
	BASE_EXTENSIONS += ftfntfmt.c

= Since build 497
Tested for platform Android using following shell POSIX build command line:
	./configure --host=arm-linux-androideabi --disable-shared --with-zlib=no --with-bzip2=no --with-png=no --with-harfbuzz=no CFLAGS="-O3 -Wall -Wl,--gc-sections -s -fdata-sections -ffunction-sections -fomit-frame-pointer -DNDEBUG"
	make

= Since build 493
Tested 'make' command line with following options for platform MinGW32:
	CFLAGS="-c -O3 -Wall -fomit-frame-pointer -DNDEBUG"

= Since build 459
Based on previous revision b192, one effective line is appended to avoid linker error about undefined GZip function used by WOFF support of FreeType 2.5.1:
AUX_MODULES += gzip
Tested for all platforms with this version of configuration.

= Since build 420
Based on previous revision b406, several effective line are removed:
	HINTING_MODULES += autofit
	AUX_MODULES += cache
	BASE_EXTENSIONS += ftfstype.c
	BASE_EXTENSIONS += ftstroke.c
	BASE_EXTENSIONS += ftsynth.c
This version is only tested for platform DS. Old version is still used for platform MinGW32.

= Since build 406
Based on previous revision b266, several effective line are removed:
	HINTING_MODULES += pshinter
	AUX_MODULES += gzip
	BASE_EXTENSIONS += ftbbox.c
Tested with 'CFLAGS ?= -c -g -O3 -Wall' in makefile. Option '-g' is removed to reduce binary image size.

= Since build 266
Based on previous revision b192, one effective line is removed:
	AUX_MODULES += psnames

= Since build 192
Based on previous revision b185, several effective lines are appended:
	BASE_EXTENSIONS += ftbbox.c
	BASE_EXTENSIONS += ftstroke.c
	BASE_EXTENSIONS += ftsynth.c

= Since build 185
File "modules.cfg" is based on official version of "modules.cfg" in the FreeType 2.4.4 source tree, with following effective lines:
	FONT_MODULES += truetype
	FONT_MODULES += sfnt
	HINTING_MODULES += autofit
	HINTING_MODULES += pshinter
	RASTER_MODULES += smooth
	AUX_MODULES += cache
	AUX_MODULES += gzip
	AUX_MODULES += psnames
	BASE_EXTENSIONS += ftbitmap.c
	BASE_EXTENSIONS += ftglyph.c

= Known bugs
The patched library of "src/sfnt/ttmtx.c" has a bug of leaking TrueType font resource on destruction of object with type "YSLib::Drawing::Typeface" until build 425. However, this fix is not compatible with previous versions of FreeType2, until build 552.
Direct fix on FreeType source is not planned. See module "YSLib::Adaptor::Font" of YFramework for the client-side workaround.

= Known upstream bugs
There is a known bug in build system of version 2.5.1 and 2.5.2 for Win32. Make junction "builds/windows" back to "build/win32" as older versions to workaround.

== Build instructions
FreeType2 source files are needed.
Get the source from http://freetype.org .
Be careful with EOL. If "mingw32-make" is used to build for Windows, make sure the ".zip" source is used.
Note that the build command might have to be run more than once because the fisrt run might only generate "ftmodule.h" and there would be no actual library output.
Also note files for different platforms can clash. Clean before a new build except for DS platform (where the script will always clean before the build).

= Caveats for headers
Headers of FreeType may or may not work with different versions of FreeType binary library.
The include path has been modified since 2.5.1. No "freetype/" would be in the destination header path. This is changed back (except for "ft2build.h") after 2.6, to resolve http://savannah.nongnu.org/bugs/index.php?45097.

= Patching
For platform DS, patching is necessary. For other platforms, it is optional.
Copy all files of "build" and "src" directory to the source directory and overwrite original files before building the library.
The directory "include" is not required to be copied currently. It should be identical to the corresponding file in recently versions of FreeType, except for some comments.
See "Additional source replacement" and "Custom configuration" sections above.

= DS
Currently only building on Windows is tested.
MSYS or MSYS2 (providing a POSIX-compatible shell) and devkitARM are required.
Since build 826, official devkitPro instructions are used on a preinstalled MSYS2 environment. See release note https://github.com/devkitPro/pacman/releases/tag/devkitpro-pacman-1.0.1 for steps.
The environment variable "DEVKITARM" should be set properly like "/C/devkitPro/devkitARM", to make sure the toolchain files can be found.
Copy the directory "builds" to the freetype source directory, change current working directory to "builds" and run the script "build-ds" to build the static library.
The script "build-ds.sh" is for UNIX shell. Alternatively, use "build-ds.cmd" with Windows command shell.
The script will clean "objs" directory in the freetype source directory, then copy necessary files, call "make -j -f buildlib.mk".
The output is "objs/ds/libfreetype.a" in the freetype source directory.

= MinGW32
Toolchain compatible to i686-pc-mingw32 is required. Only MinGW-w64 GCC has been tested.
The Windows command line shell ("cmd") is used.
The environment variable "PATH" should be set properly like "C:\msys\mingw32\bin;C:\Windows\System32", to make sure the toolchain files can be found.
Note that if "PATH" is too long or UNIX shell (e.g. from MSYS) can be found in the PATH, "mingw32-make" would probably fail.
Copy the file "builds/ds/modules.cfg" to "objs" in FreeType source directory.
Then run "mingw32-make" (optionally with "-j" to build concurrently) with proper options using official makefile to build static library for MinGW32.
The working directory should be FreeType source directory.
Basically, YSLib uses:

	mingw32-make CFLAGS="-c -O3 -Wall -DNDEBUG"

See history above for actually used CFLAGS.
The output is "objs/freetype.a" in the freetype source directory. To use in YSLib, rename it to "libfreetype.a".
Run "mingw32-make clean" to cleanup built files.

= Android
Currently only building on Windows is tested.
MSYS (providing a POSIX-compatible shell) and Android standalone toolchain compatible to arm-linux-androideabi is required.
The environment variable "PATH" should be set properly to make sure the toolchain files can be found.
Copy the file "builds/ds/modules.cfg" to "objs" in FreeType source directory.
Run the configure script with "--host=arm-linux-androideabi" in the shell.
Then run "make" (optionally with "-j" to build concurrently) with proper options using official makefile to build static library for Android.
The working directory should be FreeType source directory.
Currently, YSLib uses:

	./configure --host=arm-linux-androideabi --disable-shared --with-zlib=no --with-bzip2=no --with-png=no --with-harfbuzz=no CFLAGS="-O3 -Wall -Wl,--gc-sections -s -fdata-sections -ffunction-sections -fomit-frame-pointer -DNDEBUG"
	make

The output is "objs/.libs/libfreetype.a" in the freetype source directory.
Run "make distclean" to cleanup built files.

