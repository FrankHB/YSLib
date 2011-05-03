//v0.3121; *Build 206 r16;
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
r1:
/ @ \cl ShlSetting::TFormTest @ \u Shells;
	+ \m Button btnMenuTest;
	/ \impl @ \ctor;

r2:
* \impl @ \ctor @ \cl ShlSetting::TFormTest @ \u Shells;

r3:
/ @ \cl YFile:
	/ \mf rewind => Rewind;
	/ typedef std::ptrdiff_t OffType => OffsetType;
	/ \mf ftell => GetPosition;
	/ \mf fseek => SetPosition;
	/ \mf fread => Read;
	/ \mf feof => CheckEOF;
	+= \i \mf ^ \mac ImplRet;
	/ \mf Release => Close;
	/= \mf order;
	/ \inh YObject -> NonCopyable;
/ \mf GetPos => GetTextPosition @ \cl YTextFile;
/ \a \cl YFile => File;
/ \a \cl YTextFile => TextFile;
/ @ \h YStandardEx:
	+ using ::memcmp;
	* @ \h:
		-= duplicate \inc;
	+ \inc \h <cstring>;
+ using ystdex::memcmp @ \impl \u YFile_(Text);
+ using File::Read @ \cl TextFile;

r4:
/ @ \cl FileList @ \u YFileSystem:
	/ protected \m GHWeak<ListType> wpList -> GHandle<ListType> pList;
	/ \tr \impl @ \ctor;
	/ \mf DefGetter(GHandle<ListType>, ListWeakPtr, pList)
		-> DefGetter(GHandle<ListType>, ListPtr, pList);
	/ \tr \impl @ \mf;
/ @ \cl MTextList @ \u YLabel:
	/ protected mutable \m GHWeak<ListType> wpList -> GHandle<ListType> pList;
	/ \tr \impl @ \ctor;
	/ \tr \impl @ \mf;
/ \tr \decl @ \ctor @ \cl (Menu & ListBox & FileBox);

r5:
/= test 1 ^ \conf release;

r6:
/ \u YFile @ \dir Core >> \dir Service; 
/ \u YFile_(Text) @ \dir Core >> \dir Service; 
+ \u YGDIBase["ygdibase.h", "ygdibase.cpp"] @ \dir Core;
/ @ \u YObject:
	/ \ns Drawing >> \u YGDIBase;
	- \inc \h (YShell & YGlobal) @ \impl \u;
/ \tr + \inc "ygdibase.h" @ \h YDevice;
- using Drawing::Point @ \h YShellMessage;
/ \h Config["config.h"] @ \dir Adaptor => Configuration["config.h"];
- \rem \mac YSLIB_NO_CURSOR @ \h Configuration;
/ \tr \inc "../Core/yftext.h" -> \inc "yftext.h"
	@ \h YTextManager @ \dir Service;
/ \tr \inc "../Core/yftext.h" -> \inc "../Service/yftext.h"
	@ \h YText;
/ \tr @ \h YGlobal:
	+ \inh \c "../Core/ygdibase.h";
	/ \a Point @ \ns Messaging -> Drawing::Point;
/ \tr \inc \h "ystring.h" -> "../Core/ycutil.h" @ \h YFile;
/ \tr @ \h YGDI:
	- \inc \h YObject;
	+ \inc \h YCoreUtility;
	+ \inc \h YGDIBase;
/ \tr @ \h YGUI:
	/ \a Point -> Drawing::Point;
/ \tr \inc <YSLib/Core/yftext.h> -> <YSLib/Service/yftext.h> @ \h DSReader;
/ @ \impl \u YGlobal:
	/ \a Drawing::Point -> Point;
	+ using \ns Drawing;
+ \inc \h YString @ \h YText;

r7:
/= test 2 ^ \conf release;

r8:
+ '-std=c++0x' @ \a \mac CXXFLAGS @ makefiles;
/ \a '> >' -> '>>' @ \lib YCLib;
/ @ \h Platform:
	/ \ns std => ystdex ;
	/ @ \ns ystdex:
		/ char16_t => uchar_t;
		/ char32_t => fchar_t;
/ @ \h CHRDefinition:
	/ typedef std::char32_t fchar_t -> using ystdex::fchar_t;
	/ typedef std::char16_t uchar_t -> using ystdex::uchar_t;
/ \a '-std=c++0x' -> '-std=gnu++0x' @ \mac CXXFLAGS
	@ makefiles \exc makefile @ \lib CHRLib;
* implicit narrowing conversion @ \impl \u (YStyle & Scroll);

r9:
/= test 3 ^ \conf release;

