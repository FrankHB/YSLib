//v0.3027; *Build 195 r187;
/*
$Record prefix and abbrevations:
<statement> ::= statement;
; ::= statement termination
= ::= equivalent
+ ::= added
- ::= removed
* ::= fixed
/ ::= modified
% ::= reformed
~ ::= from
! ::= not
& ::= and
| ::= or
^ ::= used
-> ::= changed to
>> ::= moved to
=> ::= renamed to
<=> ::= swaped names
@ ::= identifier
@@ ::= in / belonged to
\a ::= all
\ab ::= abstract
\ac ::= access
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
	\cl YWindow;
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
	\cl AButton;
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
/ @@ \h Base:
	+ \mac DefMutableConverter;
	+ \mac DefMutableConverterBase;
	+ \mac DefMutableConverterMember;
+ \clt GDependence @@ \h YObject;

r2:
/ @@ \h Base:
	+ \mac DefMemberFunctionImage1;
	+ \mac DefMemberFunctionImage2;
	+ \mac DefMemberFunctionImage3;
	+ \mac DefMemberFunctionImage4;
/ @@ \clt GDependence;
	* \impl @@ getter;
	+ operators;

r3:
* 5 \mac 'DefMemberFunctionImage?' @@ \h Base;

r4:
/ 5 \mac 'DefMemberFunctionImage?'(_name)
	-> 'DefMemberFunctionImage?'(_type, _name) @@ \h Base;
/ \tr \impl @@ \clt GDependence;

r5:
- 5 \mac 'DefMemberFunctionImage';
/ \decl @@ \clt GDependence;

r6-r7:
/= test 1;

r8:
* \mf GetCopyOnWritePtr @@ \clt GDependence;

r9-r15:
/ test 2:
	* \tr \def @@ \mac @@ \a makefile \exc \proj YSTest;

r16-r27:
/= test 3;

r28:
/ \clt<class _type> GDependence
	-> \clt<typename _type, class _tOwnerPointer = SmartPtr<_type> >;

r29-r31:
/= test 4;

r32:
/ @@ \h YEvent:
	+ \clt GDependencyEvent @@ \ns Runtime;
	/ \a _tEventHandler => EventHandlerType @@ \clt GDependencyEvent;

r33:
/ @@ \h YEvent:
	/ \a _tEventHandler => EventHandlerType @@ \clt GDependencyEvent;
	+ \mac (DepEventT & DeclDepEvent & DeclDepEventRef & DefIDepEventEntry
		& GetDepEvent & GetDepEventBase & GetDepEventMember & GetMutableDepEvent
		& GetMutableDepEventBase & GetMutableDepEventMember;

r34-r35:
/ test 5;
+ \mf uintptr_t GetCount() const @@ \clt GeneralCastRefCounted
	@@ \h YReference;
/ \impl @@ \mf GetCopyOnWritePtr @@ \clt GDependence;
/ \impl @@ \clt GDependenceEvent @@ \h YEvent;

r36-r38:
/= test 6;

r39:
/ \impl @@ \u (ListBox & Scroll);

r40:
/ \tr \impl @@ \clt RefCounted @@ \h SmartPtr @@ \lib Loki; 7
/ \tr \impl \h YReference;
/ \tr \impl @@ \clt GDependence @@ \h YObject;

r41-r49:
+ \mf bool Insert(const ID&, const ItemType&) @@ \clt GEventMap @@ \h YEvent;
/ test 7;

r50:
/ @@ \h YShellHelper:
	+ \t<class _tShl> \i \f GHHandle<YShell> FetchStored();
	+ \t<class _tShl> \i \f void ReleaseStored();
	/ \impl @@ \t<class _tShl> \i \f errno_t NowShellToStored() ^ FetchStored;
	/ \impl @@ \t<class _tShl> \i \f void SetShellToStored() ^ FetchStored;
/ \impl @@ \mf ReleaseShells @@ \impl \u Shells;

r51:
+ \h YStatic["ystatic.hpp"] @@ \dir Core;
/ @@ \h YObject:
	/ \clt GStaticCache >> \h YStatic;
	- \in \t GIClonable;
	- \cl \t GContainer;
/ \a GContainer -> set;
/ \t<typename _type> \i \f _type& GetStaticRef() @@ \h YCoreUtilities
	>> \h YStatic;
+ \inc \h YStatic @@ (\impl \u YApplication & \h YFocus);
- \inc \h (YCoreUtilities & YObject) @@ \h YFocus;
/ \inc \h (YObjcet -> YStatic) @@ \h YResource;

r52:
+ \clt GLocalStaticCache @@ \h YStatic;
/ \impl @@ \i \tf (FetchStored & ReleaseStored) @@ \h YShellHelper
	^ GLocalStaticCache ~ GStaticCache;

r53-r63:
/= test 8;

r64:
/ @@ \h YEvent:
	/ \decl @@ \clt GEventWrapper:
		/ \inh _tEvent -> GDependencyEvent<_tEvent>;
		+ typedef GDependencyEvent<_tEvent> DependencyType;
		/ \tr \impl @@ typedef;
	/ @@ \clt GEventMap:
		/ \impl @@ \mf GetEvent;
		/ \tr \impl @@ \mf DoEvent;
	+ typedef Runtime::GDependencyEvent<EventType> DependencyType
		@@ 2 \stt GSEvent;
	/ \impl @@ \mac DepEventT;
/ \impl @@ \ft FetchEvent @@ \h YControl;

r65-r66:
/= test 9;

r67:
/ @@ \h YControl:
	+ typedef Runtime::GEventMap<VisualEvent> VisualEventMapType;
	/= \a Runtime::GEventMap<VisualEvent> \exc typedef -> VisualEventMapType;
	+ 1 overloaded \i \ft FetchEvent;
	+ 1 overloaded \i \ft CallEvent;
	/ \tr @@ \i \ft (FetchEvent & CallEvent) ^ overloaded \i \ft;

r68:
+ \i \mf PointerType& at(const ID& k) const ythrow(std::out_of_range)
	@@ \clt GEventMap @@ \h YEvent;

r69:
/ @@ \cl Control:
	+ \smf const VisualEventMapType& GetDefaultEventMap();
	/ \impl @@ \ctor;

r70-r75:
/= test 10;

r76:
* \impl @@ \smf GetDefaultEventMap @@ \cl Control;

r77-r116:
/= test 11;

r117:
/ @@ \clt GDependency @@ \h YObject:
	- \i \mf Create;
	* \impl @@ \mf GetCopyOnWritePtr;
* \impl @@ \f OnTouchMove @@ \u YControl;

r118-r122:
/= test 12;

r123:
/ @@ \cl YControl:
	- \mf GetDefaultEventMap;
	/ \impl @@ \ctor;
* \def @@ \mf Insert @@ \clt GEventMap @@ \h YEvent;

r124:
/ @@ \u YControl:
	/ \impl @@ \ctor @@ \cl YControl;
	/ \ret \tp @@ 2 \ft FetchEvent;
/ @@ \h YEvent:
	/ \decl @@ \clt (GEventWrapper & GEventMap);
	- \clt GAHEventCallback;

r125:
/= test 13 ^ \conf release;

r126:
/ \a YFrameWindow => YWindow;

r127:
/ @@ \h YControl:
	+ \st RoutedEventArgs;
	/ \st ScreenPositionEventArgs => MScreenPositionEventArgs;
	/ @@ \st MScreenPositionEventArgs:
		- \inh \st EventArgs;
		/ \ac @@ \ctor protected ~ public;
		/ \tr \impl @@ \ctor;
	* @@ \st InputEventArgs:
		+ public \inh \st RoutedEventArgs;
		/ \tr \impl @@ \ctor;
	/ @@ \st KeyEventArgs:
		- \inh \st EventArgs;
		/ \tr \def @@ \ctor;
	/ @@ \st TouchEventArgs:
		/ order @@ \inh;
		/ \tr \def @@ \ctor;

r128:
/ @@ \cl YGUIShell @@ \u YGUI:
	/ \mf bool ResponseTouchUp(IUIBox&, Components::Controls::TouchEventArgs&)
		-> \f bool ResponseTouchUp(IControl&,
		Components::Controls::TouchEventArgs&);
	/ \mf bool ResponseTouchDown(IUIBox&, Components::Controls::TouchEventArgs&)
		-> \f bool ResponseTouchUp(IControl&,
		Components::Controls::TouchEventArgs&);
	/ \mf bool ResponseTouchHeld(IUIBox&, Components::Controls::TouchEventArgs&)
		-> \f bool ResponseTouchUp(IControl&,
		Components::Controls::TouchEventArgs&);
	/ \mf bool ResponseTouchBase(IUIBox&, Components::Controls::TouchEventArgs&,
		bool(YGUIShell::*)(IControl&, Components::Controls::TouchEventArgs&))
		-> bool ResponseTouchBase(IControl&, Components::Controls
		::TouchEventArgs&, bool(YGUIShell::*)(IControl&,
		Components::Controls::TouchEventArgs&));
	/ \mf IControl* GetTouchedVisualControlPtr(IUIBox&, Point&)
		-> IControl* GetTouchedVisualControlPtr(IControl&, Point&);

r129:
/ @@ \cl YGUIShell @@ \u YGUI:
	/ private \m ExOpType ExtraOperation
		-> Components::Controls::VisualEvent OperationType;
		- typedef \en ExOpType;
	/ \tr \impl @@ \mf (GetTouchedVisualControlPtr & ResponseTouchUp
		& ResponseTouchDown & ResponseTouchHeld) & \ctor;

r130:
/ @@ \cl RoutedEventArgs @@ \h YControl:
	/ typedef \en RoutingType => RoutingStrategy;
	/ \m Type => Strategy;

r131:
/ @@ \u Shell_DS:
	- \i \f (3 'OnKey*' & 3 'OnTouch*') @@ \h;
	/ \impl @@ \f ResponseInput;

r132:
/ @@ \cl YGUIShell @@ \u YGUI:
	/ \mf IControl* GetTouchedVisualControlPtr(IControl&, Point&)
		-> IControl* GetTouchedVisualControlPtr(IControl&, Point&,
		Components::Controls::VisualEvent);
	- private \m Components::Controls::VisualEvent OperationType;
	/ private \mf bool ResponseTouchBase(IControl&,
		Components::Controls::TouchEventArgs&,
		bool(YGUIShell::*)(IControl&, Components::Controls::TouchEventArgs&))
		-> public \mf bool ResponseTouch(IControl&, Components::Controls
		::TouchEventArgs&, Components::Controls::VisualEvent);
	- \mf ResponseTouchUp;
	- \mf ResponseTouchDown;
	- \mf ResponseTouchHeld;
	/ \tr \impl @@ \ctor;
/ \impl @@ \f ResponseInput @@ \impl \u Shell_DS;

r133:
/ @@ \cl YGUIShell @@ \u YGUI:
	- \mf GetTouchedVisualControlPtr;
	/ \impl @@ ResponseTouch;

r134:
/ \impl @@ \f ResponseInput @@ \impl \u Shell_DS;
/ @@ \cl YGUIShell @@ \u YGUI:
	/ private \mf bool ResponseKeyBase(YDesktop&,
		Components::Controls::KeyEventArgs&, bool(YGUIShell::*)(IControl&,
		Components::Controls::KeyEventArgs&)
		-> public \mf bool ResponseKey(YDesktop&, Components::Controls
		::KeyEventArgs&, Components::Controls::VisualEvent);
	- \mf (ResponseKeyUp & ResponseKeyDown & ResponseKeyHeld);

r135:
/ \impl @@ \mf YGUIShell::ResponseTouch;

r136:
/ \tr \impl @@ (\f (OnTouchHeld & OnTouchMove & OnTouchMove_Dragging)
	& \mf Control::OnTouchDown) @@ \impl \u YControl;
/ \tr \impl @@ \mf (ATrack::OnTouchDown
	& YHorizontalTrack::OnTouchMove_Thumb_Horizontal
	& YVerticalTrack::OnTouchMove_Thumb_Vertical & (OnTouchDown_PrevButton
	& OnTouchDown_NextButton) @@ \cl AScrollBar) @@ \impl \u Scroll;
/ \tr \impl @@ \mf (OnTouchDown & OnTouchMove) @@ \cl YSimpleListBox
	@@ \impl \u ListBox;

r137-r146:
/= test 14;

r147:
/ @@ \cl YGUIShell @@ \u YGUI:
	/ \impl @@ \mf ResponseTouch;
	- private \mf (ResponseTouchUpBase & ResponseTouchDownBase
		& ResponseTouchHeldBase);

r148-r150:
/= test 15;

r151-r152:
* \impl @@ \mf YGUIShell::ResponseTouch;

r153-r154:
/= test 16 ^ \conf release;

r155:
/= test 17 ^ \conf debug;

r156-r186:
/ test 18 ^ \conf release:
	/ YSLib ARM9 Makefile optimizing commands ^ \conf release;

r187:
/= test 19 ^ \conf debug;


$DOING:

relative process:
2011-03-20:
-27.6d;

/ ...


$NEXT_TODO:

b196-b270:
+ routing event for the view tree;
/ fully \impl \u DSReader;
	* moved text after setting lnGap;
* non-ASCII character path error in FAT16;
/ non-ordinary operator usage in \clt GSequenceViewer @@ \h YComponent;
/ impl 'real' RTC;

b271-b648:
+ data configuragion;
+ shared property: additional;
/ GDI brushes;
/ text alignment;
+ \impl pictures loading;
/ improve efficiency @@ \ft polymorphic_crosscast @@ \h YCast;
+ correct DMA (copy & fill);
* platform-independence @@ alpha blending:
	+ \impl general Blit algorithm;
/ user-defined bitmap buffer @@ \cl YDesktop;
+ (compressing & decompressing) @@ gfx copying;


$LOW_PRIOR_TODO:
r325-r768:
+ \impl styles @@ widgets;
+ general component operations:
	+ serialization;
	+ designer;
+ database interface;


$NOTHING_TODO:
* fatal \err @@ b16x:
[
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
	-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 02055838
	ftc_snode_weight
	ftcsbits.c:271
]
* fatal \err @@ since b177 when opening closed lid @@ real DS:
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


$LAST_SUCCESSFULLY_FIXED:

$TODO:

Clarify the log levels.

Make "UpdateFont()" more efficienct.

More efficient %YTextRegion output:
Use in-buffer background color rendering and function %CopyToScreen()
	to inplements %YTextRegion background;
Use pre-refershing to make font changing.

Consider to simplify the general window class: @YForm.

Rewrite system RTC.

Build a more advanced console wrapper.

Build a series set of robust gfx APIs.

More GUI features needed:
Icons;
Other controls.

Other stuff to be considered to append:
Design by contract: DbC for C/C++, GNU nana.


$HISTORY:

all:
{
"dependency events",
$fix "makefiles"
},

b170_b194:
{
"controls: track",
"controls: scroll bar",
"controls: scrollable container",
"controls: listbox"
},

b159_b169:
{
"controls: buttons": class ("%YThumb", "%YButton"),
"controls: listbox class",
"events",
},

b132_b158:
{
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
},

b1_b131:
{
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
"dual screen text file reader",
"output devices & desktops",
"messaging",
"program initialization",
"simple GUI: widgets & windows",
"simple GDI",
"simple resource classes"
};


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

