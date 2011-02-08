//v0.2959; *Build 190 r38;
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
/ @@ \h Base:
	+ \mac DeclBasedInterface1;
	+ \mac DeclBasedInterface2;
	+ \mac DeclBasedInterface3;
	+ \mac ImplI1;
	+ \mac ImplI2;
	+ \mac ImplI3;
	+ \mac ImplI4;
	+ \mac ImplA1;
	+ \mac ImplA2;
	+ \mac ImplA3;
	+ \mac ImplA4;
	- \mac ImplBodyBaseVoid;
	- \mac ImplBodyMemberVoid;
	- \mac ImplExpr;
	/ \a _paralist -> _paramlist;
	+ \mac PDefH1;
	+ \mac PDefH2;
	+ \mac PDefH3;
	+ \mac PDefH4;
	+ \mac PDefHOperator1;
	+ \mac PDefHOperator2;
	+ \mac PDefHOperator3;
	+ \mac PDefHOperator4;
	+ \mac ImplBodyBase1;
	+ \mac ImplBodyBase2;
	+ \mac ImplBodyBase3;
	+ \mac ImplBodyBase4;
	+ \mac ImplBodyMember1;
	+ \mac ImplBodyMember2;
	+ \mac ImplBodyMember3;
	+ \mac ImplBodyMember4;
	+ \mac PDefH0;
	+ \mac PDefHOperator0;
	+ \mac ImplBodyBase0;
	+ \mac ImplBodyMember0;
/ \a DeclBasedInterface -> DeclBasedInterface1 \ex \h Base;
/ \a ImplI -> ImplI1 \ex \h Base;
/ \a ImplA -> ImplA1 \ex \h Base;
/ \a ImplBodyBaseVoid -> ImplBodyBase;
/ \a ImplBodyMemberVoid -> ImplBodyMember;
/ \a ImplExpr -> ImplRet;
/ \a PDefH -> PDefH1 \ex \h Base;
/ \a PDefHOperator -> PDefHOperator1 \ex \h Base;
/ \a ImplBodyBase -> ImplBodyBase1 \ex \h Base;
/ \a ImplBodyMember -> ImplBodyMember1 \ex \h Base;
* \mac \param number mismatch \mac \n;
/ @@ \u YFocus:
	/ \ret \tp @@ \mf ClearFocusingPtr @@ \clt GMFocusResponser
		-> void ~ bool;
	/ \tr \impl @@ (\mf & \mft) AFocusRequester::ReleaseFocus;
/ \test 1:
	/ try '-pedantic' @@ \mac CFLAG @@ YSLib Makefile;
	* \rem \mac YSL_TL @@ \h YAdaptor;
	* comma at end of enumerator list;
		/ \tr @@ \lib CHRLib;
		/ \impl @@ \mac DefColorH @@ \h YCommon @@ \lib YCLib;
		/ \tr @@ \h YFont;

r2-r9:
/= test 2 ^ \conf release;

r10:
* undid makefile to increase file size;

r11:
/= test 3 ^ \conf release;

r12:
/ @@ \cl MDualScreenReader @@ \u DSReader:
	* fake memory leak @@ \impl @@ \ctor;
	/ \m pTextRegionUp => spTextRegionUp;
	/ \m pTextRegionDn => spTextRegionDn;

r13:
/ \a _fCompare => _gfCompare @@ \h YCLib::Utilities;
/ @@ \cl TextRegion @@ \u YText:
	/ \mf void Move(s16) -> void Scroll(std::ptrdiff_t);
	/ \mf void Move(s16, SDST) -> void Scroll(std::ptrdiff_t, SDST);

r14:
/ @@ \u YText:
	/ \mf SDST TextRegion::GetBufferHeightResized() const
		-> !\m \f SDST FetchResizedBufferHeight(const TextState&, SDST);
	/ \mf SDST GetMarginResized() const
		-> !\m \f SDST FetchResizedMargin(const TextState&, SDST);
