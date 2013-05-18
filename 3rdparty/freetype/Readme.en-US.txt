== FreeType build instructions
FreeType2 source files are needed.
Get the source from http://freetype.org .

= DS
Currently only building on Windows is supported.
Msys and devkitARM are needed.
Copy the directory "build" to the freetype source directory, and run "builds\build-ds.cmd" to build the static library.
The script will clean "objs" directory in the freetype source directory, then copy necessary files, call "make -j -f buildlib.mk".
The output is "objs/ds/libfreetype.a" in the freetype source directory.

= MinGW32
MinGW is needed.
Copy the file "builds/ds/modules.cfg" to "objs" in the freetype source directory.
Then run "mingw32-make"(add "-j" to build concurrently) using official makefile to build static library for MinGW32.
The output is "objs/freetype.a" in the freetype source directory.

