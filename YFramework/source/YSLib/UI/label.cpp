/*
	© 2011-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file label.cpp
\ingroup UI
\brief 样式无关的用户界面标签。
\version r1373
\author FrankHB <frankhb1989@gmail.com>
\since build 188
\par 创建时间:
	2011-01-22 08:32:34 +0800
\par 修改时间:
	2014-07-31 20:11 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Label
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_Label
#include YFM_YSLib_Service_TextLayout
#include YFM_YSLib_UI_YWidgetEvent

namespace YSLib
{

namespace UI
{

MLabel::MLabel(const Drawing::Font& fnt, TextAlignment a)
	: Font(fnt), Margin(Drawing::DefaultMargin),
	HorizontalAlignment(a), VerticalAlignment(TextAlignment::Center),
	AutoWrapLine(false), /*AutoSize(false), AutoEllipsis(false),*/ Text()
{}

Point
MLabel::GetAlignedPenOffset(const Size& s) const
{
	Point pt;

	if(!AutoWrapLine)
	{
		switch(HorizontalAlignment)
		{
		case TextAlignment::Center:
		case TextAlignment::Right:
			{
				SPos horizontal_offset(s.Width - GetHorizontalOf(Margin)
					- FetchStringWidth(Font, Text));

				if(horizontal_offset > 0)
				{
					if(HorizontalAlignment == TextAlignment::Center)
						horizontal_offset /= 2;
					pt.X += horizontal_offset;
				}
			}
		case TextAlignment::Left:
		default:
			break;
		}
		switch(VerticalAlignment)
		{
		case TextAlignment::Center:
		case TextAlignment::Down:
			{
				SPos vertical_offset(s.Height - GetVerticalOf(Margin)
					- Font.GetHeight());

				if(vertical_offset > 0)
				{
					if(VerticalAlignment == TextAlignment::Center)
						vertical_offset /= 2;
					pt.Y += vertical_offset;
				}
			}
		case TextAlignment::Up:
		default:
			break;
		}
	}
	return pt;
}

void
MLabel::DrawText(const Size& s, Color c, const PaintContext& e)
{
	const Rect bounds(e.Location, s);
	const auto r(bounds + Margin);
	Drawing::TextState ts(Font, FetchMargin(r, e.Target.GetSize()));

	ts.Color = c,
	ts.ResetPen(e.Location, Margin);
	ts.Pen += GetAlignedPenOffset(s);
	DrawClippedText({e.Target, e.Location, e.ClipArea & r}, ts);
}

void
MLabel::DrawClippedText(const PaintContext& pc, Drawing::TextState& ts)
{
	Drawing::DrawClippedText(pc.Target, pc.ClipArea, ts, Text, AutoWrapLine);
}


void
Label::Refresh(PaintEventArgs&& e)
{
	DrawText(GetSizeOf(*this), ForeColor, e);
}

} // namespace UI;

} // namespace YSLib;

