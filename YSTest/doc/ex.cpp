//v 0.2935; *Build 177 r81;
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
/ @@ \u YWindow:
	/ @@ \cl MWindow:
		/ protected \m Drawing::BitmapBuffer Buffer => \cl YFrameWindow;
		/ \impl @@ \exp \ctor;
	/ @@ \cl AWindow:
		- \vt \mf DefGetterMember(BitmapPtr, BufferPtr, Buffer)
			=> \cl YFrameWindow;
		/ \vt \mf DefGetter(const Drawing::BitmapBuffer&, Buffer, Buffer) 
			-> ImplI(AWindow) \mf @@ \cl YFrameWindow;
		+ \amf DeclIEntry(BitmapPtr GetBufferPtr() const ythrow());
		+ \amf DeclA(IWindow) DeclIEntry(operator const Graphics&()
			const ythrow())
		/ \mf ImplI(IWindow) DefConverter(const Graphics&, Buffer)
			-> ImplA(AWindow) DefConverter(const Graphics&, Buffer);
		+ \amf DeclIEntry(const Drawing::BitmapBuffer& GetBuffer()
			const ythrow());
		/ \tr \impl @@ \i \mf (ClearBackground & BeFilledWith()
			& SetSize & DrawBackgroundImage & UpdateToScreen & UpdateToWindow);
		+ \amf DeclIEntry(Drawing::BitmapBuffer& GetBuffer() ythrow());
	/ @@ \cl YFrameWindow:
		/ \tr \impl @@ \exp \ctor;
		+ \mf ImplI(AWindow) Drawing::BitmapBuffer& GetBuffer() ythrow();
/ \tr \impl @@ \ctor @@ \cl MDualScreenReader @@ \u DSReader;
/ \impl @@ \mf (DrawBackground & Update) @@ \cl YDesktop @@ \u YDesktop;

r2:
/ \mf DefGetter(std::size_t, SizeOfBuffer,
	sizeof(PixelType) * GetAreaFrom(Size)) @@ \cl BitmapBuffer @@ \h YGDI
	>> \cl Graphics @@ \h YObject;

r3:
/ \tr \impl @@ \mf AWindow::DrawBackgroundImage @@ \u YWindow;

r4:
/ @@ \u YGDI:
	+ \f void CopyBuffer(const Graphics&,
		BitmapPtr, ROT, const Drawing::Size&,
		const Point&, const Point&, const Drawing::Size&);
	+ \f void CopyBuffer(const Graphics&, const Graphics&);
	/ @@ \cl BitmapBuffer:
		/ \simp \impl @@ \vt \mf void
			CopyToBuffer(BitmapPtr, ROT = RDeg0,
			const Drawing::Size& = Drawing::Size::FullScreen,
			const Point& = Point::Zero, const Point& = Point::Zero,
			const Drawing::Size& = Drawing::Size::FullScreen) const
			^ \f CopyBuffer;
		/ \simp \impl @@ \vt \mf void ClearImage() const;
	+ \f void ClearImage(const Graphics&);
/ @@ \cl AWindow @@ \u YWindow:
	/ \impl @@ \mf void UpdateToScreen(YDesktop&) const;
	/ \impl @@ \mf void UpdateToWindow(IWindow&) const;
	- \amf DeclIEntry(const Drawing::BitmapBuffer& GetBuffer() const ythrow());
	/ \impl @@ \mf ClearBackground;
	- \mf const BitmapBuffer& GetBuffer() const;

r5:
/ @@ \u YGDI:
	/ \f void CopyBuffer(const Graphics&, BitmapPtr, ROT, const Drawing::Size&,
		const Point&, const Point&, const Drawing::Size&);
		-> void CopyBuffer(BitmapPtr, const Graphics&, ROT,
		const Drawing::Size&, const Point&, const Point&, const Drawing::Size&);
	/ \impl @@ \i \mf BitmapBuffer::CopyToBuffer(BitmapPtr, ROT,
		const Drawing::Size&, const Point&, const Point&,
		const Drawing::Size&) const;
/ \tr \impl @@ \mf (CopyBufferToWindow & CopyBufferToScreen)
	@@ \cl AWindow @@ \u YWindow;
/ \a CopyBuffer => CopyToBuffer;

r6:
/ @@ \u YGDI:
	+ \f void CopyToBuffer(const Graphics&, const Graphics&, ROT = RDeg0);
	/ \simp \impl @@ \f void CopyToBuffer(BitmapPtr, const Graphics&,
		ROT, const Size&, const Point&, const Point&, const Size&);

