//v0.3134; *Build 213 r36;
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
	\cl Console;
}
\u YWidget
{
	\in IWidget;
	\cl Visual;
	\cl Widget;
}
\u YLabel
{
	\cl MLabel;
	\cl Label;
	\cl MTextList;
}
\u YUIContainer
{
	\in IUIBox;
	\in IUIContainer;
	\cl UIContainer;
}
\u YControl
{
	\in IControl;
	\cl Control;
}
\u YMenu
{
	\cl TextList;
}
\u YPanel
{
	\in IPanel;
	\cl Panel;
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
	\cl Frame;
}
\u YForm
{
	\cl Form;
}
\u YGUI
{
	\cl YGUIShell;
}
\u Button
{
	\cl Thumb;
	\cl Button;
}
\u Scroll
{
	\cl ATrack;
	\cl HorizontalTrack;
	\cl VerticalTrack;
	\cl MScrollBar;
	\cl AScrollBar;
	\cl HorizontalScrollBar;
	\cl VerticalScrollBar;
}
\u ListBox
{
	\cl ListBox;
	\cl FileBox;
}


$DONE:
r1:
* \as \str @ 5 \mf Paint @ \impl \u (ListBox & Button & CheckBox & Scroll);

r2:
* \impl @ \ctor @ \cl Menu @ \impl \u Shells;

r3:
/ @ \cl ShlSetting::TFormTest @ \u Shells:
	/ \mf void OnClick_btnShowWindow(TouchEventArgs&&)
		-> \mf @ \cl ShlSetting;
	/ \tr \impl @ \ctor;

r4:
/ @ \cl ShlSetting @ \u Shells:
	/ \!s \mf void OnClick_btnShowWindow(TouchEventArgs&&)
		-> \smf void OnClick_ShowWindow(IControl&, TouchEventArgs&&);
	/ \tr \impl @ \ctor @ \cl TFormTest;

r5:
/ \a NewScrImage => CreateSharedScreenImage;
/ \a NewBitmapRaw => CreateRawBitmap;
/ \a NewWindow => CreateWindow;
/ @ \h YShellHelper:
	+ \ft FetchShell;
	- \mac DefDynInitRef;
/ @ \impl \u Shells:
	/ \tr \impl @ 4 \mf ^ direct \decl ~ ^ \a \mac DefdynInitRef;
	* \impl @ \mf ShlSetting::TFormExtra::OnKeyPress_btnDragTest;
	/ \simp \impl @ \mf ShlSetting::OnClick_ShowWindow;

r6:
/= test 1 ^ \conf release;

r7:
/ \simp \impl @ \mf (OnTouchUp_btnDragTest & OnTouchDown_btnDragTest)
	^ FetchShell ~ (FetchShellHandle & dynamic_pointer_cast)
	@ \cl ShlSetting::TFormExtra @ \impl \u Shells;

r8:
/ \impl @ \mf ShlSetting::OnActivated @ \impl \u Shells;

r9:
* \impl @ \mf YGUIShell::ResponseTouch @ \impl \u YGUI;

r10:
/= test 2 ^ \conf release;

r11:
/ @ \impl \u Shells:
	/ @ \cl Menu:
		/ private \m MenuHost* pMenuHost -> Menu* pParent;
		/ \exp \ctor Menu(const Rect& = Rect::Empty, const shared_ptr<ListType>&
			= shared_ptr<ListType>(), MenuID = 0) -> \exp Menu(const Rect&
			= Rect::Empty, const shared_ptr<ListType>& = shared_ptr<ListType>(),
			MenuID = 0, Menu* = nullptr);
		- \decl friend class MenuHost;
	+ \f void ResizeForContent(Menu&);
	* \impl \mf ShlSetting::TFormTest::OnClick_btnMenuTest ^ ResizeForContent;
	/ \tr \impl @ 2 \mf \op+= @ \cl MenuHost;
	- \pre \decl \cl MenuHost;

r12-r13:
* \impl @ \mf ShlSetting::TFormTest::OnClick_btnMenuTest @ \impl \u;

r14:
/ @ \impl \u Shells:
	/ @ \cl Menu:
		+ \decl friend class MenuHost;
		/ \ac @ private \m pMenu -> protected;
		+ protected \m MenuHost* pHost;
	+ \pre \decl \cl MenuHost;
	/ @ \cl MenuHost;
		/ \tr \impl @ 2 \mf \op+=;
		/ \i \mf (\op-= & Clear) -> !\i \mf with \tr \impl;
	
r15:
/ @ \impl \u Shells:
	/ @ \cl Menu:
		+ private \mf void OnLostFocus(EventArgs&&);
		/ \tr \impl @ \ctor;

r16:
/ \impl @ \mf void Menu::OnLostFocus(EventArgs&&) @ \impl \u Shells;

