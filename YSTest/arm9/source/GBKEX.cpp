// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-10-03 00:38 + 08:00;
// Version = 0.2682; *Build 158 r78;


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
\mo ::= member objects
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
\un ::= unnamed
\v ::= volatile
\vt ::= virtual
\val ::= values

$using:
\u YControl
{
	\cl MVisualControl;
	\cl YListBox;
	\cl YFileBox;
}

$DONE:
r1:
/= test 0;

r2:
/ @@ \un \ns @@ \u YGUI:
	/ \impl @@ \f bool ResponseTouchUpBase(MVisualControl&, const MTouchEventArgs&);
	/ \impl @@ \f bool ResponseKeyUpBase(MVisualControl&, const MKeyEventArgs&);

r3:
/ @@ \cl MVisualControl:
	+ \eh \vt \mf void OnDrag(const Runtime::MTouchEventArgs&) ;
	/ \impl @@ \ctor;
	/ \impl @@ \eh \vt \mf void OnTouchHeld(const Runtime::MTouchEventArgs&);
/ \tr \impl @@ \cl ShlSetting @@ \u Shells:
	/ \simp \impl @@ \ctor @@ \st TFormB;
	/ \simp \impl @@ \ctor @@ \st TFormC;

r4:
/ @@ \cl MVisualControl:
	+ public \m \eh DefEvent(YTouchEventHandler, Enter);
	+ public \m \eh DefEvent(YTouchEventHandler, Leave);
	/ \impl @@ \eh \vt \mf void OnTouchMove(const Runtime::MTouchEventArgs&);
/ @@ \u YGUI
	/ @@ \cl SInputStatus:
		+ \sf void CheckTouchInBounds(Components::Controls::MVisualControl&, const MTouchEventArgs&)
		+ typedef {
			StillNotInBounds = 0,
			EnterBounds = 1,
			LeaveBounds = 2,
			StillInBounds = 3
			} TouchSwitchStateType;
		+ \sm TouchSwitchStateType TouchSwitchState;
		/ \ac @@
		{
			static TouchSwitchStateType TouchSwitchState;
			static KeyHeldStateType KeyHeldState;
			static Timers::YTimer KeyTimer;
		} ~ public -> private;
		+ \mf DefStaticGetter(TouchSwitchStateType, TouchSwitchState, TouchSwitchState);
		+ \mf DefStaticGetter(KeyHeldStateType, KeyHeldState, KeyHeldState);
	/ @@ \un \ns:
		/ MVisualControl* p_TouchDown -> \sm @@ \cl SInputState;
		/ MVisualControl* p_KeyDown -> \sm @@ \cl SInputState;
	/ \tr \impl;

r5:
/ \tr \impl @@ \cl ShlSetting @@ \u Shells;

r6-r8:
/ test 1;

r9-r10:
/ \impl @@ \cl ShlSetting @@ \u Shells;

r11-r12:
/= test 2;

r13-r17:
* \smf void CheckTouchInBounds(MVisualControl&, const MTouchEventArgs&) @@ \cl SInputState
	-> void CheckTouchedControlBounds(MVisualControl&, const MTouchEventArgs&);
/= test 3;

