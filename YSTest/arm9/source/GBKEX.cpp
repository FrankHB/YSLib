// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-11-09 06:45 + 08:00;
// Version = 0.2822; *Build 169 r46;


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
\amb ::= ambiguities
\amf ::= abstract/pure virtual member function
\as ::= assertions
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
\ln ::= lines
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
\parm ::= parameters
\parm.de ::= default parameters
\pt ::= points
\ptr ::= pointers
\q ::= qualifiers
\rem ::= remarked
\ren ::= renamed
\ref ::= references
\refact ::= refactorings
\res ::= resources
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
	\in IWidget;
	\in IUIBox;
	\cl Widget;
	\cl YUIContainer;
	\cl YLabel;
}
\u YControl
{
	\in IControl;
	\in IVisualControl;
	\cl MVisualControl;
	\cl YControl;
	\cl AVisualControl;
	\cl YVisualControl;
}
\u YGUIComponent
{
	\cl AButton;
	\cl YThumb;
	\cl YButton;
	\cl ATrack;
	\cl MScrollBar;
	\cl AScrollBar;
	\cl YHorizontalScrollBar;
	\cl YVerticalScrollBar;
	\cl YListBox;
	\cl YFileBox;
}
\u YWindow
{
	\in IWindow;
	\cl MWindow;
	\cl AWindow;
	\cl YFrameWindow;
}

$DONE:
r1:
= test 1;

r2-r6:
/ \impl @@ \f void OnTouchHeld(IVisualControl&, const TouchEventArgs&)
	@@ \u YControl;

r7:
/ \impl @@ \f bool ResponseTouchHeldBase(IVisualControl&, const TouchEventArgs&)
	@@ \un \ns @@ \u YGUI;

r8-r17:
/= test 2;

r18:
/ \impl @@ \f void OnTouchHeld(IVisualControl&, const TouchEventArgs&)
	@@ \u YControl;

r19:
/ \a VisualControlLocationOffset => VisualControlLocation;

r20:
* \impl @@ \f void OnTouchHeld(IVisualControl&, const TouchEventArgs&)
	@@ \u YControl;

r21-r24:
/= test 3;

r25:
/ @@ \u YWidget:
	+ \f Point LocateForWidget(IWidget&, IWidget&) @@ \ns Widgets;
	+ \f void GetContainersListFrom(IWidget&, std::map<IUIBox*, Point>&)
		@@ \un \ns;
* \impl @@ \f bool ResponseTouchHeldBase(IVisualControl&, const TouchEventArgs&)
	@@ \un \ns @@ \u YGUI;

r26-r27:
/= test 4;

r28:
/ @@ \u YWidget:
	* \impl @@ \f Point LocateForWidget(IWidget&, IWidget&) @@ \ns Widgets;
	* \impl @@ \f void GetContainersListFrom(IWidget&, std::map<IUIBox*, Point>&)

r29:
* \impl @@ \f IVisualControl* GetTouchedVisualControlPtr(IUIBox&, Point&)
	@@ \un \ns @@ \u YGUI;

r30:
/ \impl @@ \f void OnDrag(IVisualControl&, const TouchEventArgs&)
	@@ \u YControl;

r31:
/ - \c @@ \a \parm '*Args*';
/ \impl @@ \mf bool SetFocusingPtr(_type*) @@ \cl \t GMFocusResponser
	@@ \u YFocus;
/ \impl \i \f @@ \u ShlDS;
/ \tr \impl @@ \u Shells;
/ \impl @@ \ctor @@ \cl YApplication @@ \u YApplication;
/ \impl @@ \f OnTouchHeld @@ \u YControl;
/ \impl @@ \f @@ \un \ns @@ \u YGUI;
/ \impl @@ \mf (CallSelected & CallConfirmed) \cl YListBox @@ \u YGUIComponent;

r32:
/ @@ \u YControl:
	+ \o TouchEventArgs LastVisualControlLoication @@ \un \ns;
	/ \f OnTouchMove -> OnDrag;
	/ \impl @@ \f OnTouchHeld;
	+ \f OnTouchMove;

r33:
* \tr @@ \u Shells;

r34:
/ \o TouchEventArgs LastVisualControlLoication @@ \un \ns @@ \u YControl
	-> Point LastVisualControlLoication @@ \ns InputStatus @@ \u YGUI;
* \mf OnTouchMove_Thumb -> \mf OnDrag_Thumb
	@@ \cl (YHorizontalTrack & YVerticalTrack) @@ \u YGUIComponent;

r35:
* \impl @@ \f OnDrag @@ \u YControl;

r36:
* \impl @@ \f GetTouchedVisualControlPtr @@ \un \ns @@ \u YGUI;

r37:
/ \impl @@ \ctor @@ \cl ATrack @@ \u YGUIComponent;

r38:
/ \impl @@ \mf OnTouchDown @@ \cl (YHorizontalTrack & YVerticalTrack)
	@@ \u YGUIComponent;

r39:
/= test 5;

r40:
* \impl @@ \mf OnTouchDown @@ \cl (YHorizontalTrack & YVerticalTrack)
	@@ \u YGUIComponent;

r41:
* \impl @@ \f OnDrag & \f OnTouchHeld @@ \u YControl;
/ \simp @@ \impl @@ \mf OnTouchDown @@ \cl (YHorizontalTrack & YVerticalTrack)
	@@ \u YGUIComponent;

r42-r43:
/= test 6;

r44:
* \impl @@ \mf OnTouchDown @@ \cl (YHorizontalTrack & YVerticalTrack)
	@@ \u YGUIComponent;

r45:
/= test 7;

r46:
* \impl @@ \f GetTouchedVisualControlPtr @@ \un \ns @@ \u YGUI;


$DOING:

/ ...


$NEXT:
* \evt Leave \parm err;
	/ should offset to original, not destination visual control;
* behavior when touching out of widget bounds;
* behavior of ATrack when touch point hoding in bounds and around the thumb;
* blank-clicked \evt OnClick @@ ListBox;

b170-b190:
/ fully \impl \u DSReader;
	* moving text after setting lnGap;
* non-ASCII character filename error in FAT16;
+ \impl loading pictures;
+ \impl style on widgets;
* fatal \err:
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
	-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 02055838
	ftc_snode_weight
	ftcsbits.c:271

$TODO:

Clarify the log levels.

Make "UpdateFont()" more efficienct.

More efficient @YTextRegion output:
Use in-buffer background color rendering and function @CopyToScreen()
	to inplements @YTextRegion background;
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

