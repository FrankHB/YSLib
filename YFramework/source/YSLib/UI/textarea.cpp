/*
	© 2011-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file textarea.cpp
\ingroup UI
\brief 样式无关的用户界面文本区域部件。
\version r466
\author FrankHB <frankhb1989@gmail.com>
\since build 222
\par 创建时间:
	2011-06-30 20:10:27 +0800
\par 修改时间:
	2014-08-12 02:27 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::TextArea
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_TextArea
#include YFM_YSLib_UI_YWidgetEvent

namespace YSLib
{

using namespace Drawing;

namespace UI
{

TextArea::TextArea(const Rect& r, HBrush b, Drawing::Color c)
	: Widget(r, b), TextState(), TextRenderer(*this),
	Rotation(Drawing::RDeg0)
{
	Color = c;
}
TextArea::TextArea(const Rect& r, FontCache& fc, HBrush b, Drawing::Color c)
	: Widget(r, b), TextState(fc), TextRenderer(*this),
	Rotation(Drawing::RDeg0)
{
	Color = c;
}


BufferedTextArea::BufferedTextArea(const Rect& r, HBrush b, Drawing::Color c)
	: Widget(r, b), TextRegion(),
	Rotation(Drawing::RDeg0)
{
	Color = c,
	TextRegion::SetSize(GetWidth(), GetHeight());
}
BufferedTextArea::BufferedTextArea(const Rect& r, FontCache& fc, HBrush b,
	Drawing::Color c)
	: Widget(r, b), TextRegion(fc),
	Rotation(Drawing::RDeg0)
{
	Color = c,
	TextRegion::SetSize(GetWidth(), GetHeight());
}

void
BufferedTextArea::Refresh(PaintEventArgs&& e)
{
	const auto& g(e.Target);
	const Rect& bounds(e.ClipArea);

	BlitTo(g.GetBufferPtr(), *this, g.GetSize(), bounds.GetPoint(),
		bounds.GetPoint() - e.Location, bounds.GetSize(), Rotation);
}

} // namespace UI;

} // namespace YSLib;