r18:
/ \simp @@ \cl SInputState @@ \u YGUI:
	- \smf void CheckTouchedControlBounds(MVisualControl&, const MTouchEventArgs&);
	- typedef TouchSwitchStateType;
	- \sm TouchSwitchStateType TouchSwitchState;
	- \mf DefStaticGetter(TouchSwitchStateType, TouchSwitchState, TouchSwitchState);
	/ @@ \un \ns:
		/ \impl @@ \f bool ResponseKeyUpBase(MVisualControl&, const MKeyEventArgs&);
		/ \impl @@ \f bool ResponseKeyDownBase(MVisualControl&, const MKeyEventArgs&);
		/ \impl @@ \f bool ResponseTouchUpBase(MVisualControl&, const MKeyTouchArgs&);
		/ \impl @@ \f bool ResponseTouchDownBase(MVisualControl&, const MKeyTouchArgs&);
		/ \impl @@ \f bool ResponseTouchHeldBase(MVisualControl&, const MKeyTouchArgs&);
	/@@ \u YControl
		+ DefDelegate(YInputEventHandler, IVisualControl, Runtime::MInputEventArgs);
		/ @@ \cl MVisualControl:
			/ empty \impl @@ \mf void TouchMove(const Runtime::MTouchEventArgs&);
			/ DefEvent(YTouchEventHandler, Enter) -> DefEvent(YInputEventHandler, Enter);
			/ DefEvent(YTouchEventHandler, Leave) -> DefEvent(YInputEventHandler, Leave);
	/ @@ \st ShlSetting::TFormB @@ \u Shells:
		\smf void btnB_Enter(IVisualControl& sender, const MTouchEventArgs&)
			-> void btnB_Enter(IVisualControl& sender, const MInputEventArgs&);
		\smf void btnB_Leave(IVisualControl& sender, const MTouchEventArgs&)
			-> void btnB_Leave(IVisualControl& sender, const MInputEventArgs&);

r19:
/ \simp @@ \cl MVisualControl:
	- \impl @@ \eh \vt \mf void OnTouchMove(const Runtime::MTouchEventArgs&);
	/ \impl @@ \eh \vt \mf void OnDrag(const Runtime::MTouchEventArgs&)
		-> void OnTouchMove(const Runtime::MTouchEventArgs&);
	/ \tr \impl @@ \ctor;
/ \tr \impl @@ \cl ShlSetting @@ \u Shells:
	/ \simp \impl @@ \ctor @@ \st TFormB;
	/ \simp \impl @@ \ctor @@ \st TFormC;

r20-r21:
/ test 4:
	/ \tr \impl @@ \cl ShlSetting @@ \u Shells:
		/ \simp \impl @@ \ctor @@ \st TFormB;
		/ \simp \impl @@ \ctor @@ \st TFormC;

r22:
/ @@ \u YGUI:
	/ \impl @@ \f bool ResponseTouchHeldBase(MVisualControl&, const MTouchEventArgs&)
		@@ \un \ns;
	/ @@ \cl SInputState:
		/ \sm MVisualControl* p_TouchDown -> !\sm @@ \un \ns;
		/ \sm MVisualControl* p_KeyDown -> !\sm @@ \un \ns;

r23:
/ @@ \un \ns @@ \u YGUI:
	/ \impl @@ \f bool ResponseTouchHeldBase(MVisualControl&, const MTouchEventArgs&):
		/s MVisualControl* p_TouchDown_locked(NULL) -> \un \ns;
	/ \impl @@ \f bool ResponseTouchHeld(IVisualControl&, const MTouchEventArgs&);

r24:
/ \impl @@ \f bool ResponseTouchHeld(IWidgetContainer&, const MTouchEventArgs&) @@ \u YGUI;

r25:
* \impl @@ \f bool ResponseTouchHeldBase(MVisualControl&, const MTouchEventArgs&)  @@ \un \ns @@ \u YGUI;

r26:
/ @@ \u YGUI:
	/ \@@ \un \ns:
		/ +\v @@ MVisualControl* p_TouchDown;
		/ +\v @@ MVisualControl* p_KeyDown;
		/ +\v @@ MVisualControl* p_TouchDown_locked;
		/ \impl @@ \f bool ResponseTouchUpBase(MVisualControl&, const MTouchEventArgs&):
		/ \impl @@ \f bool ResponseTouchDownBase(MVisualControl&, const MTouchEventArgs&);
	/ \impl @@ \f bool ResponseTouchUp(IWidgetContainer&, const MTouchEventArgs&);
	/ \impl @@ \f bool ResponseTouchDown(IWidgetContainer&, const MTouchEventArgs&);
	/ \impl @@ \f bool ResponseTouchHeld(IWidgetContainer&, const MTouchEventArgs&);
	+ \f bool TryEnter(IVisualControl&, const MTouchEventArgs&);
	+ \f bool TryLeave(IVisualControl&, const MTouchEventArgs&);
	+ \inc \h <stack>;

