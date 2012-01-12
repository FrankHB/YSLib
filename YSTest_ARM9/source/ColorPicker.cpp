/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ColorPicker.cpp
\ingroup YReader
\brief Shell 拾色器。
\version r1151;
\author FrankHB<frankhb1989@gmail.com>
\since build 275 。
\par 创建时间:
	2012-01-06 21:37:51 +0800;
\par 修改时间:
	2012-01-13 00:36 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YReader::ColorPicker;
*/


#include "ColorPicker.h"


YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

ColorBox::ColorBox(const Point& pt, Color c)
	: DialogPanel(Rect(pt, 192, 96)),
	ColorArea(Rect(144, 32, 32, 32)), RTrack(Rect(8, 24, 128, 16)),
	GTrack(Rect(8, 48, 128, 16)), BTrack(Rect(8, 72, 128, 16))
{
	const auto update_color([this](ScrollEventArgs&& e){
		ColorArea.BackColor = Color(RTrack.GetValue(), GTrack.GetValue(),
			BTrack.GetValue());
		Invalidate(ColorArea);
	});

	*this += ColorArea,
	*this += RTrack,
	*this += GTrack,
	*this += BTrack,
	RTrack.SetMaxValue(255),
	GTrack.SetMaxValue(255),
	BTrack.SetMaxValue(255),
	yunseq(
		ColorArea.BackColor = c,
		FetchEvent<Paint>(ColorArea).Add(Border, &BorderStyle::OnPaint),
		RTrack.GetScroll() += update_color,
		GTrack.GetScroll() += update_color,
		BTrack.GetScroll() += update_color
	);
}

void
ColorBox::SetColor(Color c)
{
	ColorArea.BackColor = c,
	RTrack.SetValue(c.GetR()),
	GTrack.SetValue(c.GetG()),
	BTrack.SetValue(c.GetB());
	Invalidate(ColorArea),
	Invalidate(RTrack), Invalidate(GTrack), Invalidate(BTrack);
}

YSL_END_NAMESPACE(Components)

YSL_END

