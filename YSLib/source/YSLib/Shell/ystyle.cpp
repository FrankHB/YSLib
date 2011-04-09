/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ystyle.cpp
\ingroup Shell
\brief 图形用户界面样式实现。
\version 0.1295;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-05-01 13:52:56 +0800;
\par 修改时间:
	2011-04-09 21:14 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YStyle;
*/


#include "ystyle.h"
#include "ygui.h"
#include "ywindow.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

void
WndDrawFocus(IWindow* pWnd, const Size&)
{
	YAssert(pWnd, "Window pointer is null.");

	DrawWindowBounds(pWnd, ColorSpace::Fuchsia);

	IWidget* pWgt(pWnd->GetFocusingPtr());

	if(pWgt)
		DrawWidgetBounds(*pWgt, ColorSpace::Aqua);
}

// 实现参照： http://130.113.54.154/~monger/hsl-rgb.html 。

hsl_t
rgb2hsl(rgb_t c)
{
	using ystdex::vmin;
	using ystdex::vmax;

	const u8 min_color(vmin(vmin(c.r, c.g), c.b)),
		max_color(vmax(vmax(c.r, c.g), c.b));
	u16 s(0), l;
	s32 h(0); // 此处 h 的值每 0x60000 对应一个圆周。 
	
	if(min_color == max_color)
		l = min_color << 8;
	else
	{
		l = ((min_color + max_color) << 8) / 2;
	/*
		l = static_cast<u16>((0.2126 * c.r + 0.7152 * c.g
			 + 0.0722 * c.b) * 256); // Rec. 601 luma;
		l = static_cast<u16>((0.299 * c.r + 0.588 * c.g
			 + 0.114 * c.b) * 256); // Rec. 709 luma;
	*/

		const u32 p((max_color + min_color) << 8);
		const u16 q((max_color - min_color) << 8); // chroma;
		
		s = (q << 16) / (l < 0x8000 ? p : 0x20000 - p);
		if(c.r == max_color)
			h = ((c.g - c.b) << 24) / q;
		else if(c.g == max_color)
			h = ((c.b - c.r) << 24) / q + 0x20000;
		else if(c.b == max_color)
			h = ((c.r - c.g) << 24) / q + 0x40000;
		if(h < 0)
			h += 0x60000;
	}

	const hsl_t r = {h * 15 >> 8, s, l};

	return r;
}

rgb_t
hsl2rgb(hsl_t c)
{
	rgb_t r;

	if(c.s == 0)
		r.r = r.g = r.b = c.l >> 8;
	else
	{
		u32 t2(c.l < 0x8000 ? (c.l >> 8) * (0x10000 + c.s)
			: ((c.l + c.s) << 8) - (static_cast<u32>(c.l * c.s) >> 8)),
			t1((c.l * 2 << 8) - t2); // t1 和 t2 为 8.24 定点数 。
		u32 t3((c.h << 8) / 5); // t3 值 0x120000 对应一个圆周。 
		s32 temp3[3] = {t3 + 0x60000, t3, t3 - 0x60000}; \
			// temp3 每个元素对应一个 RGB 分量，值 0x120000 对应一个圆周。

			for(size_t i(0); i < 3; ++i)
			{
				if(temp3[i] < 0)
					temp3[i] += 0x120000;
				else if(temp3[i] > 0x120000)
					temp3[i] -= 0x120000;
			}

			u32 dc[3] = {t1, t1, t1}; \
				//对应 RGB 分量，为 8.24 定点数 。

			for(size_t i(0); i < 3; ++i)
			{
				if(temp3[i] < 0x30000)
					dc[i] += ((t2 - t1) >> 8) * ((temp3[i] / 3) >> 8);
				else if(temp3[i] < 0x90000)
					dc[i] = t2;
				else if(temp3[i] < 0xC0000)
					dc[i] += ((t2 - t1) >> 8) * (((0xC0000 - temp3[i]) / 3)
						>> 8);
			}
			r.r = dc[0] >> 16;
			r.g = dc[1] >> 16;
			r.b = dc[2] >> 16;
	}
	return r;
}

YSL_END_NAMESPACE(Drawing)
	
YSL_BEGIN_NAMESPACE(Components)


YSL_END_NAMESPACE(Components)

YSL_END

