//v0.3115; *Build 204 r59;
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
\u YConsole
{
	\cl YConsole;
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
	\cl MLabel;
	\cl YLabel;
	\cl MTextList;
}
\u YUIContainer
{
	\in IUIBox;
	\in IUIContainer;
	\cl YUIContainer;
}
\u YControl
{
	\in IControl;
	\cl Control;
	\cl YControl;
}
\u YMenu
{
	\cl Menu;
	\cl YMenu;
}
\u YPanel
{
	\in IPanel;
	\cl YPanel;
}
\u YUIContainerEx
{
	\cl AUIBoxControl;
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
/ \simp \impl @ \mf bool operator!() const ^ \mf expired @ \clt GHWeak
	@ \h YReference;

r2:
/ @ \h YReference:
	/ \ft GetPointer => raw;
	/ \ft ResetHandle => reset_pointer;
	+ \ft<typename _type>_type* raw(auto_ptr<_type>);
	/ \def @ \a \ft raw ^ \c \ref \param;
	+ \es ythrow() @ \a \ft raw;
	+ \ft<typename _type>_type* reset_pointer(auto_ptr<_type>&) ythrow();
	+ \ft<typename _type>_type* reset_pointer(shared_ptr<_type>&) ythrow();
+ \mac ynothrow @ \h Base;
/ \h Base["base.h"] => YBase["ybase.h"];
/ \tr \inc @ \h YNew;
/ \a ythrow() -> ynothrow;

r3:
/= test 1 ^ \conf release;

r4:
/ \impl @ \mf (void Global::ReleaseDevices() ynothrow
	& int YShell::DefShlProc(const Message&)
	& int ShlSetting::OnDeactivated(const Message& msg) ^ (\mf GHandle::reset
	~ !\m \ft reset @ \h YReference);
- \mf T* Release() @ \clt GHandle @ \h YReference;
- \ft<class _type> _type* FetchContextRawPtr(const Message&)
	@ \h YShellMessage;
/ \impl ^ \ft dynamic_handle_cast ~ \ft HandleCast @ \impl \u Shells;
/ @ \h YShellHelper:
	- \ft<class _type, class _tHandle> \i _type* HandleCast(_tHandle);
	- \ft<class _tHandle> \i void ReplaceHandle(_tHandle&, _tHandle);

r5:
/ \impl @ \tf CastMessage ^ (\mf get @ \clt GHandle ~ \ft raw @ \h YReference);

r6:
/= test 2 ^ \conf release;

r7:
/ DeclareHandle(IWindow, HWND) -> typedef IWindow* HWND @ \h YComponent;
- \mac DeclareHandle(type, handle) @ \h YShellDefinition;
/ \tr @ \cl AFrame:
	- \mf !\vt void operator+=(HWND);
	- \mf !\vt bool operator-=(HWND);
/ \tr @ \impl \u Shells;
* \tf<typename _type> \i _type* raw(const _type*&) @ \h YReference
	-> \tf<typename _type> \i _type* raw(_type* const&);

r8:
- typedef IWindow* HWND @ \h YComponent;
/ \a hWindow => pWindow;
/ \a hWnd => pWnd;
/ \a HWND -> IWindow*;
/ \tr @ \impl \u Shells;
/ \a GetWindowHandle => GetWindowPtr;
/= \impl @ \f LocateForParentWindow @ \impl \u YUIContainer;

r9:
/ \a hWndExtra => pWndExtra;
/ @ \u YWindow:
	/ \f bool Show(IWindow*) -> void Show(IWindow&);
	/ \f bool Hide(IWindow*) -> void Hide(IWindow&);
/ \tr @ \impl @ \mf ShlSetting::TFormTest::OnClick_btnShowWindow
	@ \impl \u Shells;

r10:
* \tr \impl @ \ft<class _type> IWindow* NewWindow() @ \h YShellHelper;

r11:
/ undid r10;
* @ \cl ShlSetting @ \u Shells:
	/ \m IWindow* hWndTest -> auto_ptr<IWindow> pWndTest;
	/ \m IWindow* pWndExtra -> auto_ptr<IWindow> pWndExtra;
	/ \tr @ \impl \u;

r12:
/= test 3 ^ \conf release;

r13:
/= \tr \rem @ \h (YNew & YObject);
*= \tr \rem @ \h (YEvent);
/ @ \ns Components @ \u YStyle:
	+ \ns Styles;
	/ @ \ns Styles:
		+ typedef \en Area;
		+ \cl Palette;
/ @ \u YGUI:
	+ \h YStyle;
	/ @ \cl YGUIShell:
		+ \m Components::Styles::Palette Colors;
		/ \tr \impl @ \ctor;

r14:
/ @ \u YGUI:
	/ \f bool IsFocusedByShell(const IControl&,
		GHandle<YGUIShell> = FetchGUIShellHandle()) -> bool
		IsFocusedByShell(const IControl&, const YGUIShell&);
	/ \f GHandle<YGUIShell> FetchGUIShellHandle() -> YGUIShell& FetchGUIShell();
/ \tr \impl @ \impl \u (Shell_DS & Scroll & YControl);
/ \simp \impl @ \ft NewWindow @ \h YShellHelper;

r15:
/ \impl @ \impl \u (Scroll & ) ^ \u YStyle;

r16:
* \impl @ \mf ATrack::Paint;

r17:
/= test 4 ^ \conf release;

r18:
/ \impl @ \mf ATrack::Paint;

r19:
/= test 5 ^ \conf release;

r20:
/= \simp \impl @ \i \mf BelongsTo @ \cl (MWindowObject & MWidget)
	^ \mac PDefH1 @ \h YWidget;
/ @ \h YFont:
	/= \simp \impl @ \i \mf (\op== & \op<) @ \cl FontFile
		^ \mac PDefHOperator;
	/= \simp \impl @ \i \mf void ResetGlyphCache() @ \cl YFontCache
		^ \mac PDefH0;

r21:
/ \cl NonCopyable @ \ns Design @ \h YCoreUtilities
	>> \h Utilities @ \ns ystdex \lib YCLib;
/ @ \ns ystdex @ \h Utilities:
	/ \cl NonCopyable => noncopyable;
	+ \cl nullptr_t @ #ifdef YCL_HAS_BUILTIN_NULLPTR;
+ typedef ystdex::noncopyable @ \ns Design @ \h YCoreUtilities;
+ \mac YCL_HAS_BUILTIN_NULLPTR @ #ifdef \
	YCL_IMPL_CPP >= 201103L || YCL_IMPL_MSCPP >= 1600;
+ {
		using ystdex::nullptr_t;
	#ifndef YCL_HAS_BUILTIN_NULLPTR
		using ystdex::nullptr;
	#endif
} @ \ns YSLib @ \h YAdaptor;

r22:
/ @ \h Utilities @ \lib YCLib:
	* \inh noncopy @ nullptr_t; // cause default argument conversion failure;
	+ using std::nullptr_t @ #ifdef YCL_HAS_BUILTIN_NULLPTR;
/ \a NULL -> nullptr @ \lib YSLib;
/ \tr \impl @ \mf CharBitmap YFontCache::GetGlyph(fchar_t);
/ \tr \decl @ \a \mf with nullptr \de \param;
* !^ nullptr @ \f MemoryList& GetDebugMemoryList() @ \impl \u YNew;
/ @ \clt GHandle @ YReference:
	- \de \param @ \ctor GHandle(T* = nullptr);
	+ \ctor GHandle(nullptr_t = nullptr);
	- \de \param @ \ctor GWeak(T* = nullptr);
	+ \ctor GWeak(nullptr_t = nullptr);

r23:
/ \a NULL -> nullptr @ (YSTest_ARM9 & \lib YCLib);
+ \inc \h Utilities @ \impl \u (YCommon & YStandardEx);
/ {
	#ifndef YCL_HAS_BUILTIN_NULLPTR
	using ystdex::nullptr;
	#endif
} >> \h Utilities ~ \h YAdaptor;
/ \tr \impl @ \mf void MDualScreenReader::UnloadText() @ \impl \u DSReader;

r24:
/= test 6 ^ \conf release;

r25:
/ @ \u YMenu:
	+ \cl Menu;
	/ \inh (YControl & Widgets::MTextList) -> (YComponent & Menu) @ \cl YMenu;
	/ \a \m \exc (ParentType & Paint) >> \cl Menu ~ YMenu;
	/ @ \cl Menu:
		+ \m Color HilightBackColor;
		+ \m Color HilightTextColor;
		/ \tr @ \ctor;
		+ protected \mf void DrawItems(const Graphics&);

r26:
* \de \param order @ \ctor @ \cl Menu @ \h YMenu;

r27:
/ @ \u Menu:
	/ \merge YMenu::Paint >> Menu::Paint;
	+ \vt @ protected \mf Menu::DrawItems; 

r28:
/= test 7 ^ \conf release;

r29-r40:
/= test 8;

r41:
/ \mf void MLabel::PaintText(Widget&, const Graphics&, const Point&)
	-> void MLabel::PaintText(IWidget&, Color, const Graphics&, const Point&);
/ \tr @ \mf Paint @ \cl (YLabel & YButton);
/ \ft<class _tWidget> \i Rect GetBoundsOf(const _tWidget&) @ \h YWidget
	-> \f \i Rect GetBoundsOf(const IWidget&);
- \ft<class _tWidget> Point LocateOffset(const _tWidget*, const Point&,
	const IWindow*) @ \h YUIContainer;

r42:
/ \a DrawItems => PaintItems;

r43:
/= test 9 ^ \conf release;

r44:
- \mf bool BelongsTo(IUIBox*) const @ \cl Widget @ \h YWidget;

r45-r46:
/= test 10;

r47:
/ @ \cl Rect @ \u YObject:
	/ 2 \i \mf Contains -> !\i \mf;
	/ 2 \i \mf ContainsStrict -> !\i \mf;

r48:
/ @ \cl Rect @ \u YObject:
	/ !\i \mf bool Contains(const Point&) const -> \i \mf
		^ \mac (PDefH1 & ImplRet);
	/ !\i \mf bool ContainsStrict(const Point&) const -> !\i \mf
		^ \mac (PDefH1 & ImplRet);

r49-r53:
/= test 11;

r54:
/ \a Visual -> Widget @ \u YControl;

r55:
/ \a Widget -> Control @ \h YPanel;
/ \a MWindowObject -> MWindow @ \cl AWindow @ \h YWindow;

r56-r58:
/= test 12;

r59
/= test 13 ^ \conf release;

$DOING:

$relative_process:
2011-04-26:
-23.8d;
//c1-c74:r3726;

/ ...


$NEXT_TODO:

b205-b288:
* fatel error @ direct UI drawing testing;
+ menus;
+ Z order;
/ fully \impl \u DSReader;
	* moved text after setting lnGap;
* non-ASCII character path error in FAT16;

b289-b648:
/ impl 'real' RTC;
+ data configuragion;
/ text alignment;
+ \impl pictures loading;
/ improve efficiency @ \ft polymorphic_crosscast @ \h YCast;
+ correct DMA (copy & fill);
* platform-independence @ alpha blending:
	+ \impl general Blit algorithm;
+ shared property: additional;
+ GDI brushes;
/ user-defined bitmap buffer @ \cl YDesktop;
+ \impl styles @ widgets;


$LOW_PRIOR_TODO:
r649-r1024:
+ (compressing & decompressing) @ gfx copying;
+ Microsoft Windows port;
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
$instead_of ~; //features replacing;

//$transform $list ($list_member $pattern $all($exclude $pattern
//	$string_literal "*")) +;

$ellipse_refactoring;

$now
(
	/ "weak handle improvement",
	/ $design "exception specification macro",
	- "window handles" $=
	(
		- "type %HWND",
		^ "%IWindow*" ~ "%HWND"
	),
	* track background painting ignored $since b191,
	+ "class %nullptr_t at namespace ystdex",
	^ "nullptr at YSLib",
	/ $design "widgets virtual member functions"
),

b203
(
	/ $design "units rearrangement",
	/ $design "global architecture" $=
	(
		+ "global instance function",
		- "all global objects",
		- "platform dependent global resource from class %YApplication",
		- "global object in unit YNew"
	),
	+ "class %MTextList",
	/ "class %YSimpleText List using class %MTextList inheritance",
	/ "class %YSimpleText renamed to %YMenu",
	/ "using std::tr1::shared_ptr instead of smart pointers in Loki"
),

b202
(
	/ $design "unit renaming",
	/ "improvoed windows painting efficiency",
	* "buffered coordinate delayed in painting dragged control" $since b169
),

b201
(
	/ "focused button style",
	+ "key helding response in %ShlReader",
	+ "GDI API %BlitTo",
	/ "widgets drawing",
	/ $design "GDI API %CopyTo",
	+ "controls: panel",
	/ $design "YCLib" $=
	(
		+ "partial version checking for compiler and library implementation",
		+ "minor macros in YCLib",
	),
	+ "type conversion helper template",
	+ $design "implicit member overloading by interface parameter with type %IControl
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
	+ "event routing for %KeyUp, %KeyDown and %KeyHeld",
	* "event behavior with optimizing" $since b195,
	+ "keypad shortcut for file selecter",
	+ $design "returning number of called handles in event calling"
),

b198
(
	* "font caching without default font file load successfully",
	+ "showing and hiding windows",
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
	+ "%std::string based font cache"
),

b196
(
	+ "controls: checkbox",
	* "platform color type",
	+ "horizontal text alignment in class %MLabel"
),

b195
(
	* $design "makefiles",
	+ "dependency events",
	+ "simple events routing"
),

b170_b194
(
	+ "controls: track",
	+ "controls: scroll bar",
	+ "controls: scrollable container",
	+ "controls: listbox"
),

b159_b169
(
	+ "controls: buttons": class ("%YThumb", "%YButton"),
	+ "controls: listbox class",
	+ "events"
),

b132_b158
(
	+ $design "core utility templates",
	+ "smart pointers using Loki",
	+ "Anti-Grain Geometry test",
	+ "GUI focus",
	+ "shells",
	+ "base abbreviation macros",
	+ "controls",
	+ "virtual inheritance in control classes",
	+ "exceptions",
	+ "debug macros & functions",
	+ "color type",
	+ "class template %general_cast",
	+ "timer class"
),

b1_b131
(
	+ "initial test with PALib & libnds",
	+ "shell classes",
	+ "the application class",
	+ "CHRLib: character set management",
	+ "fonts management using freetype 2",
	+ "YCLib: platform independence",
	+ "basic objects & counting",
	+ "global objects",
	+ "string class",
	+ "file classes",
	+ "dual screen text file reader framework",
	+ "output devices & desktops",
	+ "messaging",
	+ "program initialization",
	+ "simple GUI: widgets & windows",
	+ "simple GDI",
	+ "simple resource classes"
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

