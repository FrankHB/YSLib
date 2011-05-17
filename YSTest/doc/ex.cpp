//v0.3132; *Build 210 r98;
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
/ \tr \impl @ \impl \u (DSReader & YFileSystem & YApplication & Shell_DS & YGUI)
	^ is_valid;

r2-r4:
/ @ \clt GDependency @ \h YObject:
	/ \impl @ \mf GetCopyOnWritePtr ^ (\exp \ctor & is_valid);
	/ \de \param -> PointerType() ~ nullptr @ \ctor;
/ \a !\exp \i \mf @ \clt (GHandle & GHWeak) -> \exp \i \mf;

r5:
/= test 1 ^ \conf release;

r6-r11:
/ \de \param @ \h (YWindow & Form):
	* \de \param \def:
		/ \a ynew -> new;
	^ share_raw;
/ \de \param @ \h (YLabel & YMenu & ListBox);
/= test 2 ^ \conf release;

r12:
/= test 3;

r13:
/ \impl @ \f \i NewScrImage ^ share_raw @ \h YShellHelper; 
/ \impl @ \impl \u (YApplication & Shell_DS & YLabel) ^ share_raw;

r14:
/= test 4 ^ \conf release;

r15:
/ \de \param -> PointerType() ~ nullptr @ \ctor @ \clt GDependencyEvent
	@ \h YEvent;

r16:
/= test 5 ^ \conf release;

r17:
/ \impl @ \u Shells ^ \exp \de \ctor ~ nullptr;
/ \impl @ \impl \u YShell ^ \exp \de \ctor ~ nullptr;

r18:
/= test 6 ^ \conf release;

r19-r58:
/= test 7-9 ^ \conf release;
/ @ \h YReference:
	- \mf (\op bool & \op!) \clt GHandle;
	- \clt GHWeak;
	- \tr \a \ft with \param \tp 'GHWeak';
/= test 10;
/= test 11 ^ \conf release;
/= test 12;
/ \rem @ \h Memory;
/= test 13-16 ^ \conf release;
/= test 17;
/= test 18 ^ \conf release;
/ \simp @ \h YFileSystem:
	- \stt HFileNameFilter;
	- typedef bool FNFILTER(const String&);
	- typedef FNFILTER* PFNFILTER;	
	- \inc \h "yfunc.hpp"
/ \simp @ \h YShell:
	- \stt HShellProc;
	- typedef int FSHLPROC(const Message&);
	- typedef FSHLPROC* PFSHLPROC;
	- \inc \h "yfunc.hpp"
/ \simp @ \h YFunc:
	- \clt GHBase;
	/ \rem \clt GHDynamicFunction;
	/ \rem 2 \ft ConstructDynamicFunctionWith;
/ @ \h Utilities:
	+ \mac #define yforward(_expr) std::forward<typename \
		std::remove_reference<decltype(_expr)>::type>(_expr);
- \mac (CHRLIB_WCHAR_T_SIZE & UNICODE) @ \h Platform;
- \mac UNICODE @ \h YCommon;
/ \a 7 (^ std::forward) -> (^ yforward) @ \h YEvent;
/ \a 4 (^ std::forward) -> (^ yforward) @ \h YFunc;

r59:
/= test 18 ^ \conf release;

r60:
/ \impl @ \smf Check @ \clt (GStaticCache & GLocalStaticCache) @ \h YStatic;
- \a \c @ \tp shared_ptr<*> @ \param;

r61:
/= test 19 ^ \conf release;

r62:
/ @ \h YReference:
	- \t \ctor @ \clt GHandle;
	/ \i \ctor @ \clt GHandle -> SPT&& ~ const SPT @ \clt GHandle;
/ \impl @ \f WaitForGUIInput @ \un \ns @ \impl \u YGlobal;

r63:
/= test 20 ^ \conf release;

r64:
- \de \param @ \ctor \i GHandle(nullptr_t = nullptr)
	@ \clt GHandle @ \h YReference;

r65-r78:
/= test 21;

r79:
/ \impl @ \f void SendMessage(GHandle<YShell>, Messaging::ID,
	Messaging::Priority, Messaging::IContext*) ynothrow ^ share_raw
	@ \impl \u YApplication;
/ \impl @ \mft GetEvent @ \clt EventMap @ \h YEvent;
/ \impl @ \mf void ShlSetting::TFormTest::OnClick_btnMenuTest(TouchEventArgs&&)
	@ \cl Shells;

r80:
/= test 22 ^ \conf release;

r81:
/ \a GHandle -> shared_ptr \exc \h YReference;
/ @ \h YReference:
	- \clt GHandle;
	- \tr \a \ft with \param \tp 'GHandle';

r82:
/= test 23 ^ \conf release;

r83-r85:
/ @ \u YShellMessage:
	/ @ \cl Message:
		^ \exp \de @ copy \ctor;
		+ move \cotr ^ \exp \de;
		^ value \tp shared_ptr<*> -> \c lvalue \ref \param @ \ctor;
		/ \a \m pContext => hContext;
	/ @ \cl YMessageQueue:
		- \dtor;
		/ \ctor ^ \i \exp \de;

r86:
^ \a value \tp shared_ptr<*> -> \c lvalue \ref \param @ \ctor;
/ \m pList => hList @ \cl FileList @ \u YFileSystem;
/ \m spBgImage => hBgImage @ \cl MWindow @ \u YWindow;

r87:
/= test 24 ^ \conf release;

r88:
/ @ \u YGlobal:
	- \dtor @ \cl Global;
	- \f int ShlProc(const shared_ptr<YShell>&, const Message&);

