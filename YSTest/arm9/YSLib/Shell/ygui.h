// YSLib::Shell::YGUI by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-16 20:06:58 + 08:00;
// UTime = 2010-10-15 16:56 + 08:00;
// Version = 0.2019;


#ifndef INCLUDED_YGUI_H_
#define INCLUDED_YGUI_H_

// YGUI ：平台无关的 Shell 组件实现。

#include "../Core/yshell.h"
#include "ydesktop.h"
#include "yform.h"
#include "../Service/ytimer.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Runtime)

//从指定 Shell 传递指定屏幕的指针设备光标至 GUI 界面，返回部件指针。
IWidget*
GetCursorWidgetPtr(HSHL, YDesktop&, const Point&);

//取指定屏幕中的当前焦点对象。
IVisualControl*
GetFocusedObject(YDesktop&);

//级联请求可视控件及上层容器焦点。
void
RequestFocusCascade(IVisualControl&);

//级联释放可视控件及上层容器焦点。
void
ReleaseFocusCascade(IVisualControl&);


//标准 GUI 事件回调函数抽象类。
typedef GAHEventCallback<Components::Controls::IVisualControl, MEventArgs> AHEventCallback;
typedef GAHEventCallback<Components::Controls::IVisualControl, Runtime::MKeyEventArgs> AHKeyCallback;
typedef GAHEventCallback<Components::Controls::IVisualControl, Runtime::MTouchEventArgs> AHTouchCallback;

//标准 GUI 事件回调函数类型。
typedef bool FKeyCallback(Components::Controls::IVisualControl&, const Runtime::MKeyEventArgs&);
typedef bool FTouchCallback(Components::Controls::IVisualControl&, const Runtime::MTouchEventArgs&);
typedef FKeyCallback* PFKeyCallback;
typedef FTouchCallback* PFTouchCallback;

//标准 GUI 事件回调函数对象类。

struct HKeyCallback : public GHBase<PFKeyCallback>, public AHKeyCallback
{
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
	inline explicit
		HTouchCallback(Runtime::MTouchEventArgs e, PFTouchCallback p)
		: GHBase<PFTouchCallback>(p), AHTouchCallback(e)
	{}

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

bool
IsOnDragging();

const Vec&
GetDragOffset();

void
SetDragOffset(const Vec& = Vec::FullScreen);

void
CheckTouchedControlBounds(Components::Controls::IVisualControl&, const MTouchEventArgs&);

bool
RepeatHeld(HeldStateType&, const MKeyEventArgs&, Timers::TimeSpan = 240, Timers::TimeSpan = 120);

void
ResetHeldState(HeldStateType&);

YSL_END_NAMESPACE(InputStatus)


//响应标准按键状态。
bool
ResponseKeyUp(YDesktop&, const Runtime::MKeyEventArgs&);
bool
ResponseKeyDown(YDesktop&, const Runtime::MKeyEventArgs&);
bool
ResponseKeyHeld(YDesktop&, const Runtime::MKeyEventArgs&);

//响应标准屏幕点击状态。
bool
ResponseTouchUp(IWidgetContainer&, const Runtime::MTouchEventArgs&);
bool
ResponseTouchDown(IWidgetContainer&, const Runtime::MTouchEventArgs&);
bool
ResponseTouchHeld(IWidgetContainer&, const Runtime::MTouchEventArgs&);

YSL_END_NAMESPACE(Runtime)

YSL_BEGIN_NAMESPACE(Drawing)

//取图形接口上下文。
inline GraphicInterfaceContext
GetGraphicInterfaceContext(HWND hWnd)
{
	YAssert(hWnd != NULL,
		"In function \"inline GraphicInterfaceContext\nDrawing::GetGraphicInterfaceContext(HWND hWnd)\": \n"
		"The input handle is null.");

	return static_cast<GraphicInterfaceContext>(*hWnd);
}


//绘制界面元素边框。
void
DrawBounds(GraphicInterfaceContext&, const Point&, const Size&, PixelType);

//绘制窗口边框。
void
DrawWindowBounds(HWND, PixelType);

//绘制部件边框。
void
DrawWidgetBounds(IWidget&, PixelType);

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

