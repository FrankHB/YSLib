// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-9-16;
// Version = 0.2614; *Build 148 r20;


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
/ @@ \u Shells:
	+ \cl YConsole;
	/ \tr \impl @@ void ShlS::TFrmFileListSelecter::btnTest_Click(const MTouchEventArgs&);

r2:
/= @@ \u YCommon;
	/= \tr order;
/ @@ \h YAdapter:
	+ using platform::terminate;
/ @@ \tr \impl @@ \i \f void fatalError() @@ unnamed \ns @@ \u YInitialization;
/ @@ \u YGlobal:
	/ \impl @@ YSInit() @@ unnamed \ns;
	- void Terminate(int);
/ @@ \tr \impl @@ \mf void FatalError(const char*) @@ \cl YLog @@ \u YApplication;
/ @@ \tr \impl @@ LRES DefShlProc(const Message&) @@ \cl YShell @@ \u YShell;

r3:
/ \ctor YConsole()->YConsole(YScreen&) @@ \cl YConsole @@ \u Shells;
- \f iputs(const char*) @@ \u YCommon;
	/ \tr \impl @@ \u YInitialization:
		/ \tr \impl @@ \f void InitializeSystemFontCache();
		/ \tr \impl @@ \f void CheckInstall();
	/ \impl @@ void YDebug(const char*);
	/ \tr \impl @@ \u Main:
		/ \tr \impl @@ \f void YSDebug_MSG_Insert(const Message&);
		/ \tr \impl @@ \f void YSDebug_MSG_Peek(const Message&);
r4:
+ \s \f bool InitConsole(YScreen&, Drawing::PixelType, Drawing::PixelType) @@ \cl Def @@ \u YGlobal;
/ @@ \u Shells:
	/ \impl \cl YConsole;

r5:
* \cl YDesktop @@ \ns Device >> \ns Components;
	/ \tr \decl @@ \h YSDefinition;

r6:
/ \a waitForInput => WaitForInput;
/ \a waitForKey => WaitForKey;
/ \a resetVideo => ResetVideo;
/ \a waitForKeypad => WaitForKeypad;
/ \a waitForFrontKey => WaitForFrontKey;
/ \a waitForFrontKeypad => WaitForFrontKeypad;
/ \a waitForArrowKey => WaitForArrowKey;
/ \a waitForABXY => WaitForABXY;

r7-r8:
/ test 1;

r9-r12:
/ \cl @@ \u Shells >> \u YComponent;
/ test 2:
	* \tr \impl @@ \s \f bool InitConsole(YScreen& scr, Drawing::PixelType, Drawing::PixelType) @@ \cl @@ \YGlobal;

r13:
/ \tr \impl @@ \mf void ShlS::TFrmFileListSelecter::btnTest_Click(const MTouchEventArgs&);

r14:
/ @@ \u YInitialization:
	- \s bool bMainConsole;
		/ \simp \tr \impl @@ \f void InitYSConsole();
+ struct Colors { typedef enum ColorSpace {...} ColorSpace}; }; @@ \ns platform @@ \u YCommon;
+ using platform::Colors @@ \ns YSLib::Drawing @@ \h Adapter;

r15-r17:
/ test 3:
	* DefColorH(000000, White) -> DefColorH(FFFFFF, White) @@ \u YCommon;
	^ \ns Colors;

r18:
/ @@ \u YCommon:
	* #define DefColorH_(hex, name) name = RGB8(((hex >> 16) & 0xFF), ((hex >> 8) & 0xFF), (hex & 0xFF))
		-> #define DefColorH_(hex, name) name = RGB8(((hex >> 16) & 0xFF), ((hex >> 8) & 0xFF), (hex & 0xFF)) | BITALPHA;
	/= \a ColorSpace => ColorsSet;
	/ struct Colors -> class Colors:
		+ \i \ctor Colors(PIXEL);
		+ \i \mf operator PIXEL() const;

r19:
/= \a Keys => Key;
/= \a Colors => Color;
/= \a KeysSet => KeySet;
/= \a ColorsSet => ColorSet;

r20:
/ test 4;


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

