//v0.2959; *Build 189 r40;
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
r1:
/ @@ \impl \u YGDI:
	+ \ft BlitCore;
	/ \ft blit_for => \stt;
	/ \impl @@ \f 'Blit?' ^ BlitCore;

r2:
/ @@ \impl \u YGDI:
	/ @@ \un \ns:
		* \ft void blit_line(BitmapPtr, ConstBitmapPtr, int)
			-> void blit_line(BitmapPtr&, ConstBitmapPtr&, int);
	* \impl @@ \ft BlitCore;

r3:
/ @@ \impl \u YGDI:
	/ \ft BlitCore => \h;

r4:
/ @@ \u YGDI:
	+ typedef std::pair<ConstBitmapPtr, const u8*> IteratorPair;
	+ \i \f IteratorPair operator+(const IteratorPair&, std::ptrdiff_t);
	+ \i \f IteratorPair& operator+=(IteratorPair&, std::ptrdiff_t);
	+ \i \f IteratorPair& operator++(IteratorPair&);
	/ \f with \tp const u8* \param ^ IteratorPair;
	/ \impl @@ \mf void Flush(BitmapPtr, const Size&,
		const Point&, const Point&, const Size&, Rotation) const
		& \mf void BlitTo(BitmapPtr, const Size&, const Point&, const Point&,
		const Size&, Rotation) const @@ \cl BitmapBufferEx ^ IteratorPair;

r5:
/ @@ \impl \u YGDI:
	+ \ft BlitCore;
	/ \ft blit2_for => \stt;
	/ \impl @@ \f 'Blit2?' ^ BlitCore;
	/ \ft blitAlpha_for => \stt;
	/ \impl @@ \f 'BlitAlpha?' ^ BlitCore;

r6:
/ @@ \impl \u YGDI:
	/ \mac 'BLT*' & (\stt 'blit*' \un \ns) & \ft 'Blit*' => \h @@ !\un \ns;
	- \a \f 'Blit*';
	/ \impl @@ 2 \f CopyTo;
	/ \impl @@ \mf void Flush(BitmapPtr, const Size&,
		const Point&, const Point&, const Size&, Rotation) const
		& \mf void BlitTo(BitmapPtr, const Size&, const Point&, const Point&,
		const Size&, Rotation) const @@ \cl BitmapBufferEx ^ BlitCore;
	+ \i @@ \f blit_line<false>;
	/ \a blit_line => BlitLine;
	/ \a blit_for => BlitFor;
	/ \a blit2_for => BlitTransparentFor;
	/ \a blitAlpha_for => BlitBlendFor;
	/ \a BlitCore => Blit;
	+ several explicit instantiation @@ \impl \u
		to prevent the linker complaint;

r7:
/ @@ \h YGDI:
	/ \mac 'BLT_*' -> const u32 'BLT_*';
	/ + \ft<typename _tOut, typename _tIn> biltAlphaPoint;
	/ \i \f biltAlphaPoint -> template<> \i \f
		biltAlphaPointbiltAlphaPoint(u16*, std::pair<const u16*, const u8*>);
/= test 1;

r8:
/ @@ \u YGDI:
	/ \a BlitFor => BlitLoop;
	/ \a BlitTransparentFor => BlitTransparentLoop;
	/ \a BlitBlendFor => BlitBlendLoop;
	/ \a \smf Loop => Blit;

r9-r10:
/ \simp \a \impl @@ outermost for loop @@ \impl
	@@ (RectTransfomer::operator() & BlitLine<false> & BlitLoop::Blit
	& BlitTransparentLoop::Blit & BlitBlendLoop::Blit) @@ \h YGDI;
	
r11:
/ @@ \h YGDI:
	+ \i \f IteratorPair operator-(const IteratorPair&, std::ptrdiff_t);
	+ \i \f IteratorPair& operator-=(IteratorPair&, std::ptrdiff_t);
	+ \i \f IteratorPair& operator--(IteratorPair&);
	/ \impl @@ \ft Blit;
	/ \a 4 \smf Blit @@ \stt '*Loop' -> !\s \mf operator();

r12:
/ @@ \lib YCLib:
	+ \dir "ystdex";
	/ file "ycast.hpp" >> \dir "ystdex";
	/ @@ \dir "ystdex":
		/ \h YCast["ycast.hpp"] => Cast "cast.hpp";
		/ @@ \h Cast:
			/ \tr \inc;
			* \a INCLUDED_XWRAPPER_HPP_ => INCLUDED_YSTDEX_CAST_HPP_;
		+ \h Iterator["iterator.hpp"];
		+ \h Utilities["util.hpp"];
	/ \tr \inc @@ \h YCommon;
