//v0.3132; *Build 211 r47;
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
	\cl Menu;
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
/ \mf DefGetter(shared_ptr<Content>, ContentHandle, hContent) @ \cl Message
	@ \h YShellMessage -> DefGetter(const Content&, Content, *hContent);
/ \tr \impl @ \mf YShell::DefShlProc @ \impl \u YShell;
/ \tr \impl @ \f ResponseInput @ \impl \u Shell_DS;
* \tr \impl @ \mf YGUIShell::ShlProc @ \impl \u YGUI;
/ \tr \impl @ \f @ \impl \u Main;
/ \tr \impl @ \f main @ \impl \u YGlobal;

r2:
/= test 1 ^ \conf release;

r3-r11:
/ @ \cl Content @ \u YShellMessage:
	+ \i \de \ctor:
	* \impl @ move \ctor Content(Content&&);
	/ \impl @ \dtor;
	* \impl @ \mf \op==;
	* \impl @ copy \ctor Content(const Content&);
/= test 2;

r12:
/ \ctor Message(const shared_ptr<YShell>& = shared_ptr<YShell>(), ID = 0,
	Priority = 0, const shared_ptr<Content>& = shared_ptr<Content>())
	@ \cl Message @ \u YShellMessage -> Message(const Content&,
	const shared_ptr<YShell>& = shared_ptr<YShell>(), ID = 0, Priority = 0);
/ @ \impl \u YApplication:
	/ \tr \simp \impl @ \mf YApplication::SetShellHandle;
	/ \f void SendMessage(const shared_ptr<YShell>&, Messaging::ID,
		Messaging::Priority, Messaging::Content* = nullptr) ynothrow
		-> void SendMessage(const shared_ptr<YShell>&, Messaging::ID,
		Messaging::Priority, const Messaging::Content& = Messaging::Content())
		ynothrow;
/ \tr \impl @ \f WaitForGUIInput @ \impl \u YGlobal;
/ \tr \impl @ \f \i NowShellInsertDropMessage(Messaging::Priority)
	@ \h Shell_DS;
/ \tr \impl @ \f \i void SetShellTo(const shared_ptr<YShell>&,
	Messaging::Priority = 0x80) @ \h YShellHelper;
/ \tr \impl @ \f void PostQuitMessage(int, Priority) @ \impl \u YShell;

r13:
/= test 3 ^ \conf release;

r14-r20:
/= test 4;
/ @ \u YShellMessage:
	/ @ \cl Message:
		/ \m shared_ptr<Content> hContent -> Content content;
		/ \tr \mf DefGetter(const Content&, Content, hContent)
			-> DefGetter(const Content&, Content, content);
		/ \tr \impl @ \ctor;
	/ \tr \simp \impl @ \f bool operator==(const Message&, const Message&);

r21-r27:
/= test 5;

r28:
* @ \cl Content @ \u YShellMessage:
	+ \mf Content& operator=(const Content&);
	+ \mf Content& operator=(Content&&);
	+ \mf void Clear();
	+ \mf void Swap(Content&);
	/ \tr \simp \impl @ \dtor;

r29:
/= test 6 ^ \conf release;

r30:
/ @ \cl Content @ \u YShellMessage:
	/ !\i \dtor -> \i \dtor;
	/ \a 2 !\i \mf \op= -> \i \mf; 

r31:
/= test 7 ^ \conf release;

r32:
/ @ \h YShellMessage:
	/ \impl @ \mft GetObject @ \cl Content;
	/ @ \cl Message:
		/ !\i \mf Message& \op=(const Message&) -> \i \mf;
		+ \i \mf Message& \op=(Message&&);
		+ \i \mf Message& \op=(const Content&);
		+ \i \mf Message& \op=(Content&&);

r33:
- \h "../Core/yobject.h" @ \h YComponent;
- \h "yobject.h" @ \h YShell;
/ @ \ns Messaging @ \h YShellMessageDefinition:
	+ \stt<MessageID ID> SMessageMap;
	+ \ft<MessageID ID> const typename SMessageMap<ID>::TargetType&
		FetchTarget(const Message&);
	+ \mac
	{
		DefMessageTarget(SM_NULL, void)
		DefMessageTarget(SM_SET, shared_ptr<YShell>)
		DefMessageTarget(SM_DROP, shared_ptr<YShell>)
		DefMessageTarget(SM_ACTIVATED, shared_ptr<YShell>)
		DefMessageTarget(SM_DEACTIVATED, shared_ptr<YShell>)
		DefMessageTarget(SM_PAINT, shared_ptr<YShell>)
		DefMessageTarget(SM_QUIT, int)
	};
/ @ \h YGlobal:
	/ \inc \h YShellMessage -> \h YShellMessageDefiniton;
	- \inc \h "../Core/ycutil.h";
	+ \mac DefMessageTarget(SM_INPUT, shared_ptr<InputContent>) @ \ns Messaging;
/ \tr \impl @ \mf YShell::DefShlProc @ \impl \u YShell ^ FetchTarget;
/ @ \h YApplication:
	+ \ft<Messaging::MessageID ID> \i void
		SendMessage(const shared_ptr<YShell>&, Messaging::Priority,
		const typename Messaging::SMessageMap<ID>::TargetType&) ynothrow;
/ \tr \impl @ \f \i NowShellInsertDropMessage @ \h Shell_DS ^ \ft SendMessage;
/ \tr \impl @ \f \i SetShellTo @ \h YShellHelper ^ \ft SendMessage;
/ \tr \impl @ \f PostQuitMessage @ \impl \u YShell ^ \ft SendMessage;
/ \tr (\rem & \impl @ \f ResponseInput) @ \impl \u Shell_DS ^ \ft SendMessage;
/ \tr \impl @ \f WaitForGUIInput @ \un \ns @ \impl \u YGlobal ^ \ft SendMessage;
/ \tr \impl @ \mf YGUIShell::ShlProc @ \impl \u YGUI ^ \ft SendMessage;

