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
\version r1102;
\author FrankHB<frankhb1989@gmail.com>
\since build 275 。
\par 创建时间:
	2012-01-06 21:37:51 +0800;
\par 修改时间:
	2012-01-09 11:37 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YReader::ColorPicker;
*/


#include "ColorPicker.h"
#include <YSLib/UI/ygui.h>


YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

ColorBox::ColorBox(const Point& pt, Color c)
	: Control(Rect(pt, 192, 96)),
	btnClose(Rect(173, 2, 16, 16)), btnOK(Rect(158, 2, 16, 16)),
	ColorArea(Rect(144, 32, 32, 32)), RTrack(Rect(8, 24, 128, 16)),
	GTrack(Rect(8, 48, 128, 16)), BTrack(Rect(8, 72, 128, 16))
{
	const auto update_color([this](ScrollEventArgs&& e){
		ColorArea.BackColor = Color(RTrack.GetValue(), GTrack.GetValue(),
			BTrack.GetValue());
		Invalidate(ColorArea);
	});

	SetContainerPtrOf(btnClose, this),
	SetContainerPtrOf(btnOK, this),
	SetContainerPtrOf(ColorArea, this),
	SetContainerPtrOf(RTrack, this),
	SetContainerPtrOf(GTrack, this),
	SetContainerPtrOf(BTrack, this),
	RTrack.SetMaxValue(255),
	GTrack.SetMaxValue(255),
	BTrack.SetMaxValue(255),
	yunseq(
		RTrack.GetScroll() += update_color,
		GTrack.GetScroll() += update_color,
		BTrack.GetScroll() += update_color,
		ColorArea.BackColor = c,
		btnClose.Text = "×",
		btnOK.Text = "○",
		FetchEvent<Click>(btnClose) += [this](TouchEventArgs&&){
			Close(*this);
		},
		FetchEvent<Click>(btnOK) += [this](TouchEventArgs&&){
			Close(*this);
		}
	);
}

IWidget*
ColorBox::GetTopWidgetPtr(const Point& pt,
	bool(&f)(const IWidget&))
{
	IWidget* const pWidgets[] = {&btnClose, &btnOK, &ColorArea,
		&RTrack, &GTrack, &BTrack};

	for(int i(0); i < 6; ++i)
		if(auto p = CheckWidget(*pWidgets[i], pt, f))
			return p;
	return nullptr;
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

Rect
ColorBox::Refresh(const PaintContext& pc)
{
	Widget::Refresh(pc);
	DrawRect(pc.Target, pc.Location, GetSizeOf(*this),
		IsFocused(*this) ? ColorSpace::Aqua
		: FetchGUIShell().Colors[Styles::ActiveBorder]);

	IWidget* const pWidgets[] = {&btnClose, &btnOK, &ColorArea,
		&RTrack, &GTrack, &BTrack};

	for(int i(0); i < 6; ++i)
		PaintChild(*pWidgets[i], pc);
	return Rect(pc.Location, GetSizeOf(*this));
}

YSL_END_NAMESPACE(Components)

YSL_END

