/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file checkbox.cpp
\ingroup UI
\brief 样式相关的图形用户界面复选框控件。
\version r1418;
\author FrankHB<frankhb1989@gmail.com>
\since build 196 。
\par 创建时间:
	2011-03-22 07:20:06 +0800;
\par 修改时间:
	2011-12-04 11:15 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::CheckBox;
*/


#include "YSLib/UI/checkbox.h"
#include "YSLib/Service/yblit.h"
#include "YSLib/UI/ygui.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

namespace
{
	void
	Diminish(Rect& r, SDst off1 = 1, SDst off2 = 2)
	{
		YAssert(r.Width > 2 && r.Height > 2, "err: @r is too small.");

		yunseq(r.X += off1, r.Y += off1,
			r.Width -= off2, r.Height -= off2);
	}

	using namespace Drawing;

	void
	RectDrawCheckBox(const Graphics& g, const Rect& r,
		bool is_pressed = false, bool is_locked = false,
		bool is_ticked = false, Color c = Color(85, 184, 223))
	{
		YAssert(g.IsValid(), "err: @g is invalid.");

		DrawRect(g, r, c);
		if(r.Width > 10 && r.Height > 10)
		{
			Rect rt(r);
			rgb_t cs[] = {{85, 134, 163}, {222, 249, 250}, {177, 223, 253},
				{213, 254, 254}}; // color3 gradient: 207, 236, 253;
		//	u16 h(rgb2hsl(Color2rgb(c)).h);

			if(!is_locked)
				for(int i(0); i < 4; ++i)
				{
					hsl_t tmp(rgb2hsl(cs[i]));

					tmp.s >>= 2;
					cs[i] = hsl2rgb(tmp);
				}
			Diminish(rt);
			DrawRect(g, rt, rgb2Color(cs[0]));
			Diminish(rt);
			DrawRect(g, rt, rgb2Color(cs[1]));
			Diminish(rt);
			DrawRect(g, rt, rgb2Color(cs[2]));
			Diminish(rt);
			FillRect(g, rt, rgb2Color(cs[3]));
		}
		if(is_ticked)
		{
			const Color c1(4, 34, 113), c2(108, 166, 208);
			Point p1(r.X + 4, r.Y + r.Height / 2), p2(r.X + r.Width / 2 - 2,
				r.Y + r.Height - 5), p3(r.X + r.Width - 3, r.Y + 3);

			p2 += Vec(0, -1);
			DrawLineSeg(g, p1 + Vec(1, 0), p2, c2);
			DrawLineSeg(g, p2, p3 + Vec(-1, 0), c2);
			p2 += Vec(0, 2);
			DrawLineSeg(g, p1 + Vec(0, 1), p2, c2);
			DrawLineSeg(g, p2, p3 + Vec(0, 1), c2);
			p2 += Vec(0, -1);
			DrawLineSeg(g, p1, p2, c1);
			DrawLineSeg(g, p2, p3, c1);
		}
		if(is_pressed)
			TransformRect(g, r, transform_pixel_ex<56, 24, 32>);
	}
}


CheckBox::CheckBox(const Rect& r)
	: Thumb(r),
	bTicked(false)
{
	FetchEvent<Click>(*this) += [this](TouchEventArgs&&){
		bTicked ^= true;
	};
}

Rect
CheckBox::Refresh(const PaintContext& pc)
{
	auto r(Widget::Refresh(pc));

	RectDrawCheckBox(pc.Target, Rect(pc.Location, GetSizeOf(*this)), bPressed,
		IsFocusedByShell(*this), bTicked);
	if(IsFocused(*this))
		DrawRect(pc.Target, pc.Location, GetSizeOf(*this), ColorSpace::Aqua);
	return r;
}

YSL_END_NAMESPACE(Components)

YSL_END

