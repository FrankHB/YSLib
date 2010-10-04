// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-10-04 22:16 + 08:00;
// Version = 0.2687; *Build 159 r26;


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
/ @@ \un \ns @@ \u YGUI;
	- int ExtraOperationSetting;
	+
	{
		namespace ExOp
		{
			typedef enum
			{
				NoOp = 0,
				TouchUp = 1,
				TouchDown = 2,
				TouchHeld = 3
			} ExOpType;
		};
		ExOp::ExOpType ExtraOperation(NoOp);
	};
	/ \impl @@ \f MVisualControl* GetTouchedVisualControl(IWidgetContainer&, SPoint&);

r2-r4:
/ \tr \impl @@ \ctor @@ \st ShlSetting::TFormB @@ \u Shells;
/ \impl @@ \f MVisualControl* GetTouchedVisualControl(IWidgetContainer&, SPoint&) @@ \un \ns @@ \u YGUI;

r5-r13:
/ test 1;

r14-r16:
* \impl @@ \f MVisualControl* GetTouchedVisualControl(IWidgetContainer&, SPoint&) @@ \un \ns @@ \u YGUI;

r17-r20:
/ @@ \un \ns @@ \u YGUI:
	/ \impl @@ \f MVisualControl* GetTouchedVisualControl(IWidgetContainer&, SPoint&);
	- std::stack<std::pair<IVisualControl*, MTouchEventArgs> > LeaveStack;

r21:
/ \impl @@ \ctor @@ \cl YFileBox;

r22:
/ @@ \un \ns @@ \u YGUI:
	/ \impl @@ \f MVisualControl* GetTouchedVisualControl(IWidgetContainer&, SPoint&);
	/ \simp \impl @@ \f bool TryEnter(IVisualControl&, const MTouchEventArgs&);
	/ \simp \impl @@ \f bool TryLeave(IVisualControl&, const MTouchEventArgs&);

r23:
/ \tr \impl @@ \ctor @@ \st ShlSetting::TFormB @@ \u Shells;

r24:
/ @@ \cl MWidget:
	+ \i \vt \mf void Fill();
	+ \vt \mf void Fill(PixelType);
/ @@ \cl YVisualControl:
	/ \simp \impl @@ \mf void DrawBackground();
/ @@ \cl YLabel:
	/ \simp \impl @@ \mf void DrawForeground();
/ @@ \cl AWindow:
	+ \or \vt \mf void Fill(PixelType);
	/ \ac @@ private \mf void bool DrawBackgroundImage();-> public;
	/ \simp \impl @@ void DrawBackground();

r26:
+ \u YGUIComponent "yguicomp";
/ @@ \u YControl:
	/ cl YButton >> \u YGUIComponent;
	/ cl YListBox >> \u YGUIComponent;
	/ cl YFileBox >> \u YGUIComponent;
	-= \inc "../Core/yexcept.h";
	/= @@ \ns Components::Controls @@ \impl:
		-= using namespace Drawing;
		-= using namespace Widgets;
/ @@ \h YWindow:
	/ \inc "ycomponent.h" -> "yguicomp.h";
	/= @@ \ns Components::Controls @@ \impl:
/ @@ \u YGUIComponent:
	/= @@ \ns Components @@ \impl:
		-= using namespace Drawing;
		-= using namespace Widgets;


$DOING

/ ...

$NEXT:
b159:
+ \impl YButton;
b160-b190:
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