/ @@ \h YCoreUtilities:
	/ \stt (deref_op & const_deref_op & deref_comp & deref_str_comp)
		& \ft (erase_all & erase_all_if & search_map)
		>> \h YCLib::Utilities;
	- \inc <functional>;
/ \tr \impl @@ (\ft GetEvent & \mf GEvent::operator-=) @@ \h YEvent;
/ \tr \decl @@ \h YFont;
/ \tr @@ \impl @@ \mf YGUIShell::RemoveAll @@ \impl \u YComponent;

r13:
/ @@ \ns ystdex @@ \h YCLib::Iterator:
	+ \clt pair_iterator;
/ @@ \h YGDI:
	/ typedef std::pair<ConstBitmapPtr, const u8*> IteratorPair
		-> typedef ystdex::pair_iterator<ConstBitmapPtr, const u8*>
		IteratorPair;
	/ \impl @@ \mf with IteratorPair iteration;
	- \i \f IteratorPair& operator++(IteratorPair&);
	- \i \f IteratorPair& operator--(IteratorPair&);
	- \i \f IteratorPair operator+(const IteratorPair&, std::ptrdiff_t);
	- \i \f IteratorPair operator-(const IteratorPair&, std::ptrdiff_t);
	- \i \f IteratorPair& operator+=(IteratorPair&, std::ptrdiff_t);
	- \i \f IteratorPair& operator-=(IteratorPair&, std::ptrdiff_t);

r14:
/ \impl @@ \f PrintCharEx @@ \impl \u YText;

r15-r16:
/= test 2;

r17:
* 4 \as @@ 4 \f BlitScale @@ \impl \u YGDI;

r18:
/ \mg 4 \as @@ \f BlitScale >> \f BlitPosition @@ \impl \u YGDI;

r19:
/ @@ \un \ns @@ \impl \u YGDI:
	/ \mg \i \f (blit_min_x & blit_min_y) -> blit_min;
	/ \mg \i \f (blit_max_x & blit_max_y) -> blit_max;
	/ \param order @@ \i \f blit_max;

r20:
/  @@ \u YGDI:
	/ \f void BlitPosition() -> bool BlitBounds();
	/ (\decl & \impl) @@ \ft BlitScale;
	/ \impl @@ \ft Blit;

r21-r35:
/ test 3;
/ @@ \impl \u YText:
	+ \stt BlitTextLoop @@ \un \ns;
	/ \impl @@ \f PrintCharEx ^ BlitTextLoop;
	- \f PrintChar;
	/ \f PrintCharEx => PrintChar;
	/ \s \o Alpha_Threshold -> !\s \o BLT_TEXT_ALPHA_THRESHOLD @@ \un \ns;
	- \mac INVISIBLE_CHAR;

r36:
/ \tp @@ \o (BLT_ALPHA_BITS & 'BLT_THRESHOLD?') -> \c u8 ~ \c u32 @@ \h YGDI;

r37:
/ @@ \h YGDI:
	/ + \i \f blitAlphaBlend(u32, u32, u8);
	/ \ft<> \i void blitAlpha(u16*, IteratorPair)
		-> \ft<> \i void blitAlpha(PixelType*, IteratorPair);

r38:
+ \stt<typename _type, typename _tIterator = _type*> struct pseudo_iterator 
	@@ \h YCLib::Iterator;
/ @@ \h YGDI:
	+ typedef ystdex::pair_iterator<ystdex::pseudo_iterator<const PixelType>,
		const u8*> MonoIteratorPair;
	+ \ft<> \i void biltAlphaPoint(PixelType*, MonoIteratorPair);
	/ \mf void operator()(int, int, BitmapPtr, IteratorPair, int, int)
		@@ \stt BlitBlendLoop -> \mf \t<typename _tIn>
		void operator()(int, int, BitmapPtr, _tIn, int, int);

r39:
/ ((\stt xcrease_t & delta_assignment_t) & \ft (xcrease & delta_assignment))
	@@ \ns YSLib @@ \h YCoreUtilities >> \ns ystdex @@ \h YCLib::Utilities;
/ ystdex::pair_iterator<BitmapPtr, u8*> @@ \impl \u YText
	-> ystdex::pair_iterator<ConstBitmapPtr, const u8*>;

r40:
/= test 3 ^ \conf release;


$DOING:

relative process:
2011-01-29:
-21.9d;

/ ...


$NEXT_TODO:

b190-b240:
/ scroll bars @@ listbox \cl;
/ fully \impl \u DSReader;
	* moved text after setting lnGap;
* non-ASCII character path error in FAT16;
/ non-ordinary operator usage in \clt GSequenceViewer @@ \h YComponent;

b241-b648:
/ improve efficiency @@ \ft polymorphic_crosscast @@ \h YCast;
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

