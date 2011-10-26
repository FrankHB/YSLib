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
\version r1266;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-10-14 18:12:20 +0800;
\par 修改时间:
	2011-10-26 08:17 +0800;
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

namespace
{
	void
	ConvertByte(char(&dst)[3], byte data)
	{
		yunsequenced(dst[0] = (data >> 4 & 0x0F) + '0',
			dst[1] = (data & 0x0F) + '0');
		if(dst[0] > '9')
			dst[0] += 'A' - '9' - 1;
		if(dst[1] > '9')
			dst[1] += 'A' - '9' - 1;
		dst[2] = 0;
	}
}

HexViewArea::HexViewArea(const Rect& r, FontCache& fc)
	: ScrollableContainer(r),
	text_state(fc), item_num(0), lines(), source()
{
	HorizontalScrollBar.SetVisible(false);
	VerticalScrollBar.SetVisible(true);
	VerticalScrollBar.SetSmallDelta(1);
	VerticalScrollBar.GetTrack().GetScroll() += [this](IWidget&,
		ScrollEventArgs&& e){
		LocateViewPosition(e.Value);
		Invalidate(*this);
	};
	text_state.Color = ColorSpace::Black;
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
	source.Open(path);
	VerticalScrollBar.SetMaxValue(source.GetSize() / ItemPerLine);
	VerticalScrollBar.SetLargeDelta(item_num);
}

void
HexViewArea::LocateViewPosition(SDst h)
{
	UpdateData(ItemPerLine * h);
}

Rect
HexViewArea::Refresh(const PaintEventArgs& e)
{
	using namespace Text;

	// TODO: refresh for 'rect' properly;
	Widget::Refresh(PaintEventArgs(e.Target, e.Location, Rect(e.Location,
		GetWidth() - VerticalScrollBar.GetWidth(), GetHeight())));
//	Widget::Refresh(e);
	ScrollableContainer::Refresh(e);

	const auto lh(GetTextLineHeightOf(text_state));
	const SPos h(GetHeight());
	TextRenderer tr(text_state, e.Target);
	auto i_line(lines.cbegin());
	auto pos(source.GetPosition());

	text_state.ResetPen();

	auto& y(text_state.PenY);

	while(y < h && i_line != lines.cend())
	{
		const auto& line(*i_line);
		auto& x(text_state.PenX);
		char straddr[(32 >> 2) + 1];

		x = text_state.Margin.Left;
		std::sprintf(straddr, "%08X", pos);
		PutLine(tr, straddr);
		x += 10;
		for(LineType::size_type i(0); i < ItemPerLine; ++i)
		{
			char str[3];
	
			ConvertByte(str, line[i]);
			PutLine(tr, str);
			x += 8;
		}
		yunsequenced(y += lh + text_state.LineGap, ++i_line);
		pos += ItemPerLine;
	}
	return Rect(e.Location, GetSize());
}

void
HexViewArea::Reset()
{
	lines.clear();
	item_num = FetchResizedLineN(text_state, GetHeight());
	source.Close();
	Invalidate(*this);
}

void
HexViewArea::UpdateData(u32 pos)
{
	if(source.IsValid() && pos < source.GetSize())
	{
		source.SetPosition(pos, SEEK_SET);

		lines.clear();
		while(!source.CheckEOF() && lines.size() < item_num)
		{
			LineType line;

			source.Read(&line[0], 1, ItemPerLine);
			lines.push_back(std::move(line));
		}
	//	VerticalScrollBar.SetValue(pos / ItemPerLine);
		source.SetPosition(pos, SEEK_SET);
		Invalidate(*this);
	}
}

YSL_END_NAMESPACE(Components)

YSL_END

