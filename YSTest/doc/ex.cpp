//v0.3202; *Build 216 r38;
/*
$META:
//$configureation_for_custom_NPL_script_parser:
$parser
(
	$eval($ignore_not_impl);
	$eval($for_each $anything $in $.iterators $do_something) unfold;
);
$parser.$ __pstate;
$parser.unfold __unfold;
$parser.$.iterators __iterators;

__unfold.$ctor $=
(
	__pstate.behavior.ignore $= $true;
);
__unfold.$dtor $=
(
	__pstate.behavior.ignore $= $false;
);
__unfold __iterators.for_labeled_paragraph
(
	$DONE,
	$DOING,
	$NEXT_TODO,
	$LOW_PRIOR_TODO,
	$KNOWN_ISSUE,
	$TODO
);

$script_preprocessor_escapse:
//$Record prefix and abbrevations:
$parser.$preprocessor.$define_schema "<statement> ::= $statement_in_literal";
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
\smft ::= static member function templates
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
(
	\cl YObject,
	\cl YCountableObject,
	\clt GDependence
),
\u YShell
(
	\cl YShell
),
\u YApplication
(
	\cl YLog,
	\cl YApplication
),
\u YConsole
(
	\cl Console
),
\u YWidget
(
	\in IWidget,
	\cl Visual,
	\cl Widget
),
\u YLabel
(
	\cl MLabel,
	\cl Label,
	\cl MTextList
),
\u YUIContainer
(
	\in IUIBox,
	\in IUIContainer,
	\cl UIContainer
),
\u YControl
(
	\in IControl,
	\cl Control
),
\u YPanel
(
	\in IPanel,
	\cl Panel
)
\u YWindow
(
	\in IWindow,
	\cl MWindow,
	\cl AWindow,
	\cl AFrame,
	\cl Frame
),
\u YGUI
(
	\cl YGUIShell,
)
\u UIContainerEx
(
	\cl AUIBoxControl,
),
\u Form
(
	\cl Form;
),
\u Button
(
	\cl Thumb,
	\cl Button
),
\u CheckBox
(
	\cl CheckBox
),
\u TextList
(
	\cl TextList
),
\u Menu
(
	\u Menu
),
\u Scroll
(
	\cl ATrack,
	\cl HorizontalTrack,
	\cl VerticalTrack,
	\cl MScrollBar,
	\cl AScrollBar,
	\cl HorizontalScrollBar,
	\cl VerticalScrollBar
),
\u ListBox
(
	\cl ListBox,
	\cl FileBox,
);


$DONE:
r1:
- \f Destroy_Static @ \impl \u YGlobal;
/ \tr \impl @ \ctor @ \cl YApplication;

r2:
/ \def @ \f FetchMainShellHandle >> \h YApplicatin -> \i \f ~ \impl \u YGlobal;
-= \inc \h (YObject & YString & YShellMessage & YException) @ \h YApplication;
-= \decl @ \f int MainShlProc(const Message&) @ \h YGlobal;

r3:
/ @ \u YStyle:
	- \f DrawWidgetOutline;
	/ \f void DrawWindowBounds(IWindow*, Color)
		-> void DrawWindowBounds(IWindow&, Color);
	/ \impl @ \f void DrawWidgetBounds(IWidget&, Color);
	- \f DrawWidgetBounds @ \un \ns @ \impl \u;
	/ \f void WndDrawFocus(IWindow&, const Size&)
		-> void WndDrawFocus(IWindow&);
/ \tr \impl @ \mf Paint @ (CheckBox & TextList);

r4-r6:
/ \impl @ \f WndDrawFocus @ \impl \u YStyle;

r7:
- \f WndDrawFocus @ \impl \u YStyle;
/ \tr \impl @ \mf Paint @ (CheckBox & TextList);

r8:
/= test 1 ^ \conf release;

r9:
- \i \f DefShellProc @ \ns Shells @ \h YShell;
/ \tr \a DefShellProc -> (YShell::DefShlProc | DefShlProc) @ \impl \u Shells;

r10:
/ \cl YMainShell @ \ns Shells @ \u YShell >> \ns Shells @ \u YGlobal;
/ \tr \i \f FetchMainShellHandle @ \u YApplication -> !\i \f;
/ \decl extern \f int MainShlProc(const Message&) @ \ns YSLib
	>> \impl \u YGlobal ~ \impl \u YShell;
- using Shells::YMainShell @ \ns YSLib @ \h YShellDefinition;

r11-r12:
/= test 2;

r13:
/ \simp @ \f (LoadL & LoadS & LoadA) @ \un \ns @ \u Shells:
	- \a \tb;

r14:
/= test 3 ^ \conf release;

r15:
/ @ \cl MessageQueue:
	/ \mf FetchMessage => GetMessage;
	/ \impl @ \mf PeekMessage;
	+ \i \mf void PopMessage();
	/ !\i \mf PeekMessage -> \i \mf;
/ \tr \impl @ \f PeekMessage @ \impl \u YApplication;

r16:
- \mac YSL_DEBUG_MSG @ \h Configuration;
+ \mf int PeekMessage(Message&, const shared_ptr<YShell>&, bool)
	@ \cl MessageQueue;
/ \tr \impl @ \f PeekMessage @ \impl \u YApplication;
* \sf PeekMessage_ -> \un \ns;

r17:
/ @ \u YShellMessage;
	/ @ \cl MessageQueue:
		/ private \m priority_queue<Message, vector<Message>, cmp> q
			-> multiset<Message, cmp> q;
		- private \i \mf (pop & push & top);
		/ typedef priority_queue<int, vector<int>, cmp>::size_type SizeType
			-> typedef decltype(q.size()) SizeType;
		/ \simp \impl @ \mf \op() @ private \cl cmp;
		/ \tr \i \mf PeekMessage & PopMessage -> !\i \mf;
		/ \tr \impl @ \mf (Insert & Clear & Update & GetMessage);
	/ \tr \impl @ \f void Merge(MessageQueue&, list<Message>&);

r18:
/= test 4 ^ \conf release;

r19:
/ @ \cl MessageQueue:
	+ private \i \mf (top & pop & push) ^ \mac ('PDefH?' & ImplRet);
	/ \simp @ \impl @ \mf ^ \mf (top & pop & push);

r20:
/ @ \u YShellMessage:
	/ @ \cl MessageQueue:
		/= \mf order;
		* \def @ \mf void PeekMessage(Message&) const:
			- \i;
		+ \mf void Merge(MessageQueue&);
		/ \impl @ \mf int PeekMessage(Message&, const shared_ptr<YShell>&,
			bool);
	/ \impl @ \f void Merge(MessageQueue&, MessageQueue&)
		^ \mf MessageQueue::Merge;
	- \f void Merge(MessageQueue&, list<Message>&);

r21:
/ @ \impl \u YShellMessage:
	- \f void Merge(MessageQueue&, MessageQueue&);
	- \inc \h YWindow;
/ \tr \impl @ \f void RecoverMessageQueue() @ \impl \u YApplication;

r22:
/ \inc \h YShell @ \h YFileSystem >> \h YApplication;

r23:
/= test 5 ^ \conf release;

r24:
/ \simp \impl @ \mf int MessageQueue::PeekMessage(Message&,
	const shared_ptr<YShell>&, bool);

r25:
/ @ \cl MessageQueue:
	/ !\i \mf bool Insert(const Message&) -> \i \mf void Push(const Message&);
	/ !\i \mf SizeType Clear() -> \i \mf void Clear() ^ \mac (PDefH0 & ImplRet);
	/ !\i \mf void PeekMessage(Message&) const -> \i \mf;
	/ !\i \mf void PopMessage() -> \i \mf void Pop();
/ \tr \impl @ \mf (BackupMessageQueue & SendMessage) @ \impl \u YApplication;

r26:
/ @ \u Shells:
	/ \cl ShlSetting \mg -> \cl ShlExplorer;
	/ @ \cl ShlExplorer:
		/ @ \cl TFormExtra:
			/ \tr \impl @ \mf OnClick_btnReturn;
			/ \mf OnClick_btnReturn => OnClick_btnClose;
			/ \m btnReturn => btnClose;
	/ \tr \impl @ \mf OnClick_btnTest;

r27:
/ @ \cl ShlExplrer @ \impl \u Shells:
	/ \impl @ \mf OnActivated;
	+ \f void SwitchVisible(IWindow&) @ \un \ns;
	/ \impl @ \mf OnClick_btnTest ^ SwitchVisible;
	/ \simp \impl @ \mf OnClick_ShowWindow ^ SwitchVisible;

r28:
+ \o \c ZOrderType DefaultContainerZOrder(128)
	@ \ns Components::Widgets @ \h YUIContainer;
/ \impl @ \mf void AFrame::operator+=(IWindow&);

r29:
/ \impl @ \ctor @ \cl ShlExplorer @ \impl \u Shells;

r30:
/= test 6 ^ \conf release;

r31:
/ \f int FetchMessage(Message&, const shared_ptr<YShell>& = FetchShellHandle())
	-> int FetchMessage(Message&, MessageQueue::SizeType,
		const shared_ptr<YShell>& = FetchShellHandle());
/ \impl @ \f main @ \impl \u YGlobal;

r32:
/ \impl @ \f main @ \impl \u YGlobal;

r33-r34:
/= test 7;

r35:
/ \simp \impl @ \mf ShlExplorer::OnDeactivated @ \impl \u Shells;

r36:
/ \f shared_ptr<Image>& GetImage(int) @ \u Shells
	-> shared_ptr<Image>& FetchImage(size_t);
/ \f GetGlobalImage => FetchGlobalImage @ \impl \u Shells;

r37:
- \inc \h YDefinition @ \h YCommon;
/ \a PATH => path_t;
/ \a CPATH => const_path_t;
/ typedef path_t & const_path_t >> \ns ystdex @ \g \ns @ \h YDefinition;
/ @ \ns platform @ \h YCommon:
	+ using ystdex::const_path_t;
	+ using ystdex::path_t;
/ @ \ns \ns YSLib @ \h YAdaptor:
	+ using ystdex::const_path_t;
	+ using ystdex::path_t;

r38:
/= test 8 ^ \conf release;


$DOING:

$relative_process:
2011-06-08:
-20.9d;
//Mercurial rev1-rev86: r4242;

/ ...


$NEXT_TODO:
b217-b256:
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

$parser.state.old_style $= $false;
$design; // features changing only made sense to library developers;
$doc; // for documents target;
$add_features +; // features added;
$fix_bugs *; // bugs fixed;
$modify_features /; // features modified;
$remove_features -; // features removed;
$using ^; // using;
$source_from ~; // features replacing from;
$belonged_to @;
$moved_to >>;
$renamed_to =>;

//$transform $list ($list_member $pattern $all($exclude $pattern \
//	$string_literal "*")) +;

$ellipse_refactoring;

$now
(
	/ $design "header file dependencies",
	/ "focused boundary for controls",
	* "strict ISO C++2011 code compatibility" $=
	(
		* "deprecated keyword static using in ISO C++2003 now removed but"
			^ @ unit "YApplication when macro YSL_DEBUG_MSG defined"
	),
	/ "more efficient message queue implementation" ^ "%multiset"
		~ %priority_queue",
	/ "simplified shell example",
	+ "default windows Z order",
	+ "message fetching automatically calling %Idle \
		when size less or equal than a user-specified limit"
),

b215
(
	+ "disabled state style of control %Thumb",
	* "disabled control touch event handling" $b199,
	+ "checkbox and disabling test",
	+ "visible arrow indicating multi-level menus",
	/ "creating list" @ ("constructor of class %MTextList"
		~ "member function %GetList"),
	+ "event handler supporting for object without %operator== \
		including closure types",
	* $design "access of interitance of class std::function"
		@ "class template %GHEvent" $since b207,
	+ "disabled behavior test" @ "%ShlExplorer",
	/ "thumb/button boundary style",
	/ "global architecture" $=
	(
		/ "screen constant" >> "namespace scope" ~ "class %Global",
		+ "class %YDSApplication" ~ "class %Global",
		- "singleton mechanism" @ "class %YApplication" 
	)
),

b214
(
	/ "functions of menus" $=
	(
		+ "hiding on confirmed",
		+ "resizing with width of text in list",
		/ "margin of menus",
		+ "submenus" $= 
	),
	* $design "exception specification" @ "unit %YApplication",
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
	* "touch event coordinate error" @ "contianer controls" $since b195,
	+ "functions of menus" $=
	(
		+ "resizing for content",
		+ "hiding when focus lost"
	),
	+ "predicator %Contains" @ "UI container implementation"
),

b212
(
	/ $design "assertion strings",
	/ "menu constructor",
	+ "redundant menu state member" @ "class %Menu",
	/ "container member APIs" ^ "reference parameter" ~ "pointer parameter",
	* "constructor of class %Frame",
	+ "Z order for widget overlaying" @ " UI containers",
	+ "menu laid at top level" ^ "Z order"
),

b211
(
	/ "implemented messages with general object content holder"
		^ "non-pointer member" ~ "%shared_ptr",
	+ "message content mapping",
	/ $design "messaging APIs" >> "unit YApplication" ~ "unit YShell",
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
	/ "merged library CHRLib" >> "library YCLib",
	/ $design "protected function inheritance" @ "class template %GHEvent",
	/ "using directive of namespace %ystdex" @ "library YSLib",
	+ "lost %Rect operations",
	* "strict ISO C++2003 code compatibility" $=
	(
		^ "fixed macros" @ "header YFont"
	),
	/ "renamed directory %Shell to %UI @ "library YSLib",
	/ "several memory utilities for std::shared_ptr and std::unique_ptr"
		>> "library YCLib::YStandardExtend"
),

b208
(
	^ "rvalue references as parameter types of event handlers",
	/ "set default font size smaller",
	+ "vertical alignment" @ "labeled controls",
	* "fatel error" @ "direct UI drawing testing"
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
	^ "rvalue reference" @ "event class",
	* "ystdex algorithms",
	* "minor event handler parameter type mismatch" @ "test code",
	* "key events response(added as %KeyDown, but removed as %KeyPress)"
		@ "project YSTest_ARM9" $since b201,
	* "tunnel and direct key and touch response repeated" @ "class %YGUIShell"
		$since b198
),

b206
(
	+ "menu test button",
	/ "file API",
	* "strict ISO C++2003 code compatibility" $=
	(
		+ "function %memcmp declation" @ "namespace %ystdex"
	),
	* "strict ISO C++2011 code compatibility" $=
	(
		* "implicit narrowing conversion(N3242 8.5.4/6) \
			in C++2011(N3242 5.17/9)" ^ "explicit static_cast",
		/ "character types" @ "header platform.h"
	),
	/ "coding using limited C++2011 features" $=
	(
		/ $design ^ "C++2011 style nested template angle brackets",
		/ $design ^ "keyword %auto",
		/ ^ "C++2011 type_traits" @ "namespace std" ~ "std::tr1"
	),
	- "Loki type operations",
	/ ^ "namespace %ystdex" @ "namespace %YSLib"
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
	+ "class %nullptr_t" @ "namespace ystdex",
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
		- "platform dependent global resource" @ "class %YApplication",
		- "global object" @ "unit YNew"
	),
	+ "class %MTextList",
	/ "class %YSimpleText List using class %MTextList inheritance",
	/ "class %YSimpleText renamed to %YMenu",
	/ "using std::tr1::shared_ptr" ~ "smart pointers" @ "library Loki"
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
	+ "key helding response" @ "class %ShlReader",
	+ "GDI API %BlitTo",
	/ "widgets drawing",
	/ $design "GDI API %CopyTo",
	+ "controls: panel",
	/ $design "YCLib" $=
	(
		+ "partial version checking for compiler and library implementation",
		+ "minor macros" @ "library YCLib",
	),
	+ "type conversion helper template",
	+ $design "implicit member overloading by interface parameter with type \
		%IControl and %IWidget" @ "container classes"
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
		- "class %ShlGUI" @ "unit Shell_DS"
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
	- "contianer pointer parameter" @ "constructor widgets",
	/ "desktops as window" @ "shells"
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
	+ "horizontal text alignment" @ "class %MLabel"
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
	+ "controls: buttons" @ "class (%YThumb, %YButton)",
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
	+ "virtual inheritance" @ "control classes",
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

