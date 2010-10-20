// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-10-20 09:24 + 08:00;
// Version = 0.2755; *Build 164 r22;


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
/ @@ \u YWindow:
	/ \inh \cl Controls::MVisualControl @@ \cl MDesktopObject >> \cl AWindow;

r2:
/ \cl AWindow @@ \u YWindow:
	/ \inh Widgets::MWidget & Controls::MVisualControl -> Controls::AVisualControl;
	- \mf RequestFocus;
	- \mf ReleaseFocus;

r3-r5:
/ \simp \cl AWindow @@ \u YWindow;

r6:
/ \u YControl:
	/ \mf
	{
		virtual void
		OnGotFocus(const MEventArgs&);
		virtual void
		OnLostFocus(const MEventArgs&);
		virtual void
		OnKeyHeld(const Runtime::MKeyEventArgs&);
		virtual void
		OnTouchDown(const Runtime::MTouchEventArgs& = Runtime::MTouchEventArgs::Empty);
		virtual void
		OnTouchHeld(const Runtime::MTouchEventArgs&);
		virtual void
		OnTouchMove(const Runtime::MTouchEventArgs&);
	} @@ \cl MVisualControl >> \cl AVisualControl & \simp;
	/ \impl @@ \ctor @@ \cl MVisualControl;
	/ \impl @@ \ctor @@ \cl AVisualControl;
/ \impl @@ \ctor @@ \cl YFileBox @@ \u YGUIComponent;

r7:
/ @@ \in IVisualControl + \vt \inh IWidget @@ \u YVisualControl;
/ \impl @@ \u YDesktop;
/ \impl @@ \u YGUI;
/ \impl @@ \u YGUIComponent;
/ \impl @@ \u YWidget;

r8:
/ \simp @@ \cl AVisualControl @@ \u YVisualControl:
	- \vt \inh \impl IWidget;

r9:
/ \tr \impl @@ \h Adaptor::YReference;

r10:
/ @@ \h YShellHelper:
	/ \tr \impl;
	+ \mac DefDynInitRef;
/ @@ \u Shells;
	^ \mac DefDynInitRef;

r11:
/= \tr \decl @@ \cl YVisualControl @@ \u YControl:
	^ \mac ImplI;
/ \tr @@ \ctor @@ \cl YHorizontalScrollBar;
/ \tr @@ \ctor @@ \cl AScrollBar;

r12:
* \a *MaxThumbSize* => *MinThumbSize*;
/ \tr \impl \mf @@ \cl YHorizontalScrollBar;
* \a GraphicInterfaceContext => GraphicsInterfaceContext;
* \impl @@ \mf YButton::DrawForeground();

r13:
* \tr \impl @@ %.cpp \dep @@ \makefile arm9;

r14:
* \impl @@ \mf void MWidget::Fill(Color);
/ \tr \impl @@ \mf void MLabel::PaintText(MWidget&);
* \impl @@ \mf YListBox::DrawForeground();

r15:
/ \decl @@ \in IWindow:
	- \mf DeclIEntry(const Drawing::MBitmapBuffer& GetBuffer() const);
	- \mf DeclIEntry(BitmapPtr GetBufferPtr() const);
/ \tr @@ \u YGUIComponent:
	/ \decl & \impl @@ \f @@ \un \ns;
	/ \impl @@ \mf void YButton::DrawForeground();
/ \impl @@ \mf UpdateToWindow @@ \cl AWindow;
/ \simp \a GraphicsInterfaceContext => Graphics;
/ \ac @@ \m hWindow @@ \cl MWidget -> private ~ public;

r16:
/ @@ \u YGDI:
	+ \f
	{
		!\i bool FillRect(const Graphics&, const Point&, const Size&, Color);
		\i bool FillRect(const Graphics&, const Rect&, Color);
	}
	- \f
	{
		bool DrawRect(const Graphics&, SPOS, SPOS, SPOS, SPOS, Color);
		\i bool DrawRect(const Graphics&, const Point&, const Point&, Color);
	}
	/ -\i @@ \f bool DrawRect(const Graphics&, const Point&, const Size&, Color);
	/ \impl @@ \i \f bool DrawRect(const Graphics&, const Rect&, Color);
	/= DefPredicate(Valid, pBuffer && Size.Width && Size.Height) @@ \u Graphics
		-> DefPredicate(Valid, pBuffer != NULL && Size.Width != 0 && Size.Height != 0);
/ \impl \f void RectDrawButtonSurface(const Graphics&, const Point&, const Size&) @@ \un \ns @@ \u YGUIComponent;
/ @@ \u YGUI:
/ \f void DrawBounds(const Graphics&, const Point&, const Size&, PixelType)
	-> void DrawBounds(const Graphics&, const Point&, const Size&, Color);
/ \f void DrawWindowBounds(HWND, PixelType)
	-> void DrawBounds(HWND, Color);
/ \f void DrawWidgetBounds(IWidget&, PixelType)
	-> void DrawWidgetBounds(IWidget&, Color);

r17:
/ \impl @@ \f IVisualControl* GetTouchedVisualControl(IWidgetContainer&, Point&) @@ \un \ns @@ \u YGUI;

r18:
/ \simp \impl @@ \u YGUI;

r19-r21:
/= test 1;

r22:
/ \impl @@ \u YGUI;
	* quickly leave controls to another without \evt OnLeave;
	* \pt \err @@ \evt OnEnter;

$DOING:

/ ...


$NEXT:
* blank-clicked \evt OnClick @@ ListBox;
* fatal \err;
/ \impl @@ \cl YHorizontalScrollBar;

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

