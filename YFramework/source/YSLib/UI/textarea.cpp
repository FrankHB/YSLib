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
\version r451
\author FrankHB <frankhb1989@gmail.com>
\since build 222
\par 创建时间:
	2011-06-30 20:10:27 +0800
\par 修改时间:
	2014-03-09 22:30 +0800
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
	: Widget(r, b, c), TextState(), TextRenderer(*this),
	Rotation(Drawing::RDeg0)
{}
TextArea::TextArea(const Rect& r, FontCache& fc, HBrush b, Drawing::Color c)
	: Widget(r, b, c), TextState(fc), TextRenderer(*this),
	Rotation(Drawing::RDeg0)
{}


BufferedTextArea::BufferedTextArea(const Rect& r, HBrush b, Drawing::Color c)
	: Widget(r, b, c), TextRegion(),
	Rotation(Drawing::RDeg0)
{
	//初始化视图。
	TextRegion::SetSize(GetWidth(), GetHeight());
}
BufferedTextArea::BufferedTextArea(const Rect& r, FontCache& fc, HBrush b,
	Drawing::Color c)
	: Widget(r, b, c), TextRegion(fc),
	Rotation(Drawing::RDeg0)
{
	//初始化视图。
	TextRegion::SetSize(GetWidth(), GetHeight());
}

void
BufferedTextArea::Refresh(PaintEventArgs&& e)
{
	const auto& g(e.Target);
	const Rect& r(e.ClipArea);

	BlitTo(g.GetBufferPtr(), *this, g.GetSize(), r.GetPoint(),
		r.GetPoint() - e.Location, r.GetSize(), Rotation);
}

} // namespace UI;

} // namespace YSLib;

