// YSLib::Shell::YGUI by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-16 20:06:58 + 08:00;
// UTime = 2010-11-08 20:06 + 08:00;
// Version = 0.2250;


#ifndef INCLUDED_YGUI_H_
#define INCLUDED_YGUI_H_

// YGUI ：平台无关的 Shell 组件实现。

#include "../Core/yshell.h"
#include "ydesktop.h"
#include "yform.h"
#include "../Service/ytimer.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

//********************************
//名称:		GetCursorWidgetPtr
//全名:		YSLib::Runtime::GetCursorWidgetPtr
//可访问性:	public 
//返回类型:	IWidget*
//修饰符:	
//形式参数:	HSHL
//形式参数:	YDesktop &
//形式参数:	const Point &
//功能概要:	从指定 Shell 传递指定屏幕的指针设备光标至 GUI 界面，返回部件指针。
//备注:		
//********************************
IWidget*
GetCursorWidgetPtr(HSHL, YDesktop&, const Point&);

//********************************
//名称:		GetFocusedObjectPtr
//全名:		YSLib::Runtime::GetFocusedObjectPtr
//可访问性:	public 
//返回类型:	IVisualControl*
//修饰符:	
//形式参数:	YDesktop &
//功能概要:	取指定屏幕中的当前焦点对象指针。
//备注:		
//********************************
IVisualControl*
GetFocusedObjectPtr(YDesktop&);

//********************************
//名称:		RequestFocusCascade
//全名:		YSLib::Runtime::RequestFocusCascade
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	IVisualControl &
//功能概要:	级联请求可视控件及上层容器焦点。
//备注:		
//********************************
void
RequestFocusCascade(IVisualControl&);

//********************************
//名称:		ReleaseFocusCascade
//全名:		YSLib::Runtime::ReleaseFocusCascade
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	IVisualControl &
//功能概要:	级联释放可视控件及上层容器焦点。
//备注:		
//********************************
void
ReleaseFocusCascade(IVisualControl&);


//标准 GUI 事件回调函数抽象类。
typedef Runtime::GAHEventCallback<Components::Controls::IVisualControl,
	EventArgs> AHEventCallback;
typedef Runtime::GAHEventCallback<Components::Controls::IVisualControl,
	KeyEventArgs> AHKeyCallback;
typedef Runtime::GAHEventCallback<Components::Controls::IVisualControl,
	TouchEventArgs> AHTouchCallback;

//标准 GUI 事件回调函数类型。
typedef bool FKeyCallback(Components::Controls::IVisualControl&,
	KeyEventArgs&);
typedef bool FTouchCallback(Components::Controls::IVisualControl&,
	TouchEventArgs&);
typedef FKeyCallback* PFKeyCallback;
typedef FTouchCallback* PFTouchCallback;

//标准 GUI 事件回调函数对象类。

struct HKeyCallback : public GHBase<PFKeyCallback>, public AHKeyCallback
{
	//********************************
	//名称:		HKeyCallback
	//全名:		YSLib::Runtime::HKeyCallback::HKeyCallback
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	KeyEventArgs e
	//形式参数:	PFKeyCallback p
	//功能概要:	构造：从参数和回调函数指针。
	//备注:		
	//********************************
	inline explicit
	HKeyCallback(KeyEventArgs e, PFKeyCallback p)
	: GHBase<PFKeyCallback>(p), AHKeyCallback(e)
	{}

	inline bool
	operator()(Components::Controls::IVisualControl& c)
	{
		return GetPtr()(c, *this);
	}
};

struct HTouchCallback : public GHBase<PFTouchCallback>, public AHTouchCallback
{
	//********************************
	//名称:		HTouchCallback
	//全名:		YSLib::Runtime::HTouchCallback::HTouchCallback
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	TouchEventArgs e
	//形式参数:	PFTouchCallback p
	//功能概要:	构造：从参数和回调函数指针。
	//备注:		
	//********************************
	inline explicit
	HTouchCallback(TouchEventArgs e, PFTouchCallback p)
		: GHBase<PFTouchCallback>(p), AHTouchCallback(e)
	{}

