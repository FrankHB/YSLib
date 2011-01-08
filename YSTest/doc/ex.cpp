//v 0.2944; *Build 182 r23;
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
r1:
/ @@ \ns platform @@ \impl \u YCommon:
	- \o void* const main_ram(reinterpret_cast<void*>(0x02000000));
	/ @@ \un \ns:
		+ \i \f bool dma_out_of_range(u32);
		+ \f ystdex::errno_t safe_dma_fill(void *dst, int v, std::size_t size);
		+ \f ystdex::errno_t safe_dma_copy(void *dst, const void *src,
			std::size_t size);
	* \impl @@ \f (mmbset & mmbcpy);

r2:
* \i \f void ScreenSychronize(PixelType*, const PixelType*) @@ \ns platform
	@@ \u YCommon -> !\i \f void ScreenSychronize(PixelType*, const PixelType*);

r3:
/= \tr @@ arm7 "main.cpp";

r4:
/ 3 \m Key up, down, held @@ \st KeyInfo @@ \h YCommon => Key
	Up, Down, Held;

r5:
/ @@ \cl Messaging::InputContext @@ \u YGlobal:
	/ \m platform::KeysInfo* Key -> Runtime::KeysInfo Key;
	/ !\exp \ctor InputContext(Runtime::KeysInfo*, const Point&)
		-> \exp \ctor InputContext(Runtime::KeysInfo, const Point&);
	/ \impl @@ \op==;
/ \tr \impl @@ \f void WaitForGUIInput() @@ \un \ns @@ \impl \u YGlobal;
/ \tr \impl @@ \f void ResponseInput(const Message&) @@ \impl \u Shell_DS;

r6:
/ \impl @@ \f void WaitForGUIInput() @@ \un \ns @@ \impl \u YGlobal;

r7:
/ \a blitScale => BlitScale;
/ \a FillPixel => PixelFiller;
/ \a FillSeq => FillPixel;
/ \a transSeq => SequenceTransformer;
/ \a transVLine => VerticalLineTransfomer;
/ \a transRect => RectTransfomer;
/ \a FillVLine => FillVerticalLine;
/ \a blit => Blit;
/ \a blitH => BlitH;
/ \a blitV => BlitV;
/ \a blitU => BlitU;
/ \a blit2 => Blit2;
/ \a blit2H => Blit2H;
/ \a blit2V => Blit2V;
/ \a blit2U => Blit2U;
/ \a blitAlpha => BlitAlpha;
/ \a blitAlphaH => BlitAlphaH;
/ \a blitAlphaV => BlitAlphaV;
/ \a blitAlphaU => BlitAlphaU;

r8:
/ @@ \h YReference:
	/ @@ \ns Design::Policies:
		+ \clt DeleteSingle_Debug;
		+ \clt DeleteArray_Debug;
	/ \decl @@ \clt (GHStrong & GHWeak) ^ DeleteSingle_Debug ~ DeleteSingle;
	* \a @@ \ns Design => \ns YSLib;

r9:
/ \impl @@ \cl GStaticCache ^ ((ynew ~ new) & (safe_delete_obj_debug
	~ safe_delete_obj_ndebug);
/ \a YDelete_Debug => YReset_Debug;
* + 4 \i \ft YReset_Debug overloading version @@ \h YReference;

r10-r15:
/ test 1;
	* \impl @@ \i \ft<typename _type> bool YReset_Debug(GHWeak<_type>&) ythrow()
		@@ \h YReference;

r16:
/ @@ \cl MemoryList \u YNew:
	+ \m typedef std::list<std::pair<const void*, BlockInfo>,
		__gnu_cxx::malloc_allocator<std::pair<const void*, BlockInfo> > >
		ListType;
	+ \m ListType DuplicateDeletedBlocks;
	/ \m MapType m_map => Blocks;
	/ \impl @@ \ctor;
	/ !\s \mf Print -> \smf; 
	+ \smf void Print(ListType::const_iterator, std::FILE*);
	+ \mf void PrintAllDuplicate(std::FILE*);
	/ \impl @@ \mf Unregister;
/ \impl @@ \f void OnExit_DebugMemory() @@ \impl \u YGlobal;
+ \inc <list> @@ \h YNew;

r17:
/ \tr \impl @@ \f void OnExit_DebugMemory() @@ \impl \u YGlobal;
* \impl @@ \YImage* NewScrImage(PPDRAW, BitmapPtr) @@ \impl \u YShellHelper;
* 5 new -> ynew @@ \decl @@ 5 \ctor @@ \h (YWindow @@ YForm);

r18-r19:
/ \tr \impl @@ \f void OnExit_DebugMemory() @@ \impl \u YGlobal;

r20:
* 3 new -> ynew @@ \impl @@ 3 \ctor @@ \cl FileList @@ \impl \u YFileSystem;

r21-r22:
/= test 2;

r23:
/ @@ \u YShellHelper:
	* \f YImage* NewScrImage(PPDRAW, BitmapPtr)
		-> GHStrong<YImage> NewScrImage(PPDRAW, BitmapPtr);
	* \i \f YImage* NewScrImage(ConstBitmapPtr)
		-> GHStrong<YImage> NewScrImage(ConstBitmapPtr);


$DOING:

relative process:
2011-01-08:
-20.7d;

/ ...


$NEXT_TODO:

b182-b215:
* screen output polluted @@ real DS since b13x or before;
* fatal \err @@ since b178 when opening closed lid @@ real DS:
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
]
/ impl 'real' RTC;
/ improve efficiency @@ \tf polymorphic_crosscast @@ \h YCast;
/ scroll bars @@ listbox;
/ fully \impl \u DSReader;
	* moved text after setting lnGap;
* non-ASCII character path error in FAT16;
/ non-ordinary operator used in \clt GSequenceViewer @@ \h YComponent;

r196-r288:
* alpha blending platform independence;
+ \impl loading pictures;
+ \impl style on widgets;
+ \impl general Blit algorithm;
/ user-defined bitmap buffer @@ \cl YDesktop;
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
b180:
* ramdom screen output polluted @@ real machine since b177;
* fatal \err before b170(after b158) when touching on the ListBox
	& key L being released:
[
b177
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 0201f484
GetTouchedVisualControlPtr
ygui.cpp:215
??
ygui.cpp:325

b180 r56:
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 0201E8BC
YSLib::Components::Controls::(anonymous namespace)::TryLeave(YSLib::Components::
Controls::IVisualControl&, YSLib::Components::Controls::TouchEventArgs&)
ygui.cpp:176
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 020D7D6C
__dynamic_cast
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

