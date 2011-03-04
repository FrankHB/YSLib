//v0.2964; *Build 193 r103;
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
	\cl VisualControl;
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
	\cl YSimpleListBox;
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
/= test 0;

r2-r4:
* \impl @@ \mf SDST YSimpleListBox::AdjustBottomOffset()
	@@ \impl \u YGUIComponent;
/= test 1;

r5:
- \n @@ \en @@ typedef @@ \h (YCommon & YControl);
/ @@ \h YGUIComponent:
	+ \ns ScrollEventSpace;
	+ typedef \un \en ScrollEventType @@ \ns ScrollEventSpace;
	+ \st ScrollEventArgs;

r6:
/ @@ \h YGUIComponent:
	+ DefDelegate(HScrollEvent, IVisualControl, ScrollEventArgs);
	/ @@ \cl ATrack:
		+ DeclEvent(HScrollEvent, Scroll);
		+ private \mf void OnMove_Thumb(EventArgs&);

r7:
/ @@ \h YCoreUtilities:
	- \ft ReferenceEquals;
	/ (2 \ft vmin) & (2 \ft vmax) >> @@ \ns ystdex
		@@ \h Utilities @@ \dir ystdex @@ lib YCLib;
	* \impl @@ \ft<typename _type> \i _type GetZeroElement();
	+ \ft<typename _type> \i ystdex::remove_reference<_type>& GetStaticRef();
+ \stt ref_eq @@ \ns ystdex @@ \h Utilities @@ \dir ystdex @@ lib YCLib;
/ \a vmin @@ \lib YSLib -> ystdex::vmin;
/ \a vmax @@ \lib YSLib -> ystdex::vmax;
/ \tr \impl @@ \mf bool FontFamily::operator<(const FontFamily&) const
	@@ \impl \u YFont;
/ @@ \u YControl:
	- \s \c \m ScreenPositionEventArgs Empty @@ \cl ScreenPositionEventArgs;
	- \s \c \m InputEventArgs Empty @@ \cl InputEventArgs;
	- \s \c \m KeyEventArgs Empty @@ \cl KeyEventArgs;
	- \s \c \m TouchEventArgs Empty @@ \cl TouchEventArgs;

r8:
/ \a local \o EventArgs @@ \h YFocus & \impl \u (YApplication & YControl
	& YDesktop & YGUI) \def -> GetStaticRef<EventArgs>() @@ \param @@ \f;
* \ft<typename _type> \i ystdex::remove_reference<_type>&
	GetStaticRef() @@ \h YCoreUtilities -> \ft<typename _type>
	\i _type& GetStaticRef();

r9:
/ @@ \u YGUIComponent:
	/ @@ \cl ATrack:
		/ \mf \vt void SetThumbPosition(SDST)
			-> !\vt void SetThumbPositionRaw(SPOS);
		+ \mf \vt void SetThumbPosition(SPOS);
		+ private \m ScrollEventType scroll_type;
		/ \impl @@ \ctor;
		+ protected \mf void CheckScroll();
		/ \impl @@ \mf void ResponseTouchDown(SDST);
		+ public \mf void CheckScroll(ScrollEventSpace::ScrollEventType);
		/ private \m SDST MinThumbLength => min_thumb_length;
		+ DefGetter(ScrollEventSpace::ScrollEventType, ScrollType, scroll_type);
	/ @@ \cl AScrollBar:
		+ void OnClick_PrevButton(TouchEventArgs&);
		+ void OnClick_NextButton(TouchEventArgs&);
	/ \mf CallConfirmed => CheckConfirmed @@ \cl YSimpleListBox;

r10:
/ @@ \u YGUIcomponent:
	/ @@ \cl YListBox:
		+ private \mf void OnScroll_VerticalScrollBar(ScrollEventArgs&);
		/ \impl @@ \ctor;
		/ \impl @@ \mf DrawForeground;
	/ @@ \cl YSimpleListBox:
		+ \mf void LocateViewPosition(SDST);
		+ \mf SDST GetFullViewHeight() const;
		/ \impl @@ \mf Size GetFullViewSize() const;
		+ \mf void GetViewPosition(SDST) const;
	/ @@ \cl ScrollEventArgs:
		+ 2 public \m SDST Value, OldValue;
		/ \i \ctor ScrollEventArgs(ScrollEventSpace::ScrollEventType)
			-> ScrollEventArgs(ScrollEventSpace::ScrollEventType, SDST);
		+ \i \ctor ScrollEventArgs(ScrollEventSpace::ScrollEventType,
			SDST, SDST);
	/ @@ \cl ATrack:
		/ \mf void CheckScroll() -> void CheckScroll(SDST);
		/ \mf void CheckScroll(ScrollEventSpace::ScrollEventType, SDST)
			-> void CheckScroll(ScrollEventSpace::ScrollEventType, SDST, SDST);
	/ @@ \cl AScrollBar:
		/ \tr \impl @@ \mf void OnClick_PrevButton(TouchEventArgs&);
		/ \tr \impl @@ \mf void OnClick_NextButton(TouchEventArgs&);
	+ \i @@ \dtor @@ \cl YFileBox ^ DefEmptyDtor;
	/ ac @@ \m (HorizontalScrollBar & VerticalScrollBar) -> protected ~ private
		@@ \cl ScrollableContainer;

