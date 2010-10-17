// YSLib::Shell::YControl by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-02-18 13:44:24 + 08:00;
// UTime = 2010-10-17 22:45 + 08:00;
// Version = 0.3939;


#ifndef INCLUDED_YCONTROL_H_
#define INCLUDED_YCONTROL_H_

// YControl ：平台无关的控件实现。

#include "ywidget.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

using namespace Drawing;

YSL_BEGIN_NAMESPACE(Controls)

//控件事件参数类型。
struct MIndexEventArgs : public MEventArgs
{
	typedef std::ptrdiff_t IndexType;

	IVisualControl& Control;
	IndexType Index;

	MIndexEventArgs(IVisualControl& c, IndexType i)
	: MEventArgs(),
	Control(c), Index(i)
	{}
};


//控件事件类型。
typedef Runtime::GEvent<true, IControl, MEventArgs> YControlEvent;


//事件处理器类型。
DefDelegate(InputEventHandler, IVisualControl, Runtime::MInputEventArgs)
DefDelegate(KeyEventHandler, IVisualControl, Runtime::MKeyEventArgs)
DefDelegate(TouchEventHandler, IVisualControl, Runtime::MTouchEventArgs)
DefDelegate(IndexEventHandler, IVisualControl, MIndexEventArgs)


//可视控件事件空间。
struct EControl
{
	typedef enum EventSpace
	{
	//	AutoSizeChanged,
	//	BackColorChanged,
	//	ForeColorChanged,
	//	LocationChanged,
	//	MarginChanged,
	//	VisibleChanged,

	//	EnabledChanged,
	//	Resize,
	//	Move,

		GotFocus,
		LostFocus,

	//	TextChanged,
	//	FontChanged,
	//	FontColorChanged,
	//	FontSizeChanged,
	} EventID;
};


//控件接口。
DeclInterface(IControl)
	typedef Runtime::GEventMap<EControl, YControlEvent> EventMapType;

	DeclIEntry(EventMapType::Event& operator[](const EventMapType::ID&))

	DeclIEntry(bool IsEnabled() const) //判断是否有效。

	DeclIEntry(void SetEnabled(bool)) //设置有效性。
EndDecl


//可视控件接口。
DeclBasedInterface(IVisualControl, virtual IControl, virtual GIFocusRequester<IVisualControl>)
	DeclIEventEntry(InputEventHandler, Enter)
	DeclIEventEntry(InputEventHandler, Leave)
	DeclIEventEntry(KeyEventHandler, KeyUp)
	DeclIEventEntry(KeyEventHandler, KeyDown)
	DeclIEventEntry(KeyEventHandler, KeyHeld)
	DeclIEventEntry(KeyEventHandler, KeyPress)
	DeclIEventEntry(TouchEventHandler, TouchUp)
	DeclIEventEntry(TouchEventHandler, TouchDown)
	DeclIEventEntry(TouchEventHandler, TouchHeld)
	DeclIEventEntry(TouchEventHandler, TouchMove)
	DeclIEventEntry(TouchEventHandler, Click)

	DeclIEntry(void RequestFocus(const MEventArgs&))
	DeclIEntry(void ReleaseFocus(const MEventArgs&))
EndDecl


//控件模块类。
class MControl// : implements IControl
{
protected:
	bool Enabled; //控件有效性。
	IControl::EventMapType EventMap; //事件映射表。

public:
	explicit
	MControl(bool = true);

	virtual DefEmptyDtor(MControl)

	virtual PDefHOperator(IControl::EventMapType::Event&, [], const IControl::EventMapType::ID& id)
		ImplRet(EventMap[id])

	virtual DefPredicate(Enabled, Enabled)

	virtual DefSetter(bool, Enabled, Enabled)
};

inline
MControl::MControl(bool e)
	: Enabled(e), EventMap()
{}


//可视控件模块类。
class MVisualControl : public MControl, public AFocusRequester
{
public:
	DefEvent(InputEventHandler, Enter)
	DefEvent(InputEventHandler, Leave)
	DefEvent(KeyEventHandler, KeyUp)
	DefEvent(KeyEventHandler, KeyDown)
	DefEvent(KeyEventHandler, KeyHeld)
	DefEvent(KeyEventHandler, KeyPress)
	DefEvent(TouchEventHandler, TouchUp)
	DefEvent(TouchEventHandler, TouchDown)
	DefEvent(TouchEventHandler, TouchHeld)
	DefEvent(TouchEventHandler, TouchMove)
	DefEvent(TouchEventHandler, Click)

	explicit
	MVisualControl();

protected:
	GMFocusResponser<IVisualControl>*
	CheckFocusContainer(IWidgetContainer*); //检查给定的容器指针是否有效且指向接受焦点的容器。

public:
	virtual void
	OnGotFocus(const MEventArgs&);
	virtual void
	OnLostFocus(const MEventArgs&);
	virtual void
	OnKeyHeld(const Runtime::MKeyEventArgs&);
	virtual void
	OnTouchDown(const Runtime::MTouchEventArgs& = Runtime::MTouchEventArgs::Empty);
	virtual void
	OnTouchHeld(const Runtime::MTouchEventArgs&);
	virtual void
	OnTouchMove(const Runtime::MTouchEventArgs&);
};


//控件基类。
class YControl : public YComponent, public MControl
{
public:
	typedef YComponent ParentType;
};


