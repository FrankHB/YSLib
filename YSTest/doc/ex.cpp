//v0.3132; *Build 207 r96;
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
r1-r5:
+ \ft<class _tSender, class _tEventArgs> \i bool operator==(const GHEvent<
	_tSender, _tEventArgs>&, const GHEvent<_tSender, _tEventArgs>&) @ \h YEvent;
- \mac LOKI_FUNCTORS_ARE_COMPARABLE @ \h YAdaptor;
/ \a Design::Function -> std::function;

r6:
/ @ \h YAdaptor:
	/ @ \ns Design:
		- using Loki::Function;
		- using Loki::SmallObject;
	- \inc \h <loki/Function.h>;
	- \inc \h <loki/SmallObj.h>;
	- (\rem \mac) YSL_TL;
	- (\rem \inc \h) \lib AGG;
- \h AGG @ \dir Adaptor;
- \lib (AGG & Loki);
/ \tr @ \mac LIBEX @ ARM9 makefile;

r7:
/= test 1 ^ \conf release;

r8:
/ \impl @ tf<class _tSender, class _tEventArgs> \i bool
	operator==(const GHEvent<_tSender, _tEventArgs>&, const GHEvent<_tSender,
	_tEventArgs>&) ^ \mf target_type;

r9:
/ \i \f op() -> using std::function<FuncType>::operator() @ \clt GHEvent
	@ \h YEvent;

r10:
/= test 2 ^ \conf release;

r11:
/ @ \ns ystdex @ \h Utilties:
	+ union no_copy_t;
	+ union any_pod_t;
/ @ \h YEvent:
	+ \pre \decl @ \clt GHEvent with \de \t \param;
	+ \decl @ \tf<class _tSender, class _tEventArgs> bool
		operator==(const GHEvent<_tSender, _tEventArgs>&,
		const GHEvent<_tSender, _tEventArgs>&) @ friend \decl @ \clt GHEvent;
	/ @ \clt GHEvent:
		- \tr \de \t \param @ \def;
		* \ctor \param ^ \c rvalue \r;
/ @ \h YFunc:
	+ \cl PolymorphicFunctorBase;
	+ \clt GFunctor;

r12:
/ @ \h YEvent:
	/ @ \clt GEvent<true, _tSender, _tEventArgs>:
		- 3 \mf \i \op+= & 3 \mf \i \op-=;
		+ \mft \i \op+=;
		+ \mft \i \op-=;
	- \clt GEvent<false, _tSender, _tEventArgs>;

r13:
/ @ \h YEvent:
	/ \clt GEvent<true, _tSender, _tEventArgs> -> GEvent<_tSender, _tEventArgs>;
	/ \tr @ typedef EventType @ \cl GSEvent;
- \a \mac YSL_EVENT_MULTICAST;
^ delete \mf \decl @ \h (Utilities & YEvent);
/= \i \dtor @ \cl TextBlock @ \h YTextManager ^ \mac DefEmptyDtor;

r14:
/= test 3 ^ \conf release;

r15-r33:
/ \h YEvent:
	/ @ \clt GEvent:
		- 3 \mf \i \op=;
		+ \mft \i \op=;
		/ \mf \i GEvent& operator=(const HandlerType&);
		+ \mf \i GEvent& operator=(HandlerType&&);
		/ protected \mf \i GEvent& AddRaw(const HandlerType&);
		+ \mf \i GEvent& AddRaw(HandlerType&&);
		/ \mf \i GEvent& operator+=(const HandlerType&);
		+ \mf \i GEvent& operator+=(HandlerType&&);
		/ \mf \i GEvent& operator-=(const HandlerType&);
		+ \mf \i GEvent& operator-=(HandlerType&&);
		/ \simp \impl @ \mft \op=;
		/= \impl @ \mft op+=;
		/= \impl @ \mft op-=
		/= \simp \impl @ \mf op+=;
		+ move \ctor GEvent(GEvent&&) = default;
		+ \rem \mf GEvent& operator=(GEvent&&) = default;
		/ copy \ctor GEvent(const GEvent&) -> default;
		/ \mf \i GEvent& operator=(const HandlerType&)
			-> \i GEvent& operator=(HandlerType&&);
		* \mf GEvent& operator=(const GEvent&) exception safety;
		+ \mf \i GEvent& operator=(GEvent&&);
		+ \mf \i GEvent& AddRaw(HandlerType&&);
		+ private \ctor \t<typename _tHandler> \i GEvent(_tHandler);
	/ @ \clt GHEvent:
		+ copy \ctor GEvent(const GHEvent&) = default;
		+ move \ctor GEvent(GHEvent&&) = default;
		+ \mf GHEvent& operator=(const GHEvent&) = default;
		+ \rem \mf GHEvent& operator=(GHEvent&&) = default;
		* \impl @ \i \ctor \t<typename _tFunc> GHEvent(_tFunc) ^ std::forward;
	+ \i @ \a \exp defaulted \mf;
	/ @ \clt GEvent:
