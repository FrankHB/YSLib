/*
	Copyright by FrankHB 2011 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file label.cpp
\ingroup UI
\brief 样式无关的用户界面标签。
\version r1295
\author FrankHB <frankhb1989@gmail.com>
\since build 188
\par 创建时间:
	2011-01-22 08:32:34 +0800
\par 修改时间:
	2013-08-05 21:36 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Label
*/


#include "YSLib/UI/label.h"
#include "YSLib/Service/TextLayout.h"
#include "YSLib/UI/ywgtevt.h"

namespace YSLib
{

namespace UI
{

MLabel::MLabel(const Drawing::Font& fnt, TextAlignment a)
	: Font(fnt), Margin(Drawing::DefaultMargin),
	HorizontalAlignment(a), VerticalAlignment(TextAlignment::Center),
	AutoWrapLine(false), /*AutoSize(false), AutoEllipsis(false),*/ Text()
{}

void
MLabel::DrawText(const Size& s, Color c, const PaintContext& e)
{
	Drawing::TextState ts;
	const Rect bounds(e.Location, s);

	yunseq(ts.Font = Font, ts.Color = c,
		ts.Margin = FetchMargin(bounds + Margin, e.Target.GetSize())),
	ts.ResetPen(e.Location, Margin);

	if(!AutoWrapLine)
	{
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
					ts.Pen.X += horizontal_offset;
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
				SPos vertical_offset(bounds.Height - GetVerticalOf(Margin)
					- GetTextLineHeightOf(ts));

				if(vertical_offset > 0)
				{
					if(VerticalAlignment == TextAlignment::Center)
						vertical_offset /= 2;
					ts.Pen.Y += vertical_offset;
				}
			}
		case TextAlignment::Up:
		default:
			break;
		}
	}
	DrawClippedText(e.Target, e.ClipArea & (bounds + Margin), ts, Text,
		AutoWrapLine);
}


void
Label::Refresh(PaintEventArgs&& e)
{
	DrawText(GetSizeOf(*this), ForeColor, e);
	e.ClipArea = Rect(e.Location, GetSizeOf(*this));
}


MTextList::MTextList(const shared_ptr<ListType>& h, const Drawing::Font& fnt)
	: MLabel(fnt),
	hList(h), tsList(Font)
{
	if(!hList)
		hList = make_shared<ListType>();
}

MTextList::ItemType*
MTextList::GetItemPtr(const IndexType& idx)
{
	auto& lst(GetListRef());

	return IsInInterval<IndexType>(idx, lst.size()) ? &lst[idx] : nullptr;
}
const MTextList::ItemType*
MTextList::GetItemPtr(const IndexType& idx) const
{
	const auto& lst(GetList());

	return IsInInterval<IndexType>(idx, lst.size()) ? &lst[idx] : nullptr;
}

SDst
MTextList::GetMaxTextWidth() const
{
	return FetchMaxTextWidth(Font, GetList().cbegin(), GetList().cend());
}

MTextList::IndexType
MTextList::Find(const ItemType& text) const
{
	const auto& lst(GetList());
	const auto i(std::find(lst.begin(), lst.end(), text));

	return i != lst.end() ? i - lst.begin() : IndexType(-1);
}

void
MTextList::RefreshTextState()
{
	yunseq(tsList.LineGap = GetVerticalOf(Margin), tsList.Font = Font);
}

} // namespace UI;

} // namespace YSLib;