r7:
* void CopyToBuffer(const Graphics&, const Graphics&) => CopyBuffer @@ \u YGDI;
/ \simp \impl @@ \mf (void UpdateToScreen(YDesktop&)
	& void UpdateToWindow(IWindow&)) @@ \cl AWindow @@ \u YWindow;

r8:
* \f void CopyToBuffer(const Graphics&, const Graphics&, const Point&,
	ROT = RDeg0) -> void CopyToBuffer(const Graphics&, const Graphics&,
	const Point& = Point::Zero, ROT = RDeg0) @@ \u YGDI;
* \impl @@ \mf (void UpdateToScreen(YDesktop&)
	& void UpdateToWindow(IWindow&)) @@ \cl AWindow @@ \u YWindow;

r9:
* \impl @@ void CopyToBuffer(const Graphics&, const Graphics&,
	const Point& = Point::Zero, ROT = RDeg0) @@ \u YGDI;

r10:
/ @@ \cl AWindow @@ \u YWindow:
	/ \vt \mf void UpdateToWindow(IWindow&) const
		-> \mf ImplI(AWindow) void UpdateTo(const Graphics&,
		const Point& = Point::Zero) const;
	/ \vt \mf UpdateToScreen -> \mf ImplI(AWindow) UpdateToDesktop;

r11:
/ @@ \u YWidget:
	- \a \c @@ \param @@ \a (\f & \ft) 'Fetch*Direct*';
	/ \impl @@ \ft<class _tWidget> IWindow*
		FetchWidgetDirectWindowPtr(_tWidget*);
	/ \impl @@ \ft<class _tWidget> IUIBox* FetchDirectContainerPtr(_tWidget&);
	/ \impl @@ \f YDesktop* FetchWidgetDirectDesktopPtr(IWidget*);
	/ \mf void UpdateToDesktop() const
		-> void UpdateToDesktop() @@ \cl AWindow @@ \u YWindow;
	- \mf DefGetter(HWND, Handle, HWND(const_cast<AWindow*>(this)));

r12:
- \cl MDesktopObject @@ \h YWindow;

r13:
/ @@ \u YWindow:
	+ \cl AFrameWindow;
	+ \inh AWindow @@ AFrameWindow;
	/ protected Widgets::MUIContainer & public \mf
	{
		virtual PDefHOperator(void, +=, IWidget& w)
			ImplExpr(sWgtSet += w)
		virtual PDefHOperator(bool, -=, IWidget& w)
			ImplRet(sWgtSet -= w)
		virtual PDefHOperator(void, +=, IVisualControl& c)
			ImplBodyBaseVoid(MUIContainer, operator+=, c)
		virtual PDefHOperator(bool, -=, IVisualControl& c)
			ImplBodyBase(MUIContainer, operator-=, c)
		virtual PDefHOperator(void, +=, GMFocusResponser<IVisualControl>& c)
			ImplBodyBaseVoid(MUIContainer, operator+=, c)
		virtual PDefHOperator(bool, -=, GMFocusResponser<IVisualControl>& c)
			ImplBodyBase(MUIContainer, operator-=, c)
		ImplI(IWindow) PDefH(IVisualControl*, GetFocusingPtr)
			ImplBodyBase(GMFocusResponser<IVisualControl>, GetFocusingPtr)
		ImplI(IWindow) PDefH(IWidget*, GetTopWidgetPtr, const Point& p)
			ImplBodyBase(MUIContainer, GetTopWidgetPtr, p)
		ImplI(IWindow) PDefH(IVisualControl*, GetTopVisualControlPtr,
			const Point& p)
		ImplBodyBase(MUIContainer, GetTopVisualControlPtr, p)
		ImplI(IWindow) PDefH(void, ClearFocusingPtr)
			ImplBodyBaseVoid(MUIContainer, ClearFocusingPtr)
		ImplI(IWindow) PDefH(bool, ResponseFocusRequest, AFocusRequester& w)
			ImplBodyBase(MUIContainer, ResponseFocusRequest, w)
		ImplI(IWindow) PDefH(bool, ResponseFocusRelease, AFocusRequester& w)
			ImplBodyBase(MUIContainer, ResponseFocusRelease, w)
	} @@ \cl YFrameWindow >> AWindow;
	/ \inh AWindow -> \inh AFrameWindow @@ \cl YFrameWindow;
	/ \impl (\ctor & \dtor) @@ \cl (AFrameWindow & YFrameWindow);

r14:
/ @@ \h YWindow:
	/ @@ \cl AWindow:
		- \amf ImplA(IWindow)
			DeclIEntry(operator const Graphics&() const ythrow());
		-= ImplA(IWindow) @@ \amf DeclIEntry(bool DrawWidgets());
- \amf ImplA(IVisualControl) DeclIEntry(void RequestToTop())
	@@ \cl AVisualControl @@ \u YControl;

