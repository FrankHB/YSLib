// YSTest by Franksoft 2009 - 2010
// CodePage = ANSI / GBK;
// CTime = 2009-11;
// UTime = 2010-11-01 18:11 + 08:00;
// Version = 0.2812; *Build 167 r78;


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
	\cl MWidget;
	\cl YLabel;
}
\u YControl
{
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
	\cl MWindow;
	\cl AWindow;
	\cl YFrameWindow;
}

$DONE:
r1:
*= \tr \rem @@ \h (YGUI & YGDI & YControl);

r2:
/ @@ \u YGUIComponent:
	+ \f RectDrawButton @@ \un ns;
	/ \impl @@ \mf DrawForeground ^ \f DrawButton @@ \cl YButton;
	/ \impl @@ \mf DrawForeground ^ \f DrawButton @@ \cl YHorizontalScrollBar;

r3:
/ @@ \u YGUIComponent:
	/ \simp  @@ \un \ns:
		/ \impl @@ \f RectDrawButton;
		- \f RectDrawButtonSurface;
		- \f RectDrawPressed;
	* \impl @@ \mf YButton::DrawForeground();
/= \simp @@ \mf Point MWidget::LocateForWindow() const;

r4:
* \impl @@ \f void DrawWidgetBounds(IWidget&, Color) @@ \u YGUI;
/= \simp:
	-= \a code " != NULL" in poninter & handle comparison context;
/ \impl @@ \mf @@ \cl MWidget @@ \u YWidget:
	{
		Point LocateForParentContainer() const
			-> !\m Point LocateForParentContainer(const IWidget&);
		Point LocateForParentWindow() const;
			-> !\m Point LocateForParentWindow(const IWidget&);
		Point LocateForWindow() const;
			-> !\m Point LocateForWindow(const IWidget&);
	}
/ @@ \u YWidget:
	/ \mf void PaintText(MWidget&) @@ \cl MLabel
		-> void PaintText(MWidget&, const Point&);
	/ \impl @@ \mf void YLabel::DrawForeground();
	/ \f Point LocateOffset(const IWidget*, const Point&, const HWND&)
		-> Point LocateOffset(const IWidget*, Point, const HWND&);
/ @@ \u YGUIComponent:
	/ \impl @@ \mf void YButton::DrawForeground();
	/ \tr \impl @@ \fvoid RectDrawButton(const Graphics&, const Point&,
		const Size&, bool = false) @@ \un \ns;

r5:
/= \test 1;
	/ \tr @@ makefile;

r6:
/ @@ \u YGUIComponent:
	/ @@ \cl AScrollBar:
		+ public typedef u16 ValueType;
		+ private \m SDST ThumbPosition;
		+ public \mf void SetThumbPosition(ValueType);
		+ private \m SDST ThumbSize;
		+ public \mf void SetThumbSize(SDST);
		+ public \mf DefGetter(SDST, ThumbSize, ThumbSize);
		+ public \mf DefGetter(SDST, ThumbPosition, ThumbPosition);
		+ public \amf DeclIEntry(SDST GetWidgetLength() const);
	/ @@ \u YHorizontalScrollBar:
		+ \mf ImplI(AScrollBar) DefGetter(SDST, WidgetLength(), Size.Width);
		/ + ImplI(AScrollBar) @@ \mf void DrawForeground();
	+ \as @@ \mf SDST YListBox::GetItemHeight() const;
* \a restrictInIntervalRegular => restrictInInterval;
+ \f \t template<typename _type>
	void restrictInIntervalRegular(_type&, int, int) @@ \u YCoreUtilities;
