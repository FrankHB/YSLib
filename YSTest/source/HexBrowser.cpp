/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HexBrowser.cpp
\ingroup YReader
\brief 十六进制浏览器。
\version r666
\author FrankHB <frankhb1989@gmail.com>
\since build 253
\par 创建时间:
	2011-10-14 18:12:20 +0800
\par 修改时间:
	2015-08-01 13:08 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::HexBrowser
*/


#include "HexBrowser.h"
#include <new>

namespace YSLib
{

using namespace Drawing;
using namespace Text;

namespace UI
{

HexModel::HexModel(const char* path)
{
	source.open(Nonnull(path), std::ios_base::ate | std::ios_base::binary
		| std::ios_base::in);
	size = GetPosition();
}

HexView::HexView(FontCache& fc)
	: TextState(fc), item_num(0), datCurrent()
{
	TextState.Color = ColorSpace::Black;
}


HexViewArea::HexViewArea(const Rect& r, FontCache& fc)
	: ScrollableContainer(r), HexView(fc),
	model()
{
	SetVisibleOf(hsbHorizontal, false);
	SetVisibleOf(vsbVertical, true);
	vsbVertical.SetSmallDelta(1);
	yunseq(
	vsbVertical.GetTrackRef().Scroll += [this](ScrollEventArgs&& e){
		LocateViewPosition(round(e.GetValue()));
	},
	FetchEvent<KeyDown>(*this) += [this](KeyEventArgs&& e){
		using namespace KeyCodes;

		const auto& k(e.GetKeys());

		if(k.count() != 1)
			return;

		ScrollCategory t(ScrollCategory::SmallDecrement);

		if(k[Down])
			t = ScrollCategory::SmallIncrement;
		else if(k[PgUp])
			t = ScrollCategory::LargeDecrement;
		else if(k[PgDn])
			t = ScrollCategory::LargeIncrement;
		else if(!k[Up])
			return;
		vsbVertical.LocateThumb(k[Up] || k[Down] ? vsbVertical.GetSmallDelta()
			: vsbVertical.GetLargeDelta(), t);
		RequestFocus(*this);
		e.Handled = true;
	},
	FetchEvent<KeyHeld>(*this) += OnKeyHeld
	);
	Reset();
}

void
HexViewArea::Load(const char* path)
{
	model = HexModel(path);
	Reset();

	if(model)
	{
		const IndexType n_total_ln((model.GetSize() + ItemPerLine - 1)
			/ ItemPerLine);

		if(n_total_ln > GetItemNum())
		{
			vsbVertical.SetMaxValue(n_total_ln - GetItemNum());
			vsbVertical.SetLargeDelta(GetItemNum());
			return;
		}
	}
	SetVisibleOf(vsbVertical, {});
}

void
HexViewArea::LocateViewPosition(size_t line)
{
	UpdateData(std::uint32_t(ItemPerLine * line));
	UpdateView(true);
}

void
HexViewArea::Refresh(PaintEventArgs&& e)
{
	using namespace Text;

	// TODO: Refresh for 'rect' properly.
//	Background(PaintEventArgs(*this, e.Target, e.Location, Rect(e.Location,
//		GetWidth(), GetHeight())));
//	Background(PaintEventArgs(*this, e));
	ScrollableContainer::Refresh(std::move(e));
	TextState.ResetPen();

	auto& y(TextState.Pen.Y);
	const SDst lh(GetItemHeight()), h(GetHeight()),
		w_all(GetWidth() - vsbVertical.GetWidth()
			- GetHorizontalOf(TextState.Margin)),
		w_blank(w_all / (10 + ItemPerLine * 3)),
		w_ch((w_all - w_blank * (1 + ItemPerLine)) / (8 + ItemPerLine * 2)),
		w_addr(w_ch * 8 + w_blank),
		w_item(w_ch * 2 + w_blank);
	const auto fsize(model.GetSize());
	auto& pen_x(TextState.Pen.X);
	TextRenderer tr(TextState, e.Target);
	auto pos(model.GetPosition());
	auto i_data(datCurrent.cbegin());

	while((y < 0 || SDst(y) < h) && pos < fsize && i_data < datCurrent.cend())
	{
		pen_x = TextState.Margin.Left;

		auto x(pen_x);

		{
			char straddr[(32 >> 2) + 1];

			std::sprintf(straddr, "%08tX", pos);
			PutLine(tr, straddr);
		}
		// XXX: Conversion to 'SPos' might be implementation-defined.
		x += SPos(w_addr);

		const auto n(min<IndexType>(fsize - pos, IndexType(ItemPerLine)));

		// XXX: Conversion to 'ptrdiff_t' might be implementation-defined.
		for(IndexType j(0); j < n;
			yunseq(++j, i_data += 2, x += ptrdiff_t(w_item)))
		{
			pen_x = x;
			PutLine(tr, &*i_data, &*i_data + 2);
		}
		// XXX: Conversion to 'SPos' might be implementation-defined.
		yunseq(y += SPos(lh + TextState.LineGap), pos += ItemPerLine);
	}
}

void
HexViewArea::Reset()
{
	vsbVertical.SetValue(0);
	datCurrent.clear();
	UpdateItemNum(GetHeight());
	UpdateView();
}

void
HexViewArea::UpdateData(std::uint32_t pos)
{
	if(model.IsValid() && pos < model.GetSize())
	{
		const DataType::size_type n(ItemPerLine * GetItemNum());
		DataType v_buf(n);

		if(YB_UNLIKELY(v_buf.empty()))
		{
			YTraceDe(Notice, "Give up empty view area.");
			return;
		}
		// XXX: Conversion to 'ptrdiff_t' might be implementation-defined.
		model.Seek(std::streampos(pos));
		v_buf.resize(DataType::size_type(model.Fill(&v_buf[0],
			std::streamsize(n))));
		datCurrent.resize(v_buf.size() * 2);

		if(YB_UNLIKELY(datCurrent.empty()))
		{
			YTraceDe(Warning, "Empty data read.");
			return;
		}

		auto b(&datCurrent[0]);

		for(const char c : v_buf)
		{
			char h, l;

			yunseq(h = (c >> 4 & 0x0F) + '0', l = (c & 0x0F) + '0');
			*b++ = h > '9' ? h + 'A' - '9' - 1 : h;
			*b++ = l > '9' ? l + 'A' - '9' - 1 : l;
		}
	//	vsbVertical.SetValue(pos / ItemPerLine);
		// NOTE: 'Refresh' needs this to check if it is towards EOF.
		// XXX: Conversion to 'ptrdiff_t' might be implementation-defined.
		model.Seek(std::streampos(pos));
	}
}

void
HexViewArea::UpdateView(bool is_active)
{
	ViewChanged(ViewArgs(*this, is_active));
	Invalidate(*this);
}

} // namespace UI;

} // namespace YSLib;

