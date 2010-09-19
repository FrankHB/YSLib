// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-9-20;
// Version = 0.2617; *Build 151 r29;


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
\ac ::= access
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
\ext ::= extended
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
/ @@ \u YGlobal:
	/ \tr \impl @@ \f YInit() @@ unnamed \ns;
	*- \def @@ \f void Terminate(int);

r2:
* \impl @@ \f void CheckInstall() @@ \u YShellInitialization;

r3-r6:
/ \tr \impl @@ \f void installFail(const char*) @@ \u YShellInitialization;

r7-r25:
/= test 1;

r26:
/= \a _refType => _tReference;
/= \a _cmpType => _fCompare;
/= \a _predicateType => _fPredicate;
/= \a _containerType => _tContainer;
* \impl @@ \tf template<typename _tContainer, typename _fPredicate> typename _tContainer::size_type
	erase_all_if(_tContainer&, const typename _tContainer::value_type&) @@ \u YCoreUtilities;
/= \a _pixelType => _tPixel;
/= \a _transpType => _fTransformPixel
/= \a _translType => _fTransformLine
/= \a _ptrType => _tPointer

r27:
/= \a _eventArgsType => _tEventArgs;
/= \a _codemapFuncType => _fCodemapTransform;

r28:
/= \a _outIt => _tOut;
/= \a _charT => _tChar;
/= \a _EventSpace => _tEventSpace;
/= \a _Event => _tEvent;
/= \a _responser => _tResponser;
/= \a \tp \para _container => _tContainer;

r29:
/ @@ \u YCommon:
	+ \f char* getcwd_n(char*, std::size_t) @@ \ns platform;
	/= \tr \impl @@ \f bool fexists(CPATH) @@ \ns stdex;
	+ using ::chdir @@ \ns platform;
/ \tr \impl @@ \i \fint ChDir(CPATH) @@ \u YFileSystem;
/ @@ \ns YSLib @@ \h YAdapter:
	- using ::stat;
	- using ::mkdir;
	- using ::chdir;
	- using ::getcwd;
/= \tr \impl @@ \f void CheckInstall() @@ \u YShellInitialization;
/ using ::iprintf @@ \ns YSLib @@ \h YAdapter => \ns platform @@ \u YCommon;
/ @@ \h YAdapter:
	/ {
		using ::swiWaitForVBlank;

		using ::lcdMainOnTop;
		using ::lcdMainOnBottom;
		using ::lcdSwap;
		using ::videoSetMode;
		using ::videoSetModeSub;

		using ::scanKeys;
		using ::touchRead;
	} @@ \ns YSLib => \ns YSLib::DS;
	+ using platform::PixelType @@ \ns YSLib;
	+ using platform::ScreenBufferType @@ \ns YSLib;
	- typedef ::PIXEL PixelType @@ YSLib::Drawing;
	- #define SCR_MAIN 0x1
	- #define SCR_SUB 0x2
	- #define SCR_BOTH 0x3
+ typedef u16 PixelType @@ \ns platform @@ \u YCommon;
- typedef u16 PIXEL @@ \h <platform.h>;
/ typedef PIXEL ScreenBufferType[SCREEN_WIDTH * SCREEN_HEIGHT] @@ \h <platform.h> => \ns platform @@ \u YCommon;
/ #define BITALPHA BIT(15) => \u YCommon;
/ \a PIXEL -> PixelType;
- \m (insRefresh & scrType) @@ \cl YShell @@ \u YShell;
/ \simp \impl @@ \f void Def::Idle() @@ \u YGlobal;
/ \a SCRBUF => ScreenBufferType;
/= \ inc <assert.h> @@ \h YCommon => \inc <cassert>;


DOING:

/ ...

Debug message:
//

NEXT:
/ fully \impl \u YFileSystem;
/ fully \impl \cl YListBox;
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

