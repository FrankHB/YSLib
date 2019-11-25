/*
	© 2013-2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Hover.h
\ingroup UI
\brief 样式无关的指针设备悬停相关功能。
\version r203
\author FrankHB <frankhb1989@gmail.com>
\since build 448
\par 创建时间:
	2013-09-28 12:50:42 +0800
\par 修改时间:
	2019-11-25 21:49 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Hover
*/


#ifndef YSL_INC_UI_Hover_h_
#define YSL_INC_UI_Hover_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YControl // for ystdex::invoke, function;
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


/*!
\brief 设置悬停动作。
\since build 583
*/
template<typename _func, typename... _tParams>
void
ActOnHover(IWidget& wgt, _func f, _tParams&&... args)
{
	if(FetchGUIState().GetIndependentFocusPtr().get() != &wgt)
		ystdex::invoke(f, yforward(args)...);
}

/*!
\brief 设置悬停操作渲染器。
\since build 548
*/
//@{
template<typename _func>
inline void
SetRendererOnHover(IWidget& wgt, Widget& target, _func f)
{
	UI::ActOnHover(wgt, std::bind(&Widget::SetRenderer, std::ref(target),
		std::bind(f)));
}
inline PDefH(void, SetRendererOnHover, IWidget& wgt, Widget& target)
	ImplExpr(UI::SetRendererOnHover(wgt, target, make_unique<Renderer>))
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
	using Locator = function<Point(const CursorEventArgs&)>;

	Locator Locate{DefaultLocate};

private:
	Timers::Timer tmr;
	HoverCursorState state = Outside;

public:
	Point Position{Point::Invalid};

	TimedHoverState(Locator loc = DefaultLocate,
		Timers::Duration d = GetDefaultDuration())
		: Locate(loc), tmr(d, {})
	{}

	DefGetter(const ynothrow, HoverCursorState, State, state)

	bool
	Check() ynothrow;

	//! \brief 默认延迟。
	static yconstfn DefGetter(ynothrow, Timers::Duration, DefaultDuration,
		Timers::TimeSpan(yimpl(400U)))

	/*!
	\brief 默认定位映射：恒等映射。
	\since build 573
	*/
	static Point
	DefaultLocate(const CursorEventArgs&) ynothrow;

	PDefH(bool, CheckShow, CursorEventArgs& e) ynothrow
		ImplRet(e.Strategy == RoutedEventArgs::Direct && Check())

	PDefH(bool, CheckHide, CursorEventArgs& e) ynothrow
		ImplRet(e.Strategy == RoutedEventArgs::Direct && Update(e.Position))

	void
	Leave() ynothrow;

	/*!
	\brief 定位映射：相对偏移。
	\since build 573
	*/
	static Point
	LocateForOffset(const CursorEventArgs&, const Point&);

	PDefH(void, Reset, ) ynothrow
		ImplExpr(state = Outside)

	bool
	Update(const Point& pt) ynothrow;
};

} // namespace UI;

} // namespace YSLib;

#endif

