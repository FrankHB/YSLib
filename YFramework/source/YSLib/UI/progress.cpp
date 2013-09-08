/*
	Copyright by FrankHB 2011-2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file progress.cpp
\ingroup UI
\brief 样式相关的图形用户界面进度部件。
\version r363
\author FrankHB <frankhb1989@gmail.com>
\since build 219
\par 创建时间:
	2011-06-20 08:59:56 +0800
\par 修改时间:
	2013-09-07 02:31 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Progress
*/


#include "YSLib/UI/progress.h"
#include "YSLib/UI/ygui.h"
#include "YSLib/UI/Border.h"

namespace YSLib
{

namespace UI
{

ProgressBar::ProgressBar(const Rect& r, ValueType m)
	: Control(r, NoBackgroundTag()), GMRange<float>(m == 0 ? 1 : m, 0)
{
	auto& pal(FetchGUIState().Colors);
	BorderStyle style;

	style.ActiveColor = pal[Styles::InactiveBorder];
	yunseq(
	Background = SolidBrush(pal[Styles::Track]),
	ForeColor = pal[Styles::HotTracking],
	FetchEvent<Paint>(*this).Add(BorderBrush(style), BoundaryPriority)
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
	Size s(GetSizeOf(*this));

	e.ClipArea = Rect(pt, s);
	if(YB_LIKELY(s.Width > 2 && s.Height > 2))
	{
		yunseq(s.Width -= 2, s.Height -= 2, pt.X += 1, pt.Y += 1);

		const SDst w_bar(round(value * s.Width / max_value));

		FillRect(g, pt, Size(w_bar, s.Height), ForeColor);
		pt.X += w_bar;
		if(s.Width > w_bar)
			// TODO: Finish drawing with non-solid brushes.
			if(const auto p = Background.target<SolidBrush>())
				FillRect(g, pt, Size(s.Width - w_bar, s.Height), p->Color);
	}
}

} // namespace UI;

} // namespace YSLib;

