/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YBrush.cpp
\ingroup UI
\brief 图形用户界面画刷。
\version r157
\author FrankHB<frankhb1989@gmail.com>
\since build 293
\par 创建时间:
	2012-01-10 19:56:59 +0800
\par 修改时间:
	2012-09-04 12:47 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YBrush
*/


#include "YSLib/UI/YBrush.h"
#include "YSLib/Service/ydraw.h"
#include "YSLib/UI/ygui.h"
#include "YSLib/UI/ywidget.h"

YSL_BEGIN

using namespace Drawing;

YSL_BEGIN_NAMESPACE(Components)

void
SolidBrush::operator()(PaintEventArgs&& e)
{
	if(!e.GetSender().GetView().IsTransparent())
		Drawing::FillRect(e.Target, e.ClipArea, Color);
}


void
ImageBrush::operator()(PaintEventArgs&& e)
{
	if(bool(Image) && !e.GetSender().GetView().IsTransparent())
	{
		const auto& g(e.Target);
		const Rect& r(e.ClipArea);

		CopyTo(g.GetBufferPtr(), Image->GetContext(), g.GetSize(),
			r.GetPoint(), r.GetPoint(), r.GetSize());
	}
}


BorderStyle::BorderStyle()
	: ActiveColor(FetchGUIState().Colors[Styles::ActiveBorder]),
	InactiveColor(FetchGUIState().Colors[Styles::InactiveBorder])
{}


void
BorderBrush::operator()(PaintEventArgs&& e)
{
	auto& sender(e.GetSender());

	if(!sender.GetView().IsTransparent())
		DrawRect(e.Target, e.ClipArea = Rect(e.Location, GetSizeOf(sender)),
			IsFocused(sender) ? Style.ActiveColor : Style.InactiveColor);
}

YSL_END_NAMESPACE(Components)

YSL_END

