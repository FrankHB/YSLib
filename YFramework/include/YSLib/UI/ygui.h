/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ygui.h
\ingroup UI
\brief 平台无关的图形用户界面。
\version r2192
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2014-07-09 07:48 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YGUI
*/


#ifndef YSL_INC_UI_ygui_h_
#define YSL_INC_UI_ygui_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YWidgetEvent
#include YFM_YSLib_Service_YTimer
#include YFM_YSLib_UI_YStyle

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
\brief 调用指定部件的 TextInput 事件处理指定的输入。
\return 是否已处理输入的字符。
\since build 510
*/
//@{
YF_API void
CallInputEvent(IWidget&, const String&, const KeyInput&);
inline PDefH(bool, CallInputEvent, ucs2_t c, const KeyInput& k, IWidget& wgt)
	ImplRet(c != ucs2_t() ? (CallInputEvent(wgt, {c}, k), true) : false)
//@}


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
	/*!
	\brief 最近的指针设备操作时的控件全局位置（相对于顶层部件的坐标）。
	\since build 445
	*/
	Drawing::Point CursorLocation;
	Styles::Palette Colors; //!< 调色板。
	/*!
	\brief 样式映射。
	\since build 468
	*/
	Styles::StyleMap Styles;

private:
	/*!
	\brief 光标设备指针对应的部件。
	\since build 422
	*/
	IWidget* p_CursorOver = {};
	/*!
	\brief 独立焦点指针：自由状态时即时输入（按下）状态捕获的部件指针。
	\since build 464
	*/
	IWidget* p_indp_focus = {};
	/*!
	\brief 级联焦点指针：缓冲最后一次通过直接策略路由事件的进入的部件状态。
	\since build 483
	*/
	IWidget* p_cascade_focus = {};
	/*!
	\brief 记录按键时的光标是否在部件内部。
	\since build 422
	*/
	bool entered = {};

	/*!
	\brief 记录检查时的按键输入。
	\sa CheckHeldState
	\since build 487
	*/
	KeyInput checked_held{};

	/*!
	\brief 记录需要映射的主要字符的按键编码。
	\sa UpdateChar
	\since build 487
	*/
	size_t master_key = 0;

public:
	/*!
	\brief 外部文本输入焦点部件指针。
	\note 对于宿主实现，值可能由环境修改。可用于支持宿主环境的输入法相关状态。
	\since build 510
	*/
	IWidget* ExteralTextInputFocusPtr = {};
	/*!
	\brief 相对于外部文本输入指示焦点部件的插入符光标左上角位置。
	\note 除了复位清除为 Point::Invalid 外， GUIState 不访问此成员。
	\since build 512
	*/
	Point CaretLocation{Point::Invalid};

	GUIState() ynothrow;

	//! \since build 422
	DefPred(const ynothrow, Entered, entered)

	//! \since build 487
	DefGetter(const ynothrow, const KeyInput&, CheckedHeldKeys, checked_held)
	//! \since build 422
	DefGetter(const ynothrow, IWidget*, CursorOverPtr, p_CursorOver)
	//! \since build 464
	DefGetter(const ynothrow, IWidget*, IndependentFocusPtr, p_indp_focus) \
		//独立焦点指针。

	/*!
	\brief 检查输入保持状态。
	\return 保持状态是否被参数改变。
	\note 接受的按键参数一般是按下状态或保持状态。
	\since build 488

	若被记录的按键状态为空则记录参数指定的按键状态；否则若当记录的按键状态和
	参数不等时追加（位或）更新记录的按键状态并设置为参数，同时设置保持状态为
	InputTimer::Free 。
	*/
	//@{
	PDefH(bool, CheckHeldState, const KeyInput& keys)
		ImplRet(CheckHeldState(keys, KeyHeldState))
	bool
	CheckHeldState(const KeyInput&, InputTimer::HeldStateType&);
	//@}

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
	\post p_CursorOver 和 p_indp_focus 和指定部件的指针不相等。
	\note 若没有找到对指定部件的引用则不改变状态。
	\since build 428
	*/
	void
	CleanupReferences(IWidget&);

private:
	/*!
	\brief 处理级联焦点指针，保证指向的部件具有焦点。
	\since build 483
	*/
	void
	HandleCascade(RoutedEventArgs&, IWidget&);

public:
	/*!
	\brief 复位图形用户界面状态。
	\note 需要在没有销毁时自动释放焦点的相关控件对象被销毁后立即调用，
		以避免处理无效指针。
	*/
	void
	Reset();

	/*!
	\brief 复位接触保持状态。
	\note 使用参数指定的按键清除检查时的按键输入，当结果为空时复位状态和计时器。
	\since build 489
	*/
	void
	ResetHeldState(InputTimer::HeldStateType&, const KeyInput&);

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

	/*!
	\brief 向文本输入焦点发送输入字符。
	\return 是否已处理输入的字符。
	\sa UI::CallInputEvent
	\since build 510

	若文本输入焦点为空则忽略，否则调用 SendInputEvent 触发 TextInput 事件。
	*/
	//@{
	PDefH(bool, SendInput, KeyInput& k)
		ImplRet(SendInput(UpdateChar(k), k))
	PDefH(bool, SendInput, ucs2_t c, const KeyInput& k)
		ImplRet(c != ucs2_t() ? SendInput(k, {c}) : false)
	bool
	SendInput(const KeyInput&, const String&);
	//@}

private:
	//! \since build 423
	//@{
	void
	TryEntering(CursorEventArgs&&);

	void
	TryLeaving(CursorEventArgs&&);

public:
	/*!
	\brief 当指定按键状态和按键保持状态不同时按需更新映射的字符和参数。
	\note 一般用于响应 KeyDown 事件。通过参数区分是否由 KeyHeld 引起而按需更新。
	\return 更新后的字符。
	\sa MapKeyChar
	\since build 489
	*/
	char
	UpdateChar(KeyInput&);

	//! \brief 包装部件响应 Enter/Leave 事件。
	void
	Wrap(IWidget&);
	//@}
};


/*!
\brief 取默认图形用户界面公共状态。
\since build 287
*/
YF_API GUIState&
FetchGUIState();


/*!
\brief 判断指定部件是否被句柄指定的图形用户界面状态锁定为独立焦点。
\since build 287
*/
inline bool
IsFocusedByShell(const IWidget& wgt, const GUIState& st = FetchGUIState())
{
	return st.GetIndependentFocusPtr() == &wgt;
}

} // namespace UI;

} // namespace YSLib;

#endif

