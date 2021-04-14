# Modified files

Several files are modified based on official FreeImage version.

Each file is licensed as same as the original project.

Modifications are listed below.

## Compatibility caveats

Source and binary compatibility was kept before b456.

Since b456, the `Plugin` struct is renamed (to avoid global namespace pollution), new interface are introduced, and some functions are removed.

Unmodified library code should compile, but wrappers based on old code might not work as before.

## Since build 902

On platform with possibly both G++ and Clang++ available, the LTO option is refined to have `-ffat-lto-object` to be built by G++ by default.

The makefile variable `C_LTO` (on all platforms) is overridable now, to allow different options for toolchains incompatible to the default value.

This makes the generated archive compatible both to G++ and Clang++, with (hopefully) some degrees of LTO available in some configurations without rebuilt the library for different compilers.

## Since build 900

Now different configurations to build the library are used.

LTO is enabled in makefiles for release configurations through variable `C_LTO`. The option `-flto` should be compatible to both G++ and Clang++.

## Since build 600

Based on previous revision b520:

Merged base version 3.17.0 without functions with suffix `U` and real support for new formats.

For compatibility, `mkdir -p Dist` is not added as official makefiles.

Added Win64 support in Makefile.mingw. Now it uses variable `MSYSTEM` to distinguish Win32 and Win64. The value shall be one of `MINGW32` and `MINGW64`.

## Since build 520

Based on previous revision b456:

Merged base version 3.16.0 without functions with suffix `U` in the name and real support for new formats.

FreeImagePlus is not touched.

## Since build 456:

Based on previous revision b431:

Removed plugin functions:

* `FreeImage_GetFormatFromFIF`
* `FreeImage_GetFIFMimeType`
* `FreeImage_GetFIFExtensionList`
* `FreeImage_GetFIFDescription`
* `FreeImage_GetFIFRegExpr`
* `FreeImage_FIFSupportsReading`
* `FreeImage_FIFSupportsWriting`
* `FreeImage_FIFSupportsExportBPP`
* `FreeImage_FIFSupportsExportType`
* `FreeImage_FIFSupportsICCProfiles`
* `FreeImage_FIFSupportsNoPixels`
* `FreeImage_SetPluginEnabled`
* `FreeImage_IsPluginEnabled`

In `../include/FreeImage.h`:

* Exposed struct `PluginNode` as `FI_PluginNodeRec`.
* Added declaration of function `FreeImageEx_GetPluginNodeFromFIF`.
* Added a macro `nullptr` for C compatibility.
* Renamed struct `Plugin` to `FI_PluginRec`.

In `Source/Utilities.h`:

* Added typedef declaration of `Plugin` to keep source compatibility.

In `Source/Plugin.h`:

* Removed unnecessary class name declaration `Plugin`.
* Moved struct `PluginNode` to public header.
* Removed declaration of function `FreeImage_stricmp`.
* Removed including `Utilities.h`.
* Added typedef declaration of `PluginNode` to keep source compatibility.

In `Source/FreeImage/Plugin.cpp`:

* Added definition of function `FreeImageEx_GetPluginNodeFromFIF`.
* Simplified implementation using C++11.
* Made `PluginList::AddFakeNode` call `FreeImage_OutputMessageProc` on fail.

## Since build 431

Based on previous revision b417:

In `../include/FreeImage.h`, `Source/FreeImage/Plugin.cpp`, `Source/FreeImage/GetType.cpp` and `Source/FreeImageToolkit/JPEGTransform.cpp`, removed functions only supported in Win32: 

* `FreeImage_LoadU`
* `FreeImage_SaveU`
* `FreeImage_GetFileTypeU`
* `FreeImage_GetFIFFromFilenameU`
* `FreeImage_JPEGTransformU`
* `FreeImage_JPEGCropU`.

Confirmed for all modified above:

* Removed all spaces at end of lines.
* Saved as UTF-8 + BOM.
* All EOL markers are normalized as CR+LF.

**NOTE** `Source/FreeImageToolkit/JPEGTransform.cpp` is excluded in the source list. The file is reserved for future use.

Additional source of libjpeg-turbo-1.3.0 is now used to replace libjpeg-8d in `Source/LibJPEG`.

`Makefile.srcs` is edited properly to exclude unused files and to add libjepg-turbo source files.

`Makefile.mingw` is modified to handle assembly files with NASM.

## Since build 417

Based on official version 3.15.4:

Removed `#include <memory.h>` in `Source/Utilities.h` to make it conforming to the ISO C/C++ standard.

In `../include/FreeImage.h`:

* Replaced all function default argument for pointer type parameters from `NULL` or `0` to `nullptr` to `nullptr`, to avoid possible G++ warning `[-Wzero-as-null-pointer-constant]`. This requires the `nullptr` keyword being supported.
* Removed all spaces at end of lines.
* Saved as UTF-8 + BOM.

