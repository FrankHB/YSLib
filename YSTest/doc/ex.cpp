//v0.3132; *Build 209 r27;
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
/= test 0 ^ \conf release;

r2:
/ \mg \lib CHRLib -> \dir CHRLib @ \lib YCLib;
/ \tr makefile @ \lib YCLib;
/ \tr @ \h YAdaptor;
/ \tr makefile @ ARM9;

r3:
/= test 1 ^ \conf release;

r4:
/ \ac @ \inh public std::function<typename GSEventTypeSpace<_tSender,
	_tEventArgs>::FuncType> @ \clt GHEvent -> protected ~ public @ \h YEvent;

r5:
/ @ \clt GEvent \h YEvent:
	/= \impl @ \mf (Add & Remove);
	/ \a \op+= -> AddUnique;
	/ \mf Add => AddUnique;
	/ protected \a unary \mf AddRaw -> public \op+=;
	/ protected \a unary \mf AddRaw => public Add;
	+ \mf \i bool Contains(const HandlerType&) const;
	+ \mft<typename _type> \i bool Contains(_type) const;

r6:
/ \inc <platform.h> -> <ydef.h> @ \h CHRDefinition;
/ namespace ystdex
	{
		typedef ::uint16_t uchar_t;
		typedef ::uint32_t fchar_t;
	} >> \h <ydef.h> ~ <platform.h>;
/ @ \h <ydef.h>:
	+ \inc \h <cstddef>;
	+ \inc \h <cstdint>;
	+ \inc \h <sys/types.h>
	/ @ \ns ystdex:
		+ {
			typedef std::uint8_t	u8;
			typedef std::uint16_t	u16;
			typedef std::uint32_t	u32;
			typedef std::uint64_t	u64;
			typedef std::int8_t		s8;
			typedef std::int16_t	s16;
			typedef std::int32_t	s32;
			typedef std::int64_t	s64;
			typedef volatile u8		vu8;
			typedef volatile u16	vu16;
			typedef volatile u32	vu32;
			typedef volatile u64	vu64;
			typedef volatile s8		vs8;
			typedef volatile s16	vs16;
			typedef volatile s32	vs32;
			typedef volatile s64	vs64;

			typedef u8 byte;

		//	typedef decltype(nullptr) nullptr_t;
			// TODO: using std::nullptr_t;
			using std::ptrdiff_t;
			using std::size_t;
			using ::ssize_t;
		}
	/ typedef ::uint16_t uchar_t -> typedef u16 uchar_t;
	/ typedef ::uint32_t fchar_t -> typedef u32 fchar_t;
/ @ \h CHRDefinition:
	/ \tr typedef s32 uint_t -> typedef ystdex::s32 uint_t;
- \inc \h <stdint.h> @ \h <platform.h>;
/ \tr using @ \impl \u @ \lib CHRLib;
/ @ \h YAdaptor:
	*= using:
		- namespace ystdex
		{
			using CHRLib::uchar_t;
			using CHRLib::uint_t;
		};
	/ @ \ns YSLib:
		+ {
			using ystdex::u8;
			using ystdex::u16;
			using ystdex::u32;
			using ystdex::u64;
			using ystdex::s8;
			using ystdex::s16;
			using ystdex::s32;
			using ystdex::s64;

			using ystdex::errno_t;
			using ystdex::nullptr_t;
			using ystdex::ptrdiff_t;
			using ystdex::size_t;
			using ystdex::ssize_t;

			using ystdex::noncopyable;
		};
		- using namespace ystdex;
	/ \tr \n @ \h (YCoreUtilities & YFont & YEvent & YGDI & YWindow
		& YUIContainer & YViewer);
	+ using namespace ystdex @ \g \ns @ \impl \u (YFileSystem & Shells
		& Scroll & YGDI & YStyle & YText);

r7:
/= test 2 ^ \conf release;

