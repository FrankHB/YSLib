//v0.3132; *Build 208 r12;
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
^ \conf release;
/ @ \clt GDependency @ \h YObject:
	+ \exp \de copy \ctor GDependency(const GDependency&) = default;
	+ \exp \de move \ctor GDependency(GDependency&&) = default;

r2:
^ \conf debug;
/= test 1;

r3:
/ @ \h YEvent:
	- typedef typename std::pointer_to_binary_function<_tSender&, _tEventArgs&,
		void> FunctorType @ \stt GSEventTypeSpace;
	- typedef typename SEventType::FunctorType FunctorType @ \clt GEvent;
	/ @ \clt GDependencyEvent:
		- typedef typename EventType::FunctorType FunctorType;
		/ \a 4 \op+= -> 1 \mft \i \op+=;
		/ \a 4 \op-= -> 1 \mft \i \op-=;
		/ \def \mf \i GetSize ^ \mac DefGetterMember;
		/ \def \mf \i Clear ^ \mac (PDefH0 & ImplBodyMember0);

r4:
/ @ \h YEvent:
	/ typedef void FuncType(_tSender&, _tEventArgs&) @ \stt GSEventTypeSpace
		-> typedef void FuncType(_tSender&, _tEventArgs&&);
	/ \mf SizeType operator()(_tSender&, _tEventArgs&) const @ \clt GEvent
		-> SizeType operator()(_tSender&, _tEventArgs&&) const;
	/ \mf \i SizeType operator()(SenderType&, EventArgsType&) const
		@ \clt GDependencyEvent -> SizeType operator()(SenderType&,
		EventArgsType&&) const;
/ \a 'GetStaticRef<EventArgs>()' -> 'EventArgs()';
/ \a event arguments \tp @ \param @ event handlers -> rvalue \ref ~ lvalue \ref;
/ \tr \impl ^ std::move;
/ \tr \impl @ \mf \op() ^ std::forward @ \clt (ExpandMemberFunctionFirst
	& ExpandMemberFunctionBinder) @ \h YFunc;

r5:
/= test 2 ^ \conf release;

r6:
/ \simp @ \impl \u Shells:
	/ \a array \o \exc char strCount[40] @ \un \ns >> \mf;
/ const Font::SizeType Font::DefaultSize(14)
	-> const Font::SizeType Font::DefaultSize(12) @ \impl \u YFont;

r7:
/ @ \cl MLabel @ \u YLabel:
	/ @ \en TextAlignmentStyle:
		+ \m Up = 0;
		+ \m Down = 2;
	/ \m TextAlignmentStyle Alignment => HorizontalAlignment;
	+ \m TextAlignmentStyle VerticalAlignment;
	/ \tr \impl @ \ctor;
+ using MLabel::VerticalAlignment @ \cl Button @ \h Button;
/ \impl @ \mf ShlSetting::TFormTest::TFormTest @ \impl \u Shells;

r8:
* \impl @ void MLabel::PaintText(IWidget&, Color, const Graphics&, const Point&)
	@ \impl \u YLabel;

r9:
/= test 3 ^ \conf release;

r10:
/= test 4;

r11:
* fatel error @ direct UI drawing testing:
	/ \impl @ \mf bool YGUIShell::ResponseTouchBase(IControl&, TouchEventArgs&,
		Components::Controls::VisualEvent) @ \impl \u YGUI;

r12:
/= test 5 ^ \conf release;


$DOING:

$relative_process:
2011-05-11:
-23.4d;
//Mercurial rev1-rev78: r3916;

/ ...


$NEXT_TODO:

b209-b324:
+ menus;
^ unique_ptr ~ GHandle @ \clt GEventMap @ \h YEvent avoiding patching
	libstdc++ for known associative container operation bugs;
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
	^ $design "rvalue references as parameter types of event handlers",
	/ "set default font size smaller",
	+ "vertical alignment in labeled controls",
	* "fatel error in direct UI drawing testing"
),

b207
(
	/ $design "event handler implementation ^ std::function \
		instead of Loki::Function",
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
		/ ^ "C++2011 type_traits in namespace std instead of std::tr1"
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
	/ $design "using pointers instead of references in parameters \
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

