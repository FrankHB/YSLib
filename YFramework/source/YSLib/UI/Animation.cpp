/*
	© 2013-2015, 2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Animation.cpp
\ingroup UI
\brief 样式无关的动画实现。
\version r146
\author FrankHB <frankhb1989@gmail.com>
\since build 443
\par 创建时间:
	2013-10-06 22:12:10 +0800
\par 修改时间:
	2021-12-21 21:35 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Animation
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_Animation
#include YFM_YSLib_UI_YControl // for IsEnabled;

namespace YSLib
{

namespace UI
{

bool
InvalidationUpdater::operator()() const
{
	if(bool(Invalidate) && !(WidgetPtr && Invalidate(*WidgetPtr)))
		Ready = {};
	return Ready;
}

bool
InvalidationUpdater::DefaultInvalidate(IWidget& wgt)
{
	InvalidateVisible(wgt);
	return true;
}

bool
InvalidationUpdater::DefaultInvalidateControl(IWidget& wgt)
{
	if(IsEnabled(wgt))
		InvalidateVisible(wgt);
	return true;
}


void
ActAfter(IWidget& wgt, Timers::TimeSpan delay)
{
	auto p_ani(make_shared<GAnimationSession<InvalidationUpdater>>());
	auto p_timer(make_shared<Timers::Timer>(delay));

	SetupByTimer(*p_ani, wgt, *p_timer, [p_ani, p_timer, &wgt]{
		Hide(wgt);
		YTraceDe(Debug, "Widget is hidden by UI::Display callback.");
		p_ani->GetConnectionRef().Ready = {};
	});
}

} // namespace UI;

} // namespace YSLib;