r17-r20:
/= test 3;

r21:
* \impl @ \mf void Menu::OnLostFocus(EventArgs&&) @ \impl \u Shells;

r22:
/ @ \impl \u Shells:
	+ \mf void HideMenu(Menu::MenuID) @ \cl MenuHost;
	* \impl @ \mf void Menu::OnLostFocus(EventArgs&&);

r23:
/ @ \cl Menu @ \impl \u Shells:
	/ public \m MenuID ID => id;
	/ typedef size_t MenuID => ID;
	+ DefGetter(ID, ID, id);
-= \a semicolons after \mac 'DefGetter';

r24:
/ @ \cl MUIContainer:
	/ protected \mf bool CheckWidget(IWidget&)
		-> public \mf bool Contains(IWidget&);
	/ \tr \impl @ \mf (void perator+=(IWidget&)
		& void Add(IControl&, ZOrderType));
	/= \tr \impl @ \mf (GetTopWidgetPtr & GetTopControlPtr);
/= \tr \impl @ \mf IControl* Desktop::GetTopVisibleDesktopObjectPtr();

r25:
/ @ \impl \u Shells:
	+ \mf bool IsShowing(Menu::ID) @ \cl MenuHost;
	/ \impl @ \mf ShlSetting::TFormTest::OnClick_btnMenuTest
		^ \mf MenuHost::IsShowing;
+ using MUIContainer::Contains @ \cl (Panel & AFrame);

r26:
/= test 4 ^ \conf release;

r27:
/ @ \cl MenuHost @ \impl \u Shells:
	+ \mf void ShowMenu(Menu::ID, ZOrderType = DefaultMenuZOrder);
	/ \impl @ \mf void HideMenu(Menu::ID);
	* \impl @ \dtor;
/ \a \mf ShowMenu => Show;
/ \a \mf HideMenu => Hide;

r28:
/ \impl @ \mf void IsShowing(Menu::ID) @ \cl MenuHost @ \impl \u Shells;

r29:
/ @ \impl \u Shells:
	/ @ \cl MenuHost:
		/ \m AFrame* FramePointer -> AFrame& Frame;
		/ \tr \ctor MenuHost(AFrame* = nullptr) -> MenuHost(AFrame&);
		+ \inh noncopyable;
		/ \tr \simp \impl @ \mf (IsShowing & 2 Show & 2 Hide);
	/ \tr \impl @ \mf ShlSetting::OnActivated;

r30:
/ \impl @ \mf ShlSetting::TFormTest::OnClick_btnMenuTest @ \impl \u Shells;

r31:
/ typedef std::ptrdiff_t IndexType -> typedef ssize_t IndexType
	@ \st IndexEventArgs @ \h YControl;
/ \a std::ptrdiff_t -> ssize_t @ \h YViewer;
/= \a std::ptrdiff_t -> ptrdiff_t @ (\h YFile & \u (YTextManager & YText));

r32:
/= test 5 ^ \conf release;

r33:
* \impl @ \mf bool MenuHost::IsShowing(Menu::ID) @ \impl \u Shells;

r34:
/= test 6 ^ \conf release;

r35:
/ @ \impl \u Shells:
	/ @ \cl MenuHost:
		/ \ret \tp @ \i \mf ItemType operator[](Menu::ID) -> Menu&;
		* \impl @ \mf op-=;
		/ \tr \simp \impl @ \mf IsShowing;
	/ \tr \simp \impl @ \mf ShlSetting::TFormTest::OnClick_btnMenuTest;

r36:
/= test 7 ^ \conf release;


$DOING:

$relative_process:
2011-05-31:
-21.7d;
//Mercurial rev1-rev83: r4141;

/ ...


$NEXT_TODO:
b214-b256:
+ menus;
^ unique_ptr ~ GHandle @ \clt GEventMap @ \h YEvent avoiding patching
	libstdc++ for known associative container operation bugs:
	- \rem @ \impl @ \mft GetEvent @ \clt EventMap @ \h YEvent;
* non-ASCII character path error in FAT16;

b257-b768:
+ \impl styles @ widgets;
/ fully \impl \u DSReader;
	* moved text after setting lnGap;
+ data configuragion;
/ impl 'real' RTC;
/ text alignment;
+ \impl pictures loading;
/ improve efficiency @ \ft polymorphic_crosscast @ \h YCast;
+ correct DMA (copy & fill);
* platform-independence @ alpha blending:
	+ \impl general Blit algorithm;
+ shared property: additional;
+ GDI brushes;
/ user-defined bitmap buffer @ \cl Desktop;


$LOW_PRIOR_TODO:
b769-b1536:
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

