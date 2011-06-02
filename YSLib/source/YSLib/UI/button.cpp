/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file button.cpp
\ingroup Shell
\brief 样式相关的图形用户界面按钮控件。
\version 0.3497;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-10-04 21:23:32 +0800;
\par 修改时间:
	2011-06-01 08:48 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Button;
*/


#include "button.h"
#include "yuicont.h"
#include "ywindow.h"
#include "ystyle.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

namespace
{
	void
	RectDrawButton(const Graphics& g, const Point& p, const Size& s,
		bool is_pressed = false, Color c = Color(48, 216, 255))
	{
		YAssert(g.IsValid(), "err: @g is invalid.");

		FillRect(g, p, s, c);
		if(s.Width > 4 && s.Height > 4)
		{
			Size sz(s.Width - 4, (s.Height - 4) / 2);
			Point sp(p.X + 2, p.Y + 2);

			FillRect(g, sp, sz, Color(232, 240, 255));
			sp.Y += sz.Height;
			if(s.Height % 2 != 0)
				++sz.Height;
			FillRect(g, sp, sz, Color(192, 224, 255));
		}
		if(is_pressed)
			TransformRect(g, p, s, Drawing::transform_pixel_ex<56, 24, 32>);
	}
}


Thumb::Thumb(const Rect& r)
	: Control(r),
	MButton()
{
	FetchEvent<Enter>(*this) += &Thumb::OnEnter;
	FetchEvent<Leave>(*this) += &Thumb::OnLeave;
}

void
Thumb::Paint()
{
	YWidgetAssert(this, Controls::Thumb, Paint);

	Control::Paint();

	IWindow* pWnd(FetchDirectWindowPtr(*this));

	RectDrawButton(pWnd->GetContext(), LocateForWindow(*this),
		GetSize(), bPressed);
	if(IsFocused())
	{
		Size s(GetSize());

		if(s.Width > 6 && s.Height > 6)
		{
			s.Width -= 6;
			s.Height -= 6;
			DrawRect(pWnd->GetContext(), LocateForWindow(*this) + Vec(3, 3), s,
				ColorSpace::Aqua);
		}
	}
}

void
Thumb::OnEnter(TouchEventArgs&&)
{
	bPressed = true;
	Refresh();
}

void
Thumb::OnLeave(TouchEventArgs&&)
{
	bPressed = false;
	Refresh();
}


Button::Button(const Rect& r, const Drawing::Font& fnt)
	: Thumb(r),
	MLabel(fnt, MLabel::Center)
{}

void
Button::Paint()
{
	YWidgetAssert(this, Controls::Button, Paint);

	Thumb::Paint();
	PaintText(*this, ForeColor, FetchContext(*this), LocateForWindow(*this));
}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

