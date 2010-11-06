// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-11-06 15:29 + 08:00;
// Version = 0.2822; *Build 168 r139;


#include "../YCLib/ydef.h"

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
@@ ::= in / belongs to
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
\cp ::= copied
\ctor ::= constructors
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
\ext ::= extended
\exp ::= explicit
\f ::= functions
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
\mo ::= member objects
\n ::= names
\ns ::= namespaces
\o ::= objects
\op ::= operators
\or ::= overridden
\parm ::= parameters
\parm.de ::= default parameters
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
\st ::= structs
\str ::= strings
\t ::= templates
\tb ::= try blocks
\tc ::= class templates
\tf ::= function templates
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
r1-r5:
/= test 1;

r6:
/ @@ \cl ATrack:
	+ \inh public GMFocusResponser<IVisualControl>;
	/ \impl @@ \mf ClearFocusingPtr;

r7-r12:
/= test 2;

r13:
* \ac @@ \inh @@ protected MUIContainer -> public @@ \cl YUIContainer;

r14:
* \ac @@ \inh @@ protected MUIContainer -> public @@ \cl YFrameWindow;

r15:
/ @@ \cl AFocusRequester @@ \u YComponent:
	/ a \mf \t <class _type>(GMFocusResponser<_type>&) ->
		<template<class> class _tResponser, class _type>(_tResponser<_type>&);

r16:
+ DeclIEntry(bool ResponseFocusRequest(AFocusRequester&)) @@ \in IUIBox;
+ \mf bool ResponseFocusRequest(AFocusRequester&) @@ \cl MUIContainer
	& \cl YUIContainer & \cl YFrameWindow & \cl ATrack;
+ DeclIEntry(bool ResponseFocusRelease(AFocusRequester&)) @@ \in IUIBox;
+ \mf bool ResponseFocusRelease(AFocusRequester&) @@ \cl MUIContainer
	& \cl YUIContainer & \cl YFrameWindow & \cl ATrack;
/ @@ \u YControl:
	/ @@ \cl AVisualControl:
		/ \impl @@ \mf void RequestFocus(const EventArgs&);
		/ \impl @@ \mf void ReleaseFocus(const EventArgs&);
	/ @@ \cl MVisualControl:
		- \mf GMFocusResponser<IVisualControl>* CheckFocusContainer(IUIBox*)
		+ \vt DefEmptyDtor(MVisualControl);

r17:
/ \ac @@ \inh @@ public MUIContainer -> protected @@ \cl YUIContainer
	& \cl YFrameWindow;

r18:
/ \a MIndexEventArgs => IndexEventArgs;
* \ac @@ \inh @@ protected EventArgs -> public @@ \st IndexEventArgs;
/ \a MVisual => Visual;
/ \a MControl => Control;
/ \a MWidget => Widget;
/ \ac @@ \inh @@ protected Control -> public @@ \st MVisualControl;
/ \ac @@ \inh @@ protected Control -> public @@ \st YControl;
/ \ac @@ \inh @@ protected Widget -> public @@ \st YWidget;
/ \ac @@ \inh @@ protected Widget -> public @@ \st YUIContainer;

r19:
/ @@ \u YGUIComponent:
	/ @@ \cl YFileBox:
		/ \mf void OnTouchMove(const TouchEventArgs&) >> \cl YListBox;
		/ \impl @@ \ctor;
	/ \impl @@ \mf void _m_init() @@ \cl YListBox;

r20-r23:
/= test 3;

r24:
+ DeclIEntry(IVisualControl* GetFocusingPtr() const) @@ \in IUIBox;
+ \mf IVisualControl* GetFocusingPtr() const @@ \cl MUIContainer
	& \cl YUIContainer & \cl YFrameWindow & \cl ATrack;
* \impl @@ \f IVisualControl* GetFocusedObject(YDesktop&) @@ \cl YGUI;

r25:
/= \tr \impl @@ \f bool ResponseTouchBase(IUIBox&, HTouchCallback) @@ \u YGUI;

r26-r31:
/= test 4;

r32:
/ @@ \f IVisualControl* GetFocusedObject(YDesktop&) @@ \u YGUI:
	* \impl;
	/ >> GetFocusedObjectPtr;
/ - \c @@ DeclIEntry(IVisualControl* GetFocusingPtr() const) @@ \in IUIBox;
	/ \impl @@ \cl (MUIContainer & YUIContainer & YFrameWindow & ATrack);
- \amf DeclIEntry(IVisualControl* GetFocusingPtr() const) @@ \in IWindow;

r33:
/ @@ \u YGUI:
	- \i \f GetGraphicInterfaceContext;
	/ \tr \impl @@ \f DrawWindowBounds & \f DrawWidgetBounds;