/ \tr \impl @@ \mf (LineUp & LineDn) @@ \cl MDualScreenReader
	@@ \impl \u DSReader;

r15:
/ @@ \u YText:
	/ @@ \cl TextRegion:
		- \i \f GetBufWidthN;
		- \i \f GetBufHeightN;
		/ \tr \impl @@ \mf (GetLnN & GetLnNEx);
	/ \tr \impl @@ \ft (GetPreviousLinePtr & GetNextLinePtr);

r16:
/ @@ \u YText:
	/ \mf void PutNewline() -> \cl TextState ~ \cl TextRegion;
	/ @@ \cl TextRegion:
		/ \tr \impl @@ 3 \ctor;
		/ \mf u8 PutChar(fchar_t) -> !\m \ft<class _tBuffer>
			u8 TextRegion::PutChar(_tBuffer&, TextState&, fchar_t);
		/ \mft<typename _tOut> _tOut PutLine(_tOut)
			-> !\m \ft<class _tOut, typename _tBuffer>
			_tOut PutLine(_tBuffer&, TextState&, _tOut);
		/ \mft<typename _tOut> _tOut PutLine(_tOut, _tOut, _tChar = '\0')
			-> !\m \ft<class _tOut, typename _tChar, typename _tBuffer>
			_tOut PutLine(_tBuffer&, TextState&, _tOut, _tOut, _tChar = '\0');
		/ \i \mf String::size_type PutLine(const String&)
			-> !\m \i \ft<class _tBuffer>
			String::size_type PutLine(_tBuffer&, TextState&, const String&);
		/ \mft template<typename _tOut> _tOut PutString(_tOut)
			-> !\m \ft<class _tOut, typename _tBuffer> _tOut
			PutString(_tBuffer&, TextState&, _tOut);
		/ \mft template<typename _tOut, typename _tChar> _tOut
			PutString(_tOut, _tOut, _tChar = '\0')
			-> !\m \ft<typename _tOut, typename _tChar, class _tBuffer>  _tOut
			PutString(_tBuffer&, TextState&, _tOut, _tOut, _tChar = '\0');
		/ \i \mf String::size_type PutString(const String&)
			-> !\m \i \ft<class _tBuffer>
			String::size_type PutString(_tBuffer&, TextState&, const String&);
		/ \mf SPOS GetLineLast() const
			-> !\m \f SPOS FetchLastLineBasePosition(const TextState&, SDST);
	/ \tr \impl @@ \f void DrawText(TextRegion&, const Graphics&, const Point&,
		const Size&, const String&);
/ @@ \cl MDualScreenReader @@ \impl \u DSReader:
	/ \tr \impl @@ \mf void FillText();
	/ \tr \impl @@ \mf void LoadText(YTextFile&);
	/ \tr \impl @@ \mf bool LineDown();
	/ \tr \impl @@ \mf bool LineUp();
/ \tr \impl @@ \mf void YSimpleListBox::DrawForeground()
	@@ \impl \u YGUIComponent;
/ \tr @@ \impl \u GBKEX;

