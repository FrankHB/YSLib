// YSLib::Shell::YControl by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-02-18 13:44:24 + 08:00;
// UTime = 2010-10-25 13:11 + 08:00;
// Version = 0.4131;


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

	//********************************
	//名称:		MIndexEventArgs
	//全名:		YSLib::Components::Controls::MIndexEventArgs::MIndexEventArgs
	//可访问性:	public 
	//返回类型:	
	//修饰符:	: MEventArgs(), Control(c), Index(i)
	//形式参数:	IVisualControl & c
	//形式参数:	IndexType i
	//功能概要:	构造：使用可视控件引用和索引值。
	//备注:		
	//********************************
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
DeclBasedInterface(IVisualControl, virtual IWidget, virtual IControl,
	virtual GIFocusRequester<IVisualControl>)
	DeclIEventEntry(InputEventHandler, Enter) //进入控件。
	DeclIEventEntry(InputEventHandler, Leave) //离开控件。
	DeclIEventEntry(KeyEventHandler, KeyUp) //按键接触结束。
	DeclIEventEntry(KeyEventHandler, KeyDown) //按键接触开始。
	DeclIEventEntry(KeyEventHandler, KeyHeld) //按键接触保持。
	DeclIEventEntry(KeyEventHandler, KeyPress) //按键。
	DeclIEventEntry(TouchEventHandler, TouchUp) //屏幕接触结束。
	DeclIEventEntry(TouchEventHandler, TouchDown) //屏幕接触开始。
	DeclIEventEntry(TouchEventHandler, TouchHeld) //屏幕接触保持。
	DeclIEventEntry(TouchEventHandler, TouchMove) //屏幕接触移动。
	DeclIEventEntry(TouchEventHandler, Click) //屏幕点击。

	//向部件容器请求获得焦点，
	DeclIEntry(void RequestFocus(const MEventArgs&))

	//释放焦点。
	DeclIEntry(void ReleaseFocus(const MEventArgs&))
EndDecl


//控件模块类。
class MControl// : implements IControl
{
protected:
	bool Enabled; //控件有效性。
	IControl::EventMapType EventMap; //事件映射表。

public:
	//********************************
	//名称:		MControl
	//全名:		YSLib::Components::Controls::MControl::MControl
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	bool
	//功能概要:	构造：使用有效性。
	//备注:		
	//********************************
	explicit
	MControl(bool = true);

	virtual DefEmptyDtor(MControl)

	virtual PDefHOperator(IControl::EventMapType::Event&, [],
		const IControl::EventMapType::ID& id)
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
	CheckFocusContainer(IWidgetContainer*); \
		//检查指定的容器指针是否有效且指向接受焦点的容器。
};


//控件基类。
class YControl : public YComponent, public MControl
{
public:
	typedef YComponent ParentType;

	virtual DefEmptyDtor(YControl);
};


