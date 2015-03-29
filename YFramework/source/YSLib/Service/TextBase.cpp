/*
	© 2009-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TextBase.cpp
\ingroup Service
\brief 基础文本渲染逻辑对象。
\version r2512
\author FrankHB <frankhb1989@gmail.com>
\since build 275
\par 创建时间:
	2009-11-13 00:06:05 +0800
\par 修改时间:
	2014-03-24 20:56 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::TextBase
*/


#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_TextBase
#include YFM_YSLib_Service_YBlit

using namespace ystdex;

namespace YSLib
{

using namespace Drawing;
using namespace Text;

namespace Drawing
{

TextState::TextState(const Drawing::Font& font, const Padding& m)
	: PenStyle(font),
	Margin(m)
{}
TextState::TextState(FontCache& fc, const Padding& m)
	: TextState(Drawing::Font(fc.GetDefaultTypefacePtr()->GetFontFamily()), m)
{}

void
TextState::PutNewline()
{
	CarriageReturn(*this);
	// XXX: Conversion to 'SPos' might be implementation-defined.
	Pen.Y += SPos(GetTextLineHeightExOf(*this));
}

void
TextState::ResetPen()
{
	//	Pen.Y = Margin.Top + GetTextLineHeightExOf(*this);
	//	Pen.Y = Margin.Top + pCache->GetAscender();
	CarriageReturn(*this),
	Pen.Y = GetTextLineBaseOf(*this);
}
void
TextState::ResetPen(const Point& pt, const Padding& m)
{
	Pen = Point(pt.X + m.Left, pt.Y + Font.GetAscender() + m.Top);
}


void
SetCurrentTextLineNOf(TextState& ts, std::uint16_t n)
{
	// XXX: Conversion to 'SPos' might be implementation-defined.
	ts.Pen.Y = GetTextLineBaseOf(ts) + SPos(GetTextLineHeightExOf(ts) * n);
}

void
MovePen(TextState& ts, ucs4_t c)
{
	CharBitmap sbit(ts.Font.GetGlyph(c));

	ts.Pen.X += ts.Font.GetAdvance(c, sbit);
}

} // namespace Drawing;

namespace Text
{


} // namespace Text;

} // namespace YSLib;

