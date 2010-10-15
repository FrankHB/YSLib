// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-10-15 17:02 + 08:00;
// Version = 0.2738; *Build 162 r24;


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
\cp ::= copied
\ctor ::= constructors
\cv ::= const & volatile
\d ::= derived
\de ::= default
\def ::= definitions
\decl ::= declations
\dir ::= directories
\dtor ::= destructors
\e ::= exceptions
\em ::= empty
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
\in ::= interfaces
\init ::= initializations
\inc ::= included
\inh ::= inherited
\inv ::= invoke
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
}
\u YGUIComponent
{
	\cl YButton;
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
/ \simp @@ \cl YButton @@ \u YGUIComponent:
	- \inh \cl AButton;
	+ \int \cl MButton;
	+ \inh \cl Widgets::MLabel;
	- \mf \vt void OnConfirmed(const MIndexEventArgs&);
	/ \impl @@ \ctor;
- \cl AButton @@ \u YControl;

r2:
/ @@ \cl YControl:
	+ \cl MScrollBar;
	+ \cl AScrollBar;
	/ \impl @@ \cl MButton:
		/ \ac \exp \ctor ~ public -> protected; 
/ @@ \cl YGUIComponent:
	+ \cl YHorizontalScrollBar;
	+ \cl YVerticalScrollBar;
+ \f \t template<class _tBinary> _tBinary Transpose(_tBinary&) @@ \u YObject;

r3:
/ \a DefEmptyDtor => DefEmptyDtor;
+ \vt DefEmptyDtor(GMContainer) @@ \cl \t GMContainer @@ \u YModule;
+ \vt DefEmptyDtor(MWidgetContainer) @@ \cl MWidgetContainer @@ \u YWidget;

r4:
/ @@ \h YEvent:
	+ \cl \t template<class _tEventHandler> struct GSEventTemplate;
	+ \mac DeclIEventEntry(_tEventHandler, _name);
	+ \mac DefEventGetter(_tEventHandler, _name);
\a SStringTemplate => GSStringTemplate;

r5:
/ @@ \u YControl:
	/ \decl @@ \in IVisualControl;
	/ \decl @@ \cl YVisualControl;
/ @@ \h YEvent:
	* \mac DeclIEventEntry(_tEventHandler, _name);
	* \mac DefEventGetter(_tEventHandler, _name);
/ @@ \u YWindow:
	/ \decl @@ \cl AWindow;

r6:
/ @@ \u YGUI:
	/ \a MVisualControl -> IVisualControl;
	/ \a event -> EventGetter @@ \impl;
/ @@ \u YControl:
	/ + \tb @@ \impl @@ \mf void MVisualControl::OnKeyHeld(const Runtime::MKeyEventArgs&);

r7:
/= ^ DefEmptyDtor @@
	{
		\cl AWindow;
		\cl YDesktop @@ \u YDesktop;
		\cl YTextFile @@ \u YFile_(Text);
	}
/= \impl @@ \dtor @@ \cl YFontFamily @@ \u YFont;
/= \simp @@ \cl MDesktopObject:
	/= \mf bool BelongsTo(YDesktop*) const ^ \mac PDefHead;
	/= \mf YDesktop* GetDesktopPtr() const ^ \mac DefGetter;
/= \i \mf void YListBox::ClearSelected() ^ \mac PDefHead;
/ \a:
	{
		PDefHead => PDefH;
		PDefOpHead => PDefHOperator;
	};

r8:
/ @@ \u YControl:
	+ \cl AVisualControl;
	/ \impl @@ \cl YVisualControl;
	/ \impl @@ \cl AScrollBar;
		+ \inh AVisualControl;

r9:
/ @@ \u YModule:
	/ \cl \t GMContainer >> \u YComponent;
	/ \cl \t GMFocusResponser @@ \ns Runtime >> \ns Component @@ \u YComponent;
	/ \in \t GIFocusRequester @@ \ns Runtime >> \ns Component @@ \u YComponent;
	/ \cl AFocusRequester @@ \ns Runtime >> \ns Component @@ \u YComponent;
	/ \cl \t GSequenceViewer >> \u YComponent;
- \u YModule;
/ @@ \u YComponent:
	+ \inc "../Core/yevt.hpp";
	+ \inc "../Core/yevtarg.h";
/ @@ \u YWidget:
	- \inc \u YModule;
/= \a _Tp => _type;
* \tg rebuild @@ makefile;

r10:
/ @@ \u YControl:
	+ \cl MThumb;
	+ \cl MTrack;
	+ \cl MDraggableObject;
	+ \in IDraggable;
	/ \impl @@ \cl MScrollButton;
/ \a:
	{
		GHEvent => GEventHandler;
		YInputEventHandler => InputEventHandler;
		YTouchEventHandler => TouchEventHandler;
		YKeyEventHandler => KeyEventHandler;
		YIndexEventHandler => IndexEventHandler;
		EventHandler => EventHandler;
		EventHandler => EventHandler;
		\cl YEvent => Event;
		GIHEventHandler => GIEventHandler;
		IHEventHandler => IEventHandler;
	};

r11-r12:
/ \impl @@ \cl MThumb & \cl MTrack & \cl MScrollBar & \cl AScrollBar & \cl YHorizontalScrollBar;
/ @@ \u YControl:
	- \cl MThumb;
	- \cl MTrack;
	- \cl MDraggableObject;
	- \in IDraggable;
	/ \impl @@ \cl MScrollButton;

r13-r14:
/ @@ \cl InputState @@ \u YGUI:
	typedef enum
	{
		KeyFree = 0,
		KeyPressed = 1,
		KeyHeld = 2
	} KeyHeldStateType;
	->
	typedef enum
	{
		Free = 0,
		Pressed = 1,
		Held = 2
	} HeldStateType @@ \ns InputStatus;
	/ \sm Timers::YTimer HeldTimer -> Timers::YTimer HeldTimer;
	/ \smf void RepeatKeyHeld(Components::Controls::IVisualControl&, const MKeyEventArgs&)
		-> void RepeatHeld(HeldStateType&, Components::Controls::IVisualControl&, const MKeyEventArgs&);
	/ \sm 
	{
		Vec DragOffset;
		HeldStateType KeyHeldState;
		Timers::YTimer HeldTimer;
	} >> \un \ns;
	/ \cl InputStatus -> \ns InputStatus;
	- \f HeldStateType GetKeyHeldState();
	/ \f void ResetKeyHeldState()
		-> void ResetHeldState(HeldStateType&);
	+ \o HeldStateType TouchHeldState;
/ @@ \ns Timers @@ \u YTimer:
	/ typedef u32 TimeSpan;
	/ \impl @@ \cl YTimer ^ TimeSpan;
	/ \f void RepeatHeld(HeldStateType&, Components::Controls::IVisualControl&, const MKeyEventArgs&);
		-> void RepeatHeld(HeldStateType&, Components::Controls::IVisualControl&, const MKeyEventArgs&, Timers::TimeSpan = 240, Timers::TimeSpan = 120);
/ @@ \cl MVisualControl:
	/ \tr \impl @@ \mf void OnTouchHeld(const Runtime::MKeyEventArgs&);
	/ \tr \impl @@ \mf void OnKeyHeld(const Runtime::MKeyEventArgs&);
/ @@ \un \ns @@ \u YGUI:
	+ using namespace InputStatus;
	/ \tr \impl @@ \f bool ResponseKeyUpBase(IVisualControl&, const MKeyEventArgs&);
	/ \tr \impl @@ \f bool ResponseKeyHeldBase(IVisualControl&, const MKeyEventArgs&);	

r15:
/ @@ \u YGUI:
	/ \f void RepeatHeld(HeldStateType&, Components::Controls::IVisualControl&, const MKeyEventArgs&, Timers::TimeSpan = 240, Timers::TimeSpan = 120);
		-> bool RepeatHeld(HeldStateType&, const MKeyEventArgs&, Timers::TimeSpan = 240, Timers::TimeSpan = 120);
/ @@ \cl MVisualControl:
		/ \tr \impl @@ \mf void OnKeyHeld(const Runtime::MKeyEventArgs&);

r16-r19:
/ \impl @@ \mf void MVisualControl::OnTouchHeld(const Runtime::MTouchEventArgs&);
* @@ \u YGUI:
	+ \def HeldStateType TouchHeldState(Free);

r20:
/ @@ \un \ns @@ \u YGUI:
	/ \impl @@ \f bool ResponseTouchUpBase(IVisualControl&, const MTouchEventArgs&);
	/ \impl @@ \f bool ResponseTouchHeldBase(IVisualControl&, const MTouchEventArgs&);

r21:
/= test 1;

r22:
* @@ \un \ns @@ \u YGUI:
	/ \impl @@ \fbool ResponseTouchUpBase(IVisualControl&, const MTouchEventArgs&);
	/ \impl @@ \fbool ResponseTouchDownBase(IVisualControl&, const MTouchEventArgs&);
	/ \impl @@ \fbool ResponseTouchHeldBase(IVisualControl&, const MTouchEventArgs&);
* @@ \cl MVisualControl:
	/ \impl @@ \mf OnTouchHeld(const Runtime::MTouchEventArgs&);

r23-r24:
/ test 2;
/ @@ \cl MVisualControl:
	/ \impl @@ \mf OnTouchHeld(const Runtime::MTouchEventArgs&);

$DOING:

/ ...


$NEXT:
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

