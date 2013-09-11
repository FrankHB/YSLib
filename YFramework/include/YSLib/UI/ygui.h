/*
	Copyright by FrankHB 2009-2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ygui.h
\ingroup UI
\brief 平台无关的图形用户界面。
\version r2051
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2013-09-09 00:30 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YGUI
*/


#ifndef YSL_INC_UI_ygui_h_
#define YSL_INC_UI_ygui_h_ 1

#include "ywgtevt.h"
#include "../Service/ytimer.h"
#include "ystyle.h"

namespace YSLib
{

namespace UI
{

/*!
\brief 输入计时器。
\since build 300

实现两段延时的持续输入状态计时器。
*/
class YF_API InputTimer : public Timers::Timer
{
public:
	using Duration = Timers::Duration;
	/*!
	\brief 输入保持状态。
	\since build 416
	*/
	enum HeldStateType
	{
		Free = 0,
		Pressed = 1,
		Held = 2
	};

	InputTimer(const Duration& = Timers::TimeSpan(1000U));

	/*!
	\brief 重复检测输入接触保持状态。
	\since build 416
	*/
	bool
	RefreshHeld(HeldStateType&, const Duration& = Timers::TimeSpan(240),
		const Duration& = Timers::TimeSpan(120));

	/*!
	\brief 重复检测连续点击状态。
	\return 保持的点击次数：若不超过延时则为输入次数增加 1 ，否则为 0 。
	\since build 416
	*/
	size_t
	RefreshClick(size_t, const Duration& = Timers::TimeSpan(400));

	/*!
	\brief 复位输入计时状态。
	\since build 416
	*/
	void
	ResetInput();
};


/*!
\brief 向指定计时器传递参数，根据状态重复按键。
\return 当且仅当结果为 true 时或之前状态为 HeldStateType::Free 时为 true 。
\since build 300
*/
YF_API bool
RepeatHeld(InputTimer&, InputTimer::HeldStateType&,
	const Timers::Duration&, const Timers::Duration&);


/*!
\brief 图形用户界面公共状态。
\warning 非虚析构。
\since build 287
*/
class YF_API GUIState : private noncopyable, private nonmovable
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
	Drawing::Point ControlLocation; \
		//!< 最近的指针设备操作时的控件全局位置（屏幕坐标）。
	Styles::Palette Colors; //!< 调色板。

private:
	//独立焦点指针：自由状态时即时输入（按下）状态捕获的控件指针。
	IWidget* p_KeyDown;
	/*!
	\brief 光标设备指针对应的部件。
	\since build 422
	*/
	IWidget* p_CursorOver;
	IWidget* p_TouchDown;
	/*!
	\brief 记录按键时的光标是否在部件内部。
	\since build 422
	*/
	bool entered;

public:
	GUIState() ynothrow;

	//! \since build 422
	DefPred(const ynothrow, Entered, entered)

	//! \since build 422
	DefGetter(const ynothrow, IWidget*, CursorOverPtr, p_CursorOver)
	DefGetter(const ynothrow, IWidget*, KeyDownPtr, p_KeyDown) \
		//独立键焦点指针。
	DefGetter(const ynothrow, IWidget*, TouchDownPtr, p_TouchDown) \
		//独立屏幕焦点指针。

	/*!
	\brief 若拖放偏移量无效则按指定部件的屏幕坐标更新拖放偏移量。
	\return 是否已在拖放状态。
	\note 若参数为空则检查独立焦点指针，如已被按下则指定为独立焦点，否则忽略。
	\since build 434
	*/
	bool
	CheckDraggingOffset(IWidget* = {});

	/*!
	\brief 清除状态对指定部件的引用。
	\post p_KeyDown 、 p_CursorOver 和 p_TouchDown 和指定部件的指针不相等。
	\note 若没有找到对指定部件的引用则不改变状态。
	\since build 428
	*/
	void
	CleanupReferences(IWidget&);

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

public:
	//! \since build 424
	//@{
	/*!
	\brief 响应屏幕接触状态。
	\sa ResponseCursorBase
	*/
	void
	ResponseCursor(CursorEventArgs&, VisualEvent);

	/*!
	\brief 响应标准指针设备状态。
	\note 无视事件路由，直接响应。
	\note 支持 \c Touch* 和 \c Cursor* 事件。
	\note 对于 \c TouchHeld 请求实现记录坐标偏移（用于拖放）或触发事件。
	\warning 调用的事件和参数的动态类型必须匹配。
	*/
	void
	ResponseCursorBase(CursorEventArgs&, VisualEvent);
	//@}

	/*!
	\brief 响应标准按键状态。
	\sa ResponseKeyBase
	*/
	void
	ResponseKey(KeyEventArgs&, VisualEvent);

	/*!
	\brief 响应标准按键状态。
	\note 无视事件路由，直接响应。
	\note 支持 \c Key* 事件。
	\warning 调用的事件和参数的动态类型必须匹配。
	*/
	void
	ResponseKeyBase(KeyEventArgs&, VisualEvent);
	//@}

private:
	//! \since build 423
	//@{
	void
	TryEntering(CursorEventArgs&&);

	void
	TryLeaving(CursorEventArgs&&);

public:
	//! \brief 包装部件响应 Enter/Leave 事件。
	void
	Wrap(IWidget&);
	//@}
};


/*!
\brief 取默认图形用户界面公共状态。
\since build 287
\bug 线程安全性依赖于非并发地首次调用。
*/
YF_API GUIState&
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

} // namespace UI;

} // namespace YSLib;

#endif