$design; //features changing only made sense to library developers;
$add_features +; //features added;
$fix_bugs *; //bugs fixed;
$modify_features /; //features modified;
$remove_features -; //features removed;
$using ^; //using;
$instead_of ~; //features replacing;

//$transform $list ($list_member $pattern $all($exclude $pattern \
//	$string_literal "*")) +;

$ellipse_refactoring;

$now
(
	* $design "UI assertion strings",
	* "menu colors",
	* "touch event coordinate error in contianer controls" $since b195,
	+ "functions of menus" $=
	(
		+ "resizing for content",
		+ "hiding when focus lost",
	),
	+ "predicator %Contains in UI container implementation"
),

b212
(
	/ $design "assertion strings",
	/ "menu constructor",
	+ "redundant menu state member in class %Menu",
	/ "container member APIs" ^ "reference parameter" ~ "pointer parameter",
	* "constructor of class %Frame",
	+ "Z order for widget overlaying in UI containers",
	+ "menu laid at top level" ^ "Z order"
),

b211
(
	/ "implemented messages with general object content holder"
		^ "non-pointer member" ~ "%shared_ptr",
	+ "message content mapping",
	/ $design "messaging APIs moved to unit YApplication from YShell",
	/ "test menu fixed on the desktop"
),

b210
(
	+ "template %is_valid for checking pointers which cast bool explicitly",
	/ "part of template %raw moved to namespace %ystdex",
	- $design "some unused code",
	+ "macro %yforward for simply using std::forward and argument \
		type deduction",
	/ "handles and smart pointers rearranged" $=
	(

		- "%GHWeak",
		^ "%std::shared_ptr" ~ "%GHandle",
		- "%GHandle"
	),
	/ "implementation of messages" $=
	(
		/ "implementation of class %Message",
		- "message ID mapping"
	)
),

b209
(
	/ "library CHRLib moved and merged to library YCLib",
	/ $design "protected function inheritance in class template %GHEvent",
	/ "using directive of namespace %ystdex in YSLib",
	+ "lost %Rect operations",
	* "strict ISO C++2003 code compatibility" $=
	(
		^ "fixed macros in \"yfont.h\""
	),
	/ "renamed directory \"Shell\" to \"UI\" in YSLib",
	/ "several memory utilities for std::shared_ptr and std::unique_ptr \
		moved to library YCLib::YStandardExtend"
),

b208
(
	^ "rvalue references as parameter types of event handlers",
	/ "set default font size smaller",
	+ "vertical alignment in labeled controls",
	* "fatel error in direct UI drawing testing"
),

b207
(
	/ $design "event handler implementation ^ std::function" ~ "Loki::Function",
	- "library AGG",
	- "library Loki",
	+ $design "union %no_copy_t and union %any_pod_t for supressing \
		static type checking",
	+ $design "polymorphic function object template and base class",
	- "single-cast event class template",
	^ "rvalue reference in event class",
	* "ystdex algorithms",
	* "minor event handler parameter type mismatch in test code",
	* "key events response(added as %KeyDown, but removed as %KeyPress) in \
		YSTest_ARM9" $since b201,
	* "tunnel and direct key and touch response repeated in %YGUIShell"
		$since b198
),

b206
(
	+ "menu test button",
	/ "file API",
	* "strict ISO C++2003 code compatibility" $=
	(
		+ "function %memcmp declation in namespace %ystdex"
	),
	* "strict ISO C++2011 code compatibility" $=
	(
		* "implicit narrowing conversion(N3242 8.5.4/6) \
			in C++2011(N3242 5.17/9)" ^ "explicit static_cast",
		/ "character types in \"platform.h\""
	),
	/ "coding using limited C++2011 features" $=
	(
		/ $design ^ "C++2011 style nested template angle brackets",
		/ $design ^ "keyword %auto",
		/ ^ "C++2011 type_traits in namespace std" ~ "std::tr1"
	),
	- "Loki type operations",
	/ ^ "namespace ystdex in namespace YSLib"
),

b205
(
	/ "simplified widgets and controls inheritance",
	/ "simplified image resource type",
	/ $design "protected inheritance of module classes except \
		%MScreenPositionEventArgs",
	/ "listbox highlight text color"
),

b204
(
	/ "weak handle improvement",
	/ $design "exception specification macro",
	- "window handles" $=
	(
		- "type %HWND",
		^ "%IWindow*" ~ "%HWND"
	),
	* "track background painting ignored" $since b191,
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
	/ "using std::tr1::shared_ptr" ~ "smart pointers in Loki"
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
	+ $design "implicit member overloading by interface parameter with type \
		%IControl and %IWidget in container class"
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
	/ $design "using pointers" ~ "references in parameters \
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
			msg.GetPriority(), msg.GetObjectID(), msg.GetWParam(),
			msg.GetLParam());
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