	//********************************
	//名称:		operator()
	//全名:		YSLib::Runtime::HTouchCallback::operator()
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	Components::Controls::IVisualControl & c
	//功能概要:	调用函数。
	//备注:		
	//********************************
	inline bool
	operator()(Components::Controls::IVisualControl& c)
	{
		return GetPtr()(c, *this);
	}
};


//记录输入状态。

YSL_BEGIN_NAMESPACE(InputStatus)

typedef enum
{
	Free = 0,
	Pressed = 1,
	Held = 2
} HeldStateType;

extern HeldStateType KeyHeldState, TouchHeldState; //输入接触状态。
extern Vec DraggingOffset; //拖放偏移量。
extern Timers::YTimer HeldTimer; //输入接触保持计时器。
extern Point VisualControlLocation, LastVisualControlLocation; \
	//最近两次的指针设备操作时的控件全局位置（屏幕坐标）。

//********************************
//名称:		RepeatHeld
//全名:		YSLib::Runtime::InputStatus::RepeatHeld
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	HeldStateType &
//形式参数:	Timers::TimeSpan
//形式参数:	Timers::TimeSpan
//功能概要:	重复检测输入接触保持事件。
//备注:		
//********************************
bool
RepeatHeld(HeldStateType&, Timers::TimeSpan = 240, Timers::TimeSpan = 120);

//********************************
//名称:		ResetHeldState
//全名:		YSLib::Runtime::InputStatus::ResetHeldState
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	HeldStateType &
//功能概要:	复位接触保持状态。
//备注:		
//********************************
void
ResetHeldState(HeldStateType&);

YSL_END_NAMESPACE(InputStatus)


//响应标准按键状态。
//********************************
//名称:		ResponseKeyUp
//全名:		YSLib::ResponseKeyUp
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	YDesktop &
//形式参数:	KeyEventArgs &
//功能概要:	响应按键接触结束。
//备注:		
//********************************
bool
ResponseKeyUp(YDesktop&, KeyEventArgs&);
//********************************
//名称:		ResponseKeyDown
//全名:		YSLib::ResponseKeyDown
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	YDesktop &
//形式参数:	KeyEventArgs &
//功能概要:	响应按键接触开始。
//备注:		
//********************************
bool
ResponseKeyDown(YDesktop&, KeyEventArgs&);
//********************************
//名称:		ResponseKeyHeld
//全名:		YSLib::ResponseKeyHeld
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	YDesktop &
//形式参数:	KeyEventArgs &
//功能概要:	响应按键接触保持。
//备注:		
//********************************
bool
ResponseKeyHeld(YDesktop&, KeyEventArgs&);

//响应屏幕接触状态。
//********************************
//名称:		ResponseTouchUp
//全名:		YSLib::ResponseTouchUp
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	IUIBox &
//形式参数:	TouchEventArgs &
//功能概要:	响应屏幕接触结束。
//备注:		
//********************************
bool
ResponseTouchUp(IUIBox&, TouchEventArgs&);
//********************************
//名称:		ResponseTouchDown
//全名:		YSLib::ResponseTouchDown
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	IUIBox &
//形式参数:	TouchEventArgs &
//功能概要:	响应屏幕接触开始。
//备注:		
//********************************
bool
ResponseTouchDown(IUIBox&, TouchEventArgs&);
//********************************
//名称:		ResponseTouchHeld
//全名:		YSLib::ResponseTouchHeld
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	IUIBox &
//形式参数:	TouchEventArgs &
//功能概要:	响应屏幕接触保持。
//备注:		
//********************************
bool
ResponseTouchHeld(IUIBox&, TouchEventArgs&);

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Drawing)

//********************************
//名称:		DrawWindowBounds
//全名:		YSLib::Drawing::DrawWindowBounds
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	HWND
//形式参数:	Color
//功能概要:	绘制窗口边框。
//备注:		
//********************************
void
DrawWindowBounds(HWND, Color);

//********************************
//名称:		DrawWidgetBounds
//全名:		YSLib::Drawing::DrawWidgetBounds
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	IWidget &
//形式参数:	Color
//功能概要:	绘制部件边框。
//备注:		限无缓冲区的部件。
//********************************
void
DrawWidgetBounds(IWidget&, Color);

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

