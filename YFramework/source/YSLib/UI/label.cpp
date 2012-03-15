/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file label.cpp
\ingroup UI
\brief 样式无关的用户界面标签。
\version r2183;
\author FrankHB<frankhb1989@gmail.com>
\since build 188 。
\par 创建时间:
	2011-01-22 08:32:34 +0800;
\par 修改时间:
	2012-03-12 13:11 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::UI::Label;
*/


#include "YSLib/UI/label.h"
#include "YSLib/Service/TextLayout.h"
#include "YSLib/UI/ywgtevt.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

MLabel::MLabel(const Drawing::Font& fnt, TextAlignment a)
	: Font(fnt), Margin(Drawing::DefaultMargin),
	HorizontalAlignment(a), VerticalAlignment(TextAlignment::Center),
	/*AutoSize(false), AutoEllipsis(false),*/ Text()
{}

void
MLabel::PaintText(const Size& s, Color c, const PaintContext& e)
{
	Drawing::TextState ts;
	const auto& bounds(Rect(e.Location, s));

	yunseq(ts.Font = Font, ts.Color = c);
	ts.ResetForBounds(bounds, e.Target.GetSize(), Margin);

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
			SPos vertical_offset(bounds.Height - GetVerticalOf(Margin)
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
		hList = make_shared<ListType>();
}

MTextList::ItemType*
MTextList::GetItemPtr(IndexType idx) const
{
	ListType& lst(GetList());

	return IsInInterval<IndexType>(idx, lst.size()) ? &lst[idx] : nullptr;
}

SDst
MTextList::GetMaxTextWidth() const
{
	return FetchMaxTextWidth(Font, GetList().cbegin(), GetList().cend());
}

void
MTextList::RefreshTextState()
{
	yunseq(text_state.LineGap = GetVerticalOf(Margin), text_state.Font = Font);
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