r15:
- \mf ImplI(AWindow) DefGetter(const Drawing::BitmapBuffer&, Buffer, Buffer)
	@@ \cl YFrameWindow @@ \h YWindow;

r16:
/ @@ \h YWidget:
	/ typedef \en Orientation @@ \ns Widgets >> \ns Drawing @@ \h YGDI;
	/ \tr \impl @@ \cl MOriented;
/ \a Widgets::Orientation -> Orientation;
/ \a ROT -> Rotation;
/ \a Widgets::Horizontal -> Horizontal;
/ \a Widgets::Vertical -> Vertical;

r17:
/ @@ \u YWidget:
	/ @@ \cl Visual:
		/ \impl @@ 2 \mf SetSize;
		/ \i \mf void SetBounds(const Rect& r)
			-> !\m !\i \f void SetBoundsOf(IWidget&, const Rect&);
		/ \i \mf DefGetter(Rect, Bounds, Rect(GetLocation(), GetSize()))
			-> !\m !\i \f void GetBoundsOf(IWidget&, const Rect&);
	+ \amf DeclIEntry(void SetSize(const Size&)) @@ \in IWidget;
	+ ImplI(IWidget) DefSetterBase(const Size&, Size, Visual) @@ \cl YWidget;
	+ ImplI(IUIContainer) DefSetterBase(const Size&, Size, Visual)
		@@ \cl YUIContainer;
+ ImplI(IVisualControl) DefSetterBase(const Size&, Size, Visual)
	@@ \cl AVisualControl @@ \h YControl;
- 8 ythrow() @@ \impl @@ \mac 'Def*Setter*' @@ \h Base @@ \dir Adaptor;
+ \mf DefGetter(Size, Size, Size(Width, Height)) @@ \cl Rect @@ \h YObject;

r18-r21:
/ \tr \impl @@ \ctor MDualScreenReader @@ \u DSReader;
/ \tr \impl @@ \mf (DrawBackground & Update) @@ \cl YDesktop @@ \u YDesktop;

r22:
/ @@ \u YGDI:
	+ \f void Fill(const Graphics&, Color);
	/ \impl @@ \vt \mf void BeFilledWith(Color) @@ \cl BitmapBuffer;

r23:
* \impl @@ \f void Fill(const Graphics&, Color) @@ \u YGDI;

r24:
/ @@ \u YWindow:
	/ @@ \cl AWindow:
		/ \amf DeclIEntry(Drawing::BitmapBuffer& GetBuffer() ythrow())
			-> \amf DeclIEntry(void SetBufferSize(const Size&));
		/ \impl @@ \vt \mf SetSize;
		/ \mf void BeFilledWith(PixelType)
			-> \mf void beFilledWith(PixelType) const;
		/= \tr \impl @@ \mf DrawBackgroundImage;
	/ \mf ImplI(AWindow) Drawing::BitmapBuffer& GetBuffer() ythrow()
		@@ \cl YFrameWindow -> \i \mf void SetBuffer(const Size&);

r25:
/ \m Drawing::Size Size => Size size @@ \cl Graphics @@ \u YObject;
/ @@ \u YGDI:
	/ \simp \decl;
	/ @@ \cl BitmapBufferEx:
		/ \mf DefGetter(std::size_t, SizeOfBufferAlpha,
			sizeof(u8) * GetAreaFrom(Size))
			-> DefGetter(std::size_t, SizeOfBufferAlpha,
			sizeof(u8) * GetAreaFrom(GetSize()));
		* \impl @@ \ctor;
		/ \impl @@ \mf (SetSize & ClearImage);
	/ \impl @@ (\ctor & \mf (SetSize & SetSizeSwap)) @@ \cl BitmapBuffer;

r26:
/= test 1;

r27:
* \impl @@ \mac (YWindowAssert & YWidgetAssert) @@ \h YWidget;

r28-r36:
/= test 2;

r37:
/ @@ \h YWindow:
	/ \amf operator const Graphics&() const @@ \in IWindow
		-> \amf const Graphics& GetContext() const;
	/ ImplI(AWindow) DefConverter(const Graphics&, Buffer) @@ \cl YFrameWindow
		-> ImplI(AWindow) DefGetter(const Graphics&, Context, Buffer);
	/ \tr \impl @@ \mf (ClearBackground & BeFilledWith) @@ \cl AWindow;