r11-r21:
/= test 2;

r22:
* \cl YListBox @@ \impl \u YGUIComponent:
	* \impl @@ \mf void DrawForeground();
	* \impl @@ \mf void OnScroll_VerticalScrollBar(ScrollEventArgs&)

r23-r25:
* \tr \impl @@ \f RectDrawButton @@ \un \ns @@ \impl \u YGUIComponent;

r26-r28:
/= test 3;

r29:
* \impl @@ \mf SDST YSimpleListBox::GetFullViewHeight() const;

r30:
* \impl @@ \mf void ATrack::ResponseTouchDown(SDST);

r31-r32:
/ test 4;
	* \impl @@ \mf void YListBox::OnScroll_VerticalScrollBar(ScrollEventArgs&);

r33:
* \impl @@ \mf SDST YSimpleListBox::AdjustBottomOffset();

r34-r44:
/ test 5;

r45:
+ \stt GMValueEventArgs @@ \h YControl;
/ \def @@ \st ScrollEventArgs \cl YGUIComponent ^ \stt GMValueEventArgs;

r46:
/ @@ \u YGUIComponent:
	/ @@ \cl YSimpleListBox:
		+ DeclEvent(HVisualEvent, ViewChanged);
		/ \impl @@ \mf (AdjustTopOffset & AdjustDownOffset);
	/ @@ \cl YListBox:
		+ \mf void OnViewChanged_TextListBox(EventArgs&);
		/ \impl @@ \ctor;
		* \impl @@ DrawForeground;

r47-r50:
/= test 6;

r51:
* @@ \cl YListBox:
	/ \impl @@ \mf (AdjustTopOffset & AdjustDownOffset);
	/ \impl @@ \mf OnKeyDown;

r52:
/ @@ \u YGUIComponent:
	/ @@ \cl YSimpleListBox:
		+ \mf void UpdateView();
		/ \tr \simp \impl @@ \mf OnKeyDown ^ \mf UpdateView;
		/ \impl @@ \mf ResetView;
	/ @@ \cl YFileBox:
		/ \impl @@ \mf DrawForeground;
		/ \impl @@ \ctor;
		/ \simp \impl @@ \mf OnConfirmed;
	+ \i \mf void UpdateView() @@ \cl YListBox;

r53:
/ @@ \cl YFileBox:
	* \impl @@ \mf OnConfirmed;
	- \mf DrawBackground;
	- \mf DrawForeground;

r54:
+ DefMutableEventGetter(HVisualEvent, ViewChanged, TextListBox.ViewChanged)
	@@ \cl YListBox;
/ \mf void fb_Selected(IndexEventArgs&) -> void fb_ViewChanged(EventArgs&)
	@@ \cl ShlExplorer::TFrmFileListSelecter @@ \u Shells;

r55:
/= test 7 ^ \conf release;

r56:
/ \impl @@ \mf (OnTouchDown & OnTouchMove )@@ \cl YSimpleListBox;

r57-r59:
/= test 8;

r60:
/ @@ \cl ATrack:
	/ \mf SetThumbPosition -> LocateThumb;
	/ \mf SetThumbPositionRaw => SetThumbPosition;
	+ \mf void LocateThumbToFirst();
	+ \mf void LocateThumbToLast();

r61:
/ @@ \cl ATrack:
	+ private \mf void LocateThumb(ScrollEventSpace::ScrollEventType, SPOS);
	/ \mf )void LocateThumb(SPOS) & LocateThumbToFirst & LocateThumbToLast)
		-> \i \mf ^ \mf void LocateThumb(ScrollEventSpace::ScrollEventType,
		SPOS);
	/ \impl @@ \mf void SetThumbPosition(SPOS);
	- \mf void CheckScroll(SDST);
	/ \impl @@ \mf void CheckScroll(ScrollEventSpace::ScrollEventType, SDST);
	/ \impl @@ \mf void OnMove_Thumb(EventArgs&);
	- \m ScrollEventSpace::ScrollEventType scroll_type;
	- \mf DefGetter(ScrollEventSpace::ScrollEventType, ScrollType, scroll_type);

