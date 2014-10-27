/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Hover.h
\ingroup UI
\brief 样式无关的指针设备悬停相关功能。
\version r94
\author FrankHB <frankhb1989@gmail.com>
\since build 448
\par 创建时间:
	2013-09-28 12:50:42 +0800
\par 修改时间:
	2014-10-27 13:37 +0800
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
	std::reference_wrapper<IWidget> Widget;

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

} // namespace UI;

} // namespace YSLib;

#endif

