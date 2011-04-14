//v0.3086; *Build 201 r53;
/*
$Record prefix and abbrevations:
<statement> ::= statement;
; ::= statement termination
= ::= equivalent
+ ::= added
- ::= removed
* ::= fixed
/ ::= modified
% ::= identifier
~ ::= from
! ::= not
& ::= and
| ::= or
^ ::= used
-> ::= changed to
>> ::= moved to
=> ::= renamed to
<=> ::= swapped names
@ ::= in / belonged to
\a ::= all
\ab ::= abstract
\ac ::= access
\adtor ::= abstract destructor
\amb ::= ambiguities
\amf ::= abstract/pure virtual member function
\as ::= assertions
\bg ::= background
\c ::= const
\cb ::= catch blocks
\cl ::= classes
\clt ::= class templates
\conf ::= configuration
\cp ::= copied
\ctor ::= constructors
\ctort ::= constuctor templates
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
\exc ::= excluded
\ext ::= extended
\exp ::= explicit
\f ::= functions
\fn ::= \f
\ft ::= function templates
\fw ::= forward
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
\mft ::= member function templates
\mg ::= merged
\mo ::= member objects
\mt ::= member templates
\n ::= names
\ns ::= namespaces
\o ::= objects
\op ::= operators
\or ::= overridden
\param ::= parameters
\param.de ::= default parameters
\pre ::= prepared
\proj ::= projects
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
\scm ::= static const member
\sf ::= non-member static functions
\sif ::= non-member inline static functions
\simp ::= simplified
\sm ::= static member
\smf ::= static member functions
\snm ::= static non-member
\spec ::= specifications
\st ::= structs
\str ::= strings
\stt ::= struct templates
\t ::= templates
\tb ::= try blocks
\tg ::= targets
\tr ::= trivial
\tp ::= types
\u ::= units
\un ::= unnamed
\v ::= volatile
\vt ::= virtual
\val ::= values

$using:
\u YObject
{
	\cl YObject;
	\cl YCountableObject;
	\clt GDependence;
}
\u YShell
{
	\cl YShell;
}
\u YApplication
{
	\cl YLog;
	\cl YApplication;
}
\u YWidget
{
	\in IWidget;
	\cl Visual;
	\cl Widget;
	\cl YWidget;
}
\u YLabel
{
	\cl YLabel;
}
\u YUIContainer
{
	\in IUIBox;
	\in IUIContainer;
	\cl YUIContainer;
}
\u YUIContainerEx
{
	\cl AUIBoxControl;
}
\u YControl
{
	\in IControl;
	\cl Control;
	\cl YControl;
}
\u YWindow
{
	\in IWindow;
	\cl MWindow;
	\cl AWindow;
	\cl AFrame;
	\cl YFrame;
}
\u YForm
{
	\cl YForm;
}
\u YGUI
{
	\cl YGUIShell;
}
\u Button
{
	\cl YThumb;
	\cl YButton;
}
\u Scroll
{
	\cl ATrack;
	\cl YHorizontalTrack;
	\cl YVerticalTrack;
	\cl MScrollBar;
	\cl AScrollBar;
	\cl YHorizontalScrollBar;
	\cl YVerticalScrollBar;
}
\u ListBox
{
	\cl YSimpleListBox;
	\cl YListBox;
	\cl YFileBox;
}


$DONE:
r1:
/ @ \h YWindow:
	/ \m mutable GHStrong<YImage> prBackImage
		-> mutable \m GHStrong<YImage> spBgImage;
	/ @ \cl MWindow:
		/ \mf DefGetter(GHStrong<Drawing::YImage>, Background, prBackImage)
			-> DefGetter(GHStrong<Drawing::YImage>&, BackgroundImagePtr,
			spBgImage);
		- \mf DefSetterDe(GHStrong<Drawing::YImage>, Background, spBgImage,
			NULL);
	/ @ \cl AWindow:
		/ DefGetterBase(GHStrong<Drawing::YImage>, Background, MWindow)
			-> DefGetterBase(GHStrong<Drawing::YImage>&, BackgroundImagePtr,
			MWindow);
		- \mf ImplI1(IWindow) DefSetterBaseDe(GHStrong<Drawing::YImage>,
			Background, MWindow, NULL);
/ \tr @ \impl @ \mf (ShlLoad::OnActivated & ShlReader::OnActivated
	& ShlReader::OnDeactivated & ShlLoad::OnDeactivated
	& ShlExplorer::OnActivated & ShlExplorer::OnDeactivated
	& ShlSetting::OnActivated & ShlSetting::OnDeactivated) @ \impl \u Shells;

r2:
/ @ \cl ShlDS @ \h Shell_DS:
	+ \mf DefGetter(YDesktop&, DesktopUp, *hDskUp);
	+ \mf DefGetter(YDesktop&, DesktopDown, *hDskDown);
/ \simp \impl @ \mf @ \impl \u Shells
	^ GetDesktopUp & GetDesktopDown @ \cl ShlDS @ \h Shell_DS;

r3-r4:
/ \impl @ \mf void YThumb::DrawForeground();

r5:
/ @ \impl \u Shells:
	/ \simp \impl @ \mf ^ GetDesktopUp & GetDesktopDown @ \cl ShlDS
		@ \h Shell_DS;
	/ \impl @ \mf (OnActivated & OnDeactivated) @ \cl ShlReader;

r6:
/ @ \cl ShlReader @ \u Shells:
	/ \mf OnKeyPress -> OnKeyDown;
	/ \impl @ \mf OnActivated;

r7:
/ @ \u YGDI:
	/ \mf \vt void BitmapBuffer::Flush(BitmapPtr,
		const Size& = Size::FullScreen,
		const Point& = Point::Zero, const Point& = Point::Zero,
		const Size& = Size::FullScreen, Rotation = RDeg0) const -> !\m \f;
	/ \mf void BitmapBufferEx::Flush(BitmapPtr,
		const Size& = Size::FullScreen,
		const Point& = Point::Zero, const Point& = Point::Zero,
		const Size& = Size::FullScreen, Rotation = RDeg0) const -> !\m \f;
	/ \mf void BitmapBufferEx::BlitTo(BitmapPtr,
		const Size& = Size::FullScreen,
		const Point& = Point::Zero, const Point& = Point::Zero,
		const Size& = Size::FullScreen, Rotation = RDeg0) const -> !\m \f;
/ \tr \impl @ \impl \u (Shells & DSReader & YText);

r8:
/ @ \u YGDI:
	/ \param order @ \a blit functions;
	/ \tr \impl @ \f (2 CopyTo & Flush & BlitTo) @ \impl \u;
	/ \tr \impl @ \f (void RenderChar(const Graphics&, TextState&, fchar_t)
		& void RenderChar(BitmapBufferEx&, TextState&, fchar_t))
		@ \impl \u YText;

r9:
/ @ \u YGDI:
	/ \param order @ \f BlitBounds;
	/ \tr @ \ft Blit & \mft RectTransfomer::operator();

r10:
/ @ \u YGDI:
	/ \param order @ \ft BlitScale;
	/ \tr @ \ft Blit;

r11:
/ @ \u YGDI:
	/ 2 \f Flush => CopyTo;
	/ \f void CopyTo(const Graphics&, const Graphics&,
		const Point& = Point::Zero, Rotation = RDeg0);
		-> void CopyTo(const Graphics&, const Graphics&, const Point&
		= Point::Zero const Point& = Point::Zero, Rotation = RDeg0);
	/ \param order @ \f (2 CopyTo & BlitTo);
/ \tr \impl @ \impl \u (Shells & DSReader & YText);

r12:
/ @ \u YGDI:
	/ \f void CopyTo(BitmapPtr, const Graphics&, Rotation, const Size&,
		const Point&, const Point&, const Size&);
		-> void CopyTo(BitmapPtr, const Graphics&,
		const Size& ds = Size::FullScreen, const Point& sp = Point::Zero,
		const Point& dp = Point::Zero, const Size& sc = Size::FullScreen,
		Rotation rot = RDeg0);
	-= \i \f void CopyTo(BitmapPtr, const BitmapBuffer&,
		const Size& = Size::FullScreen, const Point& = Point::Zero,
		const Point& = Point::Zero, const Size& = Size::FullScreen,
		Rotation = RDeg0);
	/ \impl @ \f (2 CopyTo & BlitTo);

r13:
/ !\i \f void CopyTo(const Graphics&, const Graphics&,
	const Point& = Point::Zero, const Point& = Point::Zero,
	Rotation = RDeg0) @ \u YGDI -> \i \f;

r14:
/ \h YGDI:
	+ \i \f void CopyTo(const Graphics&, const BitmapBufferEx&,
		const Point& = Point::Zero, const Point& = Point::Zero,
		Rotation = RDeg0);
	+ \i \f void BlitTo(const Graphics&, const BitmapBufferEx&,
		const Point& = Point::Zero, const Point& = Point::Zero,
		Rotation = RDeg0);

r15:
/ @ \cl MDualScreenReader @ \u DSReader:
	/ \mf void PrintText() -> void PrintText(const Graphics&, const Graphics&);
	/ \simp \impl @ \ctor;
	- \m pBgUp;
	- \m pBgDn;
/ \tr @ \impl @ \mf void ShlReader::UpdateToScreen() @ \impl \u Shells;

r16:
/= test 1 ^ \conf release;

r17:
/ \simp @ \cl MDualScreenReader @ \u DSReader:
	- \mf DefSetterDe(SDst, Left, left, 0);
	- \m SDst left;
	- \m SDst top_up;
	- \m SDst top_down;
	/ \ctor MDualScreenReader(SDst l = 0, SDst w = Global::MainScreenWidth,
		SDst t_up = 0, SDst h_up = Global::MainScreenHeight,
		SDst t_down = 0, SDst h_down = Global::MainScreenHeight,
		YFontCache& fc_ = theApp.GetFontCache())
		-> MDualScreenReader(SDst w = Global::MainScreenWidth,
		SDst h_up = Global::MainScreenHeight,
		SDst h_down = Global::MainScreenHeight,
		YFontCache& fc_ = theApp.GetFontCache());
	/ \m GHStrong<Drawing::TextRegion> pTextRegionUp
		-> std::auto_ptr<Drawing::TextRegion> pTextRegionUp
	/ \m GHStrong<Drawing::TextRegion> pTextRegionDn
		-> std::auto_ptr<Drawing::TextRegion> pTextRegionDown;
	/ \ac @ \mf (GetColorUp & GetColorDn & GetLineGapUp & GetLineGapDn)
		-> public ~ private;
	/ \mf GetColorDn => GetColorDown;
	/ \mf GetLineGapDn => GetLineGapDown;
	/ \mf SetColorDn => SetColorDown;
	/ \mf SetLineGapDn => SetLineGapDown;

r18:
/ @ \u YWidget:
	/ @ \in IWidget:
		- \amf DeclIEntry(bool IsBgRedrawed() const);
		- \amf DeclIEntry(void SetBgRedrawed(bool));
	/ @ \cl Visual:
		- \m mutable bool background_redrawed;
		- \mf DefPredicate(BgRedrawed, background_redrawed);
		- \mf DefSetter(bool, BgRedrawed, background_redrawed);
		/ \tr \impl @ \ctor;
		/ \tr \impl @ \mf void SetSize(const Size&);
	/ \tr \impl @ Widget::DrawForeground;
	/ @ \cl YWidget:
		- \mf ImplI1(IWidget) DefPredicateBase(BgRedrawed, Visual);
		- \mf ImplI1(IWidget) DefSetterBase(bool, BgRedrawed, Visual);
		/ ImplI1(IWidget) PDefH0(void, DrawForeground)
			ImplBodyBase0(Widget, DrawForeground) -> \em \i \mf;
	/ \tr \impl @ \mf ShlDS::OnActivated @ \impl \u Shell_DS;
	/ @ \u YControl:
		/ @ \cl Control:
			- \mf ImplI1(IControl) DefPredicateBase(BgRedrawed, Visual);
			- \mf ImplI1(IControl) DefSetterBase(bool, BgRedrawed, Visual);
		/ \tr \impl @ \f OnTouchMove_Dragging;
	/ \tr \impl @ \mf YDesktop::DrawContents @ \impl \u YDesktop;
	/ @ \u YUIContainer:
		- \f bool SetContainerBgRedrawedOf(IWidget&, bool);
		/ @ \cl YUIContainer:
			- \mf ImplI1(IUIContainer) DefPredicateBase(BgRedrawed, Visual);
			- \mf ImplI1(IUIContainer) DefSetterBase(bool, BgRedrawed, Visual);
	/ @ \impl \u YWindow:
		/ \tr \impl @ \f (Show & Hide);
		/ @ \cl YFrame:
			/ \tr \impl @ \mf DrawContents;
			/ \tr \impl @ \mf DrawContensBackground;
		/ \impl @ \mf AWindow::Refresh;
	/ \tr \impl @ \mf ShlReader::UpdateToScreen @ \impl \u Shells;

r19:
* \impl @ \mf YFrame::DrawContensBackground;

r20-r21:
/= test 2;

r22:
/ \impl @ \ctor @ \cl MWindow @ \impl \u YWindow;

r23:
/ \simp \impl @ \mf ShlDS::OnActivated @ \impl \u Shell_DS;

r24:
* \impl @ \mf ShlReader::UpdateToScreen @ \impl \u Shells;

r25:
* \impl @ \mf TestObj::Blit @ \mf ShlSetting::TFormExtra::OnClick_btnTestEx
	@ \impl \u Shells;

r26:
/= test 3 ^ \conf release;

r27:
/ @ \impl @ \mf YFrame::DrawContents;
/ \simp \impl @ \mf YDesktop::DrawContents;

r28:
/ @ \cl YFrame:
	* \mf DrawContensBackground => DrawContentsBackground;
	/ \mf DrawContensBackground \mg -> \mf DrawContents;

r29:
/= test 4 ^ \conf release;

r30:
/ @ \u YWindow:
	- \amf void Draw() @ \in IWindow;
	/ \a \mf Draw => DrawForeground;
	/= \mf ImplI1(IWindow) void
		DrawForeground() -> \vt void DrawForeground() @ \cl AWindow;
/ \tr \impl @ \mf YGUIShell::ShlProc;

r31-r33:
/ \a (\ab & !\ab) \mf void DrawBackground() \mg -> void DrawForeground();

r34:
/ \a \mf DrawForeground => Draw;

r35:
/ \ret \tp @ \f (4 CopyTo & 2 BlitTo) @ \u YGDI -> bool ~ void;

r36:
/ \simp \impl @ \mf bool AWindow::DrawBackgroundImage() @ \impl \u YWindow
	^ \f CopyTo;

r37:
/ \simp \impl @ \i \f Point LocateWindowOffset(const IWidget&, const Point&)
	@ \h YUIContainer;
* \impl @ \mf void YThumb::Draw() @ \impl \u Button;

r38:
^ \conf release;
/= test 5;

r39:
* \a GHHandle => GHandle;

r40-r41:
^ \conf debug;
/= test 6;

r42:
/ @ \dir YSLib/include:
	+ \h Platform::DS::API["api.h"];
	/ \decl @ Platform::DS["platform.h"];
+ \inc \h <api.h> @ \h YCommon;
+ \inc \h <api.h> @ \impl \u Platform::DS::Main;

r43:
+ \u YPanel["ypanel.h", "ypanel.cpp"] @ \dir Shell @ \lib YSLib;
+ \in IPanel \ns Components::Controls @ \h YPanel;
+ (\pre \decl & using \decl) \in IPanel @ \h YComponent;
/ @ \h YWindow:
	- \inc \h "ycontrol.h"
	- \inc \h "yuicont.h"
	- \inc \h "../Core/yres.h"
	+ \inc \h "ypanel.h"
	/ @ \in IWindow:
		- \inh \in IUIContainer;
		- \vt \inh \in IControl;
		+ \inh \in IPanel;

r44-r45:
+ typedef Controls::Control ParentType @ \cl AWindow;
/ order @ \amf @ \in IUIContainer;
/ @ \u YPanel:
	+ \cl Panel;
	+ \cl YPanel;

r46:
/= test 7 ^ \conf release;

r47:
/ @ \h YStandardExtend:
	+ partial complier check;
	+ \mac YCL_CHAR_BIT;
	+ \mac YCL_UNUSED;
	+ \mac YCL_VOID;
	+ \mac YCL_VOIDX;
/ \a \inc <ystdex.h> -> "../ystdex.h";

r48:
+ \h TypeOperation["type_op.hpp"] @ \lib YCLib::YStandardExtend;
/ @ \h YCLib::YStandardExtend::Cast:
	/ \clt (has_nonempty_virtual_base & has_common_nonempty_virtual_base)
		>> \h TypeOperation;
	+ \inc \h "type_op.hpp";

r49:
/ @ \h YWindow:
	/ @ \cl AFrame:
		+ \ft<class _type> \i void operator+=(_type*);
		+ \ft<class _type> \i bool operator-=(_type*);
	/ @ \cl YFrame:
		+ using AFrame::operator+=;
		+ using AFrame::operator-=;
/ @ \cl YDesktop @ \h YDesktop:
		+ using ParentType::operator+=;
		+ using ParentType::operator-=;
/ \simp \impl @ \mf ShlLoad::OnActivated @ \impl \u Shells;

r50:
+ \stt (MoreConvertible & SelectConvertible) @ \ns Design @ \h YCoreUtilities;
/ \simp \impl @ \ft<class _type> \op(+= & -=) ^ MoreConvertible
	@ \cl AFrame @ \h YWindow;
/ @ \cl Panel @ \h YPanel:
	+ \ft<class _type> \i void operator+=(_type*);
	+ \ft<class _type> \i bool operator-=(_type*);
/ \simp \impl @ \mf OnActivated @ (ShlExplorer & ShlSetting) @ \impl \u Shells;

r51:
/= test 8 ^ \conf release;

r52:
/ \ns ScrollEventSpace & \st ScrollEventArgs & DefDelegate(HScrollEvent,
	IControl, ScrollEventArgs) @ \h YControl -> \h Scroll;

r53:
/= test 9 ^ \conf release;


$DOING:

$relative_process:
2011-04-15:
-24.8d;

/ ...


$NEXT_TODO:

b202-b252:
* fatel error @ direct UI drawing testing;
+ Z order;
+ menu;
/ fully \impl \u DSReader;
	* moved text after setting lnGap;
* non-ASCII character path error in FAT16;

b253-b600:
/ impl 'real' RTC;
+ data configuragion;
+ shared property: additional;
+ GDI brushes;
/ text alignment;
+ \impl pictures loading;
/ improve efficiency @ \ft polymorphic_crosscast @ \h YCast;
+ correct DMA (copy & fill);
* platform-independence @ alpha blending:
	+ \impl general Blit algorithm;
/ user-defined bitmap buffer @ \cl YDesktop;
+ (compressing & decompressing) @ gfx copying;


$LOW_PRIOR_TODO:
r601-r960:
+ \impl styles @ widgets;
+ general component operations:
	+ serialization;
	+ designer;
+ database interface;


$KNOWN_ISSUE:
* fatal \err @ b16x:
[
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
	-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 02055838
	ftc_snode_weight
	ftcsbits.c:271
]
* fatal \err @ since b177 when opening closed lid @ real DS:
[
b185:
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 02037F04
guruMeditationDump
gurumeditation.c:229
b186 r17[realease]:
pc: 09529334, addr: 09529334;
b187 r18[debug]:
pc: 020227C8, addr: 0380FDA4;
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 020227C8
guruMeditationDump
gurumeditation.c:254
]


$TODO:

Clarify the log levels.

Make "UpdateFont()" more efficienct.

More efficient %YTextRegion output:
Use in-buffer background color rendering and function %CopyToScreen()
	to inplements %YTextRegion background;
Use pre-refershing to make font changing.

Consider to simplify the general window class: @YForm.

Build a more advanced console wrapper.

Build a series set of robust gfx APIs.

More GUI features needed:
Icons;
Other controls.

Other stuff to be considered to append:
Design by contract: DbC for C/C++, GNU nana.


$HISTORY:

$design; //features only for developers;
$add_features +; //features added;
$fix_bugs *; //bugs fixed;
$modify_features /; //features modified;
$remove_features -; //features removed;
$using ^; //using;

$transform $list ($list_member $pattern $all($exclude $pattern
	$string_literal "*")) +;

$ellipse_refactoring;

$now
(
/ "focused button style",
"key held response in %ShlReader",
"GDI API %BlitTo",
/ "widgets drawing",
/ $design "GDI API %CopyTo",
"controls: panel",
/ $design  "YCLib" $=
	(
		"partial version checking for compiler and library implementation",
		"minor macros in YCLib",
	),
"type conversion helper template",
$design "implicit member overloading by interface parameter with type %IControl
	and %IWidget in container class"
),

b200
(
* "direct drawing test",
/ "test UI view",
* "%KeyHeld response" $since b199,
+ "direct desktop drawing for %ShlSetting"
),

b199
(
"event routing for %KeyUp, %KeyDown and %KeyHeld",
* "event behavior with optimizing" $since b195,
+ "keypad shortcut for file selecter",
+ $design "returning number of called handles in event calling"
),

b198
(
* "font caching without default font file load successfully",
"showing and hiding windows",
* "%ListBox scroll bar length",
* "handle constructors",
/ $design "shells for DS" $=
	(
		- "class %ShlGUI in unit Shell_DS"
	),
/ $design "using pointers instead of references in parameters
	of container methods",
/ "simplified GUI shell" $=
	(
		/ "input points matching",
		- "windows list"
	),
/ $design "simplified destructors",
/ "simplified window drawing",
+ "desktop capability of non-control widget container",
- "contianer pointer parameter from constructor of widgets",
/ "desktops as window in shells"
),

b197
(
* "label alignment",
"%std::string based font cache"
),

b196
(
"controls: checkbox",
* "platform color type",
"horizontal text alignment in class %MLabel"
),

b195
(
* $design "makefiles",
"dependency events",
"simple events routing"
),

b170_b194
(
"controls: track",
"controls: scroll bar",
"controls: scrollable container",
"controls: listbox"
),

b159_b169
(
"controls: buttons": class ("%YThumb", "%YButton"),
"controls: listbox class",
"events"
),

b132_b158
(
$design "core utility templates",
"smart pointers using Loki",
"Anti-Grain Geometry test",
"GUI focus",
"shells",
"base abbreviation macros",
"controls",
"virtual inheritance in control classes",
"exceptions",
"debug macros & functions",
"color type",
"class template %general_cast",
"timer class"
),

b1_b131
(
"initial test with PALib & libnds",
"shell classes",
"the application class",
"CHRLib: character set management",
"fonts management using freetype 2",
"YCLib: platform independence",
"basic objects & counting",
"global objects",
"string class",
"file classes",
"dual screen text file reader framework",
"output devices & desktops",
"messaging",
"program initialization",
"simple GUI: widgets & windows",
"simple GDI",
"simple resource classes"
);


*/
//---- temp code;
/*

	static YTimer Timer(1250);

	if(Timer.Refresh())
	{
	//	InitYSConsole();
	//	YDebugBegin();
		iprintf("time : %u ticks\n", GetTicks());
		iprintf("Message : 0x%04X;\nPrior : 0x%02X;\nObj : %d\n"
			"W : %u;\nL : %lx;\n", msg.GetMessageID(),
			msg.GetPriority(), msg.GetObjectID(), msg.GetWParam(), msg.GetLParam());
		WaitForInput();
	//	StartTicks();
	}*/
/*
	YDebugBegin();
	iprintf("time : %u ticks\n", GetTicks());
	iprintf("Message : 0x%04X;\nPrior : 0x%02X;\nObj : %d\n"
		"W : %u;\nL : %lx;\n", msg.GetMessageID(), msg.GetPriority(),
		msg.GetObjectID(), msg.GetWParam(), msg.GetLParam());
	WaitForInput();*/
/*
		InitYSConsole();
		iprintf("%d,(%d,%d)\n",msg.GetWParam(),
			msg.GetCursorLocation().X, msg.GetCursorLocation().Y);
*/

