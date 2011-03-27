/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ystyle.h
\ingroup Shell
\brief 图形用户界面样式实现。
\version 0.1148;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-06-08 13:21:10 +0800;
\par 修改时间:
	2011-03-23 21:02 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YGUIComponent;
*/


#ifndef INCLUDED_YGUICOMP_H_
#define INCLUDED_YGUICOMP_H_

#include "ycomp.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

void
WndDrawFocus(IWindow*, const Size&);


template<Color::MonoType r, Color::MonoType g, Color::MonoType b>
inline void
transform_pixel_ex(BitmapPtr dst)
{
	*dst ^= Color(r, g, b) & ~BITALPHA;
}


//平台无关色彩系统转换。

typedef struct
{
	u8 r, g, b;
} rgb_t;

typedef struct
{
	u16 h; //单位为 1/64 度。
	u16 s;
	u16 l;
} hsl_t;

inline
Color
rgb2Color(const rgb_t& c)
{
	return Color(c.r, c.g, c.b);
}

inline
rgb_t
Color2rgb(const Color& c)
{
	const rgb_t r = {c.GetR(), c.GetG(), c.GetB()};

	return r;
}

hsl_t
rgb2hsl(rgb_t c);

rgb_t
hsl2rgb(hsl_t c);


YSL_END_NAMESPACE(Drawing)

YSL_BEGIN_NAMESPACE(Components)


YSL_END_NAMESPACE(Components)

YSL_END

#endif

