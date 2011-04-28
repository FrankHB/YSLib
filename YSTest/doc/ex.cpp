//v0.3121; *Build 205 r19;
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
r1-r5:
/= test 1;

r6:
/ @ \h YWidget:
	/ @ \cl Widget:
		+ \inh \vt \in IWidget;
		+ 'ImplI1(IWidget)' @ \i \mf GetContainerPtr;
		/= \a \vt -> 'ImplI1(IWidget)';
	/ @ \cl YWidget:
		- \inh !\vt \in IWidget;
		- \mf (Paint & Refresh & GetContainerPtr);
		/ \a \mf >> \cl Widget;
/ \simp \impl @ \mf (Paint & Refresh) @ \cl Widget @ \impl \u YWidget;

r7:
- \a \mf \impl \inh ~ Widget @ \cl Control @ \h YControl:

r8:
/ \a \cl YLabel => Label;
/ \a YWidget \exc \h YWidget -> Widget;
- \cl YWidget @ \u YWidget;
/ \a YControl \exc \h YControl -> Control;
- \cl YControl @ \u YControl;

r9:
/ \a \cl YThumb => Thumb;
- \inh GMCounter<Thumb> @ \cl Thumb;
/ \a \cl YButton => Button;
- \inh GMCounter<Button> @ \cl Button;
/ \a \cl YCheckBox => CheckBox;
- \inh GMCounter<CheckBox> @ \cl CheckBox;
* \tr \m Thumb Thumb -> Controls::Thumb Thumb @ \cl ATrack;

r10:
^ \conf release;
/= test 2;

r11:
/ \a \cl YUIContainer => UIContainer;
/ @ \cl UIContainer:
	- \inh (GMCounter<UIContainer> & YComponent);
	/ \tr \impl @ \ctor;
	- \a \mf \impl \inh ~ Visual;
	- \a \mf \impl \inh ~ Widget;
- \cl YPanel @ \u YPanel;
/ \a YMenu \exc \h YMenu -> Menu;
/ @ \u YMenu:
	- \cl YMenu @ \u YMenu;
	/ \ac @ \ctor @ \cl Menu -> public ~ protected;

r12:
^ \conf debug;
/ \a \cl YFrame => Frame;
/ @ \cl Frame:
	- \inh (GMCounter<Frame> & YComponent);
	/ \tr \impl @ \ctor;
/ \a \cl YFrame => Form;
- \inh GMCounter<Form> @ \cl Form;
/ \a YHorizontalTrack => HorizontalTrack;
/ \a YVerticalTrack => VerticalTrack;
/ \a YHorizontalScrollBar => HorizontalScrollBar;
/ \a YVerticalScrollBar => VerticalScrollBar;
/ @ \u Scroll:
	/ @ \cl HorizontalTrack:
		- \inh (YComponent & GMCounter<HorizontalTrack>);
		/ \tr \impl @ \ctor;
	/ @ \cl VerticalTrack:
		- \inh (YComponent & GMCounter<VerticalTrack>);
		/ \tr \impl @ \ctor;
	/ @ \cl HorizontalScrollBar:
		- \inh (YComponent & GMCounter<HorizontalScrollBar>);
		/ \tr \impl @ \ctor;
		- typedef YComponent ParentType;
	/ @ \cl VerticalScrollBar:
		- \inh (YComponent & GMCounter<VerticalScrollBar>);
		/ \tr \impl @ \ctor;
		- typedef YComponent ParentType;
	* \tr @ \cl ScrollableContainer:
		/ protected \m HorizontalScrollBar HorizontalScrollBar
			-> Controls::HorizontalScrollBar HorizontalScrollBar;
		/ protected \m VerticalScrollBar VerticalScrollBar;
			-> Controls::VerticalScrollBar VerticalScrollBar;