/ \a restrictInIntervalRegular => RestrictInIntervalRegular;
/ \a restrictInInterval => RestrictInInterval;
/ \a isInIntervalRegular => IsInIntervalRegular;
/ \a isInIntervalStrict => IsInIntervalStrict;
/ \a restrictUnsigned => RestrictUnsigned;
/ \a restrictUnsignedRegular => RestrictUnsignedRegular;
/ \a restrictLessEqual => RestrictLessEqual;
/ @@ \u YGDI:
	* \impl @@ \f
		bool DrawHLineSeg(const Graphics&, SPOS, SPOS, SPOS, Color);
	* \impl @@ \f
		bool DrawVLineSeg(const Graphics&, SPOS, SPOS, SPOS, Color);
	/ \impl @@ \f
		bool DrawLineSeg(const Graphics&, SPOS, SPOS, SPOS, SPOS, Color);
	/ @@ \cl Padding:
		- \exp \ctor Padding(u64);
		/ 3 \mf SetAll -> !\m \f Drawing::SetAll with 1st \parm Padding&;
		/ \mf u64 GetAll() const -> !\m \f Drawing::GetAll(const Padding&);
		/ \mf SDST GetHorizontal() const
			-> !\m \f SDST Drawing::GetHorizontal();
		/ \mf SDST GetVertical() const
			-> !\m \f SDST Drawing::GetVertical();
		/ \decl friend Padding operator+(const Padding& a, const Padding& b)
			-> !\m !friend;
/ @@ \u YText:
	/ @@ \cl YTextRegion:
		/ \mf DefGetter(SDST, BufWidthN, Width - Margin.GetHorizontal())
			-> DefGetter(SDST, BufWidthN, Width - GetHorizontal(Margin));
		/ \mf DefGetter(SDST, BufHeightN, Height - Margin.GetVertical())
			-> DefGetter(SDST, BufHeightN, Height - GetVertical(Margin));
	/ @@ \cl YTextState:
		/ !\i \mf SDST GetLnHeight() const
			-> \i !\m \f Drawing::GetLnHeight(const TextState&);
		/ !\i \mf SDST GetLnHeightEx() const
			-> \i !\m \f Drawing::GetLnHeightEx(const TextState&);
		/ !\i \mf SDST GetLnHeightEx() const
			-> \i !\m \f Drawing::GetLnNNow(const TextState&);
		/ 3 \mf SetMargins
			-> !\m \f Drawing::SetMargins with 1st \parm TextState&;
		/ 2 \mf SetPen
			-> !\m \f Drawing::SetPens with 1st \parm TextState&;
		/ \mf void SetLnNNow(u16)
			-> !\m \f void Text::SetLnNNow(TextState&, u16);
		- \mf
		{
			DefGetter(SPOS, PenX, penX);
			DefGetter(SPOS, PenY, penY);
			DefSetter(SPOS, PenX, penX);
			DefSetter(SPOS, PenY, penY);
			DefGetter(u8, LineGap, lnGap);
			DefSetter(u8, LineGap, lnGap);
		}
		/ \ac @@ \m penX & penY & lnGap -> public ~ protected;
	/= \cl TextState -> \st TextState;
/ \impl @@ \u DSReader;
/ a penX => PenX;
/ a penY => PenY;
/ a lnGap => LineGap;
/ \tr \impl @@ \u YWidget;

r7:
* \impl @@ \mf void YListBox::DrawForeground();

r8:
* \impl @@ \f bool
	DrawRect(const Graphics&, const Point&, const Size&, Color) @@ \u YGDI;

r9:
/ \simp @@ \cl BitmapBuffer @@ \u YGDI:
	- \mf
	{
		DefGetter(SDST, Width, Width);
		DefGetter(SDST, Height, Height);
	};
/ @@ \u YCoreUtilities:
	/ @@ \cl Size 
		- \mf
		{
			DefGetter(SDST, Width, Width);
			DefGetter(SDST, Height, Height);
			DefGetter(u32, Area, Width * Height)。
		};
	+ \f u32 GetArea(const Size&);
	- \mf DefGetterBase(u32, Area, Size) @@ \cl YGraphicDevice;
/ \a MBitmapBuffer => BitmapBuffer;
/ \a MBitmapBufferEx => BitmapBufferEx;
/ \impl @@ \mf DrawForeground & \mf SetThumbPosition @@ \cl AScrollBar;

r10:
* \impl @@ \ctor @@ \mf AScrollBar;

r11:
	/ \f void RectDrawButton(const Graphics&, const Point&, const Size&,
		bool = false) -> void RectDrawButton(const Graphics&, const Point&,
		const Size&, Color = Color(232. 240, 255), bool = false);

