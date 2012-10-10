/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ygui.h
\ingroup UI
\brief 平台无关的图形用户界面。
\version r1880
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2012-10-08 11:03 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YGUI
*/


#ifndef YSL_INC_UI_YGUI_H_
#define YSL_INC_UI_YGUI_H_ 1

#include "ywgtevt.h"
#include "../Service/ytimer.h"
#include "ystyle.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 输入计时器。
\warning 非虚析构。
\since build 300

实现两段延时的持续输入状态计时器。
*/
class InputTimer
{
public:
	typedef Timers::Duration Duration;
	/*!
	\brief 输入保持状态。
	*/
	typedef enum
	{
		Free = 0,
		Pressed = 1,
		Held = 2
	} HeldStateType;

private:
	Timers::Timer timer;

public:
	InputTimer(const Duration& = Timers::TimeSpan(1000U));

	/*!
	\brief 延迟。
	\since build 302
	*/
	PDefH(void, Delay, const Duration& d)
		ImplBodyMem(timer, Delay, d)

	/*!
	\brief 重复检测输入接触保持状态。
	*/
	bool
	Refresh(HeldStateType&, const Duration& = Timers::TimeSpan(240),
		const Duration& = Timers::TimeSpan(120));

	/*!
	\brief 复位状态。
	*/
	void
	Reset();
};


/*!
\brief 向指定计时器传递参数，根据状态重复按键。
\return 当且仅当结果为 true 时或之前状态为 HeldStateType::Free 时为 true 。
\since build 300
*/
bool
RepeatHeld(InputTimer&, InputTimer::HeldStateType&,
	const Timers::Duration&, const Timers::Duration&);


/*!
\brief 图形用户界面公共状态。
\warning 非虚析构。
\since build 287
*/
class GUIState
{
public:
	/*!
	\brief 输入接触状态。
	\since build 300
	*/
	InputTimer::HeldStateType KeyHeldState, TouchHeldState;
	Drawing::Vec DraggingOffset; //!< 拖放偏移量。
	/*!
	\brief 输入接触保持计时器。
	\since build 300
	*/
	InputTimer HeldTimer;
	Drawing::Point ControlLocation, LastControlLocation; \
		//!< 最近两次的指针设备操作时的控件全局位置（屏幕坐标）。
	Components::Styles::Palette Colors; //!< 调色板。

private:
	//独立焦点指针：自由状态时即时输入（按下）状态捕获的控件指针。
	Components::IWidget* p_KeyDown;
	Components::IWidget* p_TouchDown;
	bool control_entered; //!< 记录指针是否在控件内部。

public:
	GUIState() ynothrow;

	DefPred(const ynothrow, ControlEntered, control_entered)

	DefGetter(const ynothrow, Components::IWidget*, KeyDownPtr, p_KeyDown) \
		//独立键焦点指针。
	DefGetter(const ynothrow, Components::IWidget*, TouchDownPtr, p_TouchDown) \
		//独立屏幕焦点指针。

	/*!
	\brief 复位图形用户界面状态。
	\note 需要在没有销毁时自动释放焦点的相关控件对象被销毁后立即调用，
		以避免处理无效指针。
	*/
	void
	Reset();

	/*!
	\brief 复位接触保持状态。
	*/
	void
	ResetHeldState(InputTimer::HeldStateType&);

private:
	void
	TryEntering(Components::TouchEventArgs&&);

	void
	TryLeaving(Components::TouchEventArgs&&);

public:
	/*!
	\brief 响应标准按键状态。
	\note 无视事件路由，直接响应。
	*/
	bool
	ResponseKeyBase(Components::KeyEventArgs&, Components::VisualEvent);

	/*!
	\brief 响应标准按键状态。
	\note 无视事件路由，直接响应。
	*/
	bool
	ResponseTouchBase(Components::TouchEventArgs&, Components::VisualEvent);

	/*!
	\brief 响应标准按键状态。
	\return 已被响应时为 true 。
	*/
	bool
	ResponseKey(Components::KeyEventArgs&, Components::VisualEvent);

	/*!
	\brief 响应屏幕接触状态。
	\return 已被响应时为 true 。
	*/
	bool
	ResponseTouch(Components::TouchEventArgs&, Components::VisualEvent);
};


/*!
\brief 取默认图形用户界面公共状态。
\since build 287
\bug 线程安全性依赖于非并发地首次调用。
*/
GUIState&
FetchGUIState();


/*
\brief 判断指定部件是否被句柄指定的图形用户界面状态锁定为独立焦点。
\since build 287
*/
inline bool
IsFocusedByShell(const IWidget& wgt, const GUIState& st = FetchGUIState())
{
	return st.GetTouchDownPtr() == &wgt;
}

YSL_END_NAMESPACE(Components)

YSL_END

#endif

