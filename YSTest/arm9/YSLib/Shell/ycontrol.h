// YSLib::Shell::YControl by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-02-18 13:44:24 + 08:00;
// UTime = 2010-10-15 16:56 + 08:00;
// Version = 0.3882;


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
DeclBasedInterface(IVisualControl, IControl, GIFocusRequester<IVisualControl>)
	DeclIEntry(void RequestFocus(const MEventArgs& = GetZeroElement<MEventArgs>()))
	DeclIEntry(void ReleaseFocus(const MEventArgs& = GetZeroElement<MEventArgs>()))

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

	virtual DefSetterDe(bool, Enabled, Enabled, true)
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
	OnGotFocus(const MEventArgs& = GetZeroElement<MEventArgs>());
	virtual void
	OnLostFocus(const MEventArgs& = GetZeroElement<MEventArgs>());
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
	implements IWidget, implements IVisualControl
{
public:
	explicit
	AVisualControl(HWND = NULL, const Rect& = Rect::FullScreen, IWidgetContainer* = NULL);
	~AVisualControl();

	virtual PDefH(EventMapType::Event&, operator[], const EventMapType::ID& id)
		ImplBodyBase(MVisualControl, operator[], id)

	virtual DefPredicateBase(Visible, MVisual)
	virtual DefPredicateBase(Transparent, MVisual)
	virtual DefPredicateBase(BgRedrawed, MVisual)
	virtual DefPredicateBase(Enabled, MControl)
	virtual DefPredicateBase(Focused, AFocusRequester)
	virtual PDefH(bool, IsFocusOfContainer, GMFocusResponser<IVisualControl>& c) const
		ImplBodyBase(AFocusRequester, IsFocusOfContainer, c)

	//判断包含关系。
	virtual PDefH(bool, Contains, const Point& p) const
		ImplBodyBase(MVisual, Contains, p)

	virtual PDefH(bool, CheckRemoval, GMFocusResponser<IVisualControl>& c) const
		ImplBodyBase(MVisualControl, CheckRemoval, c)

	virtual DefGetterBase(const Point&, Location, MVisual)
	virtual DefGetterBase(const Drawing::Size&, Size, MVisual)
	virtual DefGetterBase(IWidgetContainer*, ContainerPtr, MWidget)
	virtual DefGetterBase(HWND, WindowHandle, MWidget)

	virtual DefEventGetter(InputEventHandler, Enter)
	virtual DefEventGetter(InputEventHandler, Leave)
	virtual DefEventGetter(KeyEventHandler, KeyUp)
	virtual DefEventGetter(KeyEventHandler, KeyDown)
	virtual DefEventGetter(KeyEventHandler, KeyHeld)
	virtual DefEventGetter(KeyEventHandler, KeyPress)
	virtual DefEventGetter(TouchEventHandler, TouchUp)
	virtual DefEventGetter(TouchEventHandler, TouchDown)
	virtual DefEventGetter(TouchEventHandler, TouchHeld)
	virtual DefEventGetter(TouchEventHandler, TouchMove)
	virtual DefEventGetter(TouchEventHandler, Click)

	virtual DefSetterBaseDe(bool, Visible, MVisual, true)
	virtual DefSetterBaseDe(bool, Transparent, MVisual, true)
	virtual DefSetterBaseDe(bool, BgRedrawed, MVisual, true)
	virtual DefSetterBase(const Point&, Location, MVisual)
	virtual DefSetterBaseDe(bool, Enabled, MControl, true)

	virtual PDefH(void, DrawBackground)
		ImplBodyBaseVoid(MWidget, DrawBackground)
	virtual PDefH(void, DrawForeground)
		ImplBodyBaseVoid(MWidget, DrawForeground)

	virtual PDefH(void, Refresh)
		ImplBodyBaseVoid(MWidget, Refresh)

	virtual void
	RequestFocus(const MEventArgs& = GetZeroElement<MEventArgs>()); //向部件容器申请获得焦点，若成功则引发 GotFocus 事件。
	virtual void
	ReleaseFocus(const MEventArgs& = GetZeroElement<MEventArgs>()); //释放焦点，并引发 LostFocus 事件。
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