r12:
/ @@ \ns platform @@ \u YCommon:
	/ @@ \cl Color;
/ \a scrCopy => ScreenSychronize;
/ \impl @@ \ns \u YGUIComponent:
	/ \f void RectDrawButton(const Graphics&, const Point&, const Size&,
		Color = Color(232. 240, 255), bool = false) -> void
		RectDrawButton(const Graphics&, const Point&, const Size&,
		bool = false, Color = Color(232. 240, 255));
	/ \tr \impl @@ \mf YHorizontalScrollBar::DrawForeground();

r13:
/ \impl @@ \ns \u YGUIComponent:
	/ \f void RectDrawButton(const Graphics&, const Point&, const Size&,
		bool = false, Color = Color(232. 240, 255)) -> void
		RectDrawButton(const Graphics&, const Point&, const Size&,
		bool = false, Color = Color(48, 216, 255));

r14:
/ @@ \cl MScrollBar @@ \u YControl:
	- \m
	{
		bool bPrevButtonPressed;
		bool bNextButtonPressed;
	}
	+ \m
	{
	public:
		typedef enum
		{
			None = 0,
			PrevButton = 1,
			NextButton = 2,
			PrevTrack = 3,
			NextTrack = 4,
			Thumb = 5
		} CompIndex;

	protected:
		CompIndex Pressed;

	public:
		DefGetter(CompIndex, PressedState, Pressed)
	}
/ \tr \impl @@ \mf void YHorizontalScrollBar::DrawForeground()
	@@ \u YGUIComponent;
/ \a GetLocationForWindow => LocateForWindow;
/ \a GetLocationOffset => LocateOffset;
/ \a GetContainerLocationOffset => LocateContainerOffset;
/ \a GetWindowLocationOffset => LocateWindowOffset;
/ \a GetLocationForParentContainer => LocateForParentContainer;
/ \a GetLocationForParentWindow => LocateForParentWindow;

r15:
/ \simp @@ \u YGUI;
/ \simp:
	/ code "* == NULL" -> "!(*)";

r16:
/ @@ \u YWidget:
	+ \in IUIBox \vt \inh \in IWidget;
	/ \inh @@ \in IUIContainer -> \vt IUIBox ~ \vt IWidget;
	/ \decl @@ \in IWidget:
		/ DeclIEntry(IUIContainer* GetContainerPtr() const)
			-> DeclIEntry(IUIBox* GetContainerPtr() const);
	/ @@ \cl MVisual:
		/ public \m IUIContainer* const pContainer
			-> private \m IUIBox* pContainer;
/ \tr impl @@ \ctor & \mf GetContainer & \mf BelongsTo @@ \dir Shell;
/ @@ \u YGUI:
	/ \a IUIContainer -> IUIBox;
/ @@ \h YShellDefinition:
	+ PDeclInterface(IUIBox) @@ \ns Components::Widgets;
	+ using Components::Widgets::IUIBox;

r17-r18:
* \tr \impl @@ \mf YListBox::DrawForeground();

r19:
/ \simp @@ \ctor @@ \u (YWidget & YControl & YWindow);
/= \tr @@ \u YComponent;

r20:
/ \a IWidgetContainer => IUIContainer;
/ \a IWidgetBox => IUIBox;
/ \a YWidgetContainer => YUIContainer;
/ \a MWidgetContainer => MUIContainer;
/ @@ \u YWidget:
	+ \f bool Contains(const IWidget&, const Point&);
	+ \i \f bool Contains(const IWidget&, SPOS, SPOS);
	- \amf DeclIEntry(bool Contains(const Point&) const) @@ \in IWidget;
	/ \tr \impl @@ \mf IWidget*
		MUIContainer::GetTopWidgetPtr(const Point& pt) const;
	/ \tr \impl @@ \mf IVisualControl*
		MUIContainer::GetTopVisualControlPtr(const Point& pt) const;
- \a @@ \mf (bool Contains(const Point&) const & bool
	Contains(SPOS, SPOS) const) @@ \u (YWidget & YControl & YWindow & YDesktop);
/ \tr \impl @@ \mf HWND
	YShell::GetTopWindowHandle(YDesktop& d, const Point& p) const @@ \u YShell;

