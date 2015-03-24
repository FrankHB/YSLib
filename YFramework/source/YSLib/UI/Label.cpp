/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Label.cpp
\ingroup UI
\brief 样式无关的用户界面标签。
\version r1435
\author FrankHB <frankhb1989@gmail.com>
\since build 188
\par 创建时间:
	2011-01-22 08:32:34 +0800
\par 修改时间:
	2015-03-22 18:21 +0800
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

using namespace Drawing;

namespace UI
{

MLabel::MLabel(const Drawing::Font& fnt, Color c, TextAlignment a)
	: ForeColor(c), Font(fnt), HorizontalAlignment(a)
{}

void
MLabel::operator()(PaintEventArgs&& e) const
{
	DrawText(GetSizeOf(e.GetSender()), e);
}

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
				// XXX: Conversion to 'SPos' might be implementation-defined.
				SPos horizontal_offset(SPos(s.Width - GetHorizontalOf(Margin)
					- FetchStringWidth(Font, Text)));

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
				// XXX: Conversion to 'SPos' might be implementation-defined.
				SPos vertical_offset(SPos(s.Height - GetVerticalOf(Margin)
					- Font.GetHeight()));

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
MLabel::DrawText(const Size& s, const PaintContext& pc) const
{
	const auto r(Rect(pc.Location, s) + Margin);
	TextState ts(Font, FetchMargin(r, pc.Target.GetSize()));

	ts.Color = ForeColor,
	ts.ResetPen(pc.Location, Margin);
	ts.Pen += GetAlignedPenOffset(s);
	UpdateClippedText({pc.Target, pc.Location, pc.ClipArea & r}, ts, Text,
		AutoWrapLine);
}

void
MLabel::DefaultUpdateClippedText(const PaintContext& pc, TextState& ts,
	const String& text, bool auto_wrap_line)
{
	DrawClippedText(pc.Target, pc.ClipArea, ts, text, auto_wrap_line);
}


Rect
Label::CalculateBounds(const String& text, Rect r, const Drawing::Font& fnt,
	const Padding& m)
{
	if(r.GetSize() == Size::Invalid)
	{
		r.GetSizeRef() = {FetchStringWidth(fnt, text), fnt.GetHeight()};
		// TODO: Add %operator-=?
		r = r - m;
	}
	return r;
}

void
Label::Refresh(PaintEventArgs&& e)
{
	(*this)(std::move(e));
}

unique_ptr<Label>
MakeLabel(const String& text, const Rect& r, const Font& fnt,
	const Padding& m)
{
	auto p(make_unique<Label>(Label::CalculateBounds(text, r, fnt, m), fnt));

	yunseq(p->Margin = m, p->Text = text);
	return p;
}

void
SetupContentsOf(Label& lbl, const String& text, const Rect& r, const Font& fnt,
	const Padding& m)
{
	SetBoundsOf(lbl, Label::CalculateBounds(text, r, fnt, m));
	yunseq(lbl.Font = fnt, lbl.Margin = m, lbl.Text = text);
}

} // namespace UI;

} // namespace YSLib;

