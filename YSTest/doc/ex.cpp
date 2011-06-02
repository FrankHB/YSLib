//v0.3134; *Build 214 r33;
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
/ @ \impl \u Shells:
	/ @ \cl Menu:
		+ typedef map<IndexType, Menu*> SubMap;
		+ protected \m SubMap mSubMenus;
		/ \tr \impl @ \ctor;
		+ typedef SubMap::value_type ValueType;
		+ \mf void operator+=(const ValueType&);
		+ \mf bool operator-=(IndexType);
	*= \mf \decl order @ \cl MenuHost;

r2:
/= \simp \a qulified-id @ \tp @ \param @ \f \def -> less qulified-id;

r3:
/ @ \cl Menu @ \impl \u Shells:
	+ \mf void OnConfirmed(IndexEventArgs&&);
	/ \simp @ \ctor;

r4:
-= \inh <algorithm> @ \h YDesktop;
/ @ \impl \u Shells:
	/ @ \cl MenuHost:
		+ \i \mf bool Contains(Menu::ID) ^ \mac (PDefH1 & ImplRet);
		+ \mf bool Contains(Menu&);
		+ \i \mf void Hide(Menu&) ^ \mf bool Contains(Menu&);
	/ \simp \impl @ \mf Menu::OnConfirmed ^ \mf void MenuHost::Hide(Menu&);
* \impl @ \mf YApplication& YApplication::GetInstance();
* \impl @ \f void CopyBuffer(const Graphics&, const Graphics&) @ \impl \u YGDI;

r5:
* @ \cl YApplication:
	- \es @ \mf GetInstancePtr & GetInstance;

r6:
/= test 1 ^ \conf release;

r7:
* Doxygen description:
	* \a Doxygen \t \param description:
		^ '\tparam' ~ '\param' @ \h (Memory & YGDI & YObject);
	/ \a '\defGroup' -> 'defgroup' @ \h (Memory & YReference);
* Doxygen \rem @ \h YNew;
* Doxygen \rem @ \impl \u Shells;
* Doxygen \rem @ \mf void ATrack::CheckScroll(ScrollEventSpace::ScrollEventType,
	ValueType);
/ \simp \exc @ Doxygen file;
/ \a \ac @ private \inh noncopyable @ \h (YTextManager & YObject) -> public;

r8:
/ @ \impl \u Shells:
	/ @ \un \ns
		\f shared_ptr<TextList::ListType> GenerateList()
			-> shared_ptr<TextList::ListType> GenerateList(const String&);
	/ \impl @ \mf ShlSetting::OnActivated;
	/ @ \cl Menu:
		+ \mf bool Show(ZOrderType = DefaultMenuZOrder);
		+ \mf bool Hide();
		/ \impl @ \mf OnConfirmed;
		+ \i \mf Menu& operator[](size_t) ^ \mac PDefHOperator1;

r9:
* \impl @ \mf bool Rect::Contains(int px, int py) const;

r10:
/ void ResizeForContent(Menu&) -> void ResizeForContent(TextList&);
* \c @ \u YText:
	/ \f SDst FetchCharWidth(Font&, fchar_t)
		-> SDst FetchCharWidth(const Font&, fchar_t);
	/ 1st \param \tp Font& -> const Font& @ 3 (\f & \ft) FetchStringWidth;
	/ last \param \tp String& -> const String& @ 2 \f \i FetchStringWidth;

r11:
* \impl @ \f void ResizeForContent(TextList&) @ \impl \u Shells;

r12:
/= test 2 ^ \conf release;

r13-r15:
/ \impl @ \ctor @ \cl Menu @ \impl \u Shells;

r16:
/ @ \u YLabel:
	/ !\i \mf (GetTextState & GetItemHeight) @ \cl MTextList -> \i \mf;
	/ !\i \ctor @ \cl Label -> \i \ctor;

r17:
/ \impl @ \mf ShlSetting::OnActivated;

r18:
/ @ \cl MenuHost @ \impl \u Shells:
	/ \mf void Show() -> void ShowAll(ZOrderType = DefaultMenuZOrder);
	/ \mf void Hide() => void HideAll();
	/ \mf \i void Hide(Menu&) -> void HideRaw(Menu&);
	+ \mf !\i void ShowRaw(Menu&, ZOrderType = DefaultMenuZOrder);
	/ \tr \impl @ \mf (Show & ShowAll & Hide & HideAll);
		/ \tr @ \as \str;
		^ \mf ShowRaw & HideRaw;

r19:
/= test 3 ^ \conf release;

r20:
/ @ \u YControl:
	/ @ \in IControl:
		/ \amf DeclIEntry(void RequestFocus(EventArgs&&))
			-> DeclIEntry(void RequestFocus());
		/ \amf DeclIEntry(void ReleaseFocus(EventArgs&&))
			-> DeclIEntry(void ReleaseFocus());
	/ \tr @ \cl Control:
		/ \mf void RequestFocus(EventArgs&&) -> void ReleaseFocus();
		/ \mf void ReleaseFocus(EventArgs&&) -> void ReleaseFocus();
		/ \tr \simp \impl @ \dtor;
		/ \tr \simp \impl @ \mf OnTouchDown;
/ \tr \simp \impl @ (\f (RequestFocusCascade & ReleaseFocusCascade)
	& \mf YGUIShell::ResponseTouch) @ \impl \u YGUI;
