== Tool: creation time manager
This is a command line tool for maintain creation time of project files.
The output file(if any) is a text file with file name and creation time stored.

= Precondition
Currently only windows host is supported.
The source requires (limited) ISO C++11 support.
G++ 4.8.1 and VC++2012 have been tested.

= Use for hosted environment
Compile the source, then run in the command line shell.
There are 2 run modes for this program:
	Reading mode. This is the default with no command line options specified.
		The program accepts file names(one per line), then output according creation time(currently in ::FILETIME format, i.e. 2 integers).
	Writing mode. This is specified by only one command line option "-w".
		The program accepts pair of lines consisted of file name and creation time(format as above output).
Both non-directory files and directories are supported.
Symbolic links are treated as ordinary files, not the targets.
Let the program named "creation-time-manager", the specified target directory named "directory" and the output file named "backup.txt", here are the typical uses for Windows:
	Backup creation time using following command line with full file paths in target directory saved:
		DIR directory /B /S | creation-time-manager > backup.txt
	Restore creation time using following command line:
		TYPE backup.txt | creation-time-manager -w