//可视控件抽象基类。
class AVisualControl : public Widgets::MWidget, public MVisualControl,
	virtual implements IVisualControl
{
public:
	//********************************
	//名称:		AVisualControl
	//全名:		YSLib::Components::Controls::AVisualControl::AVisualControl
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	HWND
	//形式参数:	const Rect &
	//形式参数:	IWidgetContainer *
	//功能概要:	构造：使用指定窗口句柄、边界和部件容器指针。
	//备注:		
	//********************************
	explicit
	AVisualControl(HWND = NULL, const Rect& = Rect::FullScreen,
		IWidgetContainer* = NULL);
	//********************************
	//名称:		~AVisualControl
	//全名:		YSLib::Components::Controls::AVisualControl::~AVisualControl
	//可访问性:	public 
	//返回类型:	
	//修饰符:	ythrow()
	//功能概要:	析构。
	//备注:		无异常抛出。
	//********************************
	~AVisualControl() ythrow();

	ImplI(IVisualControl)
		PDefHOperator(EventMapType::Event&, [], const EventMapType::ID& id)
		ImplBodyBase(MVisualControl, operator[], id)

	ImplI(IVisualControl) DefPredicateBase(Visible, MVisual)
	ImplI(IVisualControl) DefPredicateBase(Transparent, MVisual)
	ImplI(IVisualControl) DefPredicateBase(BgRedrawed, MVisual)
	ImplI(IVisualControl) DefPredicateBase(Enabled, MControl)
	ImplI(IVisualControl) DefPredicateBase(Focused, AFocusRequester)
	ImplI(IVisualControl)
		PDefH(bool, IsFocusOfContainer,
		GMFocusResponser<IVisualControl>& c) const
		ImplBodyBase(AFocusRequester, IsFocusOfContainer, c)

	//判断包含关系。
	ImplI(IVisualControl) PDefH(bool, Contains, const Point& p) const
		ImplBodyBase(MVisual, Contains, p)

	ImplI(IVisualControl)
		PDefH(bool, CheckRemoval, GMFocusResponser<IVisualControl>& c) const
		ImplBodyBase(MVisualControl, CheckRemoval, c)

	ImplI(IVisualControl) DefGetterBase(const Point&, Location, MVisual)
	ImplI(IVisualControl) DefGetterBase(const Drawing::Size&, Size, MVisual)
	ImplI(IVisualControl)
		DefGetterBase(IWidgetContainer*, ContainerPtr, MWidget)
	ImplI(IVisualControl) DefGetterBase(HWND, WindowHandle, MWidget)

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

	ImplI(IVisualControl) DefSetterBase(bool, Visible, MVisual)
	ImplI(IVisualControl) DefSetterBase(bool, Transparent, MVisual)
	ImplI(IVisualControl) DefSetterBase(bool, BgRedrawed, MVisual)
	ImplI(IVisualControl) DefSetterBase(const Point&, Location, MVisual)
	ImplI(IVisualControl) DefSetterBase(bool, Enabled, MControl)

protected:
	ImplI(IVisualControl) PDefH(void, DrawBackground)
		ImplBodyBaseVoid(MWidget, DrawBackground)

	ImplI(IVisualControl) PDefH(void, DrawForeground)
		ImplBodyBaseVoid(MWidget, DrawForeground)

public:
	ImplI(IVisualControl) PDefH(void, Refresh)
		ImplBodyBaseVoid(MWidget, Refresh)

	ImplA(IVisualControl)
	DeclIEntry(void RequestToTop())

	//********************************
	//名称:		RequestFocus
	//全名:		YSLib::Components::Controls::AVisualControl::RequestFocus
	//可访问性:	ImplI(IVisualControl) public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const MEventArgs &
	//功能概要:	向部件容器请求获得焦点，若成功则引发 GotFocus 事件。
	//备注:		
	//********************************
	ImplI(IVisualControl) void
	RequestFocus(const MEventArgs&);

	//********************************
	//名称:		ReleaseFocus
	//全名:		YSLib::Components::Controls::AVisualControl::ReleaseFocus
	//可访问性:	ImplI(IVisualControl) public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const MEventArgs &
	//功能概要:	释放焦点，并引发失去焦点事件。
	//备注:		
	//********************************
	ImplI(IVisualControl) void
	ReleaseFocus(const MEventArgs&);

	//********************************
	//名称:		OnGotFocus
	//全名:		YSLib::Components::Controls::AVisualControl::OnGotFocus
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const MEventArgs &
	//功能概要:	处理获得焦点事件。
	//备注:		
	//********************************
	virtual void
	OnGotFocus(const MEventArgs&);

	//********************************
	//名称:		OnLostFocus
	//全名:		YSLib::Components::Controls::AVisualControl::OnLostFocus
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const MEventArgs &
	//功能概要:	处理失去焦点事件。
	//备注:		
	//********************************
	virtual void
	OnLostFocus(const MEventArgs&);

	//********************************
	//名称:		OnKeyHeld
	//全名:		YSLib::Components::Controls::AVisualControl::OnKeyHeld
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const Runtime::MKeyEventArgs &
	//功能概要:	处理按键接触保持事件。
	//备注:		
	//********************************
	virtual void
	OnKeyHeld(const Runtime::MKeyEventArgs&);

	//********************************
	//名称:		OnTouchDown
	//全名:		YSLib::Components::Controls::AVisualControl::OnTouchDown
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const Runtime::MTouchEventArgs &
	//功能概要:	处理屏幕接触开始事件。
	//备注:		
	//********************************
	virtual void
	OnTouchDown(const Runtime::MTouchEventArgs&
		= Runtime::MTouchEventArgs::Empty);

	//********************************
	//名称:		OnTouchHeld
	//全名:		YSLib::Components::Controls::AVisualControl::OnTouchHeld
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const Runtime::MTouchEventArgs &
	//功能概要:	处理屏幕接触保持事件。
	//备注:		
	//********************************
	virtual void
	OnTouchHeld(const Runtime::MTouchEventArgs&);

	//********************************
	//名称:		OnTouchMove
	//全名:		YSLib::Components::Controls::AVisualControl::OnTouchMove
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const Runtime::MTouchEventArgs &
	//功能概要:	处理屏幕接触移动事件。
	//备注:		
	//********************************
	virtual void
	OnTouchMove(const Runtime::MTouchEventArgs&);
};


