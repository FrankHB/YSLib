/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file progress.cpp
\ingroup UI
\brief 样式相关的图形用户界面进度部件。
\version r1291;
\author FrankHB<frankhb1989@gmail.com>
\since build 219 。
\par 创建时间:
	2011-06-20 08:59:56 +0800;
\par 修改时间:
	2011-12-20 19:00 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Progress;
*/


#include "YSLib/UI/progress.h"
#include "YSLib/UI/ygui.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

ProgressBar::ProgressBar(const Rect& r, ValueType m)
	: Control(r), GMRange<float>(m, 0)
{
	Styles::Palette& pal(FetchGUIShell().Colors);

	yunseq(BackColor = pal[Styles::Track],
		ForeColor = pal[Styles::HotTracking],
		BorderColor = pal[Styles::ActiveBorder]);
}

void
ProgressBar::SetMaxValue(ValueType m)
{
	if(m > 0)
	{
		if(value > m)
			value = m;
		max_value = m;
	}
}

Rect
ProgressBar::Refresh(const PaintContext& pc)
{
	const auto& g(pc.Target);
	const auto& pt(pc.Location);

	if(!IsTransparent())
	{
		FillRect(g, pt, GetSizeOf(*this), BackColor);
		DrawRect(g, pt, GetSizeOf(*this), BorderColor);
	}
	if(GetWidth() > 2 && GetHeight() > 2)
		FillRect(g, Point(pt.X + 1, pt.Y + 1),
			Size(value * (GetWidth() - 2) / max_value, GetHeight() - 2),
			ForeColor);
	return Rect(pc.Location, GetSizeOf(*this));
}

YSL_END_NAMESPACE(Components)

YSL_END

