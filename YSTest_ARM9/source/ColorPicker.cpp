/*
	Copyright by FrankHB 2012 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ColorPicker.cpp
\ingroup YReader
\brief Shell 拾色器。
\version r252
\author FrankHB <frankhb1989@gmail.com>
\since build 275
\par 创建时间:
	2012-01-06 21:37:51 +0800
\par 修改时间:
	2013-03-13 12:45 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::ColorPicker
*/


#include "ColorPicker.h"
#include <YSLib/UI/YBrush.h>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(UI)

namespace
{
	yconstexpr Size track_size(112, 16);
	yconstexpr Size label_size(44, 20);
}

ColorBox::ColorBox(const Point& pt, Color c)
	: DialogPanel(Rect(pt, 200, 96)),
	ctlColorArea(Rect(164, 32, 32, 32)), trRed(Rect(4, 24, track_size)),
	trGreen(Rect(4, 48, track_size)), trBlue(Rect(4, 72, track_size)),
	lblRed(Rect(120, 24, label_size)), lblGreen(Rect(120, 48, label_size)),
	lblBlue(Rect(120, 72, label_size))
{
	const auto update_color([this](ScrollEventArgs&&){
		SetColor(Color(trRed.GetValue(), trGreen.GetValue(),
			trBlue.GetValue()));
	});

	AddWidgets(*this, ctlColorArea, trRed, trGreen, trBlue, lblRed, lblGreen,
		lblBlue),
	trRed.SetMaxValue(255),
	trGreen.SetMaxValue(255),
	trBlue.SetMaxValue(255),
	yunseq(
		FetchEvent<Paint>(ctlColorArea).Add(BorderBrush(), BoundaryPriority),
		trRed.GetScroll() += update_color,
		trGreen.GetScroll() += update_color,
		trBlue.GetScroll() += update_color
	);
	SetColor(c);
}

Color&
ColorBox::GetColorRef() const
{
	const auto p(ctlColorArea.Background.target<SolidBrush>());

	if(!p)
		throw LoggedEvent("Invalid brush found @ ColorBox::GetColor");
	return p->Color;
}

void
ColorBox::SetColor(Color c)
{
	GetColorRef() = c,
	trRed.SetValue(c.GetR()),
	trGreen.SetValue(c.GetG()),
	trBlue.SetValue(c.GetB());
	yunseq(lblRed.Text = "R: " + to_string(c.GetR()),
		lblGreen.Text = "G: " + to_string(c.GetG()),
		lblBlue.Text = "B: " + to_string(c.GetB()));
	Invalidate(ctlColorArea),
	Invalidate(trRed), Invalidate(trGreen), Invalidate(trBlue),
	Invalidate(lblRed), Invalidate(lblGreen), Invalidate(lblBlue);
}

YSL_END_NAMESPACE(UI)

YSL_END

