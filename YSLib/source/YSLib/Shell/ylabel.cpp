/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ylabel.cpp
\ingroup Shell
\brief 样式无关的标签模块。
\version 0.2062;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-01-22 08:32:34 +0800;
\par 修改时间:
	2011-05-10 21:46 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::Label;
*/


#include "ylabel.h"
#include "yuicont.h"
#include "ywindow.h"
#include "ygui.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Widgets)

MLabel::MLabel(const Drawing::Font& f, MLabel::TextAlignmentStyle a)
	: Font(f), Margin(2, 2, 2, 2),
	HorizontalAlignment(a), VerticalAlignment(Center),
	/*AutoSize(false), AutoEllipsis(false),*/ Text()
{}

void
MLabel::PaintText(IWidget& w, Color c, const Graphics& g, const Point& pt)
{
	Drawing::TextState ts;
	const Rect& r(GetBoundsOf(w));

	ts.Font.SetFont(Font);
	ts.ResetForBounds(r, g.GetSize(), Margin);
	ts.Color = c;

	switch(HorizontalAlignment)
	{
	case Center:
	case Right:
		{
			SPos horizontal_offset(r.Width - GetHorizontalFrom(Margin)
				- FetchStringWidth(ts.Font, Text));

			if(horizontal_offset > 0)
			{
				if(HorizontalAlignment == Center)
					horizontal_offset /= 2;
				ts.PenX += horizontal_offset;
			}
		}
	case Left:
	default:
		break;
	}

	SPos vertical_offset(0);

	switch(VerticalAlignment)
	{
	case Center:
	case Down:
		{
			SPos vertical_offset(r.Height - GetHorizontalFrom(Margin)
				- GetLnHeightFrom(ts));

			if(vertical_offset > 0)
			{
				if(VerticalAlignment == Center)
					vertical_offset /= 2;
				ts.PenY += vertical_offset;
			}
		}
	case Up:
	default:
		break;
	}
	ts.PenY += vertical_offset;
	DrawText(g, ts, Text);
}


Label::Label(const Rect& r, const Drawing::Font& f)
	: Widget(r), MLabel(f)
{}

void
Label::Paint()
{
	YWidgetAssert(this, Widgets::Label, Paint);

	Widget::Paint();
	PaintText(*this, ForeColor, FetchContext(*this), LocateForWindow(*this));
}


MTextList::MTextList(GHandle<ListType> p, const Drawing::Font& f)
	: MLabel(f),
	pList(p), text_state(Font)
{}

MTextList::ListType&
MTextList::GetList() const
{
	if(!pList)
		pList = new ListType();
	return *pList;
}
MTextList::ItemType*
MTextList::GetItemPtr(MTextList::IndexType i) const
{
	ListType& list(GetList());

	return IsInInterval<IndexType>(i, list.size())
		? &list[i] : nullptr;
}
SDst
MTextList::GetItemHeight() const
{
	return GetLnHeightExFrom(text_state);
}
Drawing::TextState&
MTextList::GetTextState()
{
	return text_state;
}

void
MTextList::RefreshTextState()
{
	text_state.LineGap = GetVerticalFrom(Margin);
	text_state.Font.SetFont(Font);
}

/*void
MTextList::PaintTextList(Widget& w, const Point& pt)
{
	IWindow* pWnd(FetchDirectWindowPtr(
		polymorphic_crosscast<IWidget&>(w)));

	if(pWnd && wpTextRegion)
	{
		wpTextRegion->Font = Font;
		wpTextRegion->Font.Update();
		wpTextRegion->ResetPen();
		wpTextRegion->Color = w.ForeColor;
		wpTextRegion->SetSize(w.GetWidth(), w.GetHeight());
		SetMarginsTo(*wpTextRegion, 2, 2, 2, 2);
		DrawText(pWnd->GetContext(), pt);
		wpTextRegion->SetSize(0, 0);
	}
}*/

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

