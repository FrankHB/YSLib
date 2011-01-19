//v0.2959; *Build 187 r37;
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
\sf ::= non-member static functions
\sif ::= non-member inline static functions
\simp ::= simplified
\smf ::= static member functions
\sm ::= static member
\scm ::= static const member
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
\u YWidget
{
	\in IWidget;
	\in IUIBox;
	\cl Widget;
	\cl YUIContainer;
	\cl YLabel;
}
\u YControl
{
	\in IControl;
	\in IVisualControl;
	\cl MVisualControl;
	\cl YControl;
	\cl AVisualControl;
	\cl YVisualControl;
}
\u YGUIComponent
{
	\cl AButton;
	\cl YThumb;
	\cl YButton;
	\cl ATrack;
	\cl YHorizontalTrack;
	\cl YVerticalTrack;
	\cl MScrollBar;
	\cl AScrollBar;
	\cl YHorizontalScrollBar;
	\cl YVerticalScrollBar;
	\cl YListBox;
	\cl YFileBox;
}
\u YWindow
{
	\in IWindow;
	\cl MWindow;
	\cl AWindow;
	\cl YFrameWindow;
}

$DONE:
r1-r3:
/ \lib YSLib >> VC++ \proj YSLib;
/ YSTest \proj \conf;
/ @@ arm9 Makefile;
/ \tr \inc @@ \h DSReader.h;
/ \tr \inc @@ \h Build;
/ \tr \inc @@ \h Platform::DS;
/ \tr \inc @@ \impl \u GBKEX;
/ \tr \inc @@ \h YAdaptor;
/ \h @@ \lib (FreeType2 & Loki & AGG) >> "common/include" ~ "arm9/include";
/ \tr \inc @@ \h YShellHelper;
* \tr \inc @@ \h YFocus;

r4-r5:
/ ARM7 >> VC++ \proj YSLib;
* \proj command line(only release version was run);

r6-r7:
/ ARM9 >> VC++ \proj YSLib;
/ (top & ARM9) Makefile @@ \proj YSTest;
/ \tr \inc @@ \h YAdaptor;
/ common (\inc & data) >> \proj YSTest \dir ~ \dir "YSTest\common";
/ Makefile @@ \lib (CHRLib & YCLib & YSLib);

r8:
* \tr @@ YSTest \proj Makefile;

r9:
/= test 1;

r10:
/ \tr \impl @@ \impl \u YFile;

r11-r12:
* '-g' option @@ release mode @@ 5 Makefile;
/ ^ '-O3' & arm specified options compiled "libloki.a" @@ \proj YSTest;

r13:
/ \f BackMessage => BackMessageQueue @@ \u YShell;

r14:
/ @@ \h YCoreUtilities:
	+ \stt (xcrease_t & delta_assignment_t);
	+ \ft<bool> (xcrease & delta_assignment); 
/ @@ \impl \u YGDI:
	/= \tr \def order;
	/ @@ \un \ns:
		/ \f (blitAlphaFor & blitAlphaForU) \mg -> \ft<bool> blitAlphaFor
			^ \ft (xcrease & delta_assignment);
		/ \tr \impl @@ 4 \f BlitAlpha*;

r15-r21:
/ @@ \u YGDI:
 	/ \f BlitScale => BlitPositon;
	+ \tf<bool, bool> BlitScale;
	/ \impl @@ \a blit \f;
	/ \a maxX => max_x;
	/ \a maxY => max_y;
	/ \a deltaX => delta_x;
	/ \a deltaY => delta_y;
	/ \a minX => min_x;
	/ \a minY => min_y;
	/ \mf \op () @@ \stt RectTransfomer;
	/ \a srcOffset => src_off;
	/ \a srcA => src_alpha;
	/ @@ \un \ns:
		* \f '*H' & '*V';
		/ 4 \f blit2For \mg -> 2 \ft<bool> blit2For
			^ \ft (xcrease & delta_assignment);

r22:
/= test 2 ^ \conf release;

r23:
/ @@ \u YGDI:
	+ \as @@ 4 \f BlitScale;
	/ \a int& 'delta_*' -> std::size& 'delta_*';
	/ \a int 'delta_*' -> std::size 'delta_*';
	/ \tr \impl @@ \f @@ \un \ns;
	+ \as @@ \mft operator() @@ \st RectTransfomer;
	/ \a 'int dInc, int sInc' -> 'std::ptrdiff_t dInc, std::ptrdiff_t sInc';
	/ \a 'for(int y(0); y < delta_y; ++y)'
		-> 'for(std::size_t y(0); y < delta_y; ++y)';
	/ \a 'for(int x(0); x < delta_x; ++x)'
		-> 'for(std::size_t x(0); x < delta_x; ++x)';
	/ \tp @@ \a '*_off' -> std::ptrdiff_t ~ std::size_t;
	/ \tp @@ \a '*_off' -> std::ptrdiff_t& ~ std::size_t&;
	/ \a 'vmax<int>' -> 'vmax<std::ptrdiff_t>';
	/ \a 'vmin<int>' -> 'vmin<std::ptrdiff_t>';
	/ \a sInc => src_inc;
	/ \a dInc => dst_inc;

