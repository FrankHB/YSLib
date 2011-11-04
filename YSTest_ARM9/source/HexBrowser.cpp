/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HexBrowser.cpp
\ingroup YReader
\brief 十六进制浏览器。
\version r1366;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-10-14 18:12:20 +0800;
\par 修改时间:
	2011-11-04 18:39 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YReader::HexBrowser;
*/


#include "HexBrowser.h"

YSL_BEGIN

using namespace Drawing;
using namespace Text;

YSL_BEGIN_NAMESPACE(Components)

HexView::HexView(FontCache& fc)
	: TextState(fc), item_num(0), data()
{
	TextState.Color = ColorSpace::Black;
}


HexViewArea::HexViewArea(const Rect& r, FontCache& fc)
	: ScrollableContainer(r), HexModel(), HexView(fc)
{
	HorizontalScrollBar.SetVisible(false);
	VerticalScrollBar.SetVisible(true);
	VerticalScrollBar.SetSmallDelta(1);
	VerticalScrollBar.GetTrack().GetScroll() += [this](ScrollEventArgs&& e){
		LocateViewPosition(e.Value);
		Invalidate(*this);
	};
	Reset();
}

IWidget*
HexViewArea::GetTopWidgetPtr(const Point& pt, bool(&f)(const IWidget&))
{
	auto pCtl(ScrollableContainer::GetTopWidgetPtr(pt, f));

	pCtl = pCtl ? pCtl : this;
	return f(*pCtl) ? pCtl : nullptr;
}

void
HexViewArea::Load(const_path_t path)
{
	Reset();
	Source.Open(path);
	// FIXME: overflow;
	VerticalScrollBar.SetMaxValue((Source.GetSize() + ItemPerLine - 1)
		/ ItemPerLine);
	VerticalScrollBar.SetLargeDelta(GetItemNum());
}

void
HexViewArea::LocateViewPosition(SDst h)
{
	UpdateData(ItemPerLine * h);
}

Rect
HexViewArea::Refresh(const PaintContext& e)
{
	using namespace Text;

	// TODO: refresh for 'rect' properly;
	Widget::Refresh(PaintContext(e.Target, e.Location, Rect(e.Location,
		GetWidth() - VerticalScrollBar.GetWidth(), GetHeight())));
//	Widget::Refresh(e);
	ScrollableContainer::Refresh(e);
	TextState.ResetPen();

	yconstexpr auto ItemPerLine(HexViewArea::ItemPerLine); // TODO: fix linkage;
	auto& y(TextState.PenY);
	const SDst lh(GetTextLineHeightOf(TextState)), h(GetHeight()),
		w_all(GetWidth() - VerticalScrollBar.GetWidth()
			- GetHorizontalOf(TextState.Margin)),
		w_blank(w_all / (10 + ItemPerLine * 3)),
		w_ch((w_all - w_blank * (1 + ItemPerLine)) / (8 + ItemPerLine * 2)),
		w_addr(w_ch * 8 + w_blank),
		w_item(w_ch * 2 + w_blank);
	const int fsize(Source.GetSize());
	auto& pen_x(TextState.PenX);
	TextRenderer tr(TextState, e.Target);
	auto pos(Source.GetPosition());
	auto i_data(GetBegin());

	while(y < h && pos < fsize && i_data < GetEnd())
	{
		pen_x = TextState.Margin.Left;

		auto x(pen_x);

		{
			char straddr[(32 >> 2) + 1];

			std::sprintf(straddr, "%08X", pos);
			PutLine(tr, straddr);
		}
		x += w_addr;

		const auto n(std::min<IndexType>(fsize - pos, ItemPerLine));

		for(IndexType j(0); j < n; ++j)
		{
			pen_x = x;
			PutLine(tr, &*i_data, &*i_data + 2);
			i_data += 2;
			x += w_item;
		}
		yunsequenced(y += lh + TextState.LineGap, pos += ItemPerLine);
	}
	return Rect(e.Location, GetSize());
}

void
HexViewArea::Reset()
{
	VerticalScrollBar.SetValue(0);
	ClearData();
	UpdateItemNum(GetHeight());
	Source.Close();
	Invalidate(*this);
}

void
HexViewArea::UpdateData(u32 pos)
{
	if(Source.IsValid() && pos < Source.GetSize())
	{
		const DataType::size_type n(ItemPerLine * GetItemNum() * 2);
		DataType::size_type i(0);

		Source.SetPosition(pos, SEEK_SET);
		ResizeData(n);
		while(!Source.CheckEOF() && i < n)
		{
			byte b(std::fgetc(Source.GetPtr()));
			char h, l;

			yunsequenced(h = (b >> 4 & 0x0F) + '0', l = (b & 0x0F) + '0');
			(*this)[i++] = h > '9' ? h + 'A' - '9' - 1 : h;
			(*this)[i++] = l > '9' ? l + 'A' - '9' - 1 : l;
		}
	//	VerticalScrollBar.SetValue(pos / ItemPerLine);
		ResizeData(i);
		Source.SetPosition(pos, SEEK_SET); // Refresh 需要据此判断接近文件结尾。
		Invalidate(*this);
	}
}

YSL_END_NAMESPACE(Components)

YSL_END

