// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-10-22 13:56 + 08:00;
// Version = 0.2760; *Build 165 r26;


#include "../YCLib/ydef.h"

/*

$Record prefix and abbrevations:
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
<=> ::= swaped names
@ ::= identifier
@@ ::= in / belongs to
\a ::= all
\ab ::= abstract
\ac ::= access
\bg ::= background
\c ::= const
\cb ::= catch blocks
\cl ::= classes
\cp ::= copied
\ctor ::= constructors
\cv ::= const & volatile
\d ::= derived
\de ::= default
\def ::= definitions
\dep ::= dependencies
\decl ::= declations
\dir ::= directories
\dtor ::= destructors
\e ::= exceptions
\em ::= empty
\en ::= enums
\eh ::= exception handling
\err ::= errors
\es ::= exception specifications
\evt ::= events
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
\in ::= interfaces
\init ::= initializations
\inc ::= included
\inh ::= inherited
\inv ::= invoke
\k ::= keywords
\lib ::= library
\m ::= members
\mac ::= macros
\mem ::= memory
\mf ::= member functions
\mo ::= member objects
\n ::= names
\ns ::= namespaces
\o ::= objects
\op ::= operators
\or ::= overridden
\para ::= parameters
\para.def ::= default parameters
\pt ::= points
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
\tg ::= targets
\tr ::= trivial
\tp ::= types
\u ::= units
\un ::= unnamed
\v ::= volatile
\vt ::= virtual
\val ::= values

$using:
\u YWidget
{
	\cl MWidget;
	\cl YLabel;
}
\u YControl
{
	\cl MVisualControl;
	\cl YVisualControl;
	\cl AButton;
	\cl MScrollBar;
	\cl AScrollBar;
}
\u YGUIComponent
{
	\cl YButton;
	\cl YHorizontalScrollBar;
	\cl YVirtualScrollBar;
	\cl YListBox;
	\cl YFileBox;
}
\u YWindow
{
	\cl MWindow;
	\cl AWindow;
	\cl YFrameWindow;
}

$DONE:
r1:
/= \tr \impl @@ \u Shells;

r2:
/ \impl @@ \f IVisualControl* GetTouchedVisualControl(IWidgetContainer&, Point&) @@ \un \ns @@ \u YGUI;
	* moving ignored by shell windows;

r3-r4:
/= test 1;

r5:
/ \impl @@ \f IVisualControl* GetTouchedVisualControl(IWidgetContainer&, Point&) @@ \un \ns @@ \u YGUI;
	* focus lost when moving controls quickly;

r6:
/ \tr \impl @@ \mf DrawPrevButton & \mf DrawNextButton & \mf DrawScrollArea @@ \cl YHorizontalScrollBar;

r7:
/ \impl @@ \u YObject;
	/ \decl & \impl \ctor \t @@ \cl BinaryGroup \cl Point & \cl Vec & \cl Size;

r8:
/ @@ \u YGUI:
	/ @@ \un \ns:
		+ \f void RectDrawArrow(const Graphics&, const Point&, SDST, ROT = RDeg0, Color = ColorSpace::Black);
		/ \f void RectDrawPressed(const Graphics&, const Point&, const Size&) & \f void RectDrawButtonSurface(const Graphics&, const Point&, const Size&):
			+ assertion ^ \mac YAssert;
		/ \a RectDrawFocus => WndDrawFocus;
		+ \f void WndDrawArrow(HWND, const Rect&, SDST, ROT = RDeg0, Color = ColorSpace::Black);
		/ \f void WndDrawFocus(HWND, const Point&, const Size&) -> void WndDrawFocus(HWND, const Size&);
/ \simp accessors of \cl Point & \cl Size;
- using platform::Key @@ \h Adaptor::YAdaptor;
+ copy \ctor @@ \cl (BinaryGroup & Point & Vec & Size & Rect) @@ \cl YObject;
/ @@ \decl @@ \cl MInputEventArgs @@ \u YEventArgs
	* ambiguous \m X & \m Y ^ \inh platform::Key -> \m Key k, with typedef platform::Key Key;
	+ DefConverter(Key, k);
	+ DefGetter(Key, Key, k);
/= \tr impl @@ \u Shells & \u YControl & \u YGUIComponent & \u ShlDS;

r9:
/ @@ \ns platform @@ \u YCommon:
	+ \ns ColorSpace;
	/ \en ColorSet @@ \cl Color >> \ns ColorSpace;
^ \ns ColorSpace;
/ \tr \ns \decl ^ \k using @@ \h YAdaptor & \h YWidget;

r10:
/ @@ \ns platform @@ \u YCommon:
	+ \ns KeySpace;
	/ \en KeySet @@ \cl Key >> \ns KeySpace;
^ \ns KeySpace;
/ \tr \ns \decl ^ \k using @@ \h YAdaptor;
/= \tr impl @@ \u Shells & \u ShlDS & \u YGlobal & \u YGUIComponent;

r11:
/ \tr \impl @@ \cl ShlSetting @@ \u Shells;

r12-r13:
/ \tr @@ \u Shells;
/ \tr @@ \u YGUI;
	/ \impl drawing @@ \cl YHorizontalScrollBar;

r14:
/ \tr \decl @@ \cl AScrollBar @@ \u YControl;

r15:
/ \tr \decl @@ \cl YHorizontalScrollBar;
* \impl @@ \f bool DrawRect(const Graphics&, const Point&, const Size&, Color) @@ \u YGDI;

r16:
/ tr @@ \u Shells;

r17:
* \impl @@ \f bool DrawRect(const Graphics&, const Point&, const Size&, Color) @@ \u YGDI;
- \f void DrawBounds(const Graphics&, const Point&, const Size&, Color) @@ \u YGUI;
* \impl @@ \f void RectDrawArrow(const Graphics&, const Point&, SDST, ROT, Color) @@ \un \ns @@ \u YGUIComponent;

r18:
/ @@ \un \ns @@ \u YGUIComponent:
	/ \impl @@ \f void WndDrawArrow(HWND, const Rect&, SDST, ROT, Color);
	* \impl @@ \f void RectDrawArrow(const Graphics&, const Point&, SDST, ROT, Color);

r19:
/ @@ \cl AScrollBar:
	+ typedef AVisualControl ParentType;
	/ \impl @@ \mf void DrawForeground();
+= typedef ParentType Controls::AVisualControl @@ \cl AWindow;

r20:
/ \tr @@ \u YGUI;
	/ \impl drawing @@ \cl YHorizontalScrollBar;

r21:
/ \impl @@ \u YGUIComponent:
	/ @@ \un \ns:
		/ @@ \f void WndDrawArrow(HWND, const Rect&, SDST, ROT, Color);
	/ @@ \cl YHorizontalScrollBar:
		/ @@ \mf DrawPrevButton;
		/ @@ \mf DrawNextButton;

r22:
/ @@ \cl YHorizontalScrollBar @@ \u YGUIComponent:
	/ \simp \impl @@ \mf DrawPrevButton;
	/ \simp \impl @@ \mf DrawNextButton;

r23:
/ cl AScrollBar @@ \u YControl >> \u YGUIComponent;

r24:
/= \tr \decl @@ \cl AVisualControl;
	+ \in entry void RequestToTop() \inh IVisualControl \inh IWidget;

r25:
+ \mac ImplA @@ \h Adaptor::Base;
^ \mac ImplA;
/ @@ \cl YGUIComponent:
	/ \impl @@ \cl AScrollBar;
	/ \impl @@ \cl YHorizontalScrollBar;
		+ \inh YComponent;
		+ \mf void DrawForeground();

r26:
/= \tr:
	/= code empty lines arranged @@ \cl;
/= \tr \impl @@ \ctor @@ \cl YHorizontalScrollBar;


$DOING:

/ ...


$NEXT:


* blank-clicked \evt OnClick @@ ListBox;



b170-b190:
/ fully \impl \u DSReader;
	* moving text after setting lnGap;
* non-ASCII character filename error in FAT16;
+ \impl loading pictures;
+ \impl style on widgets;


$TODO:

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
Other controls.

Other stuff to be considered to append:
Design by contract: DbC for C/C++, GNU nana.

*/

// GBK coded definitions:

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