/ \a !m \f \n match 'Get*' & !'GetImege' & !'*DragOffset' & !'GetMessage'
	& !'GetZeroElements' => 'Get*From';
/ \a !m \f \n match 'Set*' & !'*DragOffset' => 'Set*To';
/ \a !m \f \n IsStem => IsStemOF;
/ \a !m \f \n IsExtendName => IsExtendNameOF;
/ \a \mf BeFilledWith => BeFilledWith;
/ \a GetDynamicFunctionFrom => ConstructDynamicFunctionWith;
/ \a CheckInit => CheckInitialization;

r34:
/ @@ \u YGUIComoponent:
	/ @@ \cl ATrack:
		+ \m IVisualControl* pFocusing;
		/ \impl \ctor & \mf (ClearFocusingPtr & ResponseFocusRequest
			& ResponseFocusRelease);
+ \u YFocus @@ \dir Shell;
/ \u YComponent:
	*= \tr \impl @@ \mf \t template<template<class> class _tResponser,
		class _type> bool ReleaseFocus(_tResponser<_type>& c
		@@ \cl AFocusRequester;
	/ \in \t GIFocusRequester & \cl \t & GMFocusResponser & \cl AFocusRequester
		>> \u YFocus;
	/ \cl \t GContainer >> \u YObject @@ \dir Core;
+ \inc YFocus @@ \u YWidget;
/ \a GMContainer => GContainer;
/ \ac @@ \inh _tContainer @@ \cl \t GContainer -> private ~ public
	@@ \u YObject;
/ \in \t (GIEquatable & GILess & GIClonable & GIContainer) @@ YShellDefinition
	>> \u YObject;
+ \inc YObject @@ \u YFocus;
+ \inc \h Adaptor::Container @@ \u YObject;
- \inc \h Adaptor::Container @@ \u YException;
- \inc \h Adaptor::Container @@ \u YEvent;
/= \ren file "Adaptor/yagg.h" => "Adaptor/agg.h"
/ @@ \u YFocus:
	/ \in \t template<class _type = AFocusRequester> class GIFocusRequester
	-> template<template<class> class _tResponser = GMFocusResponser,
	class _type = AFocusRequester> class GIFocusRequester;

r35:
/ \a @@ \ns Runtime @@ \u YEventArgs >> \ns Components::Controls @@ \u YControl;
- \inc \h Core::YEventArgs @@ \u YComponent;
/ \a @@ \ns Runtime @@ \u YGUI >> \ns Comopnents::Controls;
/ \i \f ToSPoint @@ \ns Components::Controls @@ \u YControl >> \un \ns
	@@ \u YGlobal;

r36:
/ @@ \u YGUIComponent:
	/ @@ \cl ATrack:
		+ \mf OnTouchDown(const TouchEventArgs&);
		+ \cl EArea;
		+ \in
		{
			DeclIEntry(EArea CheckArea(const Point&) const);
			DeclIEntry(SDST GetThumbLength(SDST) const);
			DeclIEntry(SDST GetThumbPosition(SDST) const);
		}
		+ protected \mf EArea CheckArea(SPOS) const;
	/ \cl YHorizontalTrack:
		+ \impl \in ATrack::CheckArea;
		+
		{
			ImplI(ATrack) DefGetter(SDST, ThumbLength, Thumb.Width);
			ImplI(ATrack) DefGetter(SPOS, ThumbPosition, Thumb.X);
		}
	/ \cl YverticalTrack:
		+ \impl \in ATrack::CheckArea;
		+
		{
			ImplI(ATrack) DefGetter(SDST, ThumbLength, Thumb.Height);
			ImplI(ATrack) DefGetter(SPOS, ThumbPosition, Thumb.Y);
		}
/ \a IsInIntervalRegular => IsInInterval;
/ \a IsInOpenInterval => IsInOpenInterval;
/ \a RestrictInInterval => RestrictInClosedInterval;
/ \a RestrictInIntervalRegular => RestrictInInterval;
/ \a RestrictUnsigned => RestrictUnsignedStrict;
/ \a RestrictUnsignedRegular => RestrictUnsigned;
/ @@ \u YCoreUtilities:
	+ \f \t template<typename _type> std::size_t
		SwitchInterval(_type v, _type[] a, std::size_t n);
	+ \f \t template<typename _type> std::size_t
		SwitchAddedInterval(_type v, _type[] a, std::size_t n);

r37:
/ @@ \u YGUIComponent:
	/ @@ \cl ATrack:
		/ public \mf void OnTouchDown(const TouchEventArgs&) -> protected
			\mf void ResponseTouchDown(SPOS);
	/ @@ \cl YHorizontalTrack & \cl YVerticalTrack:
		+ \mf void OnTouchDown(const TouchEventArgs&);
		/ \tr @@ \ctor;
		/ \impl @@ \mf SetThumbPosition;

r38:
/ @@ \u YGUIComponent:
	/ @@ \cl ATrack:
		* \mf EArea CheckArea(SPOS) const -> EArea CheckArea(SDST) const;

r39-r40:
/ test 5;
	/ \tr \impl @@ \mf EArea CheckArea(SDST) const @@ \cl ATrack
		@@ \u YGUIComponent;

r41:
* \impl @@ \f \t (SwitchInterval & SwitchAddedInterval) @@ \u YCoreUtilities;

r42-r45:
/= test 6;

r46-r47:
/ \simp \tr \impl @@ \f IVisualControl*
	GetTouchedVisualControl(IUIBox&, Point&) @@ \un \ns @@ \u YGUI;

r48-r49:
/= test 7;

r50:
* \f bool ResponseTouchBase(IUIBox&, HTouchCallback) @@ \un \ns @@ \u YGUI;

r51:
/ @@ \u YGUI:
	/ \f bool RepeatHeld(HeldStateType&, const KeyEventArgs&,
		Timers::TimeSpan = 240, Timers::TimeSpan = 120) -> bool RepeatHeld(
		HeldStateType&, Timers::TimeSpan = 240, Timers::TimeSpan = 120);
	*= - \decl @@ \f void CheckTouchedControlBounds(
		Components::Controls::IVisualControl&, const TouchEventArgs&);

r52-r54:
/ @@ \cl YObject:
	+ \i \f Point operator+(const Point&, const Vec&);
	+ \i \f Point operator-(const Point&, const Vec&);
* @@ \u YControl:
	/ \impl @@ \f void OnTouchHeld(IVisualControl&, const TouchEventArgs&);

r55-r66:
/= test 8;

r67-r70:
/ @@ \u YGUI:
	/ @@ \un \ns :
		/ \impl @@ \f IVisualControl* GetTouchedVisualControl(IUIBox&, Point&);
		/ \impl @@ \f bool ResponseTouchUpBase(IVisualControl&,
			const TouchEventArgs&);
	/ @@ \ns InputStatus:
		+ bool InDragging;
		- \f
		{
			bool IsValidDraggingOffset();
			const Vec& GetDragOffset();
			void SetDragOffset(const Vec& = Vec::FullScreen);
		}
		+ \decl @@ YTimer::HeldTimer;
		- !\decl Vec DragOffset;
		+ Vec DraggingOffset;
/= test 9;

r71:
/ @@ \un ns @@ \u YGUI:
	+ \f void ResetTouchHeldState();

r72:
/ \tr \impl @@ \f OnTouchHeld & \f OnTouchHeld @@ \u YControl;
/ \tr \impl @@ \u YGUIComponent;

r73:
/ GetTouchedVisualControl => GetTouchedVisualControlPtr @@ \u YGUI;

r74-r80:
/ test 10;
	/ \impl @@ \f OnTouchHeld & \f OnTouchHeld @@ \u YControl;

r81:
/ @@ \u YGUI;
	/ @@ \un \ns:
		/ \impl @@ \f IVisualControl* GetTouchedVisualControlPtr(
			IUIBox&, Point&);
		/ \impl @@ \f bool ResponseTouchBase(IUIBox&, HTouchCallback);
		/ \impl @@ \f void ResetTouchHeldState();
	- bool InDragging;
	+ Point VisualControlLocationOffset;

r82:
/= test 11;

r83:
* \impl @@ \f IVisualControl* GetTouchedVisualControlPtr(IUIBox&, Point&)
	@@ \un \ns @@ \u YGUI;

r84:
* \impl @@ \f OnTouchHeld & \f OnTouchHeld @@ \u YControl;

r85:
/ @@ \u YWidget:
	/ @@ \ns Widgets:
		+ \f Point LocateForDesktop(IWidget&);
		/ \f Point LocateForParentWindow(IWidget&);
		/ \f Point LocateOffset(const IWidget*, Point, const HWND&)
			-> Point LocateOffset(const IWidget*, Point, IWindow*);
	/ \inc "ywindow.h" -> "ydesktop.h";
/ @@ \cl ShlExplorer @@ \u Shells;

r86-r91:
/= test 12;

r92:
/ @@ \ns Widgets @@ \u YWidget:
	/ \f Point LocateForWindow(IWidget&)
		-> Point LocateForWindow(const IWidget&);
	/ \f Point LocateForDesktop(IWidget&)
		-> Point LocateForDesktop(const IWidget&);
	/ \f Point LocateForParentContainer(IWidget&)
		-> Point LocateForParentContainer(const IWidget&);
	/ \f Point LocateForParentWindow(IWidget&)
		-> Point LocateForParentWindow(const IWidget&);

r93:
/ @@ \u YGUIComponent:
	/ \impl @@ \mf EArea CheckArea(const Point&) const
		@@ \cl YHorizontalTrack & \cl YVerticalTrack;

r94-r95:
/= test 13;

r96:
/ @@ \u YGUIComponent:
	* @@ \cl YHorizontalTrack & \cl YVerticalTrack:
		- \mf EArea CheckArea(const Point&) const;
		* \mf void OnTouchDown(const TouchEventArgs&);
	- \amf DeclIEntry(EArea CheckArea(const Point&) const) @@ \cl ATrack;

r97-r105:
/= test 14;

r106:
* \f Point Widgets::LocateForDesktop(const IWidget&) @@ \u YWidget;
/= \simp \impl @@ \mf void YButton::DrawForeground() @@ \u YGUIComponent;

r107-r111:
/= test 15;

r112-r113:
* \impl @@ \mf void ATrack::ResponseTouchDown(SDST) @@ \u YGUICompoonent:

r114:
/ \a IsInBounds => Contains;
/ \a IsInBoundsRegular => ContainsRegular;
/ \a IsInBoundsStrict => ContainsStrict;

r115:
/ @@ \cl Rect @@ \u YObject:
	- \mf Contains;
	* 2 \mf ContainsStrict;
/ \a ContainsRegular => Contains;

r116-r118:
/= test 16;

r119:
* @@ \cl WriteKeysInfo @@ \u YCommon;

r120:
/= test 17;

r121:
* @@ \f WriteKeysInfo @@ \u YCommon;
/ @@ \f WaitForGUIInput @@ \un \ns @@ \u YGlobal;

r122:
* incorresponding coordinates between \evh @@ \evt Enter & Leave:
	/ \impl @@ \f bool ResponseTouchBase(IUIBox&, HTouchCallback) @@ \u YGUI;
	/ \impl @@ \f OnTouchHeld @@ \u YControl;
	/ \simp \impl @@ \mf (YHorizontalTrack::OnTouchDown & YVerticalTrack::OnTouchDown)
		@@ \u YGUIComponent;

r123-132:
/= test 18;

r133:
/ \tr \simp \impl @@ \ctor @@ \cl ATrack @@ \u YGUIComponent;

r134:
* \ctor @@ \u YHorizontalTrack @@ \u YGUIComponent;

r135:
* \ctor @@ \u YVerticalTrack @@ \u YGUIComponent;

r136:
/ - \vt @@ \a \evh \mf with \n 'On*';
/ @@ \u YGUIComponent:
	/ \impl @@ \ctor @@ \cl (YHorizontalTrack & YVerticalTrack & YFileBox);
	/ \impl @@ \mf void YFileBox::OnConfirmed(const IndexEventArgs&);

r137:
/ @@ \u YGUIComponent:
	* @@ \cl YListBox:
		/ \impl @@ \mf void _m_init();
		* \impl @@ \mf void OnTouchDown(const TouchEventArgs&);
	* \impl @@ \ctor @@ \cl (YHorizontalTrack & YVerticalTrack);

r138:
* \impl @@ \ctor @@ \cl (YHorizontalTrack & YVerticalTrack) @@ \u YGUIComponent;

r139:
/ undo r85 @@ \cl ShlExplorer @@ \u Shells;


$DOING:

/ ...


$NEXT:

* blank-clicked \evt OnClick @@ ListBox;

b170-b190:
/ fully \impl \u DSReader;
	* moving text after setting lnGap;
* non-ASCII character filename error in FAT16;
+ \impl loading pictures;
+ \impl style on widgets;


$TODO:

Clarify the log levels.

Make "UpdateFont()" more efficienct.

More efficient @YTextRegion output:
Use in-buffer background color rendering and function @CopyToScreen()
	to inplements @YTextRegion background;
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

// GBK coded definitions:

namespace YSLib{
	CPATH DEF_DIRECTORY = "/Data/";
	//const char* DEF_FONT_NAME = "方正姚体";
	//CPATH DEF_FONT_PATH = "/Font/FZYTK.TTF";
	CPATH DEF_FONT_PATH = "/Font/FZYTK.TTF";
	CPATH DEF_FONT_DIRECTORY = "/Font/";
/*
using namespace Components;
using namespace Widgets;
using namespace Controls;

*/
}

char gstr[128] = "你妹喵\t的= =ijkAB DEǎ。i:みま╋㊣F2\t3Xsk\nw\vwwww";

