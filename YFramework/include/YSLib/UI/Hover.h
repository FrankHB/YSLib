/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Hover.h
\ingroup UI
\brief 样式无关的指针设备悬停相关功能。
\version r148
\author FrankHB <frankhb1989@gmail.com>
\since build 448
\par 创建时间:
	2013-09-28 12:50:42 +0800
\par 修改时间:
	2015-02-01 08:32 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Hover
*/


#ifndef YSL_INC_UI_Hover_h_
#define YSL_INC_UI_Hover_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YControl
#include YFM_YSLib_UI_YGUI

namespace YSLib
{

namespace UI
{

/*!
\brief 悬停更新器。
\warning 非虚析构。
\since build 448

储存 Enter 事件和 Leave 事件的状态以备查询，同时无效化对应部件。
*/
class YF_API HoverUpdater
{
private:
	bool entered = {};

public:
	//! \since build 554
	lref<IWidget> Widget;

	HoverUpdater(IWidget&);

	DefPred(const ynothrow, Entered, entered)
};


//! \since build 548
//@{
//! \brief 设置悬停动作。
template<typename _func>
void
ActOnHover(IWidget& sender, _func f)
{
	if(&sender != FetchGUIState().GetIndependentFocusPtr())
		f();
}

//! \brief 设置悬停操作渲染器。
//@{
template<typename _func>
inline void
SetRendererOnHover(IWidget& sender, Widget& wgt, _func f)
{
	UI::ActOnHover(sender, std::bind(&Widget::SetRenderer, std::ref(wgt),
		std::bind(f)));
}
inline PDefH(void, SetRendererOnHover, IWidget& sender, Widget& wgt)
	ImplExpr(UI::SetRendererOnHover(sender, wgt, make_unique<Renderer>))
//@}

//! \brief 处理悬停事件：设置 Renderer 为渲染器。
inline PDefH(void, OnHover_SetRenderer, CursorEventArgs&& e, Widget& wgt)
	ImplExpr(UI::SetRendererOnHover(e.GetSender(), wgt))
//@}


/*!
\brief 带延时的悬停状态。
\since build 572
*/
class YF_API TimedHoverState
{
public:
	/*!
	\brief 悬停光标状态。
	\note 枚举值保持顺序以便于优化。
	*/
	enum HoverCursorState : yimpl(size_t)
	{
		Outside = 0,
		Over,
		Left
	};
	/*!
	\brief 定位函数类型。

	用于接收 CursorOver 事件参数并映射为显示的部件位置的函数的类型。
	*/
	using Locator = std::function<ystdex::id_func_clr_t<Drawing::Point>>;

	static yconstexpr const Timers::Duration
		DefaultDuration{Timers::TimeSpan(400U)};
	Locator Locate;

private:
	Timers::Timer tmr;
	HoverCursorState state = Outside;

public:
	Point Position{Point::Invalid};

	TimedHoverState(Locator loc = DefaultLocate,
		Timers::Duration d = DefaultDuration)
		: Locate(loc), tmr(d, {})
	{}

	DefGetter(const ynothrow, HoverCursorState, State, state)

	bool
	Check() ynothrow;

	//! \brief 默认映射：恒等映射。
	static Drawing::Point
	DefaultLocate(const Drawing::Point&) ynothrow;

	PDefH(bool, CheckShow, CursorEventArgs& e) ynothrow
		ImplRet(e.Strategy == RoutedEventArgs::Direct && Check())

	PDefH(bool, CheckHide, CursorEventArgs& e) ynothrow
		ImplRet(e.Strategy == RoutedEventArgs::Direct && Update(e.Position))

	void
	Leave() ynothrow;

	bool
	Update(const Point& pt) ynothrow;

	PDefH(void, Reset, ) ynothrow
		ImplExpr(state = Outside)
};

} // namespace UI;

} // namespace YSLib;

#endif

