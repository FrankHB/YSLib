// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-8-31;
// Version = 0.2614; *Build 143 r50;


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
r1-r7:
= test 1;

r8:
/ @@ \cl YDesktop @@ \u YDesktop:
	/ \impl @@ \mf void DrawDesktopObjects();
	/ \impl @@ \mf void Draw();
	/ \impl @@ \mf void Refresh();
/ @@ \cl AWindow @@ \u YWindow:
	/ \impl @@ \mf void Draw();
	/ \impl @@ \mf void Update();
	/ \impl @@ \mf void Refresh();
/ \impl @@ \mf void Refresh() @@ \cl MWidget @@ \u YWidget;

r9:
* @@ \cl YDesktop @@ \u YDesktop:
	/ \impl @@ \mf void DrawDesktopObjects();
/= @@ \cl AWindow @@ \u YWindow:
	/= \tr \impl @@ \mf void Update();
	/= \tr \impl @@ \ctor & \dctor @@ \cl YFrameWindow;
/= @@ \u YWidget:
	/= @@ \cl YWidgetContainer:
		/= \tr \impl @@ \mf void DrawBackground()
		/= \tr \impl @@ \mf void DrawForeground()
		/= \tr \impl @@ \ctor & \dctor @@
	/= \tr \impl @@ \mf void Refresh() @@ \cl MWidget
	/= \tr \impl @@ \ctor & \dctor @@ \cl YWidget
/= \tr \impl @@ \ctor & \dctor @@ \cl YVisualContainer @@ \u YControl;

r10-r31:
* \impl @@ \mf OnGotFocus(IControl&, const MEventArgs&) @@ \cl MVisualControl @@ \u YControl;
= test2;
/ \impl @@ \mf void DrawDesktopObjects() @@ \cl YDesktop @@ \u YDesktop;

r32:
/ undo \u YWindow & \u YDesktop & \u YWidget;
/= @@ \cl AWindow @@ \u YWindow:
	/= \tr \impl @@ \mf void Update();
	/= \tr \impl @@ \ctor & \dctor @@ \cl YFrameWindow;
/= @@ \u YWidget:
	/= @@ \cl YWidgetContainer:
		/= \tr \impl @@ \mf void DrawBackground()
		/= \tr \impl @@ \mf void DrawForeground()
		/= \tr \impl @@ \ctor & \dctor @@
	/= \tr \impl @@ \mf void Refresh() @@ \cl MWidget
	/= \tr \impl @@ \ctor & \dctor @@ \cl YWidget
/= \tr \impl @@ \ctor & \dctor @@ \cl YVisualContainer @@ \u YControl;

r33-r44:
/ \cl AWindow @@ \u YWindow:
	/ \impl @@ \mf void Update();
	/ \impl @@ \mf void Draw();
/ \cl YDesktop @@ \u YDesktop:
	/ \impl @@ \mf void DrawDesktopObjects();
	/ \impl @@ \mf void Update();

r45-r48:
= test3:
	= \tr @@ \u YWindow;
	= \tr @@ \u YControl;

r49-r50:
/ \impl @@ \mf void Refresh() @@ \cl MWidget @@ \u YWidget;
/ \impl @@ \mf void Refresh() @@ \cl AWindow @@ \u YWindow;


DOING:


/ ...

NEXT:
* fatal error in refreshing while changing windows;
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

