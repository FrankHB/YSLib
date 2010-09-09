// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-9-9;
// Version = 0.2614; *Build 145 r96;


#include "../YCLib/ydef.h"

/*

Record prefix and abbrevations:
<statement> ::= statement;
; ::= statement termination
= ::= equivalent
+ ::= added
- ::= removed
* ::= fixed
/ ::= modified
% ::= reformed
~ ::= from
! ::= not
& ::= and
| ::= or
^ ::= used
-> ::= changed to
>> ::= moved to
=> ::= renamed to
@ ::= identifier
@@ ::= in / belongs to
\a ::= all
\bg ::= background
\c ::= const
\cb ::= catch blocks
\cl ::= classes
\ctor ::= constructors;
\cv ::= const & volatile
\d ::= derived
\de ::= default
\def ::= definitions
\decl ::= declations
\dtor ::= destructors;
\e ::= exceptions
\en ::= enums
\eh ::= exception handling
\err ::= errors
\es ::= exception specifications
\ev ::= events
\evh ::= event handling
\ex ::= extra
\exp ::= explicit
\f ::= functions
\g ::= global
\gs ::= global scpoe
\h ::= headers
\i ::= inline
\impl ::= implementations
\init ::= initializations
\inc ::= included
\inh ::= inherited
\inv ::= invoke
\m ::= members
\mac ::= macros
\mem ::= memory
\mf ::= member functions
\ns ::= namespaces
\op ::= operators
\para ::= parameters
\para.def ::= default parameters
\ptr ::= pointers
\rem ::= remarked
\ref ::= references
\ret ::= return
\s ::= static
\sf ::= non-member static functions
\sif ::= non-member inline static functions
\simp ::= simplified
\smf ::= static member functions
\sm ::= static member
\scm ::= static const member
\snm ::= static non-member
\st ::= structs
\str ::= strings
\t ::= templates
\tb ::= try blocks
\tc ::= class templates
\tf ::= function templates
\tr ::= trivial
\tp ::= types
\u ::= units
\v ::= volatile
\val ::= values

DONE:
r1:
/ @@ \cl ShlReader @@ \u Shells:
	* \impl @@ \mf void UpdateToScreen();

r2:
/= \a TextIterator => HText;
/= \a DirIter => HDirectory;

r3:
/= \a MFont => Font:
	/= \tp name Font => Drawing::Font;
/= \a MFontFamily => FontFamily;
/= \a MTypeface => Typeface;
/= \a MFontFile => FontFile;
/= \a MException => Exception;
/= \a MGeneralError => GeneralError;
/= \a MLoggedEvent => LoggedEvent;
/= \a MString => String;
/= \a MMSG => Message;
/= \a MGIC => GraphicInterfaceContext;
/= \a MPenStyle => PenStyle;
/= \a MPadding => Padding;
/= \a MTouchStatus => STouchStatus;
/= \a MTextState => TextState;
/= \a MTextRegion => TextRegion;
/= \a MTextBuffer => TextBuffer;
/= \a MTextBlock => TextBlock;
/= \a MTextMap => TextMap;
/= \a MTextFileBuffer => TextFileBuffer;

r4:
/ @@ \u YFileSystem:
	+ typedef char NativeCharType;
	+ \cl Path;
	+ \i \f bool operator==(const Path&, const Path&);
	+ \i \f bool operator!=(const Path&, const Path&);
	+ \i \f bool operator<(const Path&, const Path&);
	+ \i \f bool operator<=(const Path&, const Path&);
	+ \i \f bool operator>(const Path&, const Path&);
	+ \i \f bool operator>=(const Path&, const Path&);
	+ \i \f Path operator/(const Path&, const Path&);
	+ \i \f void swap(Path&, Path&);

r5:
/ @@ \u YFileSystem:
	/ typedef std::string MPath -> typedef Path MPath;
	/ std::string GetFileName(const MPath&)
		-> std::string GetFileName(const std::string&);
	/ std::string GetDirectoryName(const MPath&)
		-> std::string GetDirectoryName(const std::string&);
	/ MPath::size_type SplitPath(const std::string&, std::string&, std::string&)
		-> std::string::size_type SplitPath(const std::string&, std::string&, std::string&);
	/ bool HaveSameBaseNames(const String&, const String&)
		-> bool HaveSameBaseNames(const std::string&, const std::string&);
	/ int ChDir(const std::string&)
		-> int ChDir(const std::string&);
	/ \a HaveSameBaseNames -> HaveSameStems;
	/ \a IsBaseName -> IsStem;
	/ \a GetBaseName -> GetStem;
	/ MPath GetNowDirectory()
		-> std::string GetNowDirectory();
r6:
/ @@ \u YFileSystem:
	- typedef Path MPath;
/ \a MPath => Path;

r7-r33:
= test 1;

r34:
/ @@ \cl YDesktop @@ \u YDesktop:
	/ \impl @@ \mf void DrawBackground();
	/ \impl @@ \mf void Draw();
	/ \impl @@ \mf void Refresh();
/ @@ \cl AWindow @@ \u YWindow:
	/ \impl @@ \mf void DrawBackground();

r35-r96:
/ test 2;
	* \tr \impl @@ \u Shells;


DOING:


/ ...

NEXT:
+ fully \impl \u YFileSystem;
/ fully \impl \cl ShlReader;
/ fully \impl btnTest;

TODO:

Clarify the log levels.

Make "UpdateFont()" more efficienct.

More efficient  @YTextRegion output:
Use in-buffer background color rendering and function @CopyToScreen() to inplements @YTextRegion background;
Use pre-refershing to make font changing.

Consider to simplify the general window class: @YForm.

Rewrite system RTC.

Build a more advanced console system.

Build a series set of robust gfx APIs.

GUI characteristics needed:
Icons;
Buttons;
Other controls.

Other stuff to be considered to append:
Design by contract: DbC for C/C++, GNU nana.

*/

namespace YSLib{
	CPATH DEF_DIRECTORY = "/Data/";
	//const char* DEF_FONT_NAME = "方正姚体";
	//CPATH DEF_FONT_PATH = "/Font/FZYTK.TTF";
	CPATH DEF_FONT_PATH = "/Font/FZYTK.TTF";
	CPATH DEF_FONT_DIRECTORY = "/Font/";
/*
using namespace Components;
using namespace Widgets;
using namespace Controls;

*/
}
char gstr[128] = "你妹喵\t的= =ijkAB DEǎ。i:みま╋㊣F2\t3Xsk\nw\vwwww";

