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
\version 0.1229;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-06-20 08:59:56 +0800;
\par 修改时间:
	2011-06-12 09:09 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Progress;
*/


#include "progress.h"
#include "ywindow.h"
#include "ygui.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Widgets)

ProgressBar::ProgressBar(const Rect& r, u16 m)
	: Widget(r), GMRange<u16>(m, 0)
{}

void
ProgressBar::Paint()
{
	const Graphics& g(FetchDirectWindowPtr(*this)->GetContext());
	const Point loc(LocateForWindow(*this));
	Styles::Palette& pal(FetchGUIShell().Colors);

	if(!IsTransparent())
	{
		FillRect(g, loc, GetSize(), pal[Styles::Track]);
		DrawRect(g, loc, GetSize(), pal[Styles::ActiveBorder]);

	}
	if(GetWidth() > 2 && GetHeight() > 2)
		FillRect(g, Point(loc.X + 1, loc.Y + 1),
			Size(value * (GetWidth() - 2) / max_value, GetHeight() - 2),
			pal[Styles::HotTracking]);
}

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

