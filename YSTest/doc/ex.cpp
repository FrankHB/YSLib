//v0.3110; *Build 203 r24;
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
	\cl YWidget;
}
\u YLabel
{
	\cl MLabel;
	\cl YLabel;
	\cl MTextList;
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
/ @ \dir Shell:
	+ \u YMenu;
	+ \u Viewer;
	+ \u YConsole;
/ \h guard @ \a \h @ \lib (YSLib & YCLib & CHRLib);
/ @ \u YComponent:
	/ \cl YConsole >> \u YConsole;
	/ \clt GSequenceViewer >> \h Viewer;
	- \inc "../Helper/yglobal.h" @ (\h & \impl \u);
+ \inc "../Helper/yglobal.h" @ \h YConsole;
/ \a ustring => u16string;
+ \inc "viewer.h" @ \h ListBox;
+ \inc "yconsole.h" @ \impl \u YUIcontainer;

r2:
/= test 1 ^ \conf release;

r3:
/ \mg \f YMain @ \impl \u Platform::DS::Main -> \f main \impl \u YGlobal;
/ @ \impu \u YGlobal:
	/ \mg \f void YDestroy() @ \un \ns -> \f main;
	/ \mg \f void YSInit() @ \un \ns -> \f main;
	- \pre \decl @ \f int YMain(int, char*[]);

r4:
- \sm DefaultShellHandle @ \cl YApplication;
/ @ \u YGlobal:
	+ \f YApplication& GetApp();
	+ \f const GHandle<YShell>& GetMainShellHandle();
	- \g reference theApp;
/ \a theApp -> GetApp();
- \pre \decl extern YApplication& theApp @ \h YShellDefinition;
+ \tr \inc \h @ \impl \u (YFont & YShellInitialization);
/ \tr \impl @ \mf void YApplication::ResetShellHandle() ythrow();
/ \tr \impl @ (\dtor & \mf DefShlProc & \mf PostQuitMessage) @ cl YShell;

r5:
/= test 2 ^ \conf release;

r6:
/ @ \cl YApplication:
	- \m Global* pResource;
	/ \tr \impl @ (\ctor & \dtor);
	/ \mf GetPlatformResource() ythrow()
		-> !\m \f GetGlobal() ythrow() @ \u YGlobal;
	- \pre \decl @ \cl Global @ \h;
/ @ \u YGlobal:
	/ \impl @ \f GetApp;
	/ friend \decl \cl YApplication -> friend \f Global& GetGlobal() ythrow()
		@ \cl Global;

r7:
/= test 3 ^ \conf release;

r8:
/ @ \u YNew:
	- \g \o extern YSLib::MemoryList DebugMemory;
	+ \f MemoryList& GetDebugMemoryList();
	/ \tr @ \impl \u;
	/ \tr @ \mac (ydelete & ydelete_array);
/ \tr @ \impl \u YGlobal;

r9:
/ @ \u YNew:
	+ \m \cl NewRecorder @ \cl MemoryList;
	/ \impl @ \mac ^ MemoryList::NewRecorder
		~ ::operator new(__FILE__, __LINE__);

r10:
/= test 4 ^ \conf release;

r11:
/ \rem \a overload (\f ::operator new & ::operator delete) @ \u YNew;

r12:
/ \u Label["label.h", "label.cpp"] => YLabel["ylabel.h", "ylabel.cpp"];
+ \u YLabelEx["ylabelx.h", "ylabelx.cpp"];
/ \cl MTextList @ \ns Components::Widgets => \ns Components::Widgets
	@ \u YLabelEx ~ \u YLabel;
/ \tr \inc @ \h (ListBox & Button);
/ \inc \h YText -> \h YLabelEx @ \h ListBox;

r13:
/ @ \cl MTextList:
	/ \ac @ \inh MLabel -> public ~ private;
	/ \ctor \exp MTextList(const Drawing::Font& = Drawing::Font::GetDefault())
		-> \exp MTextList(GHWeak<ListType> = NULL,
		const Drawing::Font& = Drawing::Font::GetDefault());
	+ typedef typename ListType::size_type IndexType;
	+ \mf void RefreshTextState();
/ @ \u ListBox:
	/ @ \cl YSimpleListBox:
		/ protected \m GHWeak<ListType> wpList -> protected mutable \m
			@ \cl MTextList;
		/ private \m Drawing::TextState text_state >> \cl MTextList;
		+ \inh \cl Widgets::MTextList;
		- \m Drawing::Font Font;
		- \m Drawing::Padding Margin;
		/ \tr \impl @ \ctor;
		/ \mf ListType& GetList() const ythrow()
			-> ListType& GetList() @ \cl MTextList;
		/ \mf Drawing::TextState& GetTextState() ythrow()
			-> protected Drawing::TextState& GetTextState() @ \cl MTextList;
		/ typedef String ItemType >> \cl MTextList;
		/ typedef vector<ItemType> ListType >> \cl MTextList;
		/ \mf ItemType* GetItemPtr(ViewerType::IndexType)
			-> ItemType* GetItemPtr(IndexType) const @ \cl MTextList;
		/ \tr \impl @ \mf Paint;
		/ \mf GetItemHeight >> \cl MTextList;
	/ \tr \impl @ \ctor @ \cl YListBox;

r14:
/ \cl MTextList >> \u YLabel;
- \u YLabelEx;
/ \tr \inh \h YLabelEx -> \h YLabel @ \h ListBox;

r15:
/= test 5 ^ \conf release;

r16:
/ @ \dir Shell
	- file "viewer.cpp";
	/ \h Viewer["viewer.h"] => YViewer["yviewer.hpp"];
/ @ \u ListBox:
	/ \cl YSimpleListBox -> \cl YMenu @ \u YMenu;
	+ \inc \h YMenu @ \h;
	- \inc \h YControl @ \h;
	- \inc \h YLabel @ \h;
	- \inc \h Viewer @ \h;
/ @ \h YMenu:
	+ \inc \h YControl;
	+ \inc \h YLabel;
	- \inc \h YWindow;
	+ \inc \h YViewer;
	+ \h YWindow @ \impl \u;
	+ \h YStyle @ \impl \u;

r17:
/= test 6 ^ \conf release;

r18:
/ @ \h YReference:
	+ \inc \h <memory>;
	+ \inc \h <tr1/memory>;
	/ @ \ns YSLib:
		+ using std::auto_ptr;
		+ using std::tr1::bad_weak_ptr;
		+ using std::tr1::const_pointer_cast;
		+ using std::tr1::dynamic_pointer_cast;
		+ using std::tr1::enable_shared_from_this;
		+ using std::tr1::get_deleter;
		+ using std::tr1::shared_ptr;
		+ using std::tr1::static_pointer_cast;
		+ using std::tr1::weak_ptr;
	- 2 \ft handle2int;
	- \ft static_handle_cast;
/= \a std::auto_ptr -> auto_ptr @ (\h Scroll & \impl \u YFont);

r19:
/ @ \h YReference:
	/ \t<typename T,
		template<class> class OP = Policies::GeneralCastRefCounted,
		class CP = Policies::DisallowConversion,
		template<class> class KP = Policies::AssertCheck,
		template<class> class SP = Policies::DefaultSPStorage,
		typename SPT = SmartPtr<T, OP, CP, KP, SP> > class GHandle
		-> \clt<typename T, class SPT = shared_ptr<T> >;
	/ @ \clt GHandle:
		+ \mf \op==;
		+ !\exp \ctor @ GHandle(SPT);
		- \mf \op->;
	/ \tr @ \ft<typename _type> \i _type* GetPointer(GHandle<_type>);
/ \impl @ \f FetchGUIShellHandle @ \impl \u YGUI:
	^ dynamic_pointer_cast ~ general_handle_cast;
/ \tr \impl @ \f InitConsole @ \impl \u YGlobal;

r20:
/ @ \h YReference:
	- \ft general_handle_cast;
	/ \mg \a YReset_debug => YReset_ndebug;
	/ \a YReset_ndebug => ResetHandle;
	- \mac YReset;
	- \clt GeneralCastRefCounted @ \ns Policies;
	- \ns Policies;
/ @ \h YCoreUtilities:
	/ \tr @ \cl safe_delete_obj_ndebug;
	/ \cl safe_delete_obj_debug \mg -> safe_delete_obj_ndebug;
	/ \cl safe_delete_obj => safe_delete_obj;
	- \mac safe_delete_obj;
/ \tr @ \impl \u YShell;
/ \tr \impl @ \mf Global::ReleaseDevices @ \impl \u YGlobal;
/ \tr \impl @ \impl \u Shells;

r21:
* \tr \impl @ \ft<typename _type> \i bool ResetHandle(_type*&) ythrow()
	@ \h YReference;

r22:
/ @ \h YReference:
	/ \t<typename T,
		template<class> class DP = Policies::DeleteSingle,
		class OP = Policies::TwoRefCounts,
		class CP = Policies::DisallowConversion,
		template<class> class KP = Policies::AssertCheck,
		template<class> class RP = Policies::CantResetWithStrong,
		typename SPT = StrongPtr<T, false, OP, CP, KP, RP, DP> > class GHWeak
		-> \clt<typename T, SPT = weak_ptr<T> >;
	- \clt GHStrong;
	/ \a GHStrong -> GHandle;
	- \mf Reset @ \clt (GHandle & GHWeak);
	/ \tr \impl 2 @ \ft ResetHandle;
	/ @ \clt (GHandle & GHWeak):
		- \mf \op RefToValue<GHandle>();
		+ \mf bool \op!() const;
		+ \mf \op bool() const;
	+ \mf typename std::tr1::add_reference<_Tp>::type operator*() const
		@ \clt GHWeak;
- using Loki::StrongPtr @ \h YAdaptor;

r23:
/ @ \h YAdaptor:
	- using Loki::RefToValue;
	- {
		using Loki::PropagateConst;
		using Loki::DontPropagateConst;
		using Loki::RefCounted;
		using Loki::AllowConversion;
		using Loki::DisallowConversion;
		using Loki::RejectNull;
		using Loki::AssertCheck;
		using Loki::DefaultSPStorage;
		using Loki::HeapStorage;
		using Loki::TwoRefCounts;
		using Loki::CantResetWithStrong;
		using Loki::AllowReset;
		using Loki::NeverReset;
		using Loki::DeleteSingle;
		using Loki::DeleteNothing;
		using Loki::DeleteArray;
	} @ \ns Policies
	- using Loki::SmartPtr;
	- \ns Policies;
/ \tr @ \h Container @ \dir Adaptor;
/ \a SmartPtr -> GHandle;
/ \tr \impl @ \ft<typename _type> \i _type* GetPointer(shared_ptr<_type>)
	@ \h YReference;
/ \tr \impl @ \f bool operator==(const Message&, const Message&)
	@ \impl \u YShellMessage;
/ \tr \impl @ \mf GetCopyOnWritePtr @ \clt GDependency @ \h YObject;

r24:
/= test 7 ^ \conf release;


$DOING:

$relative_process:
2011-04-22:
-23.7d;

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
	/ $design "units rearrangement",
	/ $design "global architecture" $=
	(
		"global instance function",
		- "all global objects",
		- "platform dependent global resource from class %YApplication",
		- "global object in unit YNew"
	),
	"class %MTextList",
	/ "class %YSimpleText List using class %MTextList inheritance",
	/ "class %YSimpleText renamed to %YMenu",

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
	"key held response in %ShlReader",
	"GDI API %BlitTo",
	/ "widgets drawing",
	/ $design "GDI API %CopyTo",
	"controls: panel",
	/ $design "YCLib" $=
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

