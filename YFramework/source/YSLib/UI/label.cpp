﻿/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file label.cpp
\ingroup UI
\brief 样式无关的用户界面标签。
\version r2165;
\author FrankHB<frankhb1989@gmail.com>
\since build 188 。
\par 创建时间:
	2011-01-22 08:32:34 +0800;
\par 修改时间:
	2011-11-30 19:27 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Label;
*/


#include "YSLib/UI/label.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

MLabel::MLabel(const Drawing::Font& fnt, TextAlignment a)
	: Font(fnt), Margin(2, 2, 2, 2),
	HorizontalAlignment(a), VerticalAlignment(TextAlignment::Center),
	/*AutoSize(false), AutoEllipsis(false),*/ Text()
{}

void
MLabel::PaintText(const Size& s, Color c, const PaintContext& e)
{
	Drawing::TextState ts;
	const auto& bounds(Rect(e.Location, s));

	(ts.Font = Font).Update();
	ts.ResetForBounds(bounds, e.Target.GetSize(), Margin);
	ts.Color = c;

	switch(HorizontalAlignment)
	{
	case TextAlignment::Center:
	case TextAlignment::Right:
		{
			SPos horizontal_offset(bounds.Width - GetHorizontalOf(Margin)
				- FetchStringWidth(ts.Font, Text));

			if(horizontal_offset > 0)
			{
				if(HorizontalAlignment == TextAlignment::Center)
					horizontal_offset /= 2;
				ts.PenX += horizontal_offset;
			}
		}
	case TextAlignment::Left:
	default:
		break;
	}

	SPos vertical_offset(0);

	switch(VerticalAlignment)
	{
	case TextAlignment::Center:
	case TextAlignment::Down:
		{
			SPos vertical_offset(bounds.Height - GetHorizontalOf(Margin)
				- GetTextLineHeightOf(ts));

			if(vertical_offset > 0)
			{
				if(VerticalAlignment == TextAlignment::Center)
					vertical_offset /= 2;
				ts.PenY += vertical_offset;
			}
		}
	case TextAlignment::Up:
	default:
		break;
	}
	ts.PenY += vertical_offset;
	DrawClippedText(e.Target, e.ClipArea, ts, Text);
}


Rect
Label::Refresh(const PaintContext& pc)
{
	Widget::Refresh(pc);
	PaintText(GetSizeOf(*this), ForeColor, pc);
	return Rect(pc.Location, GetSizeOf(*this));
}


MTextList::MTextList(const shared_ptr<ListType>& h, const Drawing::Font& fnt)
	: MLabel(fnt),
	hList(h), text_state(Font)
{
	if(!hList)
		hList = share_raw(new ListType());
}

MTextList::ItemType*
MTextList::GetItemPtr(IndexType idx) const
{
	ListType& lst(GetList());

	return IsInInterval<IndexType>(idx, lst.size()) ? &lst[idx] : nullptr;
}

void
MTextList::RefreshTextState()
{
	text_state.LineGap = GetVerticalOf(Margin);
	(text_state.Font = Font).Update();
}

/*void
MTextList::PaintTextList(Widget& wgt, const Point& pt)
{
	IWindow* pWnd(FetchDirectWindowPtr(
		polymorphic_crosscast<IWidget&>(wgt)));

	if(pWnd && wpTextRegion)
	{
		wpTextRegion->Font = Font;
		wpTextRegion->Font.Update();
		wpTextRegion->ResetPen();
		wpTextRegion->Color = wgt.ForeColor;
		wpTextRegion->SetSize(wgt.GetWidth(), wgt.GetHeight());
		SetMarginsTo(*wpTextRegion, 2, 2, 2, 2);
		DrawText(pWnd->GetContext(), pt);
		wpTextRegion->SetSize(0, 0);
	}
}*/

YSL_END_NAMESPACE(Components)

YSL_END

