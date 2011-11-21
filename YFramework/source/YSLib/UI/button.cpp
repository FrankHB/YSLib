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
\version r3605;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-10-04 21:23:32 +0800;
\par 修改时间:
	2011-11-19 18:51 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Button;
*/


#include "YSLib/UI/button.h"
#include "YSLib/Service/yblit.h"
#include "YSLib/UI/ygui.h"

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
	yunsequenced(
		FetchEvent<Enter>(*this) += [this](TouchEventArgs&&){
			bPressed = true;
			Invalidate(*this);
		},
		FetchEvent<Leave>(*this) += [this](TouchEventArgs&&){
			bPressed = false;
			Invalidate(*this);
		}
	);
}

Rect
Thumb::Refresh(const PaintContext& e)
{
	auto enabled(IsEnabled(*this));
	const auto& g(e.Target);
	const auto& pt(e.Location);

	if(!enabled)
		bPressed = false;
	RectDrawButton(g, pt, GetSizeOf(*this), bPressed, enabled);
	if(enabled && IsFocused(*this))
	{
		Size s(GetSizeOf(*this));

		if(s.Width > 6 && s.Height > 6)
		{
			yunsequenced(s.Width -= 6, s.Height -= 6);
			DrawRect(g, pt + Vec(3, 3), s, ColorSpace::Aqua);
		}
	}
	return Rect(pt, GetSizeOf(*this));
}


Button::Button(const Rect& r, const Drawing::Font& fnt)
	: Thumb(r),
	MLabel(fnt, TextAlignment::Center)
{}

Rect
Button::Refresh(const PaintContext& e)
{
	auto r(Thumb::Refresh(e));

	PaintText(GetSizeOf(*this), IsEnabled(*this) ? ForeColor
		: FetchGUIShell().Colors[Styles::Workspace], e);
	return r;
}

YSL_END_NAMESPACE(Components)

YSL_END

