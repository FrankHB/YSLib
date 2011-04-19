//v0.3086; *Build 202 r63;
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
\u YPanel
{
	\in IPanel;
	\cl YPanel;
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
^ \conf release;
/ @ \dir Shell:
	/ \u YLabel["ylabel.h", "ylabel.cpp"] => Label["label.h", "label.cpp"];
	/ \tr @ \inc @ \h Button;
	/ \u YUIContainerEx["yuicontx.h", "yuicontx.cpp"]
		=> UIContainerEx["uicontx.h", "uicontx.cpp"];
	/ \tr @ \inc @ \h Scroll;
	/ \u YForm["yform.h", "yform.cpp"] => Form["form.h", "form.cpp"];
	/ \tr @ \inc @ \h Build;

r2:
^ \conf debug;
/= test 1;

r3
/ @ \cl MUIContainer:
	/ typedef set<IWidget*> WGTs -> typedef list<IWidget*> WGTs;
	/ WGTs sWgtSet => WidgetsList;
	/ \tr \impl @ \mf void operator+=(IWidget*);
	/ \impl @ \mf bool operator-=(IWidget*);
	/ \tr \impl @ \mf void operator+=(IControl*);
	/ \tr \impl @ \mf bool operator-=(IControl*);
/ \simp \impl @ \mf bool YDesktop::operator-=(IControl*);
* \tr \simp \impl @ \mf (void operator+=(IWidget*) & bool operator-=(IWidget*))
	@ \cl YUIContainer;
/= \impl @ \f const Graphics& FetchContext(IWidget&);

r4:
/ @ \cl AFrame:
	+ typedef set<IWindow*> WNDs;
	+ protected \m WNDs WindowsSet;
	/ \tr \impl @ \ctor;
	-= empty \dtor;
	+ \mf \vt void operator+=(IWindow*);
	+ \mf \vt bool operator-=(IWindow*);
	/ \i \mft<typename _type> void operator+=(_type*)
		-> \i \mft<typename _type> void operator+=(_type);
	/ \i \mft<typename _type> bool operator-=(_type*)
		-> \i \mft<typename _type> bool operator-=(_type);
	+ \i \mf void operator+=(HWND) ^ \mac(PDefHOperator1 & ImplBodyBase1);
	+ \i \mf bool operator-=(HWND) ^ \mac(PDefHOperator1 & ImplBodyBase1);
/ \simp @ \impl @ \mf ShlSetting::OnActivated @ \impl \u Shells;

r5:
* @ \cl YDesktop:
	+ \mf \vt void operator+=(IWindow*);
	+ \mf \vt bool operator-=(IWindow*);
	
r6-r12:
/= test 2;

r13:
* ^ \mac ImplBodyBase @ \impl @ \i \mf (void operaotr+=(HWND)
	& bool operaotr-=(HWND)) @ \cl AFrame;

r14-r15;
/= test 3;

r16:
/ @ \cl MUIContainer:
	+ typedef pair<IWidget*, bool> ItemType;
	/ typedef list<IWidget*> WGTs -> typedef list<ItemType> WGTs;
	+ \mf void operator+=(IWindow*);
	+ \mf bool operator-=(IWindow*);
	+ protected \mf RemoveWidget(IWidget*, bool);
	/ \tr \impl @ \mf GetTopWidgetPtr;
/ \tr \impl @ \mf YFrame::DrawContents;

r17:
/ \simp @ \cl AFrame:
	- typedef set<IWindow*> WNDs;
	- protected \m WNDs WindowsSet;
	/ \tr @ \ctor;
	/ \tr @ \mf \vt void operator+=(IWindow*);
	/ \tr @ \mf \vt bool operator-=(IWindow*);

r18-r19:
/ \impl @ YFrame::DrawContents;

r20-r21:
/= test 4;

r22-r36:
/ \simp @ \cl YDesktop:
	- protected \mf \vt bool DrawContents();
	- \mf void RemoveTopDesktopObject();
	- \mf IControl* GetFirstDesktopObjectPtr() const;
	- \mf IControl* GetTopDesktopObjectPtr() const;
	- \mf \vt void operator+=(IControl*);
	- \mf \vt void operator+=(IWindow*);
	- \mf \vt bool operator-=(IControl*);
	- \mf \vt bool operator-=(IWindow*);
	- using ParentType::operator+=;
	- using ParentType::operator-=;
	- DOs sDOs;
	- typedef list<IControl*> DOs;
	/ \tr \impl @ \ctor;
	/ \tr \impl @ \mf ClearContents;
* @ \cl MUIContainer:
	* \impl @ \mf void operator+=(IWidget*);
	* \impl @ \mf void operator+=(IControl*);
	* \impl @ \mf void operator+=(IWindow*);
	* \impl @ \mf RemoveWidget;
	+ protected \mf bool CheckWidget(IWidget*);
/ @ \cl YFrame:
	- using AFrame::operator+=;
	- using AFrame::operator-=;

r37:
/ \simp \impl @ \mf void AWindow::Update();

r38-r41:
/= test 5;

r42:
/= test 6 ^ \conf release;

r43-r44:
/ \a \mf Draw => Paint;

r45:
/ @ \cl AWindow;
	+ protected \mf \vt void Paint();
	/ \tr @ \mf orders;
	/ \impl @ \mf Draw;
	/ \impl @ \mf Refresh;
	/ \simp \impl @ \mf DrawContents;

r46-r47:
/= test 7;

r48:
/ @ \cl MUIContainer:
	/ typedef pair<IWidget*, bool> ItemType -> typedef IWidget* ItemType;
	/ \tr \impl @ \mf void operator+=(IWidget*);
	/ \tr \impl @ \mf void operator+=(IControl*);
	- \mf void operator+=(IWindow*);
	/ \tr \impl @ \mf bool operator-=(IWidget*);
	/ \tr \impl @ \mf bool operator-=(IControl*);
	- \mf bool operator-=(IWindow*);
	/ \mf bool RemoveWidget(IWidget*, bool) -> bool RemoveWidget(IWidget*);
	/ \tr \impl @ \mf CheckWidget;
	/ \tr \impl @ \mf GetTopWidgetPtr;
/ \tr \impl @ \mf YFrame::DrawContents;
/ \tr \impl @ \mf (GetTopVisibleDesktopObjectPtr & MoveToTop) @ \cl YDesktop;

r49-r50:
/= test 8 ^ \conf release;

r51-r54:
/= test 9;

r55-r56:
/ @ ShlSetting::TFormExtra @ \u Shells:
	+ \mf void OnMove_btnDragTest(TouchEventArgs&);
	/ \impl @ \ctor;

r57-r61:
/= test 10;

r62:
* \impl @ \f OnTouchMove_Dragging @ \impl \u YControl;

r63:
/= test 11 ^ \conf release;


$DOING:

$relative_process:
2011-04-19:
-25.9d;

/ ...


$NEXT_TODO:

b203-b288:
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

$transform $list ($list_member $pattern $all($exclude $pattern
	$string_literal "*")) +;

$ellipse_refactoring;

$now
(
/ $design "unit renaming",
/ "improvoed windows painting efficiency",
* "buffered coordinate delayed in painting dragged control" $since b169
),

b201
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