/ \tr \simp \impl @ \mf SetFocusingPtr @ \clt GMFocusResponser @ \h YFocus;
/ \tr \simp \impl @ \rem @ \impl \u YDesktop;
/ \tr \simp \impl \mf MenuHost::ShowRaw @ \impl \u Shells;

r21:
/ @ \impl \u Shells:
	/ @ \cl MenuHost:
		+ \m Menu* pLocked;
		/ \tr \impl @ \ctor;
		+ DefGetter(Menu*, LockedPtr, pLocked);
		+ \mf \i void Lock(Menu&);
		/ \mf !\i void ShowRaw(Menu&, ZOrderType = DefaultMenuZOrder)
			-> \i void Show(Menu&, ZOrderType = DefaultMenuZOrder);
		+ private !\i \mf void ShowRaw(Menu&, ZOrderType = DefaultMenuZOrder);
		/ \mf void HideRaw(Menu&) -> void Hide(Menu&);
		+ private !\i \mf void HideRaw(Menu&);
		/ \impl @ \a \mf (Hide & HideAll) ^ \mf HideRaw;
	/ \impl @ \mf Menu::OnConfirmed;

r22-r25:
/= test 4;

r26-r28:
/ @ \impl \u Shells:
	/ @ \cl MenuHost:
		- \mf DefGetter(Menu*, LockedPtr, pLocked);
		/ protected \m Menu* pLock -> public Menu* \m SubMenuPointer;
		/ \m order;
		/ \tr \impl @ \ctor;
		- \tr \mf Lock;
		/ \tr \impl @ \mf HideRaw;
	/ \impl @ \mf (OnConfirmed & OnLostFocus) @ \cl Menu;

r29:
/ @ \cl Menu @ \impl \u Shells:
	* \impl @ \mf operator+=;
	* \impl @ \mf operator-=;
	+ \mf DefGetter(Menu*, ParentPtr, pParent);

r30:
* \impl @ \mf OnLostFocus @ \cl Menu @ \impl \u Shells;

r31:
/= test 5 ^ \conf release;

r32:
/ @ \dir UI:
	/ \u YMenu -> TextList["textlist.h", "textlist.cpp"];
	+ \u Menu["menu.h", "menu.cpp"];
	/ (\cl (Menu & MenuHost) & \o DefaultMenuZOrder) @ \impl \u Shells
		>> \ns Components::Controls @ \u Menu;
	/ \f ResizeForConent @ \impl \u Shells >> \ns Components::Controls
		@ \u TextList;
/ \u YTextManager["ytmgr.h", "ytmgr.cpp"] @ \dir Service
	=> TextManager["textmgr.h", "textmgr.cpp"];
/ @ \h Build:
	/ \tr \inc
	- \inc \h YShellDefinition;
	+ \inc \h Menu;
	/= \inc order;
/ \tr \inc @ \h (DSReader & ListBox);
/ @ \h YComponent:
	+ \pre \decl \cl (AWindow & AFrame) @ \ns Components::Forms;
	- using Components::Forms::Frame @ \ns YSLib;
-= \impl \u YComponent;
/ \tr @ \decl @ \cl Desktop;
+ \inc \h YUIContainer @ \h Menu;

r33:
/= test 6 ^ \conf release;


$DOING:

$relative_process:
2011-06-02:
-19.9d;
//Mercurial rev1-rev84: r4177;

/ ...


$NEXT_TODO:
b215-b256:
+ menus;
^ unique_ptr ~ GHandle @ \clt GEventMap @ \h YEvent avoiding patching
	libstdc++ for known associative container operation bugs:
	- \rem @ \impl @ \mft GetEvent @ \clt EventMap @ \h YEvent;
* non-ASCII character path error in FAT16;

b257-b768:
+ key accelerators;
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

$design; // features changing only made sense to library developers;
$doc; // for documents target;
$add_features +; // features added;
$fix_bugs *; // bugs fixed;
$modify_features /; // features modified;
$remove_features -; // features removed;
$using ^; // using;
$instead_of ~; // features replacing;

//$transform $list ($list_member $pattern $all($exclude $pattern \
//	$string_literal "*")) +;

$ellipse_refactoring;

$now
(
	/ "functions of menus" $=
	(
		+ "hiding on confirmed",
		+ "resizing with width of text in list",
		/ "margin of menus",
		+ "submenus" $= 
	),
	* $design "exception specification in %YApplication",
	/ $doc $= 
	(
		* "template parameter description" $since b189 $=
		(
			^ "\tparam" ~ "\param"
		),
		* "\defgroup description spell error" $since b209,
		* $design "operator new & delete conmments" $since b203,
		/ "simplified doxygen file excluded paths"
	),
	/ $design ^ "public %noncopyable inheritance"
		~ "all private %noncopyable inheritance",
	* "point containing test for zero width or height rectangle \
		turned out asseration failure" $since b204,
	* "constness of text width mersuring" $since b197,
	/ "simplified focus operations interface" $=
	(
		- "unused parameter and argument"
	)
),

b213
(
	* $design "UI assertion strings",
	* "menu colors",
	* "touch event coordinate error in contianer controls" $since b195,
	+ "functions of menus" $=
	(
		+ "resizing for content",
		+ "hiding when focus lost"
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
	+ "controls: tracks",
	+ "controls: scroll bars",
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

