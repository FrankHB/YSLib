// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-9-18;
// Version = 0.2615; *Build 149 r39;


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
/ \simp \impl @@ \mf void ShlS::TFrmFileListSelecter::fb_Selected(const MIndexEventArgs&) @@ \u Shells;

r2:
/= @@ \cl MIndexEventArgs @@ \u YControl:
	/= \m index => Index;
	/= \m con = > Control;
		/= \tr \impl @@ \u Shells;
/ @@ \cl YPath \u YFileSystem:
	/= \m value_type => ValueType;
	/= PDefHead(const value_type*, c_str) => PDefHead(const ValueType*, c_str);

r3:
/ \a MBCSToMString => MBCSToString;

r4-r9:
/ test 1:
	* \impl @@ std::string GetNowDirectory() @@ \ns IO @@ \u YFileSystem;

r10-r11:
/ @@ \u YFileSystem:
	- \inc "../Shell/ywindow.h";
	/= @@ \cl MFileList:
		/= \ret \tp @@ \mf u32 LoadSubItems() -> ListType::size_type;
		/= \ret \tp @@ \mf u32 ListItems() -> ListType::size_type;

r12-r20:
/= test 2;

r21:
/ \impl @@ \mf void YFontCache::LoadFontFileDirectory(CPATH, CPATH) @@ \u YFont:
	^ HDirectory;
	/ \simp;

r22:
/ \tr @@ \u YShellInitialization:
	* \impl @@ \f void CheckInstall();
	/ \tr \impl @@ \f void InitYSConsole();

r23-r24:
/= test 3;
	/ \tr \impl @@ \f void InitYSConsole() @@ \u YShellInitialization;

r25-r26:
/ \tr @@ \u YShellInitialization:
	/ \impl @@ \f void InitYSConsole();
	/ \f void installFail() -> void installFail(const char*) @@ unnamed \ns;
	/ \impl @@ \f void CheckInstall();

r27-r35:
/= test 4;
	/ @@ \u YShellInitialization:
		/ \tr \impl @@ \f void InitYSConsole();
		* \impl @@ \f void CheckInstall();

r36:
/ void YConsoleInit(u8 dspIndex, PIXEL = RGB15(31, 31, 31), PIXEL = RGB15( 0, 0, 31))
	-> void YConsoleInit(u8 dspIndex, PIXEL = Color::White, PIXEL = Color::Black)
	@@ \u YCommon;

r37-r38:
	/ \tr \impl @@ \f void InitYSConsole() @@ \u YShellInitialization;	

r39:
/ edited \g makefile to make a copy without DLDI automatically while building;


DOING:

/ ...

NEXT:
* fatal error;
+ fully \impl \u YFileSystem;
/ fully \impl \cl ShlReader;
/ fully \impl btnTest;

TODO:

Clarify the log levels.

Make "UpdateFont()" more efficienct.

More efficient @YTextRegion output:
Use in-buffer background color rendering and function @CopyToScreen() to inplements @YTextRegion background;
Use pre-refershing to make font changing.

Consider to simplify the general window class: @YForm.

Rewrite system RTC.

Build a more advanced console wrapper.

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