r27:
/ \impl @@ \f bool ResponseTouchUp(IWidgetContainer&, const MTouchEventArgs&) @@ \u YGUI;

r28:
/= test 5;

r29:
/ \impl @@ \f bool ResponseTouchHeld(IWidgetContainer&, const MTouchEventArgs&) @@ \u YGUI;

r30-r34:
/= test 6;

r35-r36:
/ @@ \u YGUI:
	* \impl @@ \f bool ResponseTouchHeld(IWidgetContainer&, const MTouchEventArgs&);
	* \impl @@ \f bool ResponseTouchUp(IWidgetContainer&, const MTouchEventArgs&);

r37-38:
/= test 7;

r39:
/ @@ \un \ns @@ \u YGUI:
	* \impl @@ \f bool TryEnter(IVisualControl&, const MTouchEventArgs&);
	* \impl @@ \f bool TryLeave(IVisualControl&, const MTouchEventArgs&);

r40:
* \impl @@ \f bool ResponseTouchHeld(IWidgetContainer&, const MTouchEventArgs&) @@ \u YGUI;

r41:
* \impl @@ \f bool TryEnter(IVisualControl&, const MTouchEventArgs&) @@ \un \ns @@ \u YGUI;

r42:
* \impl @@ \f bool TryLeave(IVisualControl&, const MTouchEventArgs&) @@ \un \ns @@ \u YGUI;

r43-r44:
/= test 8;

r45:
/ \impl @@ \f bool ResponseTouchUp(IWidgetContainer&, const MTouchEventArgs&) @@ \u YGUI;

r46-r48:
/ \simp \u YGUI:
	+ \f MVisualControl* GetTouchedVisualControl(IWidgetContainer&, SPoint&) @@ \un \ns;
	/ \simp \f of touching input;

r49-r53:
/ \impl \f MVisualControl* GetTouchedVisualControl(IWidgetContainer&, SPoint&) @@ \un \ns @@ \u YGUI;

r54-r58:
/= test 9;

r59-r60:
/ \impl \f MVisualControl* GetTouchedVisualControl(IWidgetContainer&, SPoint&) @@ \un \ns @@ \u YGUI;

r61:
/ @@ \u YGUI:
	/ impl @@ \f bool ResponseTouchUpBase(MVisualControl&, const MTouchEventArgs&);
	/ impl @@ \f bool ResponseTouchDownBase(MVisualControl&, const MTouchEventArgs&);

r62-r65:
/ \ns @@ \u YGUI:
	/ @@ \un:
		* \impl @@ \f bool TryEnter(IVisualControl&, const MTouchEventArgs&);
		* \impl @@ \f bool TryLeave(IVisualControl&, const MTouchEventArgs&);
	* impl @@ \f bool ResponseTouchDownBase(MVisualControl&, const MTouchEventArgs&);

r66-r67:
/ \impl \f MVisualControl* GetTouchedVisualControl(IWidgetContainer&, SPoint&) @@ \un \ns @@ \u YGUI;

r68-r73:
/= test 10;

r74:
* \impl \f MVisualControl* GetTouchedVisualControl(IWidgetContainer&, SPoint&) @@ \un \ns @@ \u YGUI;

r75:
/= test 11;
	/ \tr @@ \u Shells;

r76-r78:
/ \tr \impl @@ \smf void SInputStatus::RepeatKeyHeld(MVisualControl&, const MKeyEventArgs&) @@ \u YGUI;


$DOING

/ ...

$NEXT:
* Widget bounds err;
/ fully \impl \u DSReader;
	* moving text after setting lnGap;
* non-ASCII character filename error in FAT16;
+ \impl loading pictures;

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
Buttons;
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

