// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-8-25;
// Version = 0.2614; *Build 141 r39;


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
= test 0;

r2:
/ @@ \u Shells:
	+ \m std::string path @@ \cl ShlReader;
	/ \impl @@ void switchShl2() @@ unnamed \ns;

r3:
/ void switchShl2() -> void switchShl2(CPATH) @@ unnamed \ns @@ \u Shells;

r4:
/ \a YPath => MPath;
/ @@ \u YFileSystem:
	/ typedef MString YPath -> typedef std::string MPath;
	/ \ret \ty @@ MString GetFileName(const MPath&) -> std::string;
	/ \ret \ty @@ MString GetDirectoryName(const MPath&) -> std::string;
	/ MPath::size_type SplitPath(const MPath&, MPath&, MString&) ->
		MPath::size_type SplitPath(const MPath&, MPath&, std::string&);
	/ MString GetBaseName(const MString&, const MString&) ->
		std::string GetBaseName(const std::string&, const std::string&);
	/ bool IsBaseName(const MString&, const MString&) ->
		bool IsBaseName(const std::string&, const std::string&);
	/ bool SameBaseNames(const char*, const char*) ->
		bool HaveSameBaseNames(const char*, const char*);
	/ bool SameBaseNames(const MString&, const MString&) ->
		bool HaveSameBaseNames(const std::string&, const std::string&);
	/ MString GetExtendName(const MString&) ->
		std::string GetExtendName(const std::string&);
	/ bool IsExtendName(const MString&, const MString&) ->
		bool IsExtendName(const std::string&, const std::string&);
	/ bool SameExtendNames(const char*, const char*) ->
		bool HaveSameExtendNames(const char*, const char*);
	/ bool SameExtendNames(const MString&, const MString&) ->
		bool HaveSameExtendNames(const std::string&, const std::string&);
	/ @@ \u MFileList:
		/ \mf void GoToSubDirectory(const MString&) ->
			\mf void GoToSubDirectory(const std::string&);

r5-r27:
/ test 1:
	/ \tr \impl @@ \mf u32 MFileList::LoadSubItems() @@ \u YFileSystem;
	/ \tr \impl @@ \f YInit @@ unnamed \ns @@ \u YGlobal;

r28:
* @@ \u CHRMap:
	* \impl @@ \f template<> uchar_t codemap<CharSet::GBK>(ubyte_t&, const char*);
	* \impl @@ \f template<> uchar_t codemap<CharSet::GBK>(ubyte_t&, FILE*);

r29:
/ test 2;

r30:
/ @@ \u YString:
	/ \a private \m S_str => s_str @@ \cl MString;
	+ \f MString MBCSToMString(const char*, const CSID& = CS_Local);
	+ \i \f MString MBCSToMString(const std::string&, const CSID& = CS_Local);
/ \tr \impl @@ u32 MFileList::LoadSubItems() @@ \u YFileSystem;

r31-36:
/ test 3;

r37:
+ \i \ctor MString(uchar_t*) @@ \cl MString @@ \u YString;

r38-r39:
/ test 4;


DOING:


/ ...

NEXT:
+ \impl \cl ShlReader;
+ fully \impl btnTest;

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

