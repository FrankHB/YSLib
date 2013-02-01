/*
	Copyright by FrankHB 2010 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file button.cpp
\ingroup UI
\brief 样式相关的图形用户界面按钮控件。
\version r3002
\author FrankHB <frankhb1989@gmail.com>
\since build 194
\par 创建时间:
	2010-10-04 21:23:32 +0800
\par 修改时间:
	2013-01-28 19:21 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Button
*/


#include "YSLib/UI/button.h"
#include "YSLib/Service/yblit.h"
#include "YSLib/UI/ygui.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

namespace
{

using Drawing::Hue;

/*!
\brief 色调偏移。
\since build 302
*/
inline Hue
change_hue(Hue base_h, Hue h)
{
	base_h += h;
	return base_h < 360 ? base_h : base_h - 360;
}

/*!
\since build 302
*/
void
RectDrawButton(const Graphics& g, Point pt, Size s, Hue base_hue,
	bool is_pressed = false, bool is_enabled = true)
{
	YAssert(bool(g), "Invalid graphics context found.");

	DrawRectRoundCorner(g, pt, s, is_enabled ? HSLToColor(
		{change_hue(base_hue, 25.640625F), 0.493671F, 0.462891F})
		: FetchGUIState().Colors[Styles::Workspace]);
	if(YB_LIKELY(s.Width > 2 && s.Height > 2))
	{
		yunseq(pt.X += 1, pt.Y += 1, s.Width -= 2, s.Height -= 2);
		FillRect(g, pt, s, is_enabled ? HSLToColor({change_hue(base_hue,
			11.304688F), 0.990431F, 0.591797F}) : Color(244, 244, 244));
		if(is_enabled)
		{
			if(s.Width > 2 && s.Height > 2)
			{
				Size sz(s.Width - 2, (s.Height - 2) / 2);
				Point sp(pt.X + 1, pt.Y + 1);

				FillRect(g, sp, sz, HSLToColor({change_hue(base_hue,
					39.132872F), 0.920000F, 0.951172F}));
				sp.Y += sz.Height;
				if(s.Height % 2 != 0)
					++sz.Height;
				FillRect(g, sp, sz, HSLToColor({change_hue(base_hue,
					29.523438F), 0.969231F, 0.873047F}));
			}
			if(is_pressed)
			{
				const Color tc(HSLToColor({change_hue(base_hue, 165), 0.4F,
					0.16F}));

				TransformRect(g, pt, s, [=](BitmapPtr dst){
					const Color d(*dst);

					*dst = Color(d.GetR() ^ tc.GetR(), d.GetG() ^ tc.GetG(),
						d.GetB() ^ tc.GetB());
				});
			}
		}
	}
}

} // unnamed namespace;


Thumb::Thumb(const Rect& r, Hue hue)
	: Thumb(r, NoBackgroundTag())
{
	Background = std::bind(DrawThumbBackground, std::placeholders::_1,
		std::ref(*this), hue);
}
Thumb::Thumb(const Rect& r, NoBackgroundTag)
	: Control(r, NoBackgroundTag()),
	bPressed(false)
{
	yunseq(
		FetchEvent<Enter>(*this) += [this](TouchEventArgs&&){
			if(!bPressed)
			{
				bPressed = true;
				Invalidate(*this);
			}
		},
		FetchEvent<Leave>(*this) += [this](TouchEventArgs&&){
			if(bPressed)
			{
				bPressed = false;
				Invalidate(*this);
			}
		}
	);
}


void
DrawThumbBackground(PaintEventArgs&& e, Thumb& tmb, Hue base_hue)
{
	const bool enabled(IsEnabled(tmb));
	const auto& g(e.Target);
	const auto& pt(e.Location);
	Size s(GetSizeOf(tmb));

	// TODO: Work out what should happened when a disabled button pressed.
	RectDrawButton(g, pt, s, base_hue, tmb.IsPressed(), enabled);
	if(enabled && IsFocused(tmb) && YB_LIKELY(s.Width > 6 && s.Height > 6))
	{
		yunseq(s.Width -= 6, s.Height -= 6);
		DrawRect(g, pt + Vec(3, 3), s, HSLToColor({base_hue, 1, 0.5F}));
	}
	e.ClipArea = Rect(pt, GetSizeOf(tmb));
}


void
DecorateAsCloseButton(Thumb& tmb)
{
	yunseq(
		FetchEvent<Click>(tmb) += [&](TouchEventArgs&&)
		{
			if(const auto pCon = FetchContainerPtr(tmb))
				Close(*pCon);
		},
		FetchEvent<Paint>(tmb) += [&](PaintEventArgs&& e){
			DrawCross(e.Target, e.Location, GetSizeOf(tmb), IsEnabled(tmb)
				? tmb.ForeColor : FetchGUIState().Colors[Styles::Workspace]);
		}
	);
}


Button::Button(const Rect& r, const Drawing::Font& fnt)
	: Thumb(r),
	MLabel(fnt, TextAlignment::Center)
{}

void
Button::Refresh(PaintEventArgs&& e)
{
	// NOTE: Partial invalidation made no efficiency improved here.
	DrawText(GetSizeOf(*this), IsEnabled(*this) ? ForeColor
		: FetchGUIState().Colors[Styles::Workspace], {e.Target, e.Location,
		Rect(e.Location, GetSizeOf(*this))});
}

YSL_END_NAMESPACE(Components)

YSL_END

