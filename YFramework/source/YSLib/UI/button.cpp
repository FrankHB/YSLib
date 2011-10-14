/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file button.cpp
\ingroup UI
\brief 样式相关的图形用户界面按钮控件。
\version r3587;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-10-04 21:23:32 +0800;
\par 修改时间:
	2011-10-08 23:40 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Button;
*/


#include "button.h"
#include "../Service/yblit.h"
#include "ygui.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

namespace
{
	void
	RectDrawButton(const Graphics& g, Point pt, Size s, bool is_pressed = false,
		bool is_enabled = true)
	{
		YAssert(g.IsValid(), "err: @g is invalid.");

		DrawRectRoundCorner(g, pt, s, is_enabled ? Color(60, 127, 177)
			: FetchGUIShell().Colors[Styles::Workspace]);
		if(s.Width > 2 && s.Height > 2)
		{
			yunsequenced(pt.X += 1, pt.Y += 1, s.Width -= 2, s.Height -= 2);
			FillRect(g, pt, s, is_enabled ? Color(48, 216, 255)
				: Color(244, 244, 244));
			if(is_enabled)
			{
				if(s.Width > 2 && s.Height > 2)
				{
					Size sz(s.Width - 2, (s.Height - 2) / 2);
					Point sp(pt.X + 1, pt.Y + 1);

					FillRect(g, sp, sz, Color(232, 240, 255));
					sp.Y += sz.Height;
					if(s.Height % 2 != 0)
						++sz.Height;
					FillRect(g, sp, sz, Color(192, 224, 255));
				}
				if(is_pressed)
					TransformRect(g, pt, s,
						Drawing::transform_pixel_ex<56, 24, 32>);
			}
		}
	}
}


Thumb::Thumb(const Rect& r)
	: Control(r),
	MButton()
{
	FetchEvent<Enter>(*this) += [this](IWidget&, TouchEventArgs&&){
		bPressed = true;
		Invalidate(*this);
	};
	FetchEvent<Leave>(*this) += [this](IWidget&, TouchEventArgs&&){
		bPressed = false;
		Invalidate(*this);
	};
}

Rect
Thumb::Refresh(const PaintEventArgs& e)
{
	auto enabled(IsEnabled(*this));
	const auto& g(e.Target);
	const auto& pt(e.Location);

	if(!enabled)
		bPressed = false;
	RectDrawButton(g, pt, GetSize(), bPressed, enabled);
	if(enabled && IsFocused(*this))
	{
		Size s(GetSize());

		if(s.Width > 6 && s.Height > 6)
		{
			yunsequenced(s.Width -= 6, s.Height -= 6);
			DrawRect(g, pt + Vec(3, 3), s, ColorSpace::Aqua);
		}
	}
	return Rect(pt, GetSize());
}


Button::Button(const Rect& r, const Drawing::Font& fnt)
	: Thumb(r),
	MLabel(fnt, MLabel::Center)
{}

Rect
Button::Refresh(const PaintEventArgs& e)
{
	auto r(Thumb::Refresh(e));

	PaintText(GetSize(), IsEnabled(*this) ? ForeColor
		: FetchGUIShell().Colors[Styles::Workspace], e);
	return r;
}

YSL_END_NAMESPACE(Components)

YSL_END

