// YSLib::Shell::YControl by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-02-18 13:44:24 + 08:00;
// UTime = 2010-11-08 19:45 + 08:00;
// Version = 0.4308;


#ifndef INCLUDED_YCONTROL_H_
#define INCLUDED_YCONTROL_H_

// YControl ：平台无关的控件实现。

#include "ywidget.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

using namespace Drawing;

//屏幕事件参数类。
struct ScreenPositionEventArgs : public EventArgs, public Drawing::Point
{
	static const ScreenPositionEventArgs Empty;

	//********************************
	//名称:		ScreenPositionEventArgs
	//全名:		YSLib::Runtime::ScreenPositionEventArgs
	//				::ScreenPositionEventArgs
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const Drawing::Point &
	//功能概要:	构造：使用指定点。
	//备注:		
	//********************************
	explicit
	ScreenPositionEventArgs(const Drawing::Point& = Drawing::Point::Zero);
};

inline
ScreenPositionEventArgs::ScreenPositionEventArgs(const Drawing::Point& pt)
	: EventArgs(), Point(pt)
{}


//输入事件参数类。
struct InputEventArgs
{
public:
	static InputEventArgs Empty;

	typedef platform::Key Key;

	Key k;

	//********************************
	//名称:		InputEventArgs
	//全名:		YSLib::InputEventArgs::InputEventArgs
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const Key &
	//功能概要:	构造：使用本机按键对象。
	//备注:		
	//********************************
	InputEventArgs(const Key& = 0);

	DefConverter(Key, k)

	DefGetter(Key, Key, k)
};

inline
InputEventArgs::InputEventArgs(const Key& k)
	: k(k)
{}


//指针设备输入事件参数类。
struct TouchEventArgs : public ScreenPositionEventArgs,
	public InputEventArgs
{
	typedef Drawing::Point InputType; //输入类型。

	static TouchEventArgs Empty;

	//********************************
	//名称:		TouchEventArgs
	//全名:		YSLib::TouchEventArgs::TouchEventArgs
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const InputType &
	//功能概要:	构造：使用输入类型对象。
	//备注:		
	//********************************
	TouchEventArgs(const InputType& = InputType::Zero);
};

inline
TouchEventArgs::TouchEventArgs(const InputType& pt)
	: ScreenPositionEventArgs(pt), InputEventArgs()
{}


//键盘输入事件参数类。
struct KeyEventArgs : public EventArgs, public InputEventArgs
{
	typedef Key InputType; //输入类型。

	static KeyEventArgs Empty;

	//********************************
	//名称:		KeyEventArgs
	//全名:		YSLib::KeyEventArgs::KeyEventArgs
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const InputType &
	//功能概要:	构造：使用输入类型对象。
	//备注:		
	//********************************
	KeyEventArgs(const InputType& = 0);
};

inline
KeyEventArgs::KeyEventArgs(const InputType& k)
	: InputEventArgs(k)
{}

//控件事件参数类型。
struct IndexEventArgs : public EventArgs
{
	typedef std::ptrdiff_t IndexType;

	IVisualControl& Control;
	IndexType Index;

	//********************************
	//名称:		IndexEventArgs
	//全名:		YSLib::Components::Controls::IndexEventArgs::IndexEventArgs
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	IVisualControl & c
	//形式参数:	IndexType i
	//功能概要:	构造：使用可视控件引用和索引值。
	//备注:		
	//********************************
	IndexEventArgs(IVisualControl& c, IndexType i)
	: EventArgs(),
	Control(c), Index(i)
	{}
};


//控件事件类型。
typedef Runtime::GEvent<true, IControl, EventArgs> YControlEvent;


//事件处理器类型。
DefDelegate(InputEventHandler, IVisualControl, InputEventArgs)
DefDelegate(KeyEventHandler, IVisualControl, KeyEventArgs)
DefDelegate(TouchEventHandler, IVisualControl, TouchEventArgs)
DefDelegate(IndexEventHandler, IVisualControl, IndexEventArgs)


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
	virtual GIFocusRequester<GMFocusResponser, IVisualControl>)
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
	DeclIEntry(void RequestFocus(EventArgs&))

	//释放焦点。
	DeclIEntry(void ReleaseFocus(EventArgs&))
EndDecl


//********************************
//名称:		OnKeyHeld
//全名:		YSLib::Components::Controls::OnKeyHeld
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	IVisualControl &
//形式参数:	KeyEventArgs &
//功能概要:	处理按键接触保持事件。
//备注:		
//********************************
void
OnKeyHeld(IVisualControl&, KeyEventArgs&);

//********************************
//名称:		OnTouchHeld
//全名:		YSLib::Components::Controls::OnTouchHeld
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	IVisualControl &
//形式参数:	TouchEventArgs &
//功能概要:	处理屏幕接触保持事件。
//备注:		
//********************************
void
OnTouchHeld(IVisualControl&, TouchEventArgs&);

//********************************
//名称:		OnTouchMove
//全名:		YSLib::Components::Controls::OnTouchMove
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	IVisualControl &
//形式参数:	TouchEventArgs &
//功能概要:	处理屏幕接触移动事件。
//备注:		
//********************************
void
OnTouchMove(IVisualControl&, TouchEventArgs&);

//********************************
//名称:		OnTouchMove
//全名:		YSLib::Components::Controls::OnDrag
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	IVisualControl &
//形式参数:	TouchEventArgs &
//功能概要:	处理屏幕接触移动事件：使用拖放。
//备注:		
//********************************
void
OnDrag(IVisualControl&, TouchEventArgs&);


