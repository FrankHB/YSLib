// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-10-17 22:51 + 08:00;
// Version = 0.2743; *Build 163 r19;


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
/= @@ \cl AWindow @@ \u AWindow:
	-= \mf void SetBounds(const Rect&);
	/= \mf void Fill(PixelType) ^ \mac PDefH;

r2-r7:
/= test 1;

r8:
/ \ns Widgets @@ \u YWidget:
	/ \f Point GetLocationOffset(IWidget*, const Point&, const HWND&)
	-> Point GetLocationOffset(const IWidget*, const Point&, const HWND&);
	/ \simp \impl @@ \mf Point GetWindowLocationOffset(const Point&) const @@ \cl YWidgetContainer;
/ \simp \impl @@ \mf Point GetWindowLocationOffset(const Point&) const @@ \cl YFrameWindow @@ \u YWindow;

r9-r10:
/= test 2;

r11:
/ unnecessary \ac protected -> private @@ \h YFunc;

r12:
/= \tr \decl @@ \h YCounter ^ \mac DefGetter & DefStaticGetter;

r13:
- \def \para:
	/ \a \mf ReleaseFocus(const MEventArg& = GetZeroElement<MEventArgs>());
	/ \a \mf RequestFocus(const MEventArg& = GetZeroElement<MEventArgs>());
	/ \a \mf SetVisible(bool = true);
	/ \a \mf SetTransparent(bool = true);
	/ \a \mf SetBgRedrawed(bool = true);
	/ \a \mf SetRefresh(bool = true);
	/ \a \mf SetUpdate(bool = true);
	/ \a \mf SetEnabled(bool = true);
	/ \a \mf Point GetContainerLocationOffset(const Point& = Point::Zero) const;
	/ \a \mf Point GetWindowLocationOffset(const Point& = Point::Zero) const;

r14:
/ @@ \u YWidget:
	/ @@ \cl MVisual:
		/ unnecessary \ac public -> protected;
		/ unnecessary \ac protected -> private;
		- \vt @@ \mf void SetLocation(SPOS, SPOS);
		- \vt @@ \mf void SetSize(SPOS, SPOS);
		+ \vt \mf void SetSize(Size&);
		/ virtual DefGetter(Rect, Bounds, Rect(Location, Size.Width, Size.Height))
			-> DefGetter(Rect, Bounds, Rect(GetLocation(), GetSize()));
		+= \vt @@ \mf void Contains(Point&);
		/ \impl @@ \mf void Contains(Point&);
		/ \mf void Contains(const int&, const int&) -> void Contains(SPOS, SPOS);
		/= \mf DefGetter(SPOS, X, Location().X) -> DefGetter(SPOS, X, GetLocation().X);
		/= \mf DefGetter(SPOS, Y, Location().Y) -> DefGetter(SPOS, Y, GetLocation().Y);
		/= \mf DefGetter(SDST, Width, Size.Width) -> DefGetter(SDST, Width, Size.Width);
		/= \mf DefGetter(SDST, Height, Size.Height) -> DefGetter(SDST, Height, Size.Height)
		- \a \vt @@ getters @@ \mf;
		/ \mf public void SetSize(SPOS, SPOS) -> private void _m_SetSize(SPOS, SPOS);
		+ \mf public void SetSize(SPOS, SPOS);
		/ \impl @@ \mf \vt void SetSize(Drawing::Size&);
	/ @@ \cl MVisual:
		- \a \vt @@ !\dtor @@ \mf;
/= \tr \impl @@ \u Shells:
	\ctor @@ \cl ShlLoad::TFrmLoadDown;
	\ctor @@ \cl ShlSetting::TFormA;
	\mf ShlSetting::TFormC::btnC_KeyPress;
/= \tr \impl @@ \u YGUIComponent:
	/ \mf YButton::DrawForeground();
	/ \mf YListBox::DrawForeground();
/= \tr \impl @@ \ctor @@ \cl YFrameWindow @@ \u YWindow;
+ \mac ImplI @@ \h Adaptor::Base;
/ ^ \mac ImplI @@ \a \cl \impl \in;
/= \tr \impl @@ \u YComponent:
	/ ^ \mac DefGetter @@ \cl \t GMContainer;
	/ ^ \mac PDefH & DefGetter & PDefHOperator @@ \cl \t GMFocusResponser;
	/ ^ \mac PDefHOperator @@ \cl \t GSequenceViewer;
/ @@ \cl AWindow @@ \u YWindow:
	/ \mf \vt void SetSize(SDST, SDST) -> void SetSize(const Drawing::Size&);

r15:
/ \tr \impl @@ \u YGDI;

r16:
/ @@ \u YWindow:
	/ @@ \u YFrameWindow:
		\vt \mf Point GetContainerLocationOffset(const Point&) const >> \cl Awindow;
		\vt \mf Point GetWindowLocationOffset(const Point&) const >> \cl Awindow;

r17:
/ \simp @@ \u YWidget:
	/ @@ \ns Widgets:
		+ \i \f Point GetContainerLocationOffset(const IWidget&, const Point&);
		+ \i \f Point GetWindowLocationOffset(const IWidget&, const Point&);
	/ @@ \cl YWidgetContainer:
		- \mf Point GetContainerLocationOffset(const Point&) const;
		- \mf Point GetWindowLocationOffset(const Point&) const;
	/ @@ \in IWidgetContainer:
		- \mf Point GetContainerLocationOffset(const Point&) const;
		- \mf Point GetWindowLocationOffset(const Point&) const;
	/ @@ \cl MWidget:
		/ \impl @@ \mf Point GetLocationForParentContainer() const;
		/ \impl @@ \mf Point GetLocationForParentWindow() const;
/ @@ \u YWindow:
	/ @@ \cl AWindow:
		- \mf Point GetContainerLocationOffset(const Point&) const;
		- \mf Point GetWindowLocationOffset(const Point&) const;

r18-r19:
/ \vt \inh \impl @@ (\in & \ab \cl)\dir Shell;
/ \tr \impl @@ \u Shells;


$DOING:

/ ...


$NEXT:
{

/ \impl @@ \cl \t GHHandle @@ \h YReference:
	/ static_cast -> dynamic_cast @@ \ctor;
};

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