r34:
^ \conf release;
/= test 8;

r35:
/ !\i \ft FetchTarget -> \i \ft @ \h YShellMessageDefinition;

r36:
^ \conf debug;
/ @ \u YShellMessage:
	/ @ \cl MessageQueue:
		/ \inh public YObject -> public noncopyable;
		+ \dtor \vt DefEmptyDtor(MessageQueue);
/ \a MessageQueue => MessageQueue;
/ @ \h YShellDefinition:
	- \pre \decl \cl Messaging::MessageQueue;
	/ using Messaging::MessageQueue >> \h YApplication;

r37:
/ @ \cl Content @ \u YShellMessage:
	/ \impl @ 2 \mf \op=;
	* \impl @ \mf Content& operator=(Content&&) for self-assignment;
	/ \mf \i Content& operator=(Content&&) -> !\i \mf;

r38:
/= test 9 ^ \conf release;

r39:
/ @ \impl \u Shells:
	/ @ \un \ns:
		+ \f shared_ptr<Menu::ListType> GenerateList();
		+ \g \o MenuHost s_MenuHost;
	/ @ \cl MenuHost:
		+ \i \mf void Clear();
		/ protected \m MenuMap sMenus => mMenus;
		+ \i \mf ItemType operator[](ID);
	/ \impl @ \mf (OnActivated & OnDeactivated & TFormTest::OnClick_btnMenuTest)
		@ \cl ShlSetting;
+= \rem @ \amf GetContainerPtr @ \in IWidget @ \h YWidget;

r40:
/ \f PostQuitMessage @ \ns Shells @ \u YShell >> \ns YSLib @ \u YApplication;
/ \a GetCurrentShellHandle => FetchCurrentShellHandle;
/ \a GetMainShellHandle => FetchMainShellHandle;
/ \a GetApp => FetchAppInstance;
/ \a GetGlobal => FetchGlobalInstance;
/ \a FetchCurrentShellHandle => FetchShellHandle;
- \i \f FetchShellHandle @ \ns YSLib @ \h YComponent;
+ using (Activate & using FetchShellHandle) @ \ns YSLib @ \h YShell;
/ \tr \impl @ \mf ShlSetting::TFormExtra @ \impl \u Shells;

r41:
/ \a GetMessage => FetchMessage;
/ @ \u YShell:
	/ @ \ns Shells:
		/ \f (PeekMessage & FetchMessage & TranslateMessage & DispatchMessage
			& BackupMessageQueue & RecoverMessageQueue) >> \ns YSLib
			@ \u YApplication;
		/ !\i \f (FetchShellHandle & Activate) >> \i \f @ \ns YSLib
			@ \h YApplication;
	- using (Activate & using FetchShellHandle) @ \ns YSLib @ \h;
/ \decl @ \f FetchAppInstance @ \h YGlobal >> \h YApplication;
/ \tr \impl @ \f \i (NowShellTo & SetShellTo) @ \h YShellHelper;
/ \tr \impl @ \f \i NowShellInsertDropMessage @ \h Shell_DS;
/ \tr \impl @ \f WaitForGUIInput @ \un \ns @ \impl \u YGlobal;

r42:
/= test 10 ^ \conf release;

r43:
/ @ \impl \u Shells:
	/ @ \cl MenuHost:
		+ \mf void ShowMenu();
		+ \mf void HideMenu();
	/ \impl @ \mf ShlSetting::TFormTest::OnClick_btnMenuTest;

r44:
/ @ \impl \u Shells:
	/ \mf DefGetter(shared_ptr<Desktop>, DesktopHandle, hDesktop) @ \cl MenuHost
		-> DefMutableGetter(shared_ptr<Desktop>&, DesktopHandle, hDesktop);
	/ \impl @ \mf ShlSetting::OnActivated;

r45:
/ @ \impl \u Shells:
	/ \simp @ \cl MenuHost:
		- \mf DefMutableGetter(shared_ptr<Desktop>&, DesktopHandle, hDesktop);
		/ @ protected \m shared_ptr<Desktop> hDesktop
			-> public \m shared_ptr<Desktop> DesktopHandle;
		/ \tr @ \ctor;
	/ \tr \impl @ \mf ShlSetting::OnActivated;

r46:
/ @ \impl \u Shells:
	/ \o MenuHost s_MenuHost -> MenuHost* s_pMenuHost;
	/ \tr \impl @ \mf (OnActivated & OnDeactivated
		& TFormTest::OnClick_btnMenuTest) @ \cl ShlSetting;

r47:
/= test 11 ^ \conf release;


$DOING:

$relative_process:
2011-05-22:
-21.1d;
//Mercurial rev1-rev81: r4053;

/ ...


$NEXT_TODO:

b212-b324:
+ menus;
^ unique_ptr ~ GHandle @ \clt GEventMap @ \h YEvent avoiding patching
	libstdc++ for known associative container operation bugs:
	- \rem @ \impl @ \mft GetEvent @ \clt EventMap @ \h YEvent;
* non-ASCII character path error in FAT16;
+ overlay for \a widgets;
/ fully \impl \u DSReader;
	* moved text after setting lnGap;

b325-b768:
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
	/ "implemented messages with general object content holder"
		^ "non-pointer member" ~ "%shared_ptr",
	+ "message content mapping",
	/ $design "messaging APIs moved to unit YApplication from YShell",
	/ "test menu fixed on the desktop"
),

$b210
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

