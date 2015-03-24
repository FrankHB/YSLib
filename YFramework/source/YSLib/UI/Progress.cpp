/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Progress.cpp
\ingroup UI
\brief 样式相关的图形用户界面进度部件。
\version r396
\author FrankHB <frankhb1989@gmail.com>
\since build 219
\par 创建时间:
	2011-06-20 08:59:56 +0800
\par 修改时间:
	2015-03-22 18:51 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Progress
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_Progress
#include YFM_YSLib_UI_YGUI
#include YFM_YSLib_Service_YBrush
#include YFM_YSLib_UI_Border

namespace YSLib
{

namespace UI
{

ProgressBar::ProgressBar(const Rect& r, ValueType m)
	: Control(r), GMRange<float>(std::isnormal(m) ? m : 1.F, 0)
{
	const auto invalidator([this]{
		Invalidate(*this);
	});
	auto& pal(FetchGUIConfiguration().Colors);

	yunseq(
	Background = Drawing::SolidBrush(pal[Styles::Track]),
	ForeColor = pal[Styles::HotTracking],
	FetchEvent<Paint>(*this).Add(BorderBrush(
		BorderStyle(pal[Styles::InactiveBorder])), BoundaryPriority),
	FetchEvent<GotFocus>(*this) += invalidator,
	FetchEvent<LostFocus>(*this) += invalidator
	);
}

void
ProgressBar::SetMaxValue(ValueType m)
{
	if(YB_LIKELY(m > 0))
	{
		if(YB_LIKELY(value > m))
			value = m;
		max_value = m;
	}
}

void
ProgressBar::Refresh(PaintEventArgs&& e)
{
	const auto& g(e.Target);
	auto pt(e.Location);
	const Rect& bounds(e.ClipArea);
	Size s(GetSizeOf(*this));

	if(YB_LIKELY(s.Width > 2 && s.Height > 2))
	{
		yunseq(s.Width -= 2, s.Height -= 2, pt.X += 1, pt.Y += 1);

		const SDst w_bar(round(value * s.Width / max_value));

		FillRect(g, bounds, {pt, w_bar, s.Height}, ForeColor);
		pt.X += w_bar;
		if(s.Width > w_bar)
			// TODO: Finish drawing with non-solid brushes.
			if(const auto p = Background.target<Drawing::SolidBrush>())
				FillRect(g, bounds, Rect(pt, s.Width - w_bar, s.Height),
					p->Color);
	}
}

} // namespace UI;

} // namespace YSLib;

