// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-10-25 13:12 + 08:00;
// Version = 0.2768; *Build 166 r22;


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
\para ::= parameters
\para.de ::= default parameters
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
	\cl YControl;
	\cl AVisualControl;
	\cl YVisualControl;
}
\u YGUIComponent
{
	\cl AButton;
	\cl MScrollBar;
	\cl AScrollBar;
	\cl YButton;
	\cl YHorizontalScrollBar;
	\cl YVerticalScrollBar;
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
/ @@ \i \f void transPixelEx(BitmapPtr) @@ \un \ns @@ \u YGUIComponent
	-> \f \t template<u8 r, u8 g, u8 b> inline void transPixelEx(BitmapPtr);

r2:
/ \tr \impl @@ \mf @@ \cl YHorizontalScrollBar:
	/ void DrawPrevButton();
	/ void DrawNextButton();

r3:
/ \tr \impl @@ \f void WndDrawArrow(HWND, const Rect&, SDST, ROT, Color) @@ \un \ns @@ \u YGUIComponent;

r4:
/ @@ \u YGUI:
	+ \i \f \t template<class _fTransformPixel> bool TransformRect(const Graphics&, const Point&, const Size&, _fTransformPixel);
	+ \i \f \t template<class _fTransformPixel> inline bool TransformRect(const Graphics&, const Rect&, _fTransformPixel);
/ \simp \impl @@ \YGUIComponent;
/ \tr \impl @@ \mf void YHorizontalScrollBar::DrawScrollArea();

r5:
/ \impl @@ \mf void YHorizontalScrollBar::DrawScrollArea();

r6:
/ @@ \cl YHorizontalScrollBar @@ \u YGUIComponent:
	/ \impl @@ \mf void DrawScrollArea();
	+ \mf void DrawBackground();

r7:
/= \a \ac @@ \mf void DrawBackground() & \mf void DrawForeground()
	& \mf void DrawBackgroundImage() & \mf void DrawWidgets()
	& \mf void DrawDesktopObjects() (@@ \cl & !@@ \in) -> protected ~ public;
/ \tr @@ \mf void ShowString(const String&) @@ \cl ShlSetting::TFormA @@ \cl Shells;

r8:
+ \s \f void WaitForInput @@ \st Def @@ \u YGlobal;
+ \i \mf void Pause() @@ \cl YConsole @@ \u YComponent;
+
{
	#ifdef YC_USE_YASSERT

	#	undef YWidgetAssert

	void yassert_Widget(const IWidget*, int, const char*, const char*, const char*);

	#	define YWidgetAssert(widget_ptr, comp_name, func_name) \
	Components::Widgets::yassert_Widget(widget_ptr, __LINE__, __FILE__, #comp_name, #func_name)

	#else

	#	define YWidgetAssert(widget_ptr, comp_name, func_name) \
	assert((widget_ptr)->GetWindowHandle() != NULL)

	#endif
} @@ \u YWidget;
/ "#define YAssert(exp, message) yassert(exp, #exp, message, __LINE__, __FILE__);"
	-> "#define YAssert(exp, message) yassert(exp, #exp, message, __LINE__, __FILE__)"
	@@ \u YCommon;
/ \tr \impl @@ \u YTextManager;
^ \mac YWidgetAssert @@ \cl YGUIComponent;

r9:
^ \mac YWidgetAssert @@ \impl @@ \u YWidget;
^ \mac YWidgetAssert @@ \impl @@ \u YWindow;
^ \mac YWidgetAssert @@ \impl @@ \u YDesktop;
/ @@ \u YWidget:
	\i \f void yassert_Widget(const IWidget*, int, const char*, const char*, const char*)
		-> \f \t template<class _tWidget> void yassert_Widget(const _tWidget*, int, const char*, const char*, const char*);

r10:
/ \impl @@ \u YWidget;
	/ \f \t template<class _tWidget> void yassert_Widget(const _tWidget*, int, const char*, const char*, const char*)
		-> \i \f void yassert(bool, const char* int, const char*, const char*, const char*);
	+ \mac YWindowAssert;
/ \a \mac YWidgetAssert -> \mac YWindowAssert @@ \impl @@ (\u YWindow & \u YDesktop);

r11:
- using platform::yassert @@ \h Adaptor::YAdaptor;
/ #define YAssert(exp, message) yassert(exp, #exp, message, __LINE__, __FILE__) @@ \u YCommon
	-> #define YAssert(exp, message) platform::yassert(exp, #exp, message, __LINE__, __FILE__);

r12-r17:
/ test 1;
	/ \tr @@ \h Adaptor::Config;

r18:
/ @@ \cl YHorizontalScrollBar:
	/ \tr 
	- \mf
	{
		void DrawPrevButton();
		void DrawNextButton();
		void DrawScrollArea();
	};
- \en CROT @@ \h CHRDefinition;
/= limited \a \ln maximum width to 80 characters @@ \lib CHRLib & \lib YCommon;
/= documented \a \f @@ \h ^ VA Snippets @@ \lib CHRLib;

r19:
/= documented \a \f @@ \h ^ VA Snippets @@ \lib YCommon;
/= limited \a \ln maximum width to 80 characters @@ \lib CHRLib & \lib YShell::Adaptor;
/= documented \a \f @@ \h ^ VA Snippets @@ \lib YShell::Adaptor;

r20:
/= limited \a \ln maximum width to 80 characters @@ \lib YShell::Core;
/= documented \a \f @@ \h ^ VA Snippets @@ YShell::Core;
* \impl
	{
		template<typename _type, typename _fCompare>
		inline _type
		vmin(_type a, _type b, _fCompare _comp);
		template<typename _type, typename _fCompare>
		inline _type
		vmax(_type a, _type b, _fCompare _comp);
	} @@ \u YCoreUtilities;
/ \mf clear => Clear @@ \cl \t GEventMap @@ \YEvent;
-= \i !\vt \dtor @@ \cl \t GHBase @@ \h YFunc;
/ \simp @@ \cl Rect @@ \u YObject:
	/ \i \mf Point GetPoint() const -> DefGetter(Point, Point, Point(X, Y));

r21:
/= limited \a \ln maximum width to 80 characters @@ \lib YShell::Service;
/= documented \a \f @@ \h ^ VA Snippets @@ YShell::Service;
/ \mf void clear() => void Clear @@ \cl TextMap @@ \u YTextManager;
/= limited \a \ln maximum width to 80 characters @@ \lib YShell::Helper;
/= documented \a \f @@ \h ^ VA Snippets @@ YShell::Helper;
+ ythrow() @@ \def @@ \mac DefEmptyDtor;
/= \dtor @@ \cl ShlGUI @@ \u ShlDS -> DefEmptyDtor(ShlDS);
/ \dtor @@ \cl YFrameWindow + \es ythrow();
/ \dtor @@ \cl YForm @@ \u YForm + \es ythrow();
/ \vt DefEmptyDtor(YControl) @@ \cl YControl;
/ \dtor @@ \cl AVisualControl + \es ythrow();
/ \dtor @@ \cl YWidgetContainer + \es ythrow();
/ \vt DefEmptyDtor(MWidget) @@ \cl MWidget;
/ \vt DefEmptyDtor(YHorizontalScrollBar) @@ \cl YHorizontalScrollBar;
/ \dtor @@ \cl YWidget + \es ythrow();
/ \dtor @@ \cl YVisualControl + \es ythrow();
/ \vt DefEmptyDtor(YButton) @@ \cl YButton;
/ \dtor @@ \cl YListBox + \es ythrow();
/ \vt DefEmptyDtor(YLabel) @@ \cl YLabel;

r22:
/= limited \a \ln maximum width to 80 characters @@ \lib YShell::Shell;
/= documented \a \f @@ \h ^ VA Snippets @@ YShell::Shell;
/= \simp @@ \cl YM @@ \u YControl:
/= ImplI(IVisualControl) PDefH(EventMapType::Event&, operator[], const EventMapType::ID& id)
	-> ImplI(IVisualControl) PDefHOperator(EventMapType::Event&, [], const EventMapType::ID& id);


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

