//v0.2959; *Build 188 r21;
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
/= test 1;

r4:
/ \tr \impl @@ \mf ShlLoad::OnActivated @@ \impl \u Shells;

r5:
/ test 2;

r6-r7:
/ @@ \u YGDI:
	* lost response while moving touched button point downward out of window:
		* \a std::ptrdiff_t -> int;
		* \a std::size_t 'delta_*' -> int 'delta_*';		
		*\a std::size_t x -> int x;
		* \a std::size_t y -> int y;
		/ - \a \as;
	/ \a sa => src_alpha;
	/ blitMaxX => blit_max_x;
	/ blitMinX => blit_min_x;
	/ blitMaxY => blit_max_y;
	/ blitMinY => blit_min_y;
	/ blitLine => blit_line;
	/ \impl @@ \f template<> void
		blit_line<false>(BitmapPtr, ConstBitmapPtr, int);
	/ \a (!\tp Size&) dc => dst_iter;
	/ \a (!\tp Size&) sc => src_iter;
	/ \a '*For' => '*_for';

r8-r9:
/= test 3;

r10:
- DLDI patch commands @@ Makefile;

r11:
* invalid listbox input when the list not beginning from the top:
	/ \impl @@ \mf Contains @@ \clt GSequenceViewer @@ \h YComponent;
	/ @@ \cl YSimpleListBox @@ \impl \u YGUIComponent:
		/ \impl @@ \mf ViewerType::IndexType CheckPoint(SPOS, SPOS);
		/ \impl @@ \mf void SetSelected(ViewerType::IndexType);

r12-r14:
/= test 4;

r15:
/ \exp \ctor YListBox(const Rect& = Rect::Empty, IUIBox* = NULL,
	GHWeak<Drawing::TextRegion> = NULL, ListType* = NULL)
	@@ \cl YListBox @@ \u YGUIComponent
	-> YListBox(const Rect& = Rect::Empty, IUIBox* = NULL,
	GHWeak<Drawing::TextRegion> = NULL, GHWeak<ListType> = NULL);

r16:
/ @@ \clt GEvent @@ \h YEvent:
	+ copy \ctor;
	+ GEvent& operator=(const GEvent&);
	+ \mf void Swap(const GEvent&) ythrow();
	^ this @@ \a \impl @@ \mf;
/ @@ \cl YListBox @@ \u YGUIComponent:
	+ \m DefEvent(HIndexEvent, Selected);
	+ \m DefEvent(HIndexEvent, Confirmed);
	/ \impl @@ \ctor;

r17:
/ @@ \h YEvent:
	+ \mac EventT;
	+ \mac DefEventRef;
	/ \simp \impl @@ 5 \mac ^ \mac EventT;
/ \a DefEvent => DeclEvent;
/ \a DefEventRef => DeclEventRef;

r18:
/ @@ \h Base:
	+ \mac DefMutableGetter;
	+ \mac DefMutableGetterBase;
	+ \mac DefMutableGetterMember;
	+ \mac DefMutablePredicate;
	+ \mac DefMutablePredicateBase;
	+ \mac DefMutablePredicateMember;
	- \mac DefStaticPredicate;
	- \mac DefStaticPredicateBase;
	- \mac DefStaticPredicate;
	- \mac DefStaticPredicateBase
/ @@ \h YEvent:
	+ \mac DefMutableEventGetter;
	+ \mac DefMutableEventGetterBase;
	+ \mac DefMutableEventGetterMember;
/ \a DefStaticGetter => static DefMutableGetter;
/ \simp @@ \mf GetFocusingPtr @@ \cl (ATrack & AScrollBar & YListBox
	& YUIContainer & AFrameWindow) ^ DefMutableGetterBase;

r19:
/ @@ \u YGUIComponent:
	/ \m pwTextRegion => wpTextRegion @@ \cl YSimpleListBox;
+ \u YUIContainer @@ \dir Shell;
+ \u YLabel @@ \dir Shell;
/ @@ \ns Widgets @@ \u YWidget:
	/= \tr \inc @@ \cl YLabel;
	+ \mf void DrawText(Widget&, const Point&) @@ \u MLabel;
	/ \impl @@ \mf PaintText;
	/ \cl MUIContainer >> \u YUIContainer;
	/ \cl YUIContainer >> \u YUIContainer;
	/ \cl MLabel >> \u YLabel;
	/ \cl YLabel >> \u YLabel;
	- \inc \h YText @@ \h;
	- \inc \h YResource @@ \h;
	- \inc \h YFocus @@ \h;
	- using Drawing::YImage;
	/ \in IUIBox >> \u YUIContainer;
	/ \in IUIContainer >> \u YUIContainer;
	+ \pre \decl \cl IUIBox;
	+ \pre \decl \in IUIContainer;
	/ \a (\f & \ft) ('Fetch*' & 'MoveTo*' & Fill)
		& (\f GetContainersListFrom @@ \un \ns) >> \u YUIContainer;
	+ \inc \h YGDI @@ \h;
- \inc \h YWidget @@ \h YWindow;
+ \inc \h YLabel @@ \h YGUIComponent;
* @@ \h YFocus:
	/ \mac INCLUDED_YMODULE_H_ => \mac INCLUDED_YFOCUS_H_;
	/ \inc \h YObject -> \inc \h YComponent;
+ \inc \h (YResource & YUIContainer) @@ \h YGUIComponent;
/ @@ \impl \u YShell:
	/ \inc \h YGUI -> \inc \h YDesktop;
	- using namespace Components;
	- using namespace Components::Controls;
+ \inc \h YFocus @@ \h YControl;

r20:
/ @@ \u YLabel:
	+ \cl MTextList;
	/ @@ \cl MLabel:
		/ \mf void MLabel::DrawText(Widgets&, const Point&)
			-> !\m \f DrawText(TextRegion&, const Graphics&, const Point&,
				const Size&, const String&) @@ \u YText;
		/ \mf void PaintText(Widget&, const Point&)
			-> bool PaintText(Widget&, const Graphics&, const Point&);
	/ \impl @@ \mf DrawForeground @@ \cl YLabel;
/ \a pTr_ => wpTr_;
+ using Drawing::Graphics @@ \h YWidget;
+ \f const Graphics& FetchContext(IWidget&) @@ \u YUIContainer;
/ \tr \impl @@ \mf void YButton::DrawForeground();

r21:
/= test 5;


$DOING:

relative process:
2011-01-23:
-20.7d;

/ ...


$NEXT_TODO:

b189-b240:
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

