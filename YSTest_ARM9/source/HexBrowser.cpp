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
\version r1196;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-10-14 18:12:20 +0800;
\par 修改时间:
	2011-10-22 01:53 +0800;
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
	state(fc), lines(), position(0), Source()
{
	state.Color = ColorSpace::Black;
}

IWidget*
HexViewArea::GetTopWidgetPtr(const Point& pt, bool(&f)(const IWidget&))
{
	auto pCtl(ScrollableContainer::GetTopWidgetPtr(pt, f));

	pCtl = pCtl ? pCtl : this;
	return f(*pCtl) ? pCtl : nullptr;
}

Rect
HexViewArea::Refresh(const PaintEventArgs& e)
{
	using namespace Text;

	Widget::Refresh(e);

	const auto lh(GetTextLineHeightOf(state));
	const SPos h(GetHeight());
	TextRenderer tr(state, e.Target);
	auto i_line(lines.cbegin());
	auto pos(position);

	state.ResetPen();

	auto& y(state.PenY);

	while(y < h && i_line != lines.cend())
	{
		const auto& line(*i_line);
		auto& x(state.PenX);
		char straddr[(32 >> 2) + 1];

		x = state.Margin.Left;
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
		yunsequenced(y += lh + state.LineGap, ++i_line);
		pos += ItemPerLine;
	}
	return Rect(e.Location, GetSize());
}

void
HexViewArea::Reset()
{
	lines.clear();
	position = 0;
	Source.Close();
	Invalidate(*this);
}

void
HexViewArea::UpdateData(u32 pos)
{
	if(Source.IsValid())
	{
		const auto n(FetchResizedLineN(state, GetHeight()));

		Source.SetPosition(pos, SEEK_SET);
		pos = position;
		while(!Source.CheckEOF() && lines.size() < n)
		{
			LineType line;

			Source.Read(&line[0], 1, ItemPerLine);
			lines.push_back(std::move(line));
		}
		Invalidate(*this);
	}
}

YSL_END_NAMESPACE(Components)

YSL_END