r21-r23:
/ @@ \u YGUIComponent:
	/ @@ \cl AScrollBar:
		+ \inh \in IUIBox;
	/ \rem \cl AScrollBar;
	/ \rem \cl YHorizontalScrollBar;
	/ \rem \cl YVerticalScrollBar;
	+ \cl ATrack;
	+ \cl YHorizontalTrack;
	+ \cl YThumb;
	/ \cl YButton;
		/ ^ \inh YThumb;
/ \tr @@ \u Shells;
/ \ac @@ \a \mf DrawForeground & \mf DrawBackground => public ~ protected;
- \c \q @@ \a \mf GetTopWidgetPtr & \mf GetTopVisualControlPtr
	@@ \u YControl & \u YWindow;

r24:
/ \tr \impl @@ \ctor @@ \cl ATrack @@ \u YGUIControl;

r25-r29:
/= test 2;

r30:
* - \c \q @@ \a \mf GetTopWidgetPtr & \mf GetTopVisualControlPtr @@ \u YDesktop;

r31:
/ \tr @@ \u YGUIComponent;

r32-r33:
/= test 3;

r34:
/ \cl ATrack & \cl YHorizontalTrack @@ \u YGUIComponent;
/ @@ \cl AVisualControl @@ \u YControl:
	/ + \i @@ !\i \mf void OnLostFocus(const Runtime::EventArgs&);
	/ \mf void OnKeyHeld(const Runtime::MKeyEventArgs&)
		-> !\m \f
			Controls::OnKeyHeld(IVisualControl&, const Runtime::MKeyEventArgs&);
	/ \mf void OnTouchHeld(const Runtime::MTouchEventArgs&)
		-> !\m \f Controls::OnTouchHeld(IVisualControl&,
			const Runtime::MTouchEventArgs&);
	/ \mf void OnTouchMove(const Runtime::MTouchEventArgs&)
		-> !\m \f Controls::OnTouchMove(IVisualControl&,
			const Runtime::MTouchEventArgs&);
	/ \tr \impl @@ \ctor;
/ \tr \impl @@ \u Shells;
/ @@ \h YEvent:
	+ \mf void Clear() @@ \cl \t GEvent<true> & \t GEvent<false>;
	+ \rem @@ \mf @@ \cl \t GEvent<false>;
	+ 3 operator= @@ \cl \t GEvent<true>;
/ tr \impl @@ \ctor @@ \u YGUIComponent;

r35-r38:
/ \cl ATrack & \cl YHorizontalTrack @@ \u YGUIComponent;
	/ \impl @@ \mf;
	/ \ac @@ ATrack::Thumb -> protected ~ private;

r39:
/ @@ \h YEvent:
	/ @@ \cl \t GHEventNormal:
		- \inh GHBase<_pfEventHandler>;
		/ ^ (std::pointer_to_binary_function & std::ptr_fun);
		- void \parm \ctor;

r40:
/ @@ \h YFunc:
	- \mf empty @@ \cl \t GHBase;
	+ \inh <functional>
	+ \cl \t GHDynamicFunction;
	+ 2 helper \f \t GetDynamicFunction;
	+ \inh <typeinfo>
/ @@ \h YEvent:
	/ @@ \cl \t GHEventNormal:
		/ ^ (GHDynamicFunction & GetDynamicFunction)
			~(std::pointer_to_binary_function & std::ptr_fun);
		- void \parm \ctor;

r41:
/ @@ \h YEvent:
	/ @@ \cl \t GHEventNormal:
		/ ^ (std::pointer_to_binary_function & std::ptr_fun)
			~(GHDynamicFunction & GetDynamicFunction);

r42:
/ @@ \h YEvent:
	/ @@ \cl \t GEventHandler:
		+ typedef typename GHEventNormal<_tSender, _tEventArgs,
			_fEventHandler, _pfEventHandler>::FunctorType FunctorType;
		* \tr \ctor;
	/ @@ \cl \t GEvent:
		+ typedef typename _tEventHandler::FunctorType FunctorType;
		+ \i \mf GEvent& operator=(const FunctorType&);
		+ \i \mf GEvent& operator+=(const FunctorType&);
		+ \i \mf GEvent& operator-=(const FunctorType&);