r17:
/ @@ \u YText:
	+ \i \mf u8 operator()(fchar_t) @@ \cl TextRegion ^ \ft PutChar;
	/ \ft<class _tOut, typename _tBuffer> _tOut
		PutLine(_tBuffer&, TextState&, _tOut)
		-> \ft<class _tOut, class _fRenderer> _tOut
		PutLine(_fRenderer&, _tOut);
	/ \ft<typename _tOut, typename _tChar, class _tBuffer> _tOut
		PutLine(_tBuffer&, TextState&, _tOut, _tOut g, _tChar = '\0')
		-> \ft<class _tOut, typename _tChar, class _fRenderer> _tOut
		PutLine(_fRenderer&, _tOut, _tOut, _tChar = '\0');
	/ \i \ft<class _tBuffer> String::size_type
		PutLine(_tBuffer&, TextState&, const String&)
		-> \i \ft<class _fRenderer> String::size_type
		PutLine(_fRenderer&, const String&);
	/ \ft<class _tOut, typename _tBuffer> _tOut
		PutString(_tBuffer&, TextState&, _tOut)
		-> \ft<class _tOut, class _fRenderer> _tOut
		PutString(_fRenderer&, _tOut);
	/ \ft<typename _tOut, typename _tChar, class _tBuffer> _tOut
		PutString(_tBuffer&, TextState&, _tOut, _tOut, _tChar = '\0')
		-> ft<typename _tOut, typename _tChar, class _fRenderer> _tOut
		PutString(_fRenderer&, _tOut, _tOut, _tChar = '\0');
	/ \i \ft<class _tBuffer> String::size_type
		PutString(_tBuffer&, TextState&, const String&)
		-> \i \ft<class _fRenderer> inline String::size_type
		PutString(_fRenderer& const String&);
/ @@ \cl MDualScreenReader @@ \impl \u DSReader:
	/ \tr \impl @@ \mf void FillText();
	/ \tr \impl @@ \mf void LoadText(YTextFile&);
	/ \tr \impl @@ \mf bool LineUp();
	/ \tr \impl @@ \mf bool LineDown();
/ \tr @@ \impl \u GBKEX;
/ \tr \impl @@ \mf void YSimpleListBox::DrawForeground()
	@@ \impl \u YGUIComponent;

r18:
/ @@ \u YText:
	+ \ab \cl ATextRenderer;
	/ @@ \cl TextRegion:
		/ typedef TextState ParentType -> typedef ATextRenderer ParentType;
		/ \mf (GetLnN & GetLnNEx & SetLnLast & ClearLine & ClearLn
			& ClearLnLast) >> \ab \cl ATextRenderer;
		/ \inh \cl TextState >> \ab \cl ATextRenderer;
		+ \mf \vt void ClearLine(u16, SDST);
		+ \i \mf ImplI1(ATextRenderer)
			DefGetter(const Graphics&, Context, *this);
		/ \tr \impl @@ \i \f \op=(const TextRegion&);
		/ \impl @@ 3 \ctor;
		/ \tr \impl @@ \mf ClearLine;
	+ \vt @@ \mf ATextRenderer::ClearLine;
+ \c @@ \mf (\op[] & at) @@ \cl Graphics @@ \u YObject;

r19:
/ @@ \u YText:
	* \impl @@ \vt \mf ClearLine @@ \cl (ATextRenderer & TextRegion);
	+ \cl TextRenderer;
	+ \f void PrintChar(const Graphics&, TextState&, fchar_t);

r20:
+ \f void DrawText(const Graphics&, const String&, const Point&, Color,
	const Padding&) @@ \u YText;
/ \impl @@ \f bool PaintText(Widget&, const Graphics&, const Point&)
	@@ \impl \u YLabel;

r21:
/ @@ \impl \u YText:
	/ \tr \simp \impl @@ 2 \f PrintChar;
	* \impl @@ \f void DrawText(const Graphics&, const String&, const Point&,
		Color, const Padding&);
/ \impl @@ \mf PaintText @@ \cl MLabel @@ \u YLabel;

r22:
+ \f Padding FetchMargin(const Rect&, const Size&) @@ \u YGDI;
/ \impl @@ \mf PaintText @@ \cl MLabel @@ \u YLabel;
/ \f void DrawText(const Graphics&, const String&, const Point&,
	Color, const Padding&) -> void DrawText(const Graphics&, const String&,
	const Padding&, Color) @@ \u YText;

r23:
/ \f void DrawText(const Graphics&, const String&, const Point&,
	Color, const Padding&) -> void DrawText(const Graphics&, const Rect&,
	const String&, const Padding&, Color) @@ \u YText;
