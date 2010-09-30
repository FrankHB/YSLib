// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-10-01 02:59 + 08:00;
// Version = 0.2677; *Build 157 r29;


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
+ \mac DefTrivialDtor @@ \h YShellDefinition;
/ \u YWidget:
	* \tr \vt \dtor ^ \mac DefTrivialDtor @@ \cl MVisual @@;
	-= !\i \vt \dtor @@ \cl MWidgetContainer;
/ @@ \u YWindow:
	* @@ \cl MDesktopObject:
		/ protected \ctor -> public;
		+ protected \tr \dtor ^ \mac DefTrivialDtor;
	/ protected \ctor -> public @@ \cl MWindow;
/= @@ \u YControl
	/= \tr \vt \dtor ^ \mac DefTrivialDtor @@ \cl MControl;
	-= \i \tr \vt \dtor @@ \cl MVisualControl;
/ \vt \dtor + \tr ^ \mac DefTrivialDtor @@ \cl MFileList @@ \u YFileSystem;

r3:
+ \cl MLabel @@ \ns Components::Widgets @@ \u YWidget;
/ @@ \cl YLabel @@ \u YControl:
	/ \m {
	protected:
		GHResource<Drawing::TextRegion> prTextRegion;

	public:
		Drawing::Font Font;
		Drawing::Padding Margin;
		bool AutoSize;
		bool AutoEllipsis;
		String Text;
	} >> \cl Components::Widgets::MLabel @@ \u YWidget;
	+ public \inh @@ \cl Widgets::MLabel;
	-= empty !\i \vt \dtor;
	/ \impl @@ \ctor;
/ @@ \u YWidget:
	+ \ctor template<class _tChar>
		MLabel(const _tChar*, const Drawing::Font& = Drawing::Font::GetDefault(), GHResource<Drawing::TextRegion> = NULL)
		@@ \cl MLabel;
	+ \inc "ytext.h";
- \inc "ytext.h" @@ \u YControl;

r4:
/ @@ \u YResource:
	/= \simp \impl @@ \tf template<class T> GHResource<T>& GetGlobalResource();
	/= \i \mf BitmapPtr GetImagePtr() const -> DefGetter(BitmapPtr, ImagePtr, GetBufferPtr()) @@ \cl YImage;

r5:
/= \a YLabel => YButton;
/ @@ \ns Components::Widgets @@ \u YWidget:
	+ protected \mf void PaintText(MWidget&, Drawing::PixelType) \cl MLabel;
/ \impl @@ \mf void YButton::DrawForeground() @@ \u YControl;
	^ \mf MLabel::PaintText(MWidget&, Drawing::PixelType);

r6:
/ @@ \ns Components::Widgets @@ \u YWidget:
	+ \cl YLabel;
/ @@ \u Shells:
	/ @@ \cl ShlLoad:
		/ @@ \st TFrmLoadUp:
			/ \m YButton lblTitle, lblStatus -> YLabel lblTitle, lblStatus;;
		/ @@ \st TFrmLoadDown:
		/ \m YButton lblStatus-> YLabel lblStatus;
	/ @@ \cl ShlExplorer:
		/ @@ \st TFrmFileListMonitor:
			/ \m YButton lblTitle, lblPath -> YLabel lblTitle, lblPath;
	/ @@ \cl ShlSetting:
		/ @@ \st TFormB:
			/ \m YButton lblB, lblB2 => YButton btnB, btnB2;
		/ @@ \st TFormC:
		/ \m YButton lblC => YButton btnC;
		/ \mf lblC_$ => btnC_$;
		/ \impl @@ \mf void ShlExplorer::TFrmFileListSelecter::fb_Selected(const MIndexEventArgs&);

r7:
+ \mf \vt void DrawBackground() @@ \cl YLabel @@ \u YWidget;
/ @@ \u YControl:
+ \inh public GMCounter<YControl> @@ \cl YControl;
+ \inh public GMCounter<YVisualControl> @@ \cl YVisualControl;

r8:
/ @@ \u YControl:
	/ @@ \cl MVisualControl:
		/ \m
			{
				Drawing::PixelType BackColor; //默认背景色。
				Drawing::PixelType ForeColor; //默认前景色。
			} => \cl MVisual @@ \u YWidget;
		/ \mf
		{
			virtual void
			DrawBackground();
			virtual void
			DrawForeground();
		} => \cl MWidget @@ \u YWidget;
		/ \impl @@ \mf void YButton::DrawForeground();
		/ \ctor \exp MVisualControl(Drawing::PixelType = Drawing::Color::Black, Drawing::PixelType = Drawing::Color::White)
			-> MVisualControl();
	/ @@ \cl YVisualControl:
		/ \impl @@ \ctor;
		/ \impl @@ \mf \vt void DrawBackground();
		/ \impl @@ \mf \vt void DrawForeground();
/ @@ \u YWidget:
	/ @@ \cl MLabel:
	/ \mf void PaintText(MWidget&, PixelType)
		-> void PaintText(MWidget&);
	- \mf \vt void DrawBackground() @@ \cl YLabel;
	/ @@ \cl MVisual:
		/ \exp \ctor MVisual(const SRect& = SRect::Empty)
			-> MVisual(const SRect& = SRect::Empty,
			Drawing::PixelType = Drawing::Color::Black, Drawing::PixelType = Drawing::Color::White);
	/ @@ \cl MWidget:
		/ \exp \ctor MWidget(HWND = NULL, const SRect& = SRect::Empty, IWidgetContainer* = NULL)
			-> MWidget(HWND = NULL, const SRect& = SRect::Empty, IWidgetContainer* = NULL,
			Drawing::PixelType = Drawing::Color::Black, Drawing::PixelType = Drawing::Color::White);
	/ @@ \cl YWidget:
		/ \impl @@ \mf \vt void DrawBackground();
		/ \impl @@ \mf \vt void DrawForeground();
		+ using Drawing::PixelType @@ \h;
	/ \tr \impl @@ \ctor @@ \cl MWindow @@ \u YWindow;

r9:
/ @@ \u YWidget;
	/ \exp \ctor MVisual(HWND = NULL, const SRect& = SRect::Empty, IWidgetContainer* = NULL,
		PixelType = Drawing::Color::Black, PixelType = Color::White)
		-> \exp \ctor MVisual(HWND = NULL, const SRect& = SRect::Empty, IWidgetContainer* = NULL,
		PixelType = Drawing::Color::White, PixelType = Color::Black);
	/ \exp \ctor MWidget(HWND = NULL, const SRect& = SRect::Empty, IWidgetContainer* = NULL,
		PixelType = Drawing::Color::Black, PixelType = Color::White)
		-> \exp \ctor MWidget(HWND = NULL, const SRect& = SRect::Empty, IWidgetContainer* = NULL,
		PixelType = Drawing::Color::White, PixelType = Color::Black);

r10:
/ @@ \h YWidget:
	+ using Drawing::BitmapPtr, Drawing::ConstBitmapPtr, Drawing::ScreenBufferType, Drawing::Color;
	/ \simp \decl;
/ @@ \cl YDesktop @@ \h YDesktop:
	/ \simp \decl;
	- \m PixelColor BackColor;

r11-r29:
/= test 1;


$DOING:

/ ...



$NEXT:
/ fully \impl \u DSReader;
	* moving text after setting lnGap; 
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

