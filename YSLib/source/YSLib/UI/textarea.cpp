/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file textarea.cpp
\ingroup UI
\brief 样式无关的用户界面文本区域部件。
\version 0.1316;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-06-30 20:10:27 +0800;
\par 修改时间:
	2011-07-11 10:18 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::TextArea;
*/


#include "textarea.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Widgets)

TextArea::TextArea(const Rect& r, FontCache& fc)
	: Widget(r), TextRegion(fc),
	Rotation(Drawing::RDeg0)
{
	//初始化视图。
	TextRegion::SetSize(GetWidth(), GetHeight());
}

void
TextArea::Refresh(const Graphics& g, const Point& pt, const Rect& r)
{
	Widget::Refresh(g, pt, r);
	BlitTo(g, *this, Point::Zero, Point::Zero, Rotation);
}

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