Disabled unused plugins and make placeholders in plugin list in `Source/Plugin.h` and `Source/FreeImage/Plugin.cpp`.

`Makefile.srcs` is edited properly to exclude unused files.

# Additional source replacement

## Since build 885

Based on base version 3.18.0:

Source files of libpng are updated to 1.6.37 from 1.6.35.

Get the libpng source from [here](http://sourceforge.net/projects/libpng/files/libpng16/1.6.37/lpng1637.7z).

Source files of libjpeg-turbo are required to replace libjpeg in original FreeImage source distribution.

Get the libjpeg-turbo source from [here](http://sourceforge.net/projects/libjpeg-turbo/files/2.0.4/libjpeg-turbo-2.0.4.tar.gz).

(This version contains the newest release of zlib 1.2.11, no need to change.)

## Since build 700

Based on previous revision b600:

Source files of libpng are updated to 1.6.23 from 1.6.17.

Get the libpng source from [here](http://sourceforge.net/projects/libpng/files/libpng16/older-releases/1.6.23/lpng1623.7z).

Source files of libjpeg-turbo are required to replace libjpeg in original FreeImage source distribution.

Get the libjpeg-turbo source from [here](http://sourceforge.net/projects/libjpeg-turbo/files/1.5.0/libjpeg-turbo-1.5.0.tar.gz).

(This version contains the newest release of zlib 1.2.8, no need to change.)

## Since build 600

Based on base version 3.17.0:

Source files of libpng are updated to 1.6.17 from 1.6.12.

Get the libpng source from [here](http://sourceforge.net/projects/libpng/files/libpng16/older-releases/1.6.17/lpng1617.7z).

Source files of libjpeg-turbo are required to replace libjpeg in original FreeImage source distribution.

Get the libjpeg-turbo source from [here](http://sourceforge.net/projects/libjpeg-turbo/files/1.4.0/libjpeg-turbo-1.4.0.tar.gz).

(This version contains the newest release of zlib 1.2.8, no need to change.)

## Since build 520

Based on base version 3.16.0:

Source files of libpng are updated to 1.6.12 from 1.6.10.

Get the libpng source from [here](http://sourceforge.net/projects/libpng/files/libpng16/older-releases/1.6.12/lpng1612.7z).

Source files of libjpeg-turbo are required to replace libjpeg in original FreeImage source distribution.

Get the libjpeg-turbo source from [here](http://sourceforge.net/projects/libjpeg-turbo/files/1.3.1/libjpeg-turbo-1.3.1.tar.gz).

(This version contains the newest release of zlib 1.2.8, no need to change.)

## Since build 452

Based on previous revision b431:

Source files of libpng are updated to 1.6.6 from 1.6.3.

Get the libpng source from [here](http://sourceforge.net/projects/libpng/files/libpng16/older-releases/1.6.6/lpng166.7z).

## Since build 431

Based on official version 3.15.4:

Source files of libjpeg-turbo are required to replace libjpeg in original FreeImage source distribution.

Get the libjpeg-turbo source from [here](http://sourceforge.net/projects/libjpeg-turbo/files/1.3.0/libjpeg-turbo-1.3.0.tar.gz).

See `release/License.rtf` for the license of libjpeg-turbo, which is in BSD-style and should be compatible with the other part of the YSLib project.

Source files of zlib are updated to 1.2.8 from 1.2.7.

Get the zlib source from [here](http://sourceforge.net/projects/libpng/files/zlib/1.2.8/zlib128.zip).

Source files of libpng are updated to 1.6.3 from 1.5.13.

Get the libpng source from [here](http://sourceforge.net/projects/libpng/files/libpng16/older-releases/1.6.3/lpng163.7z).

# Build instructions

FreeImage source files are required.

Get the source from [here](http://sourceforge.net/projects/freeimage/files/Source%20Distribution).

The output files of different platform will be put in the same directory `Dist`. Make sure it is cleaned before each build.

See following sections before build.

## Patching

Use additional source above to replace specific parts of FreeImage source by directly copying (and overwriting existed files) to `Source/LibJPEG`, `Source/ZLib` and `Source/LibPNG` respectively.

Copy all files except this document in this directory to the source directory and overwrite original files before building the library.

It is now required to replace the original header file `FreeImage.h` in `Source` directory by header in `3rdparty/include` for building the library, though the content of header file `FreeImage.h` is actually equivalent with the original file before b456.

Any architectural-dependent files included here are only for MinGW32 (and assumed compatible to DS).

For other platforms (in particular, 64-bit platforms), the files should be regenerated by configuration script of corresponding dependencies.

Currently, these files include:

`3rdparty/FreeImage/Source/LibJPEG/jconfig.h` since build 431, generated by libjpeg-turbo.

`3rdparty/FreeImage/Source/LibJPEG/jconfigint.h` since build 700, generated by libjpeg-turbo.

However, the macro in `WITH_SIMD` in `3rdparty/FreeImage/Source/LibJPEG/jconfig.h` is manually maintained. It is only enabled in Win32 platforms (either 32-bit or 64-bit).

## All tagrets

All targets use makefile currently.

For GNU make, use `-R` option to avoid builtin variables which disturbs the default values defined by `?=`. This is crucial where `AR` should be `gcc-ar` (as well as `llvm-ar` specified externally) [for proper LTO builds](https://gcc.gnu.org/wiki/LinkTimeOptimizationFAQ#ar.2C_nm_and_ranlib).

Since build 900, builds for debug and release configurations of YFramework may use different configurations of binary dependencies. The `FreeImage` library now has `DEBUG` configuration specified by `CONF=DEBUG` in the `make` command line. This enables debug builds for debug configurations of `YFramework`. If `CONF` is set to any other value, it is all the same to be unset. Note the `debug` builds here only disable LTO options in the compiler command lines (wichout change to optimization flags), as the 3rd-party libraries are not intended to be debugged. Also note that GCC's `-flto -ffat-lto-objects` does not help for the output using both in the debug and release configurations of YFramework building, at least with `MinGW32` GCC 10.2.

The output filename is not changed by changing the configurations. Rename it manually with `d` postfix before the file extension if it is used for building the `debug` library and put it into proper `lib` directory under `YFramework/$PLATFORM/`, where `$PLATFORM` is one of the name of supported platform configurations. This scheme ensures library files built with different configurations can coexist in the repository tree in the same time.

## DS

Currently only building on Windows is tested.

MSYS (providing a POSIX-compatible shell) and devkitARM are required.

The environment variable `DEVKITARM` should be set properly like `/C/devkitPro/devkitARM`, to make sure the toolchain files can be found.

The file `Makefile.ds` should have been copied to the source directory.

`Source/LibJPEG/jconfigint.h` should have the macro `WITH_SIMD` effectively undefined.

Run `make -R -f Makefile.ds` (optionally with `-j` to build concurrently) to build the static library.

Run `make -R -f Makefile.ds clean` to delete all build files.

## MinGW32

MinGW-w64 GCC toolchain, the `cp` and `rm` command are required by the official or modified MinGW makefile. (MSYS should be OK.)

An assembler is required for compiling libjpeg-turbo SIMD source files. This is called by variable NASM. It now defaults to `yasm`, since `nasm` from MSYS2 does not work.

The environment variable `MSYSTEM` shall be set to `MINGW32`, e.g. run `set MSYSTEM=MINGW32` at `cmd`.

The environment variable `PATH` should be set properly like `C:\msys\mingw32\bin;C:\msys\usr\bin;C:\Windows\System32`, to make sure the toolchain files can be found.

The modified file `Makefile.mingw` should have been copied to the source directory and overwritten the original one.

Run `mingw32-make -R FREEIMAGE_LIBRARY_TYPE=STATIC -f Makefile.mingw libFreeImage.a` (optionally with `-j` to build concurrently) to build static library for MinGW32.

Run `mingw32-make -R -f Makefile.mingw clean` to delete all build files. Note the official `clean.bat` is not enough.

Note that although omission of `libFreeImage.a` is allowed, it is not recommended because:

* The `FREEIMAGE_LIBRARY_TYPE=STATIC` does not respect of the symbol visibility attributes. This makes the symbols not usable from the shared library, esp. not usable in a Win32 DLL.
* Only the static library is used by YFramework, so this is normally uncessary.

## MinGW64

The enviornment is similar to MinGW32 with the proper x86-64 toolchain, except the setting of following environment variables:

The environment variable `MSYSTEM` shall be set to `MINGW64`, e.g. run `set MSYSTEM=MINGW64` at `cmd`.

The environment variable `PATH` should be set properly like `C:\msys\mingw64\bin;C:\msys\usr\bin;C:\Windows\System32`, to make sure the toolchain files can be found.

## Linux

A working GNU toolchain is required.

NASM is required for compiling libjpeg-turbo SIMD source files.

Run `make -R FREEIMAGE_LIBRARY_TYPE=STATIC -f Makefile.gnu libfreeimage.a` (optionally with `-j` to build concurrently) to build static library.

Run `make -R -f Makefile.gnu clean` to delete all build files.

For hosted Linux environment, `-f Makefile.gnu` can be omitted.

Omission of `libfreeimage.a` in the building command line is not recommended for the similar reasons as in MinGW32.

If only the static library is built, the target `dist` is not run, so the library file is at the buidling directory but not `./Dist`.

