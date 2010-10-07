// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-10-08 00:38 + 08:00;
// Version = 0.2697; *Build 160 r19;


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
\n ::= names
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
/ @@ \u YControl:
	+ \cl MButton;
	+ \cl AButton;
/ @@ \u YGUIComponent:
	/= \decl @@ \cl YButton:
		+ \inh public AButton;
		- \inh public Widgets::MLabel;
/ \tr \decl @@ \cl MWindow;

r2-r5:
/ test 1;
	/ @@ \u YWidget:
		/ \mf void DrawBackground() @@ \cl YWidget >> \cl MWidget;
		/ \mf void DrawForeground() @@ \cl YWidget >> \cl MWidget;
		/ \decl @@ \mf void DrawBackground() @@ \cl YWidget ^ \cl MWidget;
		/ \decl @@ \mf void DrawForeground() @@ \cl YWidget ^ \cl MWidget;
	/ @@ \u YControl:
		/ \decl @@ \mf void DrawBackground() @@ \cl YVisualControl ^ \cl MWidget;
		/ \decl @@ \mf void DrawForeground() @@ \cl YVisualControl ^ \cl MWidget;

r6:
/ @@ \u YWidget:
	/ \decl @@ \mf void DrawBackground() @@ \cl YWidgetContainer ^ \cl MWidget;
	/ \decl @@ \mf void DrawForeground() @@ \cl YWidgetContainer ^ \cl MWidget;

r7:
/= @@ \u YGUIComponent:
	+= \un \ns;
	/= \s \f (\i transPixelEx) & RectDrawFocusDefault & (\i RectOnGotFocus) (>> \ns) & -\s;

r8:
/ \decl @@ \cl AButton;
/ \decl & \impl \cl YButton;
+ DefPredicate(Pressed, bPressed) @@ \cl MButton;

r9:
/ \a SInputStatus => InputStatus;
/ \a SOBG => BinaryGroup;
/ \a SPoint => Point;
/ \a SVec => Vec;
/ \a SSize => Size;
/ \a SRect => Rect;

r10:
/= \a EventHandlerSet => EventHandlerList;

r11:
/ \cl \t @@ GEvent<true> @@ \u YEvent:
	+ \de \ctor;
	+ typedef std::list<_tEventHandler> ListType;
	/= \m std::list<_tEventHandler> EventHandlerList -> ListType EventHandlerList;
	/ mf void operator()(_tSender&, const _tEventArgs&)
		-> void operator()(_tSender&, const _tEventArgs&) const;

r12:
/ @@ \u YCommon:
	+ \i \ctor Color(u8, u8, u8, bool = true) @@ \ns platform @@ \cl Color;
	/ \f void YDebugBegin(PixelType fc = RGB15(31, 31, 31), PixelType bc = RGB15( 0, 0, 31));
		-> void YDebugBegin(Color = Color::White, PixelType = Color::Blue);
	/ \f YConsoleInit(u8 dspIndex, PixelType fc = Color::White, PixelType bc = Color::Black)
		-> YConsoleInit(u8 dspIndex, Color fc = Color::White, Color bc = Color::Black);
/ @@ \u YGUIComponent:
	/ @@ \un \ns:
		+ \f void RectDrawButtonSurface(const Point&, const Size&, HWND);
		/ \impl @@ \f void RectDrawFocusDefault(const Point&, const Size&, HWND);
			^ \n Color;
		/ \impl @@ \i \f void transPixelEx(BitmapPtr);
			^ \n Color;
		/ \impl @@ \mf YListBox::DrawForeground();
			^ \n Color;
/ @@ \cl YConsole @@ \u YComponent:
	/ \exp \ctor YConsole(YScreen& = *pDefaultScreen, bool = true, Drawing::PixelType = Drawing::Color::White, Drawing::PixelType = Drawing::Color::Black);
		-> YConsole(YScreen& = *pDefaultScreen, bool = true, Drawing::Color = Drawing::Color::White, Drawing::Color = Drawing::Color::Black);
	/ \i \mf void Activate(Drawing::PixelType = Drawing::Color::White, Drawing::PixelType = Drawing::Color::Black)
		-> void Activate(Drawing::Color = Drawing::Color::White, Drawing::Color = Drawing::Color::Black);
/ \exp \ctor YDesktop(YScreen&, PixelType = 0, GHResource<Drawing::YImage> = NULL)
	-> YDesktop(YScreen&, Color = 0, GHResource<Drawing::YImage> = NULL) @@ \u YDesktop;
/ @@ \u YWidget:
	/ @@ \cl MVisual:
		/ \m PixelType BackColor -> Color BackColor;
		/ \m PixelType ForeColor -> Color BackColor;
		/ \exp \ctor MVisual(const Rect& = Rect::Empty, PixelType = Color::White, PixelType = Color::Black)
			-> MVisual(const Rect& = Rect::Empty, Color = Color::White, Color = Color::Black);
	/ @@ \cl MWidget:
		/ \exp \ctor MWidget(HWND = NULL, const Rect& = Rect::Empty, IWidgetContainer* = NULL, PixelType = Color::White, PixelType = Color::Black);
			-> MWidget(HWND = NULL, const Rect& = Rect::Empty, IWidgetContainer* = NULL, Color = Color::White, Color = Color::Black);
	/ \vt \mf void Fill(PixelType) -> void Fill(Color);
/ \mf void Update(Drawing::PixelType = 0) -> void Update(Drawing::Color = 0) @@ \cl YScreen @@ \u YDevice;
/ @@ \u YGDI:
	^ \n Drawing::Color;
	+ \i \tf template<typename _tPixel> void
		FillRect(_tPixel*, const Size&, const Point&, const Size&, _tPixel);
	+ \i \tmf template<typename _tPixel, class _fTransformPixel, class _fTransformLine>
		void operator()(_tPixel*, SDST, SDST, SPOS, SPOS, SDST, SDST, _fTransformPixel, _fTransformLine)
		@@ \cl transRect;
/ @@ \cl MDualScreenReader @@ \u DSReader:
	/ \mf void SetColor(Color = 0) -> void SetColor(Color = Color::Black);
/ \tr \impl @@ \u Shells;

r13-r14:
/= test 2;

r15:
/ @@ \u YGUIComponent:
	/= \impl @@ \mf void YListBox::DrawBackground();
	* \i \f void transPixelEx(BitmapPtr) @@ \un \ns;

r16:
/ \impl @@ \mf void YButton::DrawForeground();

r17:
/ @@ \u YGUIComponent:
	/ @@ \un \ns:
		+ \f void RectDrawPressed(const Point&, const Size&, HWND);
		/ \impl @@ \f void RectDrawFocusDefault(const Point&, const Size&, HWND);
		/ \f RectDrawFocusDefault => RectDrawFocus;
		- \i \f void RectOnGotFocus(const Point&, const Size&, HWND);
	/ \impl @@ \mf void DrawForeground() @@ \cl YButton;
	/ \impl @@ \mf void DrawForeground() @@ \cl YListBox;

r18:
/ \tr \impl @@ \f void RectDrawFocus(const Point&, const Size&, HWND) @@ \un \ns @@ \u YGUIComponent;

r19:
/ \tr \impl @@ \mf void DrawForeground() @@ \cl YListBox;

$DOING

/ ...

$NEXT:
b161-b190:
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