/ \tr \pre \decl & using \decl @ \h YComponent;
/ \a YListBox => ListBox;
/ \a YFileBox => FileBox;
/ @ \u ListBox:
	/ @ \cl ListBox:
		- \inh (YComponent & GMCounter<ListBox>);
		/ \tr \impl @ \ctor;
		- typedef YComponent ParentType;
	/ @ \cl FileBox:
		- \inh GMCounter<FileBox>;
		- typedef ListBox ParentType;
- \inh GMCounter<Label> @ \cl Label;
/ \a YImage => Image;
/ @ \h YResource:
	- \inh YCountableObject;
	- \inh GMCounter<Image>;
	- typedef YCountableObject ParentType;
/ \a YConsole => Console;
/ @ \cl Console @ \u YConsole:
	- typedef YComponent ParentType;
	- \inh YComponent;
	/ \tr \impl @ \ctor;
/ @ \h YComponent:
	- \in IComponent;
	- \cl YComponent;
	- using Components::Forms::Form @ \ns YSLib;
- \tr typedef YComponent ParentType @ \cl (UIContainer & AFrame);
- \a typedef '*' ParentType @ \lib YSLib;

r13:
/= test 3 ^ \conf release;

r14:
/= recovered \h YShellMessage ~ b204;

r15:
/ @ \cl Menu:
	/ \ac @ \inh Widgets::MTextList -> protected ~ public;
	+ public using MTextList::ItemType;
	+ public using MTextList::ListType;
	+ public using MTextList::IndexType;
	+ public using MTextList::TextAlignmentStyle;
	+ public using MTextList::Font;
	+ public using MTextList::Margin;
	+ public using MTextList::Alignment;
	+ public using MTextList::Text;
	+ public using MTextList::GetList;
	+ public using MTextList::GetItemPtr;
	+ public using MTextList::GetItemHeight;
	+ public using MTextList::RefreshTextState;
/ @ \cl Button:
	/ \ac @ \inh Widgets::MLabel -> protected ~ public;
	+ public using MLabel::TextAlignmentStyle;
	+ public using MLabel::Font;
	+ public using MLabel::Margin;
	+ public using MLabel::Alignment;
	+ public using MLabel::Text;
/ @ \cl Label:
	/ \ac @ \inh MLabel -> protected ~ public;
	+ public using MLabel::TextAlignmentStyle;
	+ public using MLabel::Font;
	+ public using MLabel::Margin;
	+ public using MLabel::Alignment;
	+ public using MLabel::Text;
/ \cl MScreenPositionEventArgs @ \h YControl
	-> typedef Drawing::Point MScreenPositionEventArgs;

r16:
+ \mf pair<Drawing::Color, Drawing::Color> GetPair(ColorListType::size_type,
	ColorListType::size_type) const @ \cl Palette @ \u YStyle;
/ @ \u YMenu:
	/ !\rem \inh \h YStyle @ \h;
	- \inh \h YStyle @ \impl \u;
	/ \ctor @ \cl Menu \exp Menu(const Rect& = Rect::Empty,
		GHWeak<ListType> = nullptr, Color = Drawing::ColorSpace::Aqua,
		Color = Drawing::ColorSpace::White) -> \exp
		Menu(const Rect& = Rect::Empty, GHWeak<ListType> = nullptr,
		pair<Color, Color> = FetchGUIShell().Colors.GetPair(Styles::Highlight,
		Styles::HighlightText));
	/ \inh \h YControl -> YGUI @ \h;

r17:
/= test 4 ^ \conf release;

r18:
/ \cl YDesktop => Desktop @ \u YDesktop;

r19:
/= test 5 ^ \conf release;


$DOING:

$relative_process:
2011-04-28:
-23.4d;
//c1-c75:r3785;

/ ...


$NEXT_TODO:

b206-b288:
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

//$transform $list ($list_member $pattern $all($exclude $pattern \
//	$string_literal "*")) +;

$ellipse_refactoring;

$now
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