r10:
/ @ \ns ystdex @ \h YStandardEx:
	+ \inc \h <type_traits>;
	/ \rem {
		using std::tr1::add_reference;

		using std::tr1::has_nothrow_assign;
		using std::tr1::has_nothrow_constructor;
		using std::tr1::has_nothrow_copy;
		using std::tr1::has_trivial_assign;
		using std::tr1::has_trivial_constructor;
		using std::tr1::has_trivial_copy;
		using std::tr1::has_trivial_destructor;
	- \a !\rem using;
	+ {
		using std::integral_constant;
		using std::true_type;
		using std::false_type;

		using std::is_void;
		using std::is_integral;
		using std::is_floating_point;
		using std::is_array;
		using std::is_pointer;
		using std::is_lvalue_reference;
		using std::is_rvalue_reference;
		using std::is_member_object_pointer;
		using std::is_member_function_pointer;
		using std::is_enum;
		using std::is_class;
		using std::is_union;
		using std::is_function;

		using std::is_reference;
		using std::is_arithmetic;
		using std::is_fundamental;
		using std::is_object;
		using std::is_scalar;
		using std::is_compound;
		using std::is_member_pointer;

		using std::is_const;
		using std::is_volatile;
		using std::is_trivial;
	//	using std::is_trivially_copyable;
		using std::is_standard_layout;
		using std::is_pod;
	//	using std::is_literal_type;
		using std::is_empty;
		using std::is_polymorphic;
		using std::is_abstract;

		using std::is_signed;
		using std::is_unsigned;

		using std::is_constructible;

		using std::has_virtual_destructor;

		using std::alignment_of;
		using std::rank;
		using std::extent;

		using std::is_same;
		using std::is_base_of;
		using std::is_convertible;

		using std::remove_const;
		using std::remove_volatile;
		using std::remove_cv;
		using std::add_const;
		using std::add_volatile;
		using std::add_cv;

		using std::remove_reference;
		using std::add_lvalue_reference;
		using std::add_rvalue_reference;

		using std::make_signed;
		using std::make_unsigned;

		using std::remove_extent;
		using std::remove_all_extents;

		using std::remove_pointer;
		using std::add_pointer;

		using std::aligned_storage;
		using std::decay;
		using std::enable_if;
		using std::conditional;
		using std::common_type;
	//	using std::underlying_type;
	//	using std::result_of;
	}
	+ \rem
	{
		using std::is_default_constructible;
		using std::is_copy_constructible;
		using std::is_move_constructible;

		using std::is_assignable;
		using std::is_copy_assignable;
		using std::is_move_assignable;

		using std::is_destructible;

		using std::is_trivially_constructible;
		using std::is_trivially_default_constructible;
		using std::is_trivially_copy_constructible;
		using std::is_trivially_move_constructible;

		using std::is_trivially_assignable;
		using std::is_trivially_copy_assignable;
		using std::is_trivially_move_assignable;
		using std::is_trivially_destructible;

		using std::is_nothrow_constructible;
		using std::is_nothrow_default_constructible;
		using std::is_nothrow_copy_constructible;
		using std::is_nothrow_move_constructible;

		using std::is_nothrow_assignable;
		using std::is_nothrow_copy_assignable;
		using std::is_nothrow_move_assignable;

		using std::is_nothrow_destructible;
	}
/ \a 'std::tr1::' -> 'std::';
/ \a auto_ptr -> unique_ptr;
/ @ \h YReference:
	- using std::auto_ptr;
	/ \inc \h <tr1/memory> -> <utility>;
	/ @ \cl GHWeak:
		/ \mf typename std::add_reference<T>::type operator*() const
		-> typename std::add_rvalue_reference<T>::type operator*() const
		^ \ft std::move;
	+ using std::unique_ptr;
/ @ \h Container:
	+ \inc \h <unordered_set>;
	+ \inc \h <unordered_map>;
	+ using std::unordered_map;
	+ using std::unordered_set;
	+ \inc \h <tuple>;
	+ using std::tuple;
	+ using std::make_tuple;

r11:
/= test 4 ^ \conf release;

r12:
/ \impl @ \f template<> uchar_t
	codemap<CharSet::SHIFT_JIS>(ubyte_t&, const char*)
	@ \impl \u CharacterMapping ^ u16 ~ unsigned short;
^ auto @ \lib YCLib;
/= \impl @ \f safe_dma_fill @ \impl \u YCommon;

r13:
^ (auto & (\mf \n cbegin & cend)) @ \lib YSLib;

r14:
/ \a '> >' -> '>>' @ \lib YSLib;

r15:
/ @ \h Adaptor:
	- \mac YSL_STATIC_CHECK;
	/ @ \ns YSLib::Design:
		- using Loki::Functor;
		- using Loki::EmptyType;
		- using Loki::IsCustomUnsignedInt;
		- using Loki::IsCustomSignedInt;
		- using Loki::IsCustomFloat;
		- using Loki::TypeTraits;
		- using Loki::Typelist;
		- namespace TL = Loki::TL;
		- using Loki::NullType;
		- using Loki::Int2Type;
		- using Loki::Type2Type;
		- using Loki::Select;
	/ \inc \h <loki/StrongPtr.h> -> <loki/SmallObj.h>;
	/ @ \ns YSLib:
		- using ystdex::errno_t;
		- using ystdex::nullptr_t;
		+ using namespace ystdex;
- \a 'using ystdex::*' @ \ns YSLib;
- \a 'ystdex::' @ \ns YSLib;
/ \a NonCopyable -> noncopyable @ \lib YSLib;
/ @ \h YCoreUtility:
	- using Design::NonCopyable;
	/ @ \ns Design:
		/ \impl @ \clt MoreConvertible ^ conditional ~ Select;
		- typedef noncopyable NonCopyable;
/ \impl @ \i \mf (void operator+=(_type) & bool operator-=(_type))
	@ \cl AFrame ^ conditional
	~ Design::Select;

r16:
/= test 5 ^ \conf release;


$DOING:

$relative_process:
2011-05-03:
-24.9d;
//c1-c76:r3804;

/ ...


$NEXT_TODO:

b207-b288:
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
	+ "menu test button",
	/ "file API",
	* strict ISO C++2003 code compatibility $=
	(
		+ "function %memcmp declation in namespace %ystdex"
	),
	* strict ISO C++2011 code compatibility $=
	(
		* implicit narrowing conversion(N3242 8.5.4/6) \
			in C++2011(N3242 5.17/9) ^ explicit static_cast,
		/ character types in "platform.h"
	),
	/ coding using limited C++2011 features $=
	(
		/ $design ^ C++2011 style nested template angle brackets,
		/ $design ^ keyword %auto,
		/ ^ C++2011 type_traits in namespace std instead of std::tr1
	),
	- Loki type operations,
	/ ^ namespace ystdex in namespace YSLib
),

b205,
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

