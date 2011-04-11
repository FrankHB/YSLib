//v0.3079; *Build 200 r20;
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
/= test 1 ^ \conf release:
	/ \m rename @ \u Shells & \impl \u GBKEX;

r2-r4:
/= test 2;

r5-r6:
/ \impl @ \impl \u GBKEX;

r7:
/ @ \impl \u GBKEX:
	/ \cl TestObj @ \ns >> \mf void
		ShlSetting::TFormDown2::OnClick_btnTestEx(TouchEventArgs&);
	/ \mf void ShlSetting::TFormDown2::OnClick_btnTestEx(TouchEventArgs&)
		>> \impl \u Shells;

r8:
/ \impl @ \mf void ShlSetting::TFormDown2::OnClick_btnTestEx(TouchEventArgs&)
	@ \impl \u Shells;

r9:
/ @ \cl ShlSetting::TFormDown2 @ \impl \u Shells:
	/ \impl @ \ctor;
	/ \impl @ \mf void OnClick_btnDragTest(TouchEventArgs&):

r10:
/ \impl @ \ctor @ \cl ShlSetting::TFormDown2 @ \impl \u Shells:

r11:
/= test 3 ^ \conf release;

r12-r13:
* \impl @ \mf bool ResponseKeyBase(IControl&, KeyEventArgs&,
	Components::Controls::VisualEvent) @ \cl YGUIShell;

r14:
^ \conf release;
/= test 4;

r15:
/ @ \impl \u Shells:
	- \g \o char strtbuf[0x400];
	/ \g \o char strtf[0x400] & char strtxt[0x400] >> \un \ns @ \ns YSLib;
	/ \mac YSL_SHL*(*);
	/ \mac YSL_BEGIN_SHELL(*);
	/ \mac YSL_END_SHELL(*);
/ @ \h Base:
	/ \mac YSL_BEGIN_SHELL(s);
	/ \mac YSL_END_SHELL(s);
	/ \mac YSL_SHL_(s);
	/ \mac YSL_SHL(s);

r16:
^ \conf debug;
/= test 5;

r17:
/ \impl background drawing functions @ \un \ns @ \impl \u Shells;

r18:
/ @ \cl ShlSetting @ \u Shells:
	/ \a \m \o @ \cl TFormUp >> \cl ShlSetting;
	+ @ \ctor;
	/ \impl @ \mf (OnActivated & OnDeactivated);
	- private \m hWndUp;
	/ private \m hWndDown -> public \m hWndTest;
	/ \sm hWndExtra -> !\s \m;
	/ \tr \impl @ \mf void ShowString(const String&);

r19:
/ @ \u Shells:
	/ @ \cl ShlSetting:
		/ \cl TFormDown1 => TFormTest;
		/ \cl TFormDown2 => TFormExtra;
		- \scm (s_left & s_size);
		/ \tr \impl @ \ctor;
		/ \tr \impl @ \ctor @ \m \cl TFormExtra;
		/ \smf OnTouchDown_FormDown2 => OnTouchDown_FormExtra;
		/ \ac @ \mf (2 ShowString & OnTouchDown_FormExtra) -> private ~ public;
	/ \a \mf 'On*' \exc (OnActivated & OnDeactivated) @ \cl ShlExplorer
		-> private ~ public;
	/ \rem \decl @ \g \o char gstr[128];
/ \rem \def @ \g \o char gstr[128] @ \impl \u GBKEX;

r20:
/= test 6 ^ \conf release;


$DOING:

$relative_process:
2011-04-11:
-26.9d;

/ ...


$NEXT_TODO:

b201-b240:
/ fully \impl \u DSReader;
	* moved text after setting lnGap;
* non-ASCII character path error in FAT16;

b241-b576:
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
r577-r864:
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

$add_features +;
$fix_bugs *;
$modify_features /;
$remove_features -;
$using ^;

$transform $list ($list_member $pattern $all($exclude $pattern
	$string_literal "*")) +;

$now
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
+ "returning number of called handles in event calling"
),

b198
(
* "font caching without default font file load successfully",
"showing and hiding windows",
* "%ListBox scroll bar length",
* "handle constructors",
/ "shells for DS" $=
	(
		- "class %ShlGUI in unit Shell_DS"
	),
/ "using pointers instead of references in parameters of container methods",
/ "simplified GUI shell" $=
	(
		/ "input points matching",
		- "windows list"
	),
/ "simplified destructors",
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
* "makefiles",
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
"events",
),

b132_b158
(
"core utility templates",
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