r43:
/ @@ \h YEvent:
	+ template<_tSender = YObject, class _tEventArgs = EventArgs>
		struct SEventType;
	/ simp \cl \t ^ SEventType;

r44-r49:
/ h YEvent:
	+ \st \t ExpandMemberFirst;
	+ \st \t ExpandMemberFirstBinder;
	/ \cl \t GEventHandler:
		+ protected \m Design::Function<FuncType> func;
		- protected \m IEventHandlerType* _h_ptr;
		- \mf Clone;
		- \mf GetSizeOf;
		- \inh GIEventHandler;
		+ typedef _tSender SenderType;
		+ typedef _tEventArgs EventArgsType;
		/ @@ operator=:
			/ \impl;
			+ \i;			
	- \cl \t GHEventNormal;
	- \cl \t GHEventMember;
	- \cl \t GHEventMemberBinder;
	- \f \t template<class _tSender, class _tEventArgs> bool operator==(
		GIEventHandler<_tSender, _tEventArgs>& l,
		GIEventHandler<_tSender, _tEventArgs>& r);
	/ @@ \t GEvent<true>
		/= \tr typedef;
		/ \i \mf GEvent& AddRaw(const EventHandlerType&);
		/ \impl @@ \mf operator= ^ \mf AddRaw @@ \mf operator=;
	- \in \t GIEventHandler;
	- \in IEventHandler;
	+ \mac LOKI_FUNCTORS_ARE_COMPARABLE @@ \h YAdaptor;

r50-r51:
/ @@ \h YEvent:
	/ \cl \t GEventHandler:
		/ protected \m Design::Function<FuncType> func
			-> public \inh Design::Function<protected Design::Function<
			typename SEventTypeSpace<_tSender, _tEventArgs>::FuncType>;
		/ \impl @@ operator()(const _tSender, const _tEventArgs&);
		- copy \ctor;
		/ @@ operator=:
			+ \vt;
			/ \impl;

r52:
* \h YAdaptor;
/ @@ \h YEvent:
	+ \mf operator== @@ \cl \t (ExpandMemberFirst & ExpandMemberFirstBinder);

r53-r63:
/ test 4;

r64:
/ @@ \h YEvent:
	+ \rem;
	+ \mf typename ListType::size_type GetSize() const @@ \cl \t GEvent<true>;
	/ \a EventHandlerList => List;

r65:
/= test 5;

r66:
/ @@ \u YGUIComponent:
	/ @@ \cl YHorizontalTrack:
		- \mf OnThumbTouchMove(IVisualControl&, const MTouchEventArgs&);
		+ \cl OnTouchMove_Thumb;
		/ \impl @@ \ctor;
/ @@ \h YEvent:
	* - typedef GEventHandler<> EventHandler for \amb;
		template<class _tFunc>
	/ @@ \cl \t GEventHandler:
		+ \i \ctor GEventHandler(_tFunc f);
		- \exp @@ \a \ctor;

r67:
/ @@ \u YGUIComponent:
	/ @@ \cl YHorizontalTrack:
		* operator() @@ \cl OnTouchMove_Thumb;
	+ \st \t template<class _tFunc, typename _tPara>
		struct InversedCurrying;
/ \a _tPara => _tParm;
/ \st \t (ExpandMemberFirst & ExpandMemberFirstBinder & InversedCurrying)
	>> \h YFunc;

r68-r69:
* \impl @@ \mf void YHorizontalTrack::OnTouchMove_Thumb::operator()(
	IVisualControl&, const MTouchEventArgs&) const;

r70:
* \f @@ \u YGDI:
{
	bool DrawHLineSeg(const Graphics&, SPOS, SPOS, SPOS, Color);
	bool DrawVLineSeg(const Graphics&, SPOS, SPOS, SPOS, Color);
	bool DrawRect(const Graphics&, const Point&, const Size&, Color);
}

r71:
* \tr \impl @@ \mf void YHorizontalTrack::OnTouchMove_Thumb::operator()(
	IVisualControl&, const MTouchEventArgs&) const;

