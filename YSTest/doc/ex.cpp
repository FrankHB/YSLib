//v0.2959; *Build 191 r48;
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
/= test 0;

r2:
/ @@ \cl YSimpleListBox @@ \u YGUIComponent:
	+ protected \m SDST TopOffset;
	/ \tr \impl @@ \ctor;
	/ \impl @@ \mf DrawForeground;
	/ \impl @@ \mf OnKeyDown;
	+ protected \mf SDST AdjustTopOffset();
	+ protected \mf SDST AdjustDownOffset();

r3-r6:
* \impl @@ \mf DrawForeground @@ \cl YSimpleListBox @@ \u YGUIComponent;

r7:
/ \rem !\tr \impl @@ \mf AdjustDownOffset @@ \cl YSimpleListBox
	@@ \u YGUIComponent;

r8:
/ @@ \cl YSimpleListBox @@ \u YGUIComponent:
	* \impl @@ \mf OnKeyDown;
	/ !\rem !\tr \impl @@ \mf AdjustDownOffset;
	/ \mf AdjustDownOffset => AdjustBottomOffset;
/ @@ \clt GSequenceViewer @@ \h YComponent:
	+ DefGetter(IndexType, Relative, IsSelected() ? GetSelected() - GetIndex()
		: -1);

r9:
* \tr \impl @@ \mf YFileBox::OnConfirmed(IndexEventArgs&) @@ \u YGUIComponent;

r10:
* \impl @@ \mf OnKeyDown @@ \cl YSimpleListBox @@ \u YGUIComponent;

r11-r12:
* \impl @@ \mf AdjustDownOffset @@ \cl YSimpleListBox @@ \u YGUIComponent;

r13:
* \impl @@ \mf OnKeyDown @@ \cl YSimpleListBox @@ \u YGUIComponent;

r14:
/ @@ \u YGUIComponent:
	+ \mf void YSimpleListBox::ResetView();
	/ \simp \impl @@ \mf YFileBox::OnConfirmed ^ YSimpleListBox::ResetView;

r15-r16:
	r15:
	* \def @@ \mf Contains @@ \clt GSequenceViewer @@ \h YComponent:
		+ \c;
/= test 1;

r17:
/ @@ \cl YListBox @@ \impl \u YGUIComponent:
	/ \impl @@ \mf DrawForeground;
/ @@ \h YComponent:
	/ \clt<class _tContainer> GSequenceViewer
		-> \clt<class _tContainer, typename _tSize = typename
		_tContainer::size_type, typename _tIndex = std::ptrdiff_t>
		GSequenceViewer:
		/ typedef typename _tContainer::size_type SizeType
			-> typedef _tSize SizeType;
		/ typedef std::ptrdiff_t IndexType -> typedef _tIndex IndexType;

r18:
/ \u YGUIComponent:
	/ @@ \cl YListBox:
		+ DefPredicateMember(Selected, TextListBox);
		+ typedef YSimpleListBox::ViewerType ViewerType;
		+ DefGetterMember(ViewerType::IndexType, Index, TextListBox);
		+ DefGetterMember(ViewerType::IndexType, Selected, TextListBox);
		+ \mf bool Contains(ViewerType) const;
	/ @@ \cl YSimpleListBox:
		/ protected \m ViewerType Viewer -> private \m ViewerType viewer;
		/ protected \m SDST TopOffset -> private \m SDST top_offset;
		/ protected \m Drawing::TextState TextState -> private \m
			TextState text_state;
		+ \mf bool Contains(ViewerType) const;
	/ \simp \impl @@ \mf (GetPath & OnConfirmed) @@ \cl YFileBox;

r19:
/ @@ \u YGUIComponent:
	/ @@ \cl YListBox:
		+ DefGetterMember(ListType&, List, TextListBox);
		+ \mf void ResetView() ^ \mac PDefH0;
	/ @@ \cl YFileBox:
		/ \inh \cl YSimpleListBox -> \cl YListBox;
		/ typedef YSimpleListBox ParentType -> typedef YListBox ParentType;
		/ \impl @@ \ctor;

r20:
* \impl @@ \ctor @@ \cl YListBox @@ \impl \u YGUIComponent;

r21:
/ @@ \clt GSequenceViewer @@ \h YComponent:
	/ \mf GetSelected => GetSelectedIndex;
	/ \a \m nIndex => head;
	/ \a \m nSelected => selected;
	/ \a \m nLength => length;
	/ \a \m bSelected => is_selected;
	/ \mf GetIndex => GetHeadIndex;
	/ \mf SetIndex => SetHeadIndex;
	/ \mf SetSelected => SetSelectedIndex;
	/ \mf GetRelative => GetRelativeIndex;
/ @@ \h YGUIComponent:
	/ @@ \cl YSimpleListBox:
		/ DefGetterMember(ViewerType::IndexType, Selected, viewer)
			-> DefGetterMember(ViewerType::IndexType, SelectedIndex, viewer);
		/ DefGetterMember(ViewerType::IndexType, Index, viewer)
			-> DefGetterMember(ViewerType::IndexType, HeadIndex, viewer);
	/ @@ \cl YListBox:
		/ DefGetterMember(ViewerType::IndexType, Selected, TextListBox)
			-> DefGetterMember(ViewerType::IndexType, SelectedIndex,
			TextListBox);
		/ DefGetterMember(ViewerType::IndexType, Index, TextListBox)
			-> DefGetterMember(ViewerType::IndexType, HeadIndex, TextListBox);