//可视控件抽象基类。
class AVisualControl : public Widgets::MWidget, public MVisualControl,
	virtual implements IWidget, virtual implements IVisualControl
{
public:
	explicit
	AVisualControl(HWND = NULL, const Rect& = Rect::FullScreen, IWidgetContainer* = NULL);
	~AVisualControl();

	ImplI(IVisualControl) PDefH(EventMapType::Event&, operator[], const EventMapType::ID& id)
		ImplBodyBase(MVisualControl, operator[], id)

	ImplI(IWidget) DefPredicateBase(Visible, MVisual)
	ImplI(IWidget) DefPredicateBase(Transparent, MVisual)
	ImplI(IWidget) DefPredicateBase(BgRedrawed, MVisual)
	ImplI(IVisualControl) DefPredicateBase(Enabled, MControl)
	ImplI(IVisualControl) DefPredicateBase(Focused, AFocusRequester)
	ImplI(IVisualControl) PDefH(bool, IsFocusOfContainer, GMFocusResponser<IVisualControl>& c) const
		ImplBodyBase(AFocusRequester, IsFocusOfContainer, c)

	//判断包含关系。
	ImplI(IWidget) PDefH(bool, Contains, const Point& p) const
		ImplBodyBase(MVisual, Contains, p)

	ImplI(IVisualControl) PDefH(bool, CheckRemoval, GMFocusResponser<IVisualControl>& c) const
		ImplBodyBase(MVisualControl, CheckRemoval, c)

	ImplI(IWidget) DefGetterBase(const Point&, Location, MVisual)
	ImplI(IWidget) DefGetterBase(const Drawing::Size&, Size, MVisual)
	ImplI(IWidget) DefGetterBase(IWidgetContainer*, ContainerPtr, MWidget)
	ImplI(IWidget) DefGetterBase(HWND, WindowHandle, MWidget)

	ImplI(IVisualControl) DefEventGetter(InputEventHandler, Enter)
	ImplI(IVisualControl) DefEventGetter(InputEventHandler, Leave)
	ImplI(IVisualControl) DefEventGetter(KeyEventHandler, KeyUp)
	ImplI(IVisualControl) DefEventGetter(KeyEventHandler, KeyDown)
	ImplI(IVisualControl) DefEventGetter(KeyEventHandler, KeyHeld)
	ImplI(IVisualControl) DefEventGetter(KeyEventHandler, KeyPress)
	ImplI(IVisualControl) DefEventGetter(TouchEventHandler, TouchUp)
	ImplI(IVisualControl) DefEventGetter(TouchEventHandler, TouchDown)
	ImplI(IVisualControl) DefEventGetter(TouchEventHandler, TouchHeld)
	ImplI(IVisualControl) DefEventGetter(TouchEventHandler, TouchMove)
	ImplI(IVisualControl) DefEventGetter(TouchEventHandler, Click)

	ImplI(IWidget) DefSetterBase(bool, Visible, MVisual)
	ImplI(IWidget) DefSetterBase(bool, Transparent, MVisual)
	ImplI(IWidget) DefSetterBase(bool, BgRedrawed, MVisual)
	ImplI(IWidget) DefSetterBase(const Point&, Location, MVisual)
	ImplI(IVisualControl) DefSetterBase(bool, Enabled, MControl)

	ImplI(IWidget) PDefH(void, DrawBackground)
		ImplBodyBaseVoid(MWidget, DrawBackground)
	ImplI(IWidget) PDefH(void, DrawForeground)
		ImplBodyBaseVoid(MWidget, DrawForeground)

	ImplI(IWidget) PDefH(void, Refresh)
		ImplBodyBaseVoid(MWidget, Refresh)

	ImplI(IVisualControl) void
	RequestFocus(const MEventArgs&); //向部件容器申请获得焦点，若成功则引发 GotFocus 事件。
	ImplI(IVisualControl) void
	ReleaseFocus(const MEventArgs&); //释放焦点，并引发 LostFocus 事件。
};


//可视控件基类。
class YVisualControl : public YComponent, public AVisualControl
{
public:
	typedef YComponent ParentType;

	explicit
	YVisualControl(HWND = NULL, const Rect& = Rect::FullScreen, IWidgetContainer* = NULL);
	~YVisualControl();

	virtual void
	RequestToTop()
	{}
};


//按钮模块。
class MButton
{
protected:
	bool bPressed;

	explicit
	MButton(bool = false);

public:
	DefPredicate(Pressed, bPressed)
};

inline
MButton::MButton(bool b)
	: bPressed(b)
{}


//滚动条模块。
class MScrollBar
{
protected:
	SDST MaxThumbSize, PrevButtonSize, NextButtonSize;
	bool bPrevButtonPressed, bNextButtonPressed;

	explicit
	MScrollBar(SDST = 8, SDST = 10, SDST = 10);

public:
	DefGetter(SDST, MaxThumbSize, MaxThumbSize)
	DefGetter(SDST, PrevButtonSize, PrevButtonSize)
	DefGetter(SDST, NextButtonSize, NextButtonSize)
};


//滚动条。
class AScrollBar : public AVisualControl, public MScrollBar
{
public:
	explicit
	AScrollBar(SDST = 10, SDST = 10, SDST = 10);

	DefGetter(SDST, ScrollAreaSize, GetWidth() - GetPrevButtonSize() - GetNextButtonSize())
	DefGetter(SDST, ScrollAreaFixedSize, GetScrollAreaSize() - GetMaxThumbSize())

	virtual void
	RequestToTop()
	{}

	DeclIEntry(void DrawPrevButton())
	DeclIEntry(void DrawNextButton())
	DeclIEntry(void DrawScrollArea())
	virtual void
	DrawForeground();
};

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