r72:
* \f @@ \u YGUI:
{
	void DrawWindowBounds(HWND, Color);
	void DrawWidgetBounds(IWidget&, Color);
}

r73:
* \impl @@ \mf IVisualControl* ATrack::GetTopVisualControlPtr(const Point&);

r74:
/ simp @@ \cl YHorizontalTrack @@ \u YGUIComponent:
	/ \f \o \st OnTouchMove_Thumb
		-> \mf OnTouchMove_Thumb(const Runtime::MTouchEventArgs&);
	- \smf void OnThumbTouchHeld(IVisualControl&,
		const Runtime::MTouchEventArgs&);
	/ \tr \impl @@ \ctor;

r75:
/ @@ \u YGUIComponent:
	/ !\rem \cl AScrollBar;
	/ !\rem \cl YHorizontalScrollBar;
	/ !\rem \cl YVerticalScrollBar;

r76:
/ \ac @@ \a \inh @@ modules ~ (MVisual & Widgets::MWidget & Widgets::MLabel)
	-> protected ~ public;
/ \a MEventArgs => EventArgs;
/ \a MScreenPositionEventArgs => ScreenPositionEventArgs;
/ \a MInputEventArgs => InputEventArgs;
/ \a MTouchEventArgs => TouchEventArgs;
/ \a MKeyEventArgs => KeyEventArgs;
/ \a MEventArgs => EventArgs;

r77:
/ @@ \cl MScrollBar => \u YGUIComponent ~ \u Control;
/ @@ \u YGUIComponent:
	/ @@ \cl MScrollBar:
		+ protected \m YThumb Prev;
		+ protected \m YThumb Next;
		- protected \m (PrevButtonSize & NextButtonSize);
		- public \mf (GetPrevButtonSize & GetNextButtonSize);
		/ protected \m MinThumbSize => MinThumbLength;
		/ DefGetter(SDST, MinThumbSize, MinThumbSize)
			-> DefGetter(SDST, MinThumbLength, MinThumbLength);
		/ \ctor MScrollBar(SDST = 8, SDST = 16, SDST = 16)
			-> MScrollBar(HWND, IUIBox*, SDST, const Rect&, const Rect&);
	/ @@ \cl AScrollBar:
		- \cl ScrollBarButton;
		/ \inc AVisualControl -> \inc ATrack;
		- \inc \in IUIBox;
		/ typedef AVisualControl ParentType -> typedef ATrack ParentType;
		- \m SDST ThumbPosition & SDST ThumbSize;
		/ \impl @@ \ctor;
		- \mf DefGetter(SDST, ThumbSize, ThumbSize)
			& DefGetter(SDST, ThumbPosition, ThumbPosition);
		/ \mf void SetThumbPosition(SDST) -> \amf;
	/ @@ \cl ATrack:
		/ \tr \impl @@ \ctor;
		+ \i \amf void SetThumbLength(SDST);
	+ \exp @@ \ctor;
	+ 1st \de \parm "HWND = NULL" @@ \ctor @@ \cl (YHorizontalScrollBar
		& YListBox & YFileBox & YThumb & ATrack & YHorizontalTrack);
	+ \exp @@ \ctor @@ \cl (YListBox & YFileBox & YThumb & ATrack
		& YHorizontalTrack);
	+ \cl YVerticalTrack;
	/ \cl YHorizontalScrollBar;
	+ \mf ImplI(ATrack) void SetThumbLength(SDST) @@ \cl YHorizontalTrack;
	/ \m MinThumbLength @@ \cl MScrollBar >> \cl ATrack;
	/ \mf DefGetter(SDST, MinThumbLength, MinThumbLength) @@ \cl MScrollBar
		>> \cl ATrack;
	/ \ctor @@ \cl MScrollBar & \cl ATrack;
/ \tr \impl @@ \u Shells;
/ @@ \u YWidget:
	+ 1st \de \parm "HWND = NULL" @@ \ctor @@ \cl YUIContainer;

r78:
* \tr \impl @@ \u Shells;


$DOING:


/ ...

$NEXT:

* fatal \err;
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

