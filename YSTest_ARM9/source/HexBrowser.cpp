/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HexBrowser.cpp
\ingroup YReader
\brief 十六进制浏览器。
\version r1472;
\author FrankHB<frankhb1989@gmail.com>
\since build 253 。
\par 创建时间:
	2011-10-14 18:12:20 +0800;
\par 修改时间:
	2012-03-18 13:54 +0800;
\par 文本编码:
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
	: ScrollableContainer(r), HexView(fc),
	model()
{
	SetTransparent(false);
	SetVisibleOf(HorizontalScrollBar, false);
	SetVisibleOf(VerticalScrollBar, true);
	VerticalScrollBar.SetSmallDelta(1);
	yunseq(
		VerticalScrollBar.GetTrack().GetScroll() += [this](ScrollEventArgs&& e){
			LocateViewPosition(SDst(round(e.GetValue())));
		},
		FetchEvent<KeyDown>(*this) += [this](KeyEventArgs&& e){
			const auto key(e.GetKeyCode());
			ScrollCategory t(ScrollCategory::SmallDecrement);

			switch(key)
			{
				case KeySpace::Down:
					t = ScrollCategory::SmallIncrement;
					break;
				case KeySpace::PgUp:
					t = ScrollCategory::LargeDecrement;
					break;
				case KeySpace::PgDn:
					t = ScrollCategory::LargeIncrement;
				case KeySpace::Up:
					break;
				default:
					return;
			}
			VerticalScrollBar.LocateThumb(key == KeySpace::Up || key
				== KeySpace::Down ? VerticalScrollBar.GetSmallDelta()
				: VerticalScrollBar.GetLargeDelta(), t);
			RequestFocus(*this);
			e.Handled = true;
		},
		FetchEvent<KeyHeld>(*this) += OnKeyHeld
	);
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
	model = make_unique<File>(path);

	const IndexType n_total_ln((model.GetSize() + ItemPerLine - 1)
		/ ItemPerLine);

	if(n_total_ln > GetItemNum())
	{
		VerticalScrollBar.SetMaxValue(n_total_ln - GetItemNum());
		VerticalScrollBar.SetLargeDelta(GetItemNum());
	}
	else
		SetVisibleOf(VerticalScrollBar, false);
}

void
HexViewArea::LocateViewPosition(u32 line)
{
	UpdateData(ItemPerLine * line);
	UpdateView(true);
}

void
HexViewArea::Refresh(PaintEventArgs&& e)
{
	using namespace Text;

	// TODO: refresh for 'rect' properly;
//	Background(PaintEventArgs(*this, e.Target, e.Location, Rect(e.Location,
//		GetWidth(), GetHeight())));
//	Background(PaintEventArgs(*this, e));
	ScrollableContainer::Refresh(std::move(e));
	TextState.ResetPen();

	yconstexpr auto ItemPerLine(HexViewArea::ItemPerLine); // TODO: fix linkage;
	auto& y(TextState.PenY);
	const SDst lh(GetItemHeight()), h(GetHeight()),
		w_all(GetWidth() - VerticalScrollBar.GetWidth()
			- GetHorizontalOf(TextState.Margin)),
		w_blank(w_all / (10 + ItemPerLine * 3)),
		w_ch((w_all - w_blank * (1 + ItemPerLine)) / (8 + ItemPerLine * 2)),
		w_addr(w_ch * 8 + w_blank),
		w_item(w_ch * 2 + w_blank);
	const int fsize(model.GetSize());
	auto& pen_x(TextState.PenX);
	TextRenderer tr(TextState, e.Target);
	auto pos(model.GetPosition());
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

		const auto n(min<IndexType>(fsize - pos, ItemPerLine));

		for(IndexType j(0); j < n; ++j)
		{
			pen_x = x;
			PutLine(tr, &*i_data, &*i_data + 2);
			i_data += 2;
			x += w_item;
		}
		yunseq(y += lh + TextState.LineGap, pos += ItemPerLine);
	}
}

void
HexViewArea::Reset()
{
	VerticalScrollBar.SetValue(0);
	ClearData();
	UpdateItemNum(GetHeight());
	UpdateView();
}

void
HexViewArea::UpdateData(u32 pos)
{
	if(model.IsValid() && pos < model.GetSize())
	{
		const DataType::size_type n(ItemPerLine * GetItemNum() * 2);

		model.SetPosition(pos, SEEK_SET);
		ResizeData(n);

		auto b(GetBegin());
		const auto e(GetEnd());

		while(!model.CheckEOF() && b != e)
		{
			byte c(std::fgetc(model.GetPtr()));
			char h, l;

			yunseq(h = (c >> 4 & 0x0F) + '0', l = (c & 0x0F) + '0');
			*b++ = h > '9' ? h + 'A' - '9' - 1 : h;
			*b++ = l > '9' ? l + 'A' - '9' - 1 : l;
		}
	//	VerticalScrollBar.SetValue(pos / ItemPerLine);
		ResizeData(b - GetBegin());
		model.SetPosition(pos, SEEK_SET); // Refresh 需要据此判断接近文件结尾。
	}
}

void
HexViewArea::UpdateView(bool is_active)
{
	ViewChanged(ViewArgs(*this, is_active));
	Invalidate(*this);
}

YSL_END_NAMESPACE(Components)

YSL_END

