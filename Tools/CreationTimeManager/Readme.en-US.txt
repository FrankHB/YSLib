== Tool: creation time manager
This is a command line tool for maintain creation time of project files.
The output file(if any) is a text file with file name and creation time stored.

= Precondition
Currently only windows host is supported.
The source requires (limited) ISO C++11 support.
G++ 4.8.1 and VC++ 2012 have been originally tested and any later versions of these implementations should be able to compile the source.
G++ command example:

	g++ -std=c++11 -O3 -s -Wall -Wextra Main.cpp

= Use for hosted environment
Compile the source, then run in the command line shell.
There are 2 run modes for this program:
	Reading mode. This is the default with no command line options specified.
		The program accepts file names(one per line), then output according creation time(currently in ::FILETIME format, i.e. 2 integers).
	Writing mode. This is specified by only one command line option "-w".
		The program accepts pair of lines consisted of file name and creation time(format as above output).
Both non-directory files and directories are supported.
Symbolic links are treated as ordinary files, not the targets.
Let the program named "creation-time-manager", the specified target directory named "directory" and the output file named "backup.txt", here are the typical uses for Windows cmd:
	Backup creation time using following command line with full file paths in target directory saved:

		DIR directory /A /B /S | creation-time-manager > directory.ct.txt

	Restore creation time using following command line:

		TYPE directory.ct.txt | creation-time-manager -w

Note the example above use system code page.
There are some known problems in the example:
1. Now the program supports path characters out of ANSI code page, but 'cmd' pipes (even with 'CHCP 65001') are not able to handle it well, see http://stackoverflow.com/questions/1259084/what-encoding-code-page-is-cmd-exe-using.
2. It does not handle reparse points well. This may be not intended.
	To ignore reparse points only, use '/A-l' instead of '/A'.
	It is difficult to reserve reparse points only or to ignore the entries in junctions, i.e. traversing the directory without following the link. A powershell script would be better, see http://superuser.com/questions/528487/list-all-files-and-dirs-without-recursion-with-junctions.

