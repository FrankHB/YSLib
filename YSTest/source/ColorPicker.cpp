/*
	© 2012-2015, 2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ColorPicker.cpp
\ingroup YReader
\brief Shell 拾色器。
\version r293
\author FrankHB <frankhb1989@gmail.com>
\since build 275
\par 创建时间:
	2012-01-06 21:37:51 +0800
\par 修改时间:
	2023-04-04 12:20 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::ColorPicker
*/


#include "ColorPicker.h" // for Size, String;
#include YFM_YSLib_UI_Border

namespace YSLib
{

namespace UI
{

namespace
{

yconstexpr const Size track_size(112, 16);
yconstexpr const Size label_size(44, 20);

//! \since build 971
template<typename _tComp>
YB_ATTR_nodiscard YB_PURE inline String
ColorToString(_tComp c, const String& str)
{
	return String(to_string(c), str.get_allocator());
}

} // unnamed namespace;

ColorBox::ColorBox(const Point& pt, Color c)
	: DialogPanel({pt, 200, 96}),
	ctlColorArea({164, 32, 32, 32}, MakeBlankBrush()),
	trRed({4, 24, track_size}), trGreen({4, 48, track_size}),
	trBlue({4, 72, track_size}), lblRed({120, 24, label_size}),
	lblGreen({120, 48, label_size}), lblBlue({120, 72, label_size})
{
	const auto update_color([this]{
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
	trRed.Scroll += update_color,
	trGreen.Scroll += update_color,
	trBlue.Scroll += update_color
	);
	SetColor(c);
}
ImplDeDtor(ColorBox)

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
	yunseq(lblRed.Text = u"R: " + ColorToString(c.GetR(), lblRed.Text),
		lblGreen.Text = u"G: " + ColorToString(c.GetG(), lblGreen.Text),
		lblBlue.Text = u"B: " + ColorToString(c.GetB(), lblGreen.Text));
	Invalidate(ctlColorArea),
	Invalidate(trRed), Invalidate(trGreen), Invalidate(trBlue),
	Invalidate(lblRed), Invalidate(lblGreen), Invalidate(lblBlue);
}

} // namespace UI;

} // namespace YSLib;