/= test 4 ^ \conf release;
/= test 5;

r34:
/ @ \h Utilities:
	/ \ft<typename _tContainer> typename _tContainer::size_type
		erase_all(_tContainer&, const typename _tContainer::value_type&);
	* \impl @ \ft erase_all_if;
/ \impl @ \mf bool MUIContainer::RemoveWidget(IWidget*) ^ erase_all;

r35:
/= \ft search_map @ \h Utilities ^ auto;
/ \impl @ (\mf bool MUIContainer::RemoveWidget(IWidget*)
	& \mf \op-= @ \clt YEvent) ^ list \mf remove ~ erase_all;

r36:
/ @ \clt GEvent @ \h YEvent:
	+ \i @ \a (\op+= & \op-=);
	+ \mf \i GEvent& operator+=(const HandlerType&);
	+ \mf \i GEvent& operator-=(const HandlerType&);

r37:
/ @ \clt GEvent @ \h YEvent:
	- \i @ private \t \ctor;
	/ !\i \mf Swap -> \i \mf;
	/= \impl \i \mf (Clear & GetSize & Swap) ^ \mac;

r38:
/= test 6 ^ \conf release;

r39:
* @ \cl ShlSetting::TFormTest @ \u Shells:
	/ \smf void OnEnter_btnEnterTest(IControl&, InputEventArgs&)
		-> \smf void OnEnter_btnEnterTest(IControl&, TouchEventArgs&);
	/ \smf void OnLeave_btnEnterTest(IControl&, InputEventArgs&)
		-> \smf void OnLeave_btnEnterTest(IControl&, TouchEventArgs&);
* @ \clt GHEvent @ \h YEvent:
	/ \i \ctor GHEvent(const FuncType&&)
		-> \i \ctor GHEvent(FuncType*);

r40-r58:
/= test 7;

r59:
/= test 8 ^ \conf release;

r60:
/ @ \h YEvent:
	/ @ \clt GHEvent:
		+ typedef bool(*Comparer)(const GHEvent&, const GHEvent&);
		+ \m \clt<class _tFunctor> GEquality;
		* \ctor \param \tp;
		/ \impl @ \ctor;
	/ \impl @ \op== for \clt GHEvent;

r61:
/= test 9;

r62:
/= test 10 ^ \conf release;

r63:
/ @ \h YEvent:
	/ @ \clt GHEvent:
		/ \mg \def -> \decl @ \mf AreEqual @ \clt GEqulity;
	/ !\m \f @ \op== for \clt GHEvent -> \f @ \clt GHEvent;
	- friend \decl @ for \clt GHEvent;
	- \pre \decl @ \clt GHEvent;

r64-r70:
/= test 11;

r71:
* \impl @ \smf AddEqual @ \clt GEquality @ \clt GHEvent @ \h YEvent;

r72-r79:
/= test 12;

r80:
* \impl @ \smf AddEqual @ \clt GEquality @ \clt GHEvent @ \h YEvent;

r81:
/= test 13 ^ \conf release;

r82-r84:
/= test 14;

r85:
* \impl @ \mf ShlReader::OnDeactivated @ \impl \u Shells;
/ @ \h YFunc:
	/ \rem \st PolymorphicFunctorBase;
	/ \rem \clt GFunctor;
	/ \rem \clt InversedCurrying;

r86-r91:
/= test 15;

r92-r94:
* \impl @ \mf (ResponseKey & ResponseTouch) @ \cl YGUIShell @ \impl \u YGUI;

r95:
/= test 16;

r96
/= test 17 ^ \conf release;


$DOING:

$relative_process:
2011-05-09:
-24.1d;
//Mercurial rev1-rev77:r3820;

/ ...


$NEXT_TODO:

b208-b288:
+ menus;
* fatel error @ direct UI drawing testing;
^ unique_ptr ~ GHandle @ \clt GEventMap @ \h YEvent avoiding patching
	libstdc++ for known associative container operation bugs;
+ overlay for \a widgets;
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