/ \impl @@ \mf PaintText @@ \cl MLabel @@ \u YLabel;

r24-r25:
* \impl @@ \f void DrawText(const Graphics&, const Rect&,
	const String&, const Padding&, Color) @@ \u YText;

r26:
/ @@ \u YText:
	/ \a !\m \f PrintChar => RenderChar;
	/ 2 \i \mf u8 operator() -> void operator();
	/ \ft<class _tBuffer> u8 PutChar(tBuffer&, TextState&, fchar_t)
		-> \ft<class _fRenderer> u8 PutChar(_fRenderer&, fchar_t);
	+ \i \tf<class _fRenderer> u8 PrintChar(_fRenderer&, fchar_t);
	/ \a _fRenderer => _tRenderer;
	/ \tr \impl @@ 2 \ft PrintLine;
	+ 3 \ft PrintLine;
	+ 3 \ft PrintString;
	/ \tr \impl @@ \f void DrawText(const Graphics&, const Rect&,
		const String&, const Padding&, Color) ^ PrintLine ~ PutLine;
	/ \tr \impl @@ \f ReadX;

r27:
/ @@ \u Text:
	/ @@ \cl ATextRenderer:
		+ DeclIEntry(TextState& GetTextState());
		- \inh TextState;
		- \mf ATextRenderer& operator=(const TextState&);
		- \ctor \exp ATextRenderer(Drawing::Font&);
		- \ctor \exp ATextRenderer(YFontCache&);
		- \ctor ATextRenderer();
		/ \impl @@ \ctor;
		- typedef TextState ParentType;		
		+ DeclIEntry(const TextState& GetTextState() const);
		/ \tr \impl @@ \mf (GetLnN & GetLnNEx & SetLnLast & ClearLn);
	/ @@ \cl TextRenderer:
		/ \ctor TextRenderer(const Graphics&)
			-> TextRenderer(TextState&, const Graphics&);
		+ \m TextState& TextState;
		+ \mf ImplI1(ATextRenderer)
			DefMutableGetter(TextState&, TextState, State);
		/= \tr \impl @@ \i \mf void operator()(fchar_t);
		+ \mf ImplI1(ATextRenderer)
			DefGetter(const TextState&, TextState, State);
	/ @@ \cl TextRegion:
		+ \inh TextState;
		+ protected \mf void InitiailizeFont();
		/ \impl @@ 3 \ctor;
		/= \tr \impl @@ \i \mf TextRegion& operator=(const TextState&);
		+ \mf ImplI1(ATextRenderer)
			DefMutableGetter(TextState&, TextState, *this);
		+ \mf ImplI1(ATextRenderer)
			DefGetter(const TextState&, TextState, *this);
	/ \impl @@ \f void DrawText(const Graphics&, const Rect&,
		const String&, const Padding&, Color);
	/ \tr \impl @@ 2 \ft PrintLine;
	/ \tr \impl @@ 2 \ft PutLine;
	/ \tr \impl @@ 2 \ft PrintString;
	/ \tr \impl @@ 2 \ft PutString;

r28:
/ @@ \u YText:
	+ \f void DrawText(const Graphics&, const TextState&, const String&);
	+ \mf ResetForBound(const Rect&) @@ \cl ATextRenderer;
	/ \impl @@ \f void DrawText(const Graphics&, const Rect&,
		const String&, const Padding&, Color) ^ ATextRenderer::ResetForBound;

r29:
/ @@ \u YWidget:
	/ \f Rect GetBoundsOf(IWidget&) -> \i \ft<class _tWidget>
		Rect GetBoundsOf(const _tWidget&);
/ @@ \cl MLabel @@ \impl \u YLabel:
	/ \mf bool PaintText(Widget&, const Graphics&, const Point&)
		-> void PaintText(Widget&, const Graphics&, const Point&);