//控件模块类。
class Control// : implements IControl
{
protected:
	bool Enabled; //控件有效性。
	IControl::EventMapType EventMap; //事件映射表。

public:
	//********************************
	//名称:		Control
	//全名:		YSLib::Components::Controls::Control::Control
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	bool
	//功能概要:	构造：使用有效性。
	//备注:		
	//********************************
	explicit
	Control(bool = true);

	virtual DefEmptyDtor(Control)

	virtual PDefHOperator(IControl::EventMapType::Event&, [],
		const IControl::EventMapType::ID& id)
		ImplRet(EventMap[id])

	virtual DefPredicate(Enabled, Enabled)

	virtual DefSetter(bool, Enabled, Enabled)
};

inline
Control::Control(bool e)
	: Enabled(e), EventMap()
{}


//可视控件模块类。
class MVisualControl : public Control, public AFocusRequester
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
	virtual DefEmptyDtor(MVisualControl)
};


//控件基类。
class YControl : public YComponent, public Control
{
public:
	typedef YComponent ParentType;

	virtual DefEmptyDtor(YControl)
};


//可视控件抽象基类。
class AVisualControl : public Widgets::Widget, public MVisualControl,
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
	//形式参数:	IUIBox *
	//功能概要:	构造：使用指定窗口句柄、边界和部件容器指针。
	//备注:		
	//********************************
	explicit
	AVisualControl(HWND = NULL, const Rect& = Rect::FullScreen,
		IUIBox* = NULL);
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

	ImplI(IVisualControl) DefPredicateBase(Visible, Visual)
	ImplI(IVisualControl) DefPredicateBase(Transparent, Visual)
	ImplI(IVisualControl) DefPredicateBase(BgRedrawed, Visual)
	ImplI(IVisualControl) DefPredicateBase(Enabled, Control)
	ImplI(IVisualControl) DefPredicateBase(Focused, AFocusRequester)
	ImplI(IVisualControl)
		PDefH(bool, IsFocusOfContainer,
		GMFocusResponser<IVisualControl>& c) const
		ImplBodyBase(AFocusRequester, IsFocusOfContainer, c)

	ImplI(IVisualControl)
		PDefH(bool, CheckRemoval, GMFocusResponser<IVisualControl>& c) const
		ImplBodyBase(MVisualControl, CheckRemoval, c)

	ImplI(IVisualControl) DefGetterBase(const Point&, Location, Visual)
	ImplI(IVisualControl) DefGetterBase(const Drawing::Size&, Size, Visual)
	ImplI(IVisualControl)
		DefGetterBase(IUIBox*, ContainerPtr, Widget)
	ImplI(IVisualControl) DefGetterBase(HWND, WindowHandle, Widget)

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

	ImplI(IVisualControl) DefSetterBase(bool, Visible, Visual)
	ImplI(IVisualControl) DefSetterBase(bool, Transparent, Visual)
	ImplI(IVisualControl) DefSetterBase(bool, BgRedrawed, Visual)
	ImplI(IVisualControl) DefSetterBase(const Point&, Location, Visual)
	ImplI(IVisualControl) DefSetterBase(bool, Enabled, Control)

	ImplI(IVisualControl) PDefH(void, DrawBackground)
		ImplBodyBaseVoid(Widget, DrawBackground)

	ImplI(IVisualControl) PDefH(void, DrawForeground)
		ImplBodyBaseVoid(Widget, DrawForeground)

	ImplI(IVisualControl) PDefH(void, Refresh)
		ImplBodyBaseVoid(Widget, Refresh)

	ImplA(IVisualControl)
	DeclIEntry(void RequestToTop())

	//********************************
	//名称:		RequestFocus
	//全名:		YSLib::Components::Controls::AVisualControl::RequestFocus
	//可访问性:	ImplI(IVisualControl) public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	EventArgs &
	//功能概要:	向部件容器请求获得焦点，若成功则引发 GotFocus 事件。
	//备注:		
	//********************************
	ImplI(IVisualControl) void
	RequestFocus(EventArgs&);

	//********************************
	//名称:		ReleaseFocus
	//全名:		YSLib::Components::Controls::AVisualControl::ReleaseFocus
	//可访问性:	ImplI(IVisualControl) public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	EventArgs &
	//功能概要:	释放焦点，并引发失去焦点事件。
	//备注:		
	//********************************
	ImplI(IVisualControl) void
	ReleaseFocus(EventArgs&);

	//********************************
	//名称:		OnGotFocus
	//全名:		YSLib::Components::Controls::AVisualControl::OnGotFocus
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	EventArgs &
	//功能概要:	处理获得焦点事件。
	//备注:		
	//********************************
	void
	OnGotFocus(EventArgs&);

	//********************************
	//名称:		OnLostFocus
	//全名:		YSLib::Components::Controls::AVisualControl::OnLostFocus
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	EventArgs &
	//功能概要:	处理失去焦点事件。
	//备注:		
	//********************************
	void
	OnLostFocus(EventArgs&);

	//********************************
	//名称:		OnTouchDown
	//全名:		YSLib::Components::Controls::AVisualControl::OnTouchDown
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	TouchEventArgs &
	//功能概要:	处理屏幕接触开始事件。
	//备注:		
	//********************************
	void
	OnTouchDown(TouchEventArgs&);
};

inline void
AVisualControl::OnLostFocus(EventArgs& e)
{
	OnGotFocus(e);
}


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
	//形式参数:	IUIBox *
	//功能概要:	构造：使用指定窗口句柄、边界和部件容器指针。
	//备注:		
	//********************************
	explicit
	YVisualControl(HWND = NULL, const Rect& = Rect::FullScreen,
		IUIBox* = NULL);
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

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