r62:
* @@ \u YGUIComponent:
	/ \impl @@ \mf void YListBox::OnViewChanged_TextListBox(EventArgs&);
	/ \ac @@ \mf void ATrack::SetThumbPosition(SPOS) -> public ~ private;

r63:
* \impl @@ \ctor @@ \cl ATrack;

r64:
/ \a OnDrag => OnTouchMove_Dragging;
/ \a OnDrag_Thumb_Horizontal => OnTouchMove_Thumb_Horizontal;
/ \a OnDrag_Thumb_Vertical => OnTouchMove_Thumb_Vertical;

r65:
/ @@ \u YGUIComponent:
	/ @@ \cl ATrack:
		+ DeclEvent(HVisualEvent, ThumbDrag);
		/ OnMove_Thumb => OnThumbDrag;
	/ \impl @@ \cl YHorizontalTrack::OnTouchMove_Thumb_Horizontal;
	/ \impl @@ \mf YVertictalTrack::OnTouchMove_Thumb_Vertical;

r66:
/ @@ \clt GSequenceViewer @@ \h YComponent:
	/ \impl @@ \mf SetHeadIndex;
	/ \mf RestrictViewer => RestrictView;
/ \impl @@ \mf (AdjustTopOffset & AdjustDownOffset) @@ \cl YSimpleListBox
	@@ \impl \u YGUIComponent;

r67-r69:
* \impl @@ \mf YSimpleListBox::ViewerType::IndexType
	YSimpleListBox::CheckPoint(SPOS, SPOS) @@ \impl \u YGUIComponent;

r70-r71:
* \impl @@ \mf void YSimpleListBox::DrawForeground() @@ \impl \u YGUIComponent;

r72-r73:
/ @@ \u YGUIComponent:
	/ @@ \cl ATrack:
		+ typedef u16 ValueType;
		+ ValueType Value;
		+ ValueType Delta;
		/ \tr \impl @@ \ctor;
	/ @@ \cl AScrollBar:
		+ typedef ATrack::ValueType ValueType;
		+ ValueType Delta;
		/ \tr \impl @@ \ctor;
	+ \inc <limits> @@ \h;

r74:
* \impl @@ \ft RestrictInClosedInterval @@ \h YCoreUtilities;
/= \simp \impl @@ \mf void ATrack::SetThumbPosition(SPOS);

r75:
+ \s \c \m ValueType MaxValue @@ \cl ATrack;

r76:
/ @@ \u YGUIComponent:
	- \inc <limits> @@ \h;
	/ @@ \cl ATrack:
		/ \s \c \m ValueType MaxValue -> !\s !\c \m ValueType MaxValue;
		/ \impl @@ \ctor;
		+ \mf void SetValue(ValueType);
		+ \mf void UpdateValue();
		+ \mf void SetLargeDelta(ValueType);

r77:
/ @@ \u YGUIComponent:
	/ @@ \cl ATrack:
		/ \i \mf void LocateThumb(SPOS) -> void LocateThumb(ValueType);
		/ \mf void LocateThumb(ScrollEventSpace::ScrollEventType, SPOS)
			-> void LocateThumb(ScrollEventSpace::ScrollEventType, ValueType);
		/ \impl @@ \mf ResponseTouchDown;
	/ @@ \cl YListBox:
		/ \impl @@ \mf OnScroll_VerticalScrollBar;
		/ \impl @@ \mf OnViewChanged_TextListBox;

r78:
* \impl @@ \mf SetLargeDelta @@ \cl ATrack;

r79:
/ @@ \u YGUIComponent:
	/ @@ \cl ATrack:
		* \impl @@ mf ResponseTouchDown;
		+ \mf void SetMaxValue(ValueType);
		/ \ac @@ \m MaxValue -> protected ~ public;
		+ \mf DefGetter(ValueType, MaxValue, MaxValue);
	/ \tr \impl @@ \mf OnViewChanged_TextListBox @@ \cl YListBox;

r80-r82:
/= test 9;
/ \impl @@ \mf SetMaxValue @@ \cl ATrack;

r83:
/ @@ \impl \u YGUIComponent:
	* \impl @@ \mf OnThumbDrag @@ \cl ATrack;
	* \impl @@ \mf (OnClick_PrevButton & OnClick_NextButton) @@ \cl AScrollBar;

r84-r85:
/= test 10;
* void CheckScroll(ScrollEventSpace::ScrollEventType, SDST) @@ \cl ATrack
	-> void CheckScroll(ScrollEventSpace::ScrollEventType, ValueType);

r86-r87:
/= test 11;

r88:
* \impl @@ \mf CheckScroll @@ \cl ATrack;

r89:
* \impl @@ \mf void OnThumbDrag(EventArgs&) @@ \cl ATrack;

r90:
* \impl @@ \mf ResponseTouchDown @@ \cl ATrack;