r30:
/ @@ \u YLabel:
	/ @@ \cl MLabel:
		- \m wpTextRegion;
		/ \ctor \exp MLabel(const Drawing::Font& = Drawing::Font::GetDefault(),
			GHWeak<Drawing::TextRegion> = NULL)
			-> \exp MLabel(const Drawing::Font& = Drawing::Font::GetDefault());
	/ \ctor \exp YLabel(const Rect& = Rect::Empty, IUIBox* = NULL,
		const Drawing::Font& = Drawing::Font::GetDefault(),
		GHWeak<Drawing::TextRegion> = NULL) @@ \cl YLabel
		-> \exp YLabel(const Rect& = Rect::Empty, IUIBox* = NULL,
		const Drawing::Font& = Drawing::Font::GetDefault());
	/ \ctor \exp MTextList(const Drawing::Font& = Drawing::Font::GetDefault(),
		GHWeak<Drawing::TextRegion> = NULL) @@ \cl MTextList
		-> \exp MTextList(const Drawing::Font& = Drawing::Font::GetDefault());
/ @@ \cl YButton @@ \u YGUIComponent:
	\ctor \exp YButton(const Rect& = Rect::Empty, IUIBox* = NULL,
		const Drawing::Font& = Drawing::Font::GetDefault(),
		GHWeak<Drawing::TextRegion> = NULL)
		-> \exp YButton(const Rect& = Rect::Empty, IUIBox* = NULL,
		const Drawing::Font& = Drawing::Font::GetDefault());

r31-r33:
/= test 4;

r34:
* \impl @@ \f MLabel::PaintText @@ \impl \u YLabel;
* \impl @@ \f void DrawText(const Graphics&, const Rect&, const String&,
	const Padding&, Color color) @@ \impl \u YText;

r35:
* \impl @@ 2 \f RenderChar @@ \impl \u YText;

r36:
/ \impl @@ \mf void YSimpleListBox::DrawForeground() @@ \impl \u YGUIComponent;

r37:
/ @@ \u YGUIComponent:
	/ @@ \cl YSimpleListBox:
		- \m wpTextRegion;
		/ \exp \ctor YSimpleListBox(const Rect& = Rect::Empty, IUIBox* = NULL,
			GHWeak<Drawing::TextRegion> = NULL, GHWeak<ListType> = NULL)
			-> YSimpleListBox(const Rect& = Rect::Empty, IUIBox* = NULL,
			GHWeak<ListType> = NULL); ; ;
		/ \tr \impl @@ \mf void DrawForeground();
		+ \m Drawing::TextState TextState;
		/ protected \mf Drawing::TextRegion& GetTextRegion() const ythrow()
			-> Drawing::TextState& GetTextRegion() ythrow();
		- \c @@ \mf SDST GetItemHeight() const;
	/ \exp \ctor YListBox(const Rect& = Rect::Empty, IUIBox* = NULL,
		GHWeak<Drawing::TextRegion> = NULL, GHWeak<ListType> = NULL)
		@@ \cl YListBox -> YListBox(const Rect& = Rect::Empty, IUIBox* = NULL,
		GHWeak<ListType> = NULL)
	/ \exp \ctor YFileBox(const Rect& = Rect::Empty, IUIBox* = NULL,
		GHWeak<Drawing::TextRegion> = NULL)
		-> YFileBox(const Rect& = Rect::Empty, IUIBox* = NULL);

r38:
/= test 5 ^ \conf release;


$DOING:

relative process:
2011-02-08:
-24.6d;

/ ...


$NEXT_TODO:

b191-b240:
/ scroll bars @@ listbox \cl;
/ fully \impl \u DSReader;
	* moved text after setting lnGap;
* non-ASCII character path error in FAT16;
/ non-ordinary operator usage in \clt GSequenceViewer @@ \h YComponent;

b241-b648:
/ improve efficiency @@ \ft polymorphic_crosscast @@ \h YCast;
/ strict stdc++ \conf \mac;
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

