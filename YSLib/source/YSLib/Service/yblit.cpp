/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yblit.cpp
\ingroup Service
\brief 平台无关的图像块操作。
\version 0.1912;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-06-16 19:45:32 +0800;
\par 修改时间:
	2011-06-16 20:24 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Service::YBlit;
*/


#include "yblit.h"

using namespace ystdex;

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

namespace
{
	inline SPos
	blit_min(SPos s, SPos d)
	{
		return s + vmax<int>(0, -d);
	}

	inline SPos
	blit_max(SPos s, SPos d, SDst sl, SDst dl, SDst cl)
	{
		return s + vmin<int>(vmin<int>(dl - d, sl - s), cl);
	}
}

bool
BlitBounds(const Point& dp, const Point& sp,
	const Size& ds, const Size& ss, const Size& sc,
	int& min_x, int& min_y, int& max_x, int& max_y)
{
	min_x = blit_min(sp.X, dp.X);
	min_y = blit_min(sp.Y, dp.Y);
	max_x = blit_max(sp.X, dp.X, ss.Width, ds.Width, sc.Width);
	max_y = blit_max(sp.Y, dp.Y, ss.Height, ds.Height, sc.Height);

	return min_x < max_x && min_y < max_y;
}

template<>
int
BlitScale<false, false>(const Point& dp, const Point& sp,
	const Size& ds, const Size& ss, const Size& sc, int delta_x, int delta_y)
{
	return vmax<int>(0, dp.Y) * ds.Width + vmax<int>(0, dp.X);
}
template<>
int
BlitScale<true, false>(const Point& dp, const Point& sp,
	const Size& ds, const Size& ss, const Size& sc, int delta_x, int delta_y)
{
	return (vmax<int>(0, dp.Y) + delta_y - 1) * ds.Width + vmax<int>(0, dp.X);
}
template<>
int
BlitScale<false, true>(const Point& dp, const Point& sp,
	const Size& ds, const Size& ss, const Size& sc, int delta_x, int delta_y)
{
	return vmax<int>(0, dp.Y) * ds.Width + vmax<int>(0, dp.X) + delta_x - 1;
}
template<>
int
BlitScale<true, true>(const Point& dp, const Point& sp,
	const Size& ds, const Size& ss, const Size& sc, int delta_x, int delta_y)
{
	return (vmax<int>(0, dp.Y) + delta_y - 1) * ds.Width
		+ vmax<int>(0, dp.X) + delta_x - 1;
}

//显式实例化：防止链接错误。
template
void Blit<BlitLoop, false, false>(BitmapPtr, const Size&,
	ConstBitmapPtr, const Size&,
	const Point&, const Point&, const Size&);
template
void Blit<BlitLoop, true, true>(BitmapPtr, const Size&,
	ConstBitmapPtr, const Size&,
	const Point&, const Point&, const Size&);
template
void Blit<BlitTransparentLoop, false, false>(BitmapPtr, const Size&,
	IteratorPair, const Size&,
	const Point&, const Point&, const Size&);
template
void Blit<BlitTransparentLoop, true, true>(BitmapPtr, const Size&,
	IteratorPair, const Size&,
	const Point&, const Point&, const Size&);
template
void Blit<BlitBlendLoop, false, false>(BitmapPtr, const Size&,
	IteratorPair, const Size&,
	const Point&, const Point&, const Size&);
template
void Blit<BlitBlendLoop, true, true>(BitmapPtr, const Size&,
	IteratorPair, const Size&,
	const Point&, const Point&, const Size&);


void
CopyBuffer(const Graphics& dst, const Graphics& src)
{
	YAssert(dst.GetBufferPtr(), "Destination bitmap pointer is null.");
	YAssert(src.GetBufferPtr(), "Source bitmap pointer is null.");
	YAssert(dst.GetSize() == src.GetSize(), "Source and destination sizes"
		"are not same.");

	if(dst.GetBufferPtr() != src.GetBufferPtr())
		mmbcpy(dst.GetBufferPtr(), src.GetBufferPtr(), src.GetSizeOfBuffer());
}

void
ClearImage(const Graphics& g)
{
	ClearPixel(g.GetBufferPtr(), GetAreaFrom(g.GetSize()));
}

void
Fill(const Graphics& g, Color c)
{
	FillPixel<PixelType>(g.GetBufferPtr(), GetAreaFrom(g.GetSize()), c);
}

YSL_END_NAMESPACE(Drawing)

YSL_END