r91-r93:
* \impl @@ \mf bool SetLength(SizeType) @@ \clt GSequenceViewer
	@@ \h YComponent;

r94:
/ @@ \ns @@ \impl \u YGUIComponent:
	/ \c SDST defMarginH(4) -> \c SDST defMarginH(2);
	/ \c SDST defMarginV(2) -> \c SDST defMarginV(1);

r95:
/ @@ \u YGUIComponent:
	/ @@ \cl ATrack:
		/ public \m ValueType Value -> private \m ValueType value;
		/ public \m ValueType Delta -> private \m ValueType large_delta;
		+ \mf DefGetter(ValueType, Value, value);
		+ \mf DefGetter(ValueType, LargeDelta, large_delta);
		/ protected \m ValueType MaxValue -> private \m ValueType max_value;
		/ DefGetter(ValueType, MaxValue, MaxValue)
			-> \mf DefGetter(ValueType, MaxValue, max_value);
	/ @@ \cl AScrollBar:
		/ public \m ValueType Delta -> private \m ValueType small_delta;
		+ \mf DefGetterMember(ValueType, MaxValue, GetTrack());
		+ \mf DefGetterMember(ValueType, Value, GetTrack());
		+ \mf DefGetterMember(ValueType, LargeDelta, GetTrack());
		+ \mf DefGetter(ValueType, SmallDelta, small_delta);
		/ \impl @@ \mf (OnClick_PrevButton & OnClick_NextButton);

r96:
* \impl @@ \mf SetLargeDelta @@ \cl ATrack;

r97:
/ @@ \cl AScrollBar:
	/ DefGetter(ATrack&, Track, *pTrack) -> \i \mf ATrack& GetTrack()
		const ythrow() ^ \as;
	/ ^ \as -> ^ \mf GetTrack @@ \impl @@ (OnClick_PrevButton
		& OnClick_NextButton);
	/ \mf OnClick_PrevButton => OnTouchDown_PrevButton;
	/ \mf OnClick_NextButton => OnTouchDown_NextButton;
	/ \impl @@ \ctor;

r98:
/ @@ \u YGUIComponent:
	/ @@ \cl AScroll:
		/ \tr \impl @@ \ctor;
		/ \impl @@ \mf (OnTouchDown_PrevButton & OnTouchDown_NextButton);
	/ @@ \cl ATrack:
		+ private \mf void LocateThumbForIncrement(
			ScrollEventSpace::ScrollEventType, ValueType);
		+ private \mf void LocateThumbForDecrement(
			ScrollEventSpace::ScrollEventType, ValueType);
		+ \i \mf void LocateThumbForLargeIncrement();
		+ \i \mf void LocateThumbForLargeDecrement();
		+ \i \mf void LocateThumbForSmallIncrement();
		+ \i \mf void LocateThumbForSmallDecrement();
		/ \simp \impl @@ \mf ResponseTouchDown;

r99:
/ @@ \u YGUIComponent:
	/ @@ \cl AScrollBar:
		+ DefSetterMember(ValueType, LargeDelta, GetTrack());
		+ DefSetter(ValueType, SmallDelta, small_delta);
		+ DefSetterMember(ValueType, MaxValue, GetTrack());
		+ DefSetterMember(ValueType, Value, GetTrack());
	/ \impl @@ \mf OnViewChanged_TextListBox @@ \cl YListBox;

r100:
/= test 12 ^ \conf release;

r101:
/ @@ \cl ATrack:
	/ \mg void ResponseTouchDown(SDST) -> void OnTouchDown(TouchEventArgs&);

r102:
/ ((\ns ScrollEventSpace) & (\st ScrollEventArgs)
	& DefDelegate(HScrollEvent, IVisualControl, ScrollEventArgs))
	>> \h YControl ~ \h YGUIComponent;
+ \clt GMRange @@ \u YControl;
/ @@ \cl ATrack @@ \u YGUIComponent:
	+ \inh GMRange<u16>;
	/ (\m (value & max_value) & \i \mf (GetValue & GetMaxValue)
		>> \clt GMRange @@ \u YControl;
	/ \impl @@ \mf SetValue;
	/ \impl @@ \mf SetMaxValue;

r103:
/= test 13 ^ \conf release;


$DOING:

relative process:
2011-03-04:
-24.9d;

/ ...


$NEXT_TODO:

b194-b270:
/ fully \impl \u DSReader;
	* moved text after setting lnGap;
* non-ASCII character path error in FAT16;
/ non-ordinary operator usage in \clt GSequenceViewer @@ \h YComponent;
/ impl 'real' RTC;

b271-b648:
/ GDI brushes;
/ text alignment;
+ \impl pictures loading;
/ improve efficiency @@ \ft polymorphic_crosscast @@ \h YCast;
+ data config;
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

