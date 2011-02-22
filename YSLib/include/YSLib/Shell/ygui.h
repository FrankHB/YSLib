/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ygui.h
\ingroup Shell
\brief 平台无关的图形用户界面实现。
\version 0.2305;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-16 20:06:58 + 08:00;
\par 修改时间:
	2011-02-20 14:34 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YGUI;
*/


#ifndef INCLUDED_YGUI_H_
#define INCLUDED_YGUI_H_

#include "../Core/ysdef.h"
#include "ycomp.h"
#include "ycontrol.h"
#include "../Core/yfunc.hpp"
#include "../Service/ytimer.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

/*!
\brief 从指定 Shell 传递指定屏幕的指针设备光标至 GUI 界面，返回部件指针。
*/
IWidget*
GetCursorWidgetPtr(GHHandle<YGUIShell>, YDesktop&, const Point&);

/*!
\brief 取指定屏幕中的当前焦点对象指针。
*/
IVisualControl*
GetFocusedObjectPtr(YDesktop&);

/*!
\brief 级联请求可视控件及上层容器焦点。
*/
void
RequestFocusCascade(IVisualControl&);

/*!
\brief 级联释放可视控件及上层容器焦点。
*/
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
	/*!
	\brief 构造：从参数和回调函数指针。
	*/
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
	/*!
	\brief 构造：从参数和回调函数指针。
	*/
	inline explicit
	HTouchCallback(TouchEventArgs e, PFTouchCallback p)
		: GHBase<PFTouchCallback>(p), AHTouchCallback(e)
	{}

	/*!
	\brief 调用函数。
	*/
	inline bool
	operator()(Components::Controls::IVisualControl& c)
	{
		return GetPtr()(c, *this);
	}
};


//! \brief 记录输入状态。
YSL_BEGIN_NAMESPACE(InputStatus)

typedef enum
{
	Free = 0,
	Pressed = 1,
	Held = 2
} HeldStateType;

extern HeldStateType KeyHeldState, TouchHeldState; //!< 输入接触状态。
extern Vec DraggingOffset; //!< 拖放偏移量。
extern Timers::YTimer HeldTimer; //!< 输入接触保持计时器。
extern Point VisualControlLocation, LastVisualControlLocation; \
	//最近两次的指针设备操作时的控件全局位置（屏幕坐标）。

/*!
\brief 重复检测输入接触保持事件。
*/
bool
RepeatHeld(HeldStateType&, Timers::TimeSpan = 240, Timers::TimeSpan = 120);

/*!
\brief 复位接触保持状态。
*/
void
ResetHeldState(HeldStateType&);

YSL_END_NAMESPACE(InputStatus)

/*!
\brief 复位 GUI 状态。
\note 为了避免处理无效指针，需要在切换 Shell 或其它控件对象被销毁后立即调用。
*/
void
ResetGUIStates();

//响应标准按键状态。
/*!
\brief 响应键接触结束。
*/
bool
ResponseKeyUp(YDesktop&, KeyEventArgs&);
/*!
\brief 响应键接触开始。
*/
bool
ResponseKeyDown(YDesktop&, KeyEventArgs&);
/*!
\brief 响应键接触保持。
*/
bool
ResponseKeyHeld(YDesktop&, KeyEventArgs&);

//响应屏幕接触状态。
/*!
\brief 响应屏幕接触结束。
*/
bool
ResponseTouchUp(IUIBox&, TouchEventArgs&);
/*!
\brief 响应屏幕接触开始。
*/
bool
ResponseTouchDown(IUIBox&, TouchEventArgs&);
/*!
\brief 响应屏幕接触保持。
*/
bool
ResponseTouchHeld(IUIBox&, TouchEventArgs&);

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Drawing)

/*!
\brief 绘制窗口边框。
*/
void
DrawWindowBounds(IWindow*, Color);

/*!
\brief 绘制部件边框。
\note 限无缓冲区的部件。
*/
void
DrawWidgetBounds(IWidget&, Color);

/*!
\brief 绘制部件边框。
\note 和 DrawWidgetBounds 类似，但对于有缓冲区的部件，
	在此部件所在窗口的图形上下文绘制。
*/
void
DrawWidgetOutline(IWidget&, Color);

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