/ \tr \impl @@ \ctor @@ \cl MDualScreenReader @@ \u DSReader;
/ \impl @@ 2 \mac YWindowAssert 
/ #undef YWindowAssert & 2 \mac YWindowAssert >> \h YWindow ~ \h YWidget;
/ \f yassert @@ \ns Components::Widgets >> \ns Components @@ \u YWidget;
/ \tr \impl @@ \mf (Update & DrawBackground) @@ \cl YDesktop @@ \u YDesktop;
/ \tr \impl @@ \f DrawWidgetBounds @@ \un \ns @@ \u YGUI;
/ \tr \impl @@ 5 \mf DrawForeground @@ \u YGUIComponent;
/ 2 \tr \impl @@ \u YWidget;
/ 4 \tr \impl @@ 3 \mf @@ \cl AWindow @@ \u YWindow;

r38-r60:
/= test 3;

r61:
* \simp \impl @@ \mf void AWindow::Update() @@ \u YWindow;

r62-r69:
/= test 4;

r70:
/ (\impl & - \i) @@ \mf void AVisualControl::OnLostFocus(EventArgs&)
	@@ \u YControl;
* \tr \impl @@ \mf YDesktop::Refresh() @@ \u YDesktop;
* \tr \impl @@ \mf AWindow::Refresh() @@ \u YWindwo;

r71-r76:
/= test 5;

r77-r78:
* \impl @@ \mf Widget::Refresh @@ \u YWidget;

r79:
/ \a MAX_FILENAME_LENGTH => YCL_MAX_FILENAME_LENGTH;
/ \a MAX_PATH_LENGTH => YCL_MAX_PATH_LENGTH;
+ \f (mmbset & mmbcpy) @@ \u YCommon;
+ {
	using platform::mmbset;
	using platform::mmbcpy;
} @@ \h YAdaptor;
/ \a std::memset @@ \a \dir \exc (\h & \u) YCommon -> mmbset;

r80:
/ \a std::memcpy @@ \lib YSLib -> mmbcpy;

r81:
/ GHStrong<ListType> pList => spList @@ \cl FileList @@ \u YFileSystem;
* \impl @@ \mf void YSimpleTextListBox::CallConfirmed(YSimpleTextListBox
	::ViewerType::IndexType) @@ \u YGUIComponent;

$DOING:


relative process:
2010-12-31:
-20.0d;

/ ...


$NEXT:
b178-b195:
/ scroll bars @@ listbox;
/ fully \impl \u DSReader;
	* moved text after setting lnGap;
* non-ASCII character path error in FAT16;

r196-r288:
+ \impl loading pictures;
+ \impl style on widgets;
/ user-defined bitmap buffer @@ \cl YDesktop;
/ general component operations:
	/ serialization;
	/ designer;
/ database interface;

$NOTHING_TODO:
* fatal \err @@ b16x:
F:\Programing\GadgetLib>F:\devkitPro\devkitARM\bin\arm-eabi-addr2line.exe -f -C
	-e F:\Programing\NDS\YSTest\YSTest\arm9\YSTest.arm9.elf -s -i 02055838
	ftc_snode_weight
	ftcsbits.c:271
* screen font output uncleared @@ real machine;

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

//! \brief 桌面对象模块。
class MDesktopObject
{
protected:
	GHHandle<YDesktop> hDesktop; //!< 桌面句柄。

public:
	//! \brief 构造：使用指定桌面句柄。
	explicit
	MDesktopObject(GHHandle<YDesktop>);

protected:
	DefEmptyDtor(MDesktopObject)

public:
	//判断从属关系。
	PDefH(bool, BelongsTo, GHHandle<YDesktop> hDsk) const
		ImplRet(hDesktop == hDsk)

	DefGetter(GHHandle<YDesktop>, DesktopHandle, hDesktop)
};

inline
MDesktopObject::MDesktopObject(GHHandle<YDesktop> hDsk)
	: hDesktop(hDsk)
{}

class RefCountedImpl
{
public:
	typedef uintptr_t CountType;

private:
	CountType* pCount;

protected:
	RefCountedImpl()
		: pCount_(static_cast<CountType*>(
		SmallObject<>::operator new(sizeof(CountType))))
	{
		assert(pCount_ != NULL);
		*pCount_ = 1;
	}

	inline
	RefCountedImpl(const RefCountedImpl& rhs)
		: pCount_(rhs.pCount_)
	{}

	inline CountType
	Clone()
	{
		return ++*pCount;
	}

	bool
	Release()
	{
		if(--*pCount_ == 0)
		{
			SmallObject<>::operator delete(pCount_, sizeof(CountType));
			pCount_ = NULL;
			return true;
		}
		return false;
	}

	void
	Merge(RefCountedImpl& rhs)
	{
		CountType count(*pCount_);

		SmallObject<>::operator delete(pCount_, sizeof(CountType));
		pCount_ = rhs.pCount_;
		*pCount_ += count;
		return true;
	}
};
*/

