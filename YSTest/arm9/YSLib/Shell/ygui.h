// YSLib::Shell::YGUI by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-16 20:06:58 + 08:00;
// UTime = 2010-10-25 07:35 + 08:00;
// Version = 0.2134;


#ifndef INCLUDED_YGUI_H_
#define INCLUDED_YGUI_H_

// YGUI ：平台无关的 Shell 组件实现。

#include "../Core/yshell.h"
#include "ydesktop.h"
#include "yform.h"
#include "../Service/ytimer.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Runtime)

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
//名称:		GetFocusedObject
//全名:		YSLib::Runtime::GetFocusedObject
//可访问性:	public 
//返回类型:	IVisualControl*
//修饰符:	
//形式参数:	YDesktop &
//功能概要:	取指定屏幕中的当前焦点对象。
//备注:		
//********************************
IVisualControl*
GetFocusedObject(YDesktop&);

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
typedef GAHEventCallback<Components::Controls::IVisualControl,
	MEventArgs> AHEventCallback;
typedef GAHEventCallback<Components::Controls::IVisualControl,
	Runtime::MKeyEventArgs> AHKeyCallback;
typedef GAHEventCallback<Components::Controls::IVisualControl,
	Runtime::MTouchEventArgs> AHTouchCallback;

//标准 GUI 事件回调函数类型。
typedef bool FKeyCallback(Components::Controls::IVisualControl&,
	const Runtime::MKeyEventArgs&);
typedef bool FTouchCallback(Components::Controls::IVisualControl&,
	const Runtime::MTouchEventArgs&);
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
	//修饰符:	: GHBase<PFKeyCallback>(p), AHKeyCallback(e)
	//形式参数:	Runtime::MKeyEventArgs e
	//形式参数:	PFKeyCallback p
	//功能概要:	构造：从参数和回调函数指针。
	//备注:		
	//********************************
	inline explicit
	HKeyCallback(Runtime::MKeyEventArgs e, PFKeyCallback p)
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
	//修饰符:	: GHBase<PFTouchCallback>(p), AHTouchCallback(e)
	//形式参数:	Runtime::MTouchEventArgs e
	//形式参数:	PFTouchCallback p
	//功能概要:	构造：从参数和回调函数指针。
	//备注:		
	//********************************
	inline explicit
	HTouchCallback(Runtime::MTouchEventArgs e, PFTouchCallback p)
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


//记录输入保持状态。

YSL_BEGIN_NAMESPACE(InputStatus)

typedef enum
{
	Free = 0,
	Pressed = 1,
	Held = 2
} HeldStateType;

extern HeldStateType KeyHeldState, TouchHeldState;

//********************************
//名称:		IsOnDragging
//全名:		YSLib::Runtime::InputStatus::IsOnDragging
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//功能概要:	判断是否处于拖放状态。
//备注:		
//********************************
bool
IsOnDragging();

//********************************
//名称:		GetDragOffset
//全名:		YSLib::Runtime::InputStatus::GetDragOffset
//可访问性:	public 
//返回类型:	const Vec&
//修饰符:	
//功能概要:	取拖放偏移量。
//备注:		
//********************************
const Vec&
GetDragOffset();

//********************************
//名称:		SetDragOffset
//全名:		YSLib::Runtime::InputStatus::SetDragOffset
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	const Vec &
//功能概要:	设置拖放偏移量。
//备注:		
//********************************
void
SetDragOffset(const Vec& = Vec::FullScreen);

//********************************
//名称:		CheckTouchedControlBounds
//全名:		YSLib::Runtime::InputStatus::CheckTouchedControlBounds
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	Components::Controls::IVisualControl &
//形式参数:	const MTouchEventArgs &
//功能概要:	检查屏幕接触的控件的容器。
//备注:		
//********************************
void
CheckTouchedControlBounds(Components::Controls::IVisualControl&,
	const MTouchEventArgs&);

//********************************
//名称:		RepeatHeld
//全名:		YSLib::Runtime::InputStatus::RepeatHeld
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	HeldStateType &
//形式参数:	const MKeyEventArgs &
//形式参数:	Timers::TimeSpan
//形式参数:	Timers::TimeSpan
//功能概要:	重复检测接触保持事件。
//备注:		
//********************************
bool
RepeatHeld(HeldStateType&, const MKeyEventArgs&, Timers::TimeSpan = 240,
	Timers::TimeSpan = 120);

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
//全名:		YSLib::Runtime::ResponseKeyUp
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	YDesktop &
//形式参数:	const Runtime::MKeyEventArgs &
//功能概要:	响应按键接触结束。
//备注:		
//********************************
bool
ResponseKeyUp(YDesktop&, const Runtime::MKeyEventArgs&);
//********************************
//名称:		ResponseKeyDown
//全名:		YSLib::Runtime::ResponseKeyDown
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	YDesktop &
//形式参数:	const Runtime::MKeyEventArgs &
//功能概要:	响应按键接触开始。
//备注:		
//********************************
bool
ResponseKeyDown(YDesktop&, const Runtime::MKeyEventArgs&);
//********************************
//名称:		ResponseKeyHeld
//全名:		YSLib::Runtime::ResponseKeyHeld
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	YDesktop &
//形式参数:	const Runtime::MKeyEventArgs &
//功能概要:	响应按键接触保持。
//备注:		
//********************************
bool
ResponseKeyHeld(YDesktop&, const Runtime::MKeyEventArgs&);

//响应屏幕接触状态。
//********************************
//名称:		ResponseTouchUp
//全名:		YSLib::Runtime::ResponseTouchUp
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	IWidgetContainer &
//形式参数:	const Runtime::MTouchEventArgs &
//功能概要:	响应屏幕接触结束。
//备注:		
//********************************
bool
ResponseTouchUp(IWidgetContainer&, const Runtime::MTouchEventArgs&);
//********************************
//名称:		ResponseTouchDown
//全名:		YSLib::Runtime::ResponseTouchDown
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	IWidgetContainer &
//形式参数:	const Runtime::MTouchEventArgs &
//功能概要:	响应屏幕接触开始。
//备注:		
//********************************
bool
ResponseTouchDown(IWidgetContainer&, const Runtime::MTouchEventArgs&);
//********************************
//名称:		ResponseTouchHeld
//全名:		YSLib::Runtime::ResponseTouchHeld
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	IWidgetContainer &
//形式参数:	const Runtime::MTouchEventArgs &
//功能概要:	响应屏幕接触保持。
//备注:		
//********************************
bool
ResponseTouchHeld(IWidgetContainer&, const Runtime::MTouchEventArgs&);

YSL_END_NAMESPACE(Runtime)

YSL_BEGIN_NAMESPACE(Drawing)

//********************************
//名称:		GetGraphicInterfaceContext
//全名:		YSLib::Drawing::GetGraphicInterfaceContext
//可访问性:	public 
//返回类型:	YSLib::Drawing::Graphics
//修饰符:	
//形式参数:	HWND hWnd
//功能概要:	取图形接口上下文。
//前置条件: 断言： hWnd != NULL 。
//备注:		
//********************************
inline Graphics
GetGraphicInterfaceContext(HWND hWnd)
{
	YAssert(hWnd != NULL,
		"In function \"inline Graphics\nDrawing"
		"::GetGraphicInterfaceContext(HWND hWnd)\": \n"
		"The input handle is null.");

	return static_cast<Graphics>(*hWnd);
}


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