r89:
/ \a IContext => IContent;
/ \a InputContext => InputContent;
/ \a GHandleContext => GHandleContent;
/ \a GObjectContext => GObjectContent;
/ @ \ns Messaging @ \u YShellMessage:
	/ \mf GetContextPtr => GetContentHandle;
	/ \m hContext => hContent;

r90:
/ @ \ns Messaging @ \u YShellMessage:
	+ \cl Content;
	/ @ \cl Message:
		/ \m shared_ptr<IContent> hContent -> shared_ptr<Content> hContent;
		/ \mf DefGetter(shared_ptr<IContent>, ContentHandle, hContent)
			-> DefGetter(shared_ptr<IContent>, ContentHandle, \
			hContent ? hContent->GetObject<shared_ptr<IContent>>() \
			: shared_ptr<IContent>());
		/ \tr \impl @ \ctor;

r91:
/ @ \cl Message @ \ns Messaging @ \u YShellMessage:
	+ \ctor Message(const shared_ptr<YShell>& = shared_ptr<YShell>(), ID = 0,
		Priority = 0, const shared_ptr<IContent>& = shared_ptr<IContent>())
		-> Message(const shared_ptr<YShell>& = shared_ptr<YShell>(), ID = 0,
		Priority = 0, const shared_ptr<Content>& = shared_ptr<Content>());
	+ \ctor Message(const shared_ptr<YShell>&, ID, Priority,
		const shared_ptr<IContent>&);
	+ \mf shared_ptr<IContent> GetIContentHandle() const;
	/ \mf DefGetter(shared_ptr<IContent>, ContentHandle, \
		hContent ? hContent->GetObject<shared_ptr<IContent>>() \
		: shared_ptr<IContent>())
		-> DefGetter(shared_ptr<Content>, ContentHandle, hContent);
/ \impl @ \h YShellMessageDefinition & \impl \u YShell;
/ \impl @ \f YSDebug_MSG_Print @ \un \ns @ \u Main;

r92:
/ @ \impl \u YShell:
	/ \impl @ \f PostQuitMessage ^ Content ~ GObjectContent;
	/ \tr \impl @ \mf YShell::DefShlProc;
/ @ \h YShellMessageDefinition:
	- DefMessageTypeMapping(EStandard::Quit, GObjectContent<int>)
	- \clt GObjectContent;
/ @ \u YApplication:
	/ \f void SendMessage(const shared_ptr<YShell>&, Messaging::ID,
		Messaging::Priority, Messaging::IContent* = nullptr) ynothrow
		-> \f void SendMessage(const shared_ptr<YShell>&, Messaging::ID,
		Messaging::Priority, Messaging::Content* = nullptr) ynothrow;
	+ \f void SendMessage(const shared_ptr<YShell>&, Messaging::ID,
		Messaging::Priority, Messaging::IContent*) ynothrow;

r93:
/ \impl @ \f \i NowShellInsertDropMessage @ \Shell_DS ^ Messaging::Content
	~ Messaging::GHandleContent;
/ \impl @ \mf YApplication::SetShellHandle @ \impl \u YApplication
	^ Messaging::Content ~ Messaging::GHandleContent;
/ \impl @ \mf YGUIShell::ShlProc @ \impl \u YGUI ^ Messaging::Content
	~ Messaging::GHandleContent;
/ \impl @ (\mf YShell::ShlProc & \f PostQuitMessage) @ \impl \u YShell
	^ Messaging::Content ~ Messaging::GHandleContent;

r94:
/ @ \h YShellMessageDefinition:
	- DefMessageTypeMapping(EStandard::Set, GHandleContent<shared_ptr<YShell>>)
	- DefMessageTypeMapping(EStandard::Drop, GHandleContent<shared_ptr<YShell>>)
	- DefMessageTypeMapping(EStandard::Activated,
		GHandleContent<shared_ptr<YShell>>)
	- DefMessageTypeMapping(EStandard::Deactivated, GHandleContent< \
		shared_ptr<YShell>>)
	- DefMessageTypeMapping(EStandard::Paint,
		GHandleContent<shared_ptr<Desktop>>)
	- \clt GHandleContent;

r95:
^ \conf release;
/= test 25;

r96:
/ \tr @ \u YGlobal:
	/ @ \cl InputContent:
		- \inh \impl IContent;
		/ \mf ImplI1(IContent) bool operator==(const IContent&) const
			-> !\vt bool operator==(const InputContent&) const;
	/ \impl @ \f WaitForGUIInput \un \ns;
/ \impl @ \f ResponseInput @ \impl \u Shell_DS;

r97:
^ \conf debug;
/ @ \h YMessageDefinition:
	- DefMessageTypeMapping(EStandard::Input, InputContent);
	- 2 \ft CastMessage;
	- \pre \decl class InputContent;
	- DefMessageTypeMapping(EStandard::Null, IContent);
	- \stt MessageTypeMapping;
	- \mac DefMessageTypeMapping;
	/ MessageID >> \ns Messaging ~ \st EStandard;
	- \st EStandard;
	/ \tr \decl @ \mac;
- \f void SendMessage(const shared_ptr<YShell>&, Messaging::ID,
	Messaging::Priority, Messaging::IContent*) ynothrow @ \u YApplication;
/ @ \u YShellMessage:
	/ \cl Message:
		- \ctor Message(const shared_ptr<YShell>&, ID, Priority,
			const shared_ptr<IContent>&);
		- \mf shared_ptr<IContent> GetIContentHandle() const;
	- \in IContent;

r98:
/= test 26 ^ \conf release;


$DOING:

$relative_process:
2011-05-17:
-20.5d;
//Mercurial rev1-rev80: r3955;

/ ...


$NEXT_TODO:

b211-b324:
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
	/ "implementation of messages"
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

