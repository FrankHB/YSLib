/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Hover.cpp
\ingroup UI
\brief 样式无关的指针设备悬停相关功能。
\version r99
\author FrankHB <frankhb1989@gmail.com>
\since build 448
\par 创建时间:
	2013-09-28 12:52:39 +0800
\par 修改时间:
	2015-02-01 08:32 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Hover
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_Hover
#include YFM_YSLib_UI_YControl

namespace YSLib
{

namespace UI
{

HoverUpdater::HoverUpdater(IWidget& wgt)
	: Widget(wgt)
{
	yunseq(
	FetchEvent<Enter>(wgt) += [this]{
		entered = true,
		Invalidate(Widget);
	},
	FetchEvent<Leave>(wgt) += [this]{
		entered = false,
		Invalidate(Widget);
	}
	);
}


yconstexpr const Timers::Duration TimedHoverState::DefaultDuration;

bool
TimedHoverState::Check() ynothrow
{
	if(state == Outside)
	{
		Activate(tmr),
		state = Over;
	}
	if(state == Over && tmr.Refresh())
	{
		state = Left;
		return true;
	}
	return {};
}

Point
TimedHoverState::DefaultLocate(const Point& pt) ynothrow
{
	return pt;
}

void
TimedHoverState::Leave() ynothrow
{
	if(state == Left)
		Reset();
}

bool
TimedHoverState::Update(const Point& pt) ynothrow
{
	if(Position != pt)
	{
		Position = pt;
		Leave();
		return true;
	}
	return {};
}

} // namespace UI;

} // namespace YSLib;