r24:
/ @@ \impl \u YGDI:
	/ @@ \un \ns:
		+ \ft blitLine;
		/ 2 \f blitFor \mg -> 1 \ft<bool> blitFor
			^ \ft (blitLine & delta_assignment);;
	/ \tr \impl @@ 4 \f 'Blit?';

r25-r29:
/= test 3;

r30:
* \ft YReset @@ \h YReference:
	- #define YDelete_Debug YReset;
	/ \ft YReset => YReset_ndebug;
	+ #define YReset YReset_ndebug @@ !defined \mac YSL_USE_MEMORY_DEBUG;
	+ #define YReset YReset_Debug @@ defined \mac YSL_USE_MEMORY_DEBUG;
/ \a YReset_Debug => YReset_debug;
/ @@ \h YCoreUtilities:
	- typedef delete_obj delete_obj_ndebug;
	- typedef safe_delete_obj safe_delete_obj_ndebug;
	+ #define delete_obj delete_obj_ndebug
		@@ !defined \mac YSL_USE_MEMORY_DEBUG;
	+ #define safe_delete_obj safe_delete_obj_ndebug
		@@ !defined \mac YSL_USE_MEMORY_DEBUG;

r31:
/= test 4;

r32:
* \impl @@ \i \ft<typename _type>
	bool YReset_debug(GHHandle<_type>& h) ythrow() @@ \h YReference;

r33:
* @@ \h YCoreUtilities:
	/ \stt delete_obj => delete_obj_ndebug;
	/ \stt safe_delete_obj => safe_delete_obj_ndebug;
* impl @@ \clt GStaticCache @@ \h YObject;

r34:
/ @@ \h YReference:
	- \clt DeleteSingle_Debug;
	- \clt DeleteArray_Debug;
	/ \t \param DeleteSingle_Debug -> DeleteSingle @@ \clt (GHWeak & GHStrong);
/ @@ \h YShellHelper:
	/ \a ynew -> new;

r35:
* \tr \impl @@ 3 \ctor @@ \cl FileList;

r36:
/ test 5 ^ \conf release;
	* \tr \mac @@ defined \mac YSL_USE_MEMORY_DEBUG @@ \h YReference;

r37:
/= test 6 ^ \conf debug;

$DOING:

relative process:
2011-01-19:
-20.1d;

/ ...


$NEXT_TODO:

b188-b240:
* invalid listbox click;
/ scroll bars @@ listbox \cl;
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
/ fully \impl \u DSReader;
	* moved text after setting lnGap;
* non-ASCII character path error in FAT16;
/ improve efficiency @@ \tf polymorphic_crosscast @@ \h YCast;
/ non-ordinary operator usage in \clt GSequenceViewer @@ \h YComponent;

r241-r324:
+ data config;
/ impl 'real' RTC;
+ correct DMA (copy & fill);
* platform-independence @@ alpha blending:
	+ \impl general Blit algorithm;
/ user-defined bitmap buffer @@ \cl YDesktop;
+ (compressing & decompressing) @@ gfx copying;
+ \impl loading pictures;


$LOW_PRIOR_TODO:
r325-r768:
+ \impl styles @@ widgets;
/ general component operations:
	/ serialization;
	/ designer;
/ database interface;


$NOTHING_TODO:
* fatal \err @@ b16x:
[
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
	-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 02055838
	ftc_snode_weight
	ftcsbits.c:271
]


$LAST_SUCCESSFULLY_FIXED:
b185 r18:
* fatal \err @@ since b177 when opening closed lid @@ real DS:
[
b178:
(pc: 02013000, addr: FFFFFFFF);

b180 r56:
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 020BBC5C
YSLib::Messaging::Message::operator=(YSLib::Messaging::Message const&)
ysmsg.h:55

b180 r57:
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 020A0BB4
Loki::SmartPtr<YSLib::Shells::YShell, YSLib::Design::Policies::GeneralCastRefCou
nted, Loki::DisallowConversion, Loki::AssertCheck, Loki::DefaultSPStorage, Loki:
:DontPropagateConst>::operator=(Loki::SmartPtr<YSLib::Shells::YShell, YSLib::Des
ign::Policies::GeneralCastRefCounted, Loki::DisallowConversion, Loki::AssertChec
k, Loki::DefaultSPStorage, Loki::DontPropagateConst> const&)
SmartPtr.h:1217

b181:
(pc: 02138EA0, addr: 02138EA0);
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 02138EA0
YSLib::(anonymous namespace)::WaitForGUIInput()::InputMessage
crtstuff.c:0

b184:
(pc: 020AD108, addr: 020AD108);
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 020AD108
SmartPtr
SmartPtr.h:1176
(pc: 020909F8, addr: 020909F8);
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 020909F8
__clzsi2
crtstuff.c:0
]

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

GUI characteristics needed:
Icons;
Other controls.

Other stuff to be considered to append:
Design by contract: DbC for C/C++, GNU nana.

*/
//---- temp code;
/*

/*	static YTimer Timer(1250);

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

