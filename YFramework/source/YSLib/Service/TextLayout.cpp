/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TextLayout.cpp
\ingroup Service
\brief 文本布局计算。
\version r2478
\author FrankHB <frankhb1989@gmail.com>
\since build 275
\par 创建时间:
	2009-11-13 00:06:05 +0800
\par 修改时间:
	2014-12-02 18:47 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::TextLayout
*/


#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_TextLayout
#include YFM_YSLib_Service_YBlit

using namespace ystdex;

namespace YSLib
{

using namespace Drawing;
using namespace Text;

namespace Drawing
{

SDst
FetchResizedBottomMargin(const TextState& ts, SDst h)
{
	YAssert(GetTextLineHeightExOf(ts) != 0, "Zero line height found.");
	return ts.Margin.Bottom + (h + ts.LineGap - GetVerticalOf(ts.Margin))
		% GetTextLineHeightExOf(ts);
}

std::uint16_t
FetchResizedLineN(const TextState& ts, SDst h)
{
	YAssert(GetTextLineHeightExOf(ts) != 0, "Zero line height found.");
	return (h + ts.LineGap - GetVerticalOf(ts.Margin))
		/ GetTextLineHeightExOf(ts);
}

SPos
FetchLastLineBasePosition(const TextState& ts, SDst h)
{
	const std::uint16_t n(FetchResizedLineN(ts, h));

	return ts.Margin.Top + ts.Font.GetAscender()
		+ GetTextLineHeightExOf(ts) * (n > 0 ? n - 1 : n);
//	return h - ts.Margin.Bottom + ts.GetCache().GetDescender() + 1;
}


SDst
FetchCharWidth(const Font& font, ucs4_t c)
{
	return font.GetAdvance(c, font.GetGlyph(c));
}

} // namespace Drawing;

} // namespace YSLib;