r8:
+ \mf void OnClick_btnMenuTest(TouchEventArgs&&) @ \cl ShlSetting::TFormTest
	@ \u Shells;
/ @ \h YGDIBase:
	+ \i \f Rect operator+(const Rect&, const Vec&);
	+ \i \f Rect operator-(const Rect&, const Vec&);

r9:
/ \tr \impl @ \ctor @ \cl ShlSetting::TFormTest @ \impl \u Shells;

r10:
* \impl @ \ctor @ \cl ShlSetting::TFormTest @ \impl \u Shells;

r11-r14:
* \impl @ \ctor @ \cl ShlSetting::TFormTest @ \impl \u Shells;

r15:
/ \impl @ \ctor @ \cl ShlSetting::TFormTest @ \impl \u Shells;

r16:
* \a 2 \mac PDefHOperator @ \h YFont => PDefHOperator1;
+ \cl MenuHost @ \impl \u Shells;
/ @ \h YAdaptor:
	/ \inc <ft2build.h> & {
		#include FT_FREETYPE_H
		#include FT_CACHE_H
		#include FT_BITMAP_H
		#include FT_GLYPH_H
	} >> \h YFont;

r17:
/ @ \lib YSLib:
	/ \simp @ \cl MUIContainer:
		/ \mg protected \mf bool RemoveWidget(IWidget*) -> public \mf
			bool operator-=(IWidget*);
		/ \tr \impl @ \mf bool MUIContainer::operator-=(IControl*);
	/ \dir Shell => UI;
	/ \u YGDI >> \dir Core ~ \dir Shell;
	/ \tr \inc \h \ren @ (\h (YResource & Build & DSReader & YText
		& YWidget & Scroll & YGDI & \impl \u (YDevice & YMessage & YGlobal));
	/ \tr @ Makefile;

r18:
/= test 3 ^ \conf release;

r19:
/ \u YFont:
	/ \a FTypes => FaceSet;
	/ \a FTypesIndex => FaceMap;
	/ \a FFiles => FileSet;
	/ \a FFaces => FamilySet;
	/ \a FFacesIndex => FamilyMap;
	/ @ \cl FontFamily:
		/ private \m FaceSet sTypes -> protected \m FaceSet sFaces;
		/ private \m FaceMap mTypesIndex -> protected \m FaceMap mFaces;
	/ \cl Typeface:
		/ private \m FT_Long faceIndex => face_index;
		/ private \m FT_Int cmapIndex => cmap_index;
	/ private mutable \m FT_Long nFace @ \cl FontFile => face_num;
	/ @ \cl YFontCache:
		/ private \m {
			FileSet sFiles;
			FaceSet sTypes;
			FamilySet sFaces;
			FamilyMap mFacesIndex;

			Typeface* pDefaultFace;
		}
		-> protected \m {
			FileSet sFiles;
			FaceSet sFaces;
			FamilySet sFamilies;
			FamilyMap mFamilies;

			Typeface* pDefaultFace;
		}
		/ \inh YObject -> noncopyable;
		/ \tr \impl @ \ctor;
/ \cl \n YFontCache => FontCache;
/ @ \u YUIContainer:
	/ @ \cl MUIContainer:
		/ \m typedef set<GMFocusResponser<IControl>*> FOCs => FocusContainerSet;
		/ \m FocusContainerSet sFOCSet => sFocusContainers;
		/ \m WGTs WidgetsList => sWidgets;
		/ \m typedef list<ItemType> WGTs => WidgetList;
/ @ \cl YTimer @ \u YTimer:
	/ private typedef map<u32, YTimer*> TMRs
		-> public typedef map<u32, YTimer*> TimerMap;
	/ \a private \m -> protected;
	/ \m static TimerMap Timers => mTimers;
/ typedef set<ItemType> MNUs => MenuSet @ \ cl MenuHost @ \impl \u Shells;
/ @ \clt GMFocusResponser @ \h YFocus:
	/ \m typedef set<_type*> FOs => FocusObjectSet;
	/ \m FocusObjectSet sFOs => sFocusObjects;
/ \tr @ \impl @ \cl;

r20:
/ @ \cl MenuHost @ \impl \u Shells:
	+ typedef size_t ID;
	/ typedef set<ItemType> MenuSet -> typedef map<ID, ItemType> MenuMap;
	/ \tr MenuSet sMenus -> MenuMap sMenus;
	+ typedef MenuMap::value_type ValueType;
	/ \param \tp @ \op+= -> const ValueType& ~ Menu*;
	/ \param \tp @ \op-= -> ID ~ Menu*;
	/ \tr \impl @ \dtor;
*= @ \impl \u YFont:
	+ using std::for_each;

r21:
-= "yaslivsp.hpp" @ \dir Adaptor;
/= test 4 ^ \conf release;

r22-r26:
/ @ \dir ystdex @ lib YCLib:
	+ @ \h Memory["memory.hpp"];
	/ \h Utilities["util.hpp"] => ["utility.hpp"];
	/ \tr \mac @ \h Utilities;
/ @ \h YCommon:
	- \inc "ystdex/cast.hpp";
	- \inc "ystdex/iterator.hpp";
	- \inc "ystdex/util.hpp" @ \impl \u;
	* nullptr:
		/ \a ystdex::nullptr -> nullptr;
/ \tr \inc "ystdex/util.hpp" -> <ystdex/utility.hpp> @ \impl \u YStandardExtend;
/ @ \h YReference:
	+ \inc \h <ystdex/memory.hpp>
	+ using ystdex::is_valid;
	+ using ystdex::raw;
	+ using ystdex::reset;
	+ using ystdex::share_raw;
	+ \ft<typename _type> \i bool operator==(const shared_ptr<_type>&, _type*);
	+ \ft<typename _type> \i bool operator!=(const shared_ptr<_type>&, _type*);
	+ \ft<typename _type> \i bool operator==(shared_ptr<_type>&&, _type*);
	+ \ft<typename _type> \i bool operator!=(shared_ptr<_type>&&, _type*);
	/ \a \ft (raw & reset) \exc \param \tp (GHandle | GHWeak)
		>> \ns ystdex @ \h Memory @ \dir ystdex @ \lib YCLib;
	- \tr 3 \ft raw;
	- \tr 2 \ft reset;
	- \mf (\op== & \op!=) @ \clt GHandle;
/ \a \m initializers '(nullptr)' @ \ctor \impl -> '()';
+ \inc \h (<ystdex/iterator.hpp> & <ystdex/cast.hpp>) @ \h YAdaptor;
/ @ \impl \u Shells:
	/ \simp \impl @ \mf OnDeactivated @ \cl (ShlSetting & ShlLoad & ShlExplorer)
	^ reset;
	/ \impl @ \f ReleaseShells ^ reset ~ \mf (reset @ \clt shared_ptr);
	/ \a GetGlobalImageRef => GetGlobalImage;
/ \impl @ \h YStatic;
/= test 5 ^ \conf release;
/ \impl @ \mf (YShell::DefShlProc & Global::ReleaseDevices):
	^ reset_pointer ~ \mf (reset @ \clt shared_ptr);
/ \a reset_pointer => reset;
/ \impl @ \mf bool YShell::IsActive() const;

r27:
/= test 6 ^ \conf release;


$DOING:

$relative_process:
2011-05-14:
-21.8d;
//Mercurial rev1-rev79: r3928;

/ ...


$NEXT_TODO:

b210-b324:
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
	/ "library CHRLib moved and merged to library YCLib",
	/ $design "protected function inheritance in class template %GHEvent",
	/ "using directive of namespace %ystdex in YSLib",
	+ "lost %Rect operations",
	* "strict ISO C++2003 code compatibility" $=
	(
		^ "fixed parameter macros in \"yfont.h\""
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

