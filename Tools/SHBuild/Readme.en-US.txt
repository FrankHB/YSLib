== Tool: simple host build
This is a command line tool to generate binary files by recursively scanning source directories and then calling backends (i.e. compiler driver or archiver).
The output can be object/archive/executable files.

= Precondition
This project depends on YSLib.
YSLib headers should be put in proper directories to be found by the compiler.
YSLib libraries should be put in proper directories to be found by the linker.
YBase.dll and YFramework.dll should be found in PATH.
Currently only windows host is supported.
The source requires ISO C++11 support as it uses YSLib libraries. The build environment is same.
See YSLib project wiki for details.

= Build
See directory "../Scripts".
See also YSLib project wiki for details.

= Test
Currently only self host test script is provided.

= Use for hosted environment
Compile the source, then run in the command line shell.
Type "shbuild SRCPATH [CXXFLAGS ...]" to build a project, where:
	SRCPATH is the source directory to be recursively searched.
	CXXFLAGS are the options to be pass to the C++ compiler.
All empty arguments would be ignored.
SRCPATH is verified to be existed. If not, or other file system operations fail, the program stops and returns 1.
The directory reading is currently implemented by YSLib::DirectorySession. Only Windows style path is supported for MinGW32.
Once the SRCPATH is verified, the program will create a directory named ".shbuild" in the current working directory to be the root of output directory. If this operation fails, the program stops and returns 2.
If the compiler returns non-zero value, the program stops and exited in failure state.
If the archiver returns non-zero value, the program stops and exited in failure state.
The program returns 0 if and only if there is no failure.
The program then performs recursively depth-first search for any probable source to be built.
	Files and directories whose name begins with "." are ignored.
	In each directory, each file whose name matches glob patterns "*.a" or "*.o" would be deleted.
	Other file whose name matches glob patterns "*.c", "*.cc", "*.cpp" or "*.cxx" is treated as a source file, to be verbatim passed to the compiler with CXXFLAGS.
		Currently the compiler is always "gcc -c" for "*.c" and "g++ -c" for others.
		Each output has its default name.
	After compiling, the output object files("*.o" files, if any) in each single directory and the previously linked static library("*.a" files, if any) are statically linked.
		Nothing would be linked if no input files found.
		Currently the linker is always "ar rcs".
		Each output has the name of the directory, to be put at parent directory.
Finally there should be a "SRCNAME.a" in the top level ".shbuild". Run compliler on demand manually to convert this file to an executable or library file.
For all other options, call without arguments to see usage.

