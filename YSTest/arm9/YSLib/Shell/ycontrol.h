// YSLib::Shell::YControl by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-02-18 13:44:24 + 08:00;
// UTime = 2010-10-05 18:35 + 08:00;
// Version = 0.3694;


#ifndef INCLUDED_YCONTROL_H_
#define INCLUDED_YCONTROL_H_

// YControl ：平台无关的控件实现。

#include "ywidget.h"

YSL_BEGIN

using namespace Drawing;

YSL_BEGIN_NAMESPACE(Components)

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
DefDelegate(YInputEventHandler, IVisualControl, Runtime::MInputEventArgs)
DefDelegate(YTouchEventHandler, IVisualControl, Runtime::MTouchEventArgs)
DefDelegate(YKeyEventHandler, IVisualControl, Runtime::MKeyEventArgs)
DefDelegate(YIndexEventHandler, IVisualControl, MIndexEventArgs)


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
DeclBasedInterface(IVisualControl, IControl, Runtime::GIFocusRequester<IVisualControl>)
	DeclIEntry(void RequestFocus(const MEventArgs& = GetZeroElement<MEventArgs>()))
	DeclIEntry(void ReleaseFocus(const MEventArgs& = GetZeroElement<MEventArgs>()))
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

	virtual DefTrivialDtor(MControl)

	virtual PDefOpHead(IControl::EventMapType::Event&, [], const IControl::EventMapType::ID& id)
		ImplRet(EventMap[id])

	virtual DefPredicate(Enabled, Enabled)

	virtual DefSetterDe(bool, Enabled, Enabled, true)
};

inline
MControl::MControl(bool e)
	: Enabled(e), EventMap()
{}


//可视控件模块类。
class MVisualControl : public MControl, public Runtime::AFocusRequester
{
public:
	DefEvent(YInputEventHandler, Enter)
	DefEvent(YInputEventHandler, Leave)
	DefEvent(YTouchEventHandler, TouchUp)
	DefEvent(YTouchEventHandler, TouchDown)
	DefEvent(YTouchEventHandler, TouchHeld)
	DefEvent(YTouchEventHandler, TouchMove)
	DefEvent(YTouchEventHandler, Click)
	DefEvent(YKeyEventHandler, KeyUp)
	DefEvent(YKeyEventHandler, KeyDown)
	DefEvent(YKeyEventHandler, KeyHeld)
	DefEvent(YKeyEventHandler, KeyPress)

	explicit
	MVisualControl();

protected:
	Runtime::GMFocusResponser<IVisualControl>*
	CheckFocusContainer(IWidgetContainer*); //检查给定的容器指针是否有效且指向接受焦点的容器。

public:
	virtual void
	OnGotFocus(const MEventArgs& = GetZeroElement<MEventArgs>());
	virtual void
	OnLostFocus(const MEventArgs& = GetZeroElement<MEventArgs>());
	virtual void
	OnTouchDown(const Runtime::MTouchEventArgs& = Runtime::MTouchEventArgs::Empty);
	virtual void
	OnTouchHeld(const Runtime::MTouchEventArgs&);
	virtual void
	OnTouchMove(const Runtime::MTouchEventArgs&);
	virtual void
	OnKeyHeld(const Runtime::MKeyEventArgs&);
};


//控件基类。
class YControl : public YComponent, public MControl
{
public:
	typedef YComponent ParentType;
};


//可视控件基类。
class YVisualControl : public YComponent, public Widgets::MWidget, public MVisualControl,
	implements IWidget, implements IVisualControl
{
public:
	typedef YComponent ParentType;

	explicit
	YVisualControl(HWND = NULL, const Rect& = Rect::FullScreen, IWidgetContainer* = NULL);
	~YVisualControl();

	virtual PDefHead(EventMapType::Event&, operator[], const EventMapType::ID& id)
		ImplBodyBase(MVisualControl, operator[], id)

	virtual DefPredicateBase(Visible, MVisual)
	virtual DefPredicateBase(Transparent, MVisual)
	virtual DefPredicateBase(BgRedrawed, MVisual)
	virtual DefPredicateBase(Enabled, MControl)
	virtual DefPredicateBase(Focused, AFocusRequester)
	virtual PDefHead(bool, IsFocusOfContainer, Runtime::GMFocusResponser<IVisualControl>& c) const
		ImplBodyBase(AFocusRequester, IsFocusOfContainer, c)

	//判断包含关系。
	virtual PDefHead(bool, Contains, const Point& p) const
		ImplBodyBase(MVisual, Contains, p)

	virtual PDefHead(bool, CheckRemoval, Runtime::GMFocusResponser<IVisualControl>& c) const
		ImplBodyBase(MVisualControl, CheckRemoval, c)

	virtual DefGetterBase(const Point&, Location, MVisual)
	virtual DefGetterBase(const Drawing::Size&, Size, MVisual)
	virtual DefGetterBase(IWidgetContainer*, ContainerPtr, MWidget)
	virtual DefGetterBase(HWND, WindowHandle, MWidget)

	virtual DefSetterBaseDe(bool, Visible, MVisual, true)
	virtual DefSetterBaseDe(bool, Transparent, MVisual, true)
	virtual DefSetterBaseDe(bool, BgRedrawed, MVisual, true)
	virtual DefSetterBase(const Point&, Location, MVisual)
	virtual DefSetterBaseDe(bool, Enabled, MControl, true)

	virtual PDefHead(void, DrawBackground)
		ImplBodyBaseVoid(MWidget, DrawBackground)
	virtual PDefHead(void, DrawForeground)
		ImplBodyBaseVoid(MWidget, DrawForeground)

	virtual PDefHead(void, Refresh)
		ImplBodyBaseVoid(MWidget, Refresh)

	virtual void
	RequestFocus(const MEventArgs& = GetZeroElement<MEventArgs>()); //向部件容器申请获得焦点，若成功则引发 GotFocus 事件。
	virtual void
	ReleaseFocus(const MEventArgs& = GetZeroElement<MEventArgs>()); //释放焦点，并引发 LostFocus 事件。
	virtual void
	RequestToTop()
	{}
};


//按钮模块。
class MButton
{
protected:
	bool bPressed;

public:
	explicit
	MButton(bool = false);

	DefPredicate(Pressed, bPressed)
};

inline
MButton::MButton(bool b)
	: bPressed(b)
{}


//抽象按钮模块基类。
class AButton : public MButton, public Widgets::MLabel
{
public:
	template<class _tChar>
	AButton(const _tChar*, const Drawing::Font& = Drawing::Font::GetDefault(), GHResource<Drawing::TextRegion> = NULL);

	DefEvent(YIndexEventHandler, Confirmed) //选中确定事件。

	DeclIEntry(void OnEnter(const Runtime::MInputEventArgs&))
	DeclIEntry(void OnLeave(const Runtime::MInputEventArgs&))
	DeclIEntry(void OnClick(const Runtime::MTouchEventArgs&))
};

template<class _tChar>
AButton::AButton(const _tChar* l, const Drawing::Font& f, GHResource<Drawing::TextRegion> prTr_)
	: MButton(), MLabel(l, f, prTr_)
{}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