//可视控件基类。
class YVisualControl : public YComponent, public AVisualControl
{
public:
	typedef YComponent ParentType;

	//********************************
	//名称:		YVisualControl
	//全名:		YSLib::Components::Controls::YVisualControl::YVisualControl
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	HWND
	//形式参数:	const Rect &
	//形式参数:	IWidgetContainer *
	//功能概要:	构造：使用指定窗口句柄、边界和部件容器指针。
	//备注:		
	//********************************
	explicit
	YVisualControl(HWND = NULL, const Rect& = Rect::FullScreen,
		IWidgetContainer* = NULL);
	//********************************
	//名称:		~YVisualControl
	//全名:		YSLib::Components::Controls::YVisualControl::~YVisualControl
	//可访问性:	public 
	//返回类型:	
	//修饰符:	ythrow()
	//功能概要:	析构。
	//备注:		无异常抛出。
	//********************************
	~YVisualControl() ythrow();

	//********************************
	//名称:		RequestToTop
	//全名:		YSLib::Components::Controls::YVisualControl::RequestToTop
	//可访问性:	ImplI(IVisualControl) public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	请求提升至容器顶端。
	//备注:		空实现。
	//********************************
	ImplI(IVisualControl) void
	RequestToTop()
	{}
};


//按钮模块。
class MButton
{
protected:
	bool bPressed; //按键状态：是否处于按下状态。

	//********************************
	//名称:		MButton
	//全名:		YSLib::Components::Controls::MButton::MButton
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	bool
	//功能概要:	构造：使用按键状态。
	//备注:		
	//********************************
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
	SDST MinThumbSize, PrevButtonSize, NextButtonSize;
	bool bPrevButtonPressed, bNextButtonPressed;

	//********************************
	//名称:		MScrollBar
	//全名:		YSLib::Components::Controls::MScrollBar::MScrollBar
	//可访问性:	protected 
	//返回类型:	
	//修饰符:	
	//形式参数:	SDST
	//形式参数:	SDST
	//形式参数:	SDST
	//功能概要:	构造：使用指定滑块长度、前按钮长度和后按钮长度。
	//备注:		
	//********************************
	explicit
	MScrollBar(SDST = 8, SDST = 16, SDST = 16);

public:
	DefGetter(SDST, MinThumbSize, MinThumbSize)
	DefGetter(SDST, PrevButtonSize, PrevButtonSize)
	DefGetter(SDST, NextButtonSize, NextButtonSize)
};

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

