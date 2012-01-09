/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TextLayout.cpp
\ingroup Service
\brief 文本布局计算。
\version r6890;
\author FrankHB<frankhb1989@gmail.com>
\since build 275 。
\par 创建时间:
	2009-11-13 00:06:05 +0800;
\par 修改时间:
	2012-01-05 15:54 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Service::TextLayout;
*/


#include "YSLib/Service/TextLayout.h"
#include "YSLib/Service/yblit.h"

using namespace ystdex;

YSL_BEGIN

using namespace Drawing;
using namespace Text;

YSL_BEGIN_NAMESPACE(Drawing)

SDst
FetchResizedBottomMargin(const TextState& ts, SDst h)
{
	YAssert(GetTextLineHeightExOf(ts) != 0,
		"Zero line height found @ FetchResizedBottomMargin");

	return ts.Margin.Bottom + (h + ts.LineGap - GetVerticalOf(ts.Margin))
		% GetTextLineHeightExOf(ts);
}

u16
FetchResizedLineN(const TextState& ts, SDst h)
{
	YAssert(GetTextLineHeightExOf(ts) != 0,
		"Zero line height found @ FetchResizedLineN");

	return (h + ts.LineGap - GetVerticalOf(ts.Margin))
		/ GetTextLineHeightExOf(ts);
}

SPos
FetchLastLineBasePosition(const TextState& ts, SDst h)
{
	const u16 n(FetchResizedLineN(ts, h));

	return ts.Margin.Top + ts.GetCache().GetAscender()
		+ GetTextLineHeightExOf(ts) * (n > 0 ? n - 1 : n);
//	return h - ts.Margin.Bottom + ts.GetCache().GetDescender() + 1;
}


SDst
FetchCharWidth(FontCache& cache, ucs4_t c)
{
	CharBitmap sbit(cache.GetGlyph(c));

	return cache.GetAdvance(c, sbit);
}

YSL_END_NAMESPACE(Drawing)

YSL_END