r22:
/ @@ \h YEvent:
	/ \mac DefMutableEventGetter(_tEventHandler, _name)
		-> DefMutableEventGetter(_tEventHandler, _name, _member);
	/ \mac DefEventGetter(_tEventHandler, _name)
		-> DefEventGetter(_tEventHandler, _name, _member);

r23:
/ @@ \u YGUIComponent:
	/ @@ \cl YListBox:
		- \m DeclEvent(HIndexEvent, Selected);
		- \m DeclEvent(HIndexEvent, Confirmed);
		+ DefMutableEventGetter(HIndexEvent, Selected, TextListBox.Selected);
		+ DefMutableEventGetter(HIndexEvent, Confirmed, TextListBox.Confirmed);
		/ \impl @@ \ctor;
	/ \impl @@ \ctor @@ \cl YFileBox;
/ \tr \impl @@ \ctor @@ \cl ShlExplorer::TFrmFileListSelecter
	@@ \impl \u Shells;

r24-r27:
* \impl @@ \f IVisualControl* GetTouchedVisualControlPtr(IUIBox&, Point&)
	@@ \un \ns @@ \impl \u YGUI;

r28:
/= \tr \impl @@ \mf DrawForeground @@ \cl (YThumb & YSimpleListBox)
	@@ \impl \u YGUIComponent;
* \h YControl:
	/ \inh AFocusRequester >> \cl AVisualControl ~ \cl MVisualControl;
	/ \tr \impl @@ \ctor @@ \cl (MVisualControl & AVisualControl);
	* \tr \impl @@ \mf AVisualControl::CheckRemoval;

r29:
/ @@ \u YFocus:
	/ @@ \cl AFocusRequester:
		/ \impl @@ \a (\mf & \mft) (ReleaseFocus & RequestFocus);
		/ \impl @@ \ctor;
		- \m bFocus;
		/ mf bool IsFocused() const -> \amf;
/ @@ \cl AVisualControl @@ \u YControl:
	- ImplI1(IVisualControl) DefPredicateBase(Focused, AFocusRequester);
	+ !\i \impl \amf IsFocused ^ \mf (GetContainerPtr);

r30-r31:
/= test 2;

r32:
* \impl @@ \mf bool SetFocusingPtr(_type*) @@ \clt GMFocusResponser
	@@ \h YFocus;

r33:
/ @@ \h YFocus:
	- \amf DeclIEntry(bool IsFocusOfContainer(_tResponser<_type>&) const)
		@@ \in \t GIFocusRequester;
	- ImplI1((GIFocusRequester<GMFocusResponser, AFocusRequester>)) @@ \mf
		IsFocusOfContainer @@ \cl AFocusRequester;
- \mf IsFocusOfContainer @@ \cl AVisualControl @@ \h YControl;

r34:
/= test 3;

r35:
/ \impl @@ \mf (bool operator-=(IVisualControl&) & DOs::size_type
	RemoveAll(IVisualControl&)) @@ \cl YDesktop @@ \impl \u YDesktop;

r36:
/ \impl @@ \mf void YDesktop::RemoveTopDesktopObject() @@ \impl \u YDesktop;
- \mf CheckRemoval @@ \cl AVisualControl @@ \h YControl;
/ @@ \u YFocus:
	/ @@ \in \t GIFocusRequester:
		- \amf DeclIEntry(void ReleaseFocus(EventArgs&));
		- \amf DeclIEntry(bool CheckRemoval(_tResponser<_type>&) const);
	/ \a GIFocusRequester<GMFocusResponser, AFocusRequester>
		-> GIFocusRequester<GMFocusResponser>;
	/ @@ \cl AFocusRequester & \in \t GIFocusRequester:
		- \amf DeclIEntry(void ReleaseFocus(EventArgs&));
		- \mf bool RequestFocus(GMFocusResponser<AFocusRequester>&);
		- \mf bool ReleaseFocus(GMFocusResponser<AFocusRequester>&);
		- \a (\mf & \mft CheckRemoval);
	/ @@ \clt GFocusResponser:
		- \de \t \param AFocusRequester;
		- friend \cl AFocusRequester;

r37:
/ @@ \h YFocus:
	/ \t<template<class> class _tResponser = GMFocusResponser,
		class _type = AFocusRequester> \in GIFocusRequester
		-> \t<template<class> class _tResponser = GMFocusResponser>
		\in GIFocusRequester;
/ \vt \inh GIFocusRequester<GMFocusResponser, IVisualControl>
	-> GIFocusRequester<GMFocusResponser> @@ \in IVisualControl @@ \h YControl;
/ \inc "ydesktop.h" -> "ywindow.h" @@ \impl \u YFocus;
- \i @@ \f (FetchDirectWindowPtr & FetchDirectDesktopPtr) @@ \u YUIContainer;

r38:
* \impl @@ \ctor @@ \cl YFileBox @@ \impl \u YGUIComponent;

r39:
/= test 4 ^ \conf release;

r40-r47:
* \impl @@ void YSimpleListBox::DrawForeground();
/= test 5;

r48:
/= test 6 ^ \conf release;


$DOING:

relative process:
2011-02-14:
-26.5d;

/ ...


$NEXT_TODO:

b192-b270:
/ scroll bars @@ listbox \cl;
/ text alignment;
/ GDI brushes;
/ fully \impl \u DSReader;
	* moved text after setting lnGap;
* non-ASCII character path error in FAT16;
/ non-ordinary operator usage in \clt GSequenceViewer @@ \h YComponent;
/ improve efficiency @@ \ft polymorphic_crosscast @@ \h YCast;

b271-b648:
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

