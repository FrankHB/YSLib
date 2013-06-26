/*
	Copyright by FrankHB 2012 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YBrush.cpp
\ingroup UI
\brief 图形用户界面画刷。
\version r170
\author FrankHB <frankhb1989@gmail.com>
\since build 293
\par 创建时间:
	2012-01-10 19:56:59 +0800
\par 修改时间:
	2013-06-26 19:41 +0800
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

YSL_BEGIN_NAMESPACE(UI)

void
SolidBrush::operator()(PaintEventArgs&& e)
{
	Drawing::FillRect(e.Target, e.ClipArea, Color);
}


void
ImageBrush::operator()(PaintEventArgs&& e)
{
	if(bool(Image))
	{
		const auto& g(e.Target);
		const Rect& r(e.ClipArea);

		CopyTo(g.GetBufferPtr(), Image->GetContext(), g.GetSize(),
			r.GetPoint(), {}, r.GetSize());
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

	DrawRect(e.Target, Rect(e.Location, GetSizeOf(sender)), IsFocused(sender)
		? Style.ActiveColor : Style.InactiveColor);
}

YSL_END_NAMESPACE(UI)

YSL_END

