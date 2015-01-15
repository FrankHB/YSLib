== Tool: revision patcher
This is a command line tool for patch source files.
Only revision number extraction is supported currently.
The input shall be content of a patch file.
The output is in stdout, consisted of groups of records. Each group has 2 line: destination file name and new revision number.
The output file name is extracted from the input patch file. The revision numbers are calculated according the rules below.

= Prerequisites
The source depends on YSLib b565.

= Source configuration
There is currently no configuration supported.

= Use for hosted environment
Currently only patches exported from 'hg diff' are tested. Example:

	hg status --color=none -amn0 | xargs -0 hg diff --color=none | RevisionPatcher

= About revision number
The revision number rules is specified by Documentation::ProjectRules.
Following are current implementation details.
New revision number is based on patch content.
	If old file does not exist, the revision number is the number of lines except for empty lines at end of the file.
	Otherwise, it is added to the base revision number by the number of modified lines.
		The base revision number is the revision number of old file, or zero if the revision number is not found in the old file.
		The revision number in the file shall be of 'version r' followed by an non-negative integer which is suited for 'unsigned long' type in the environment where this program runs.
	The modification of lines of revision number would be always excluded for purpose of calculating the new revision number.
	Empty lines would not count, unless there are no other changes.
	Each pair of non-empty adjacent lines of '+' and '-' are merged and treated as one modification. Then other single '+' or '-' lines are counted. The number of these modifications are added to the base revision number to be the output.

