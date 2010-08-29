// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-8-29;
// Version = 0.2614; *Build 142 r34;


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
/= \ns platform_type >> \ns platform;

r3:
+ \cl DirIter @@ \ns platform @@ \u YCommon;
/ @@ \ns YSLib @@ \h YAdapter:
	- using ::DIR_ITER;
	- using ::diropen;
	- using ::dirnext;
	- using ::dirreset;
	- using ::dirclose;
	+ using platform::DirIter;
/ @@ \impl @@ \u YFileSystem:
	/ ^ DirIter;
+ using ::mkdir @@ \ns platform @@ \u YCommon;

r4-r19:
/ test 1:
	/ @@ \cl platform::DirIter @@ \u YCommon:
		* \tr @@ \mf operator++();
		- \i @@ \mf operator();
/= \tr \impl @@ \f bool IsBaseName(const char*, const char*) @@ \u YFileSystem;

r20:
/ @@ \cl platform::DirIter @@ \u YCommon:
	+ \mf void Open(CPATH);
	/= \impl @@ \ctor;

r21:
= test 2;

r22:
\a SetBgRedrawing => SetBgRedrawed;
\a SetBgRedrawing => SetBgRedrawed;
\a BgRedrawing => BgRedrawed;

r23:
/ \m mutable bool bBgRedraw -> bBgRedrawed @@ \cl MVisual @@ \u YWidget;

r24:
/ \impl @@ \mf DrawWidgets() @@ \cl YFrameWindow @@ \u YWindow;

r25-r26:
/ \impl @@ void Update() @@ \cl YDesktop @@ \u YDesktop;
/ \impl @@ void Update() @@ \cl YShlGUI @@ \u ShlDS;

r27-r31:
= test 3;

r32:
/ \tr \impl @@ \mf OnActivated @@ \cl ShlLoad @@ \u Shells;

r33-r34:
= test 4:
/ \tr \impl @@ \mf void Refresh() @@ \cl MWidget @@ \u YWidget;



DOING:


/ ...

NEXT:
+ fully \impl YFileList;
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

