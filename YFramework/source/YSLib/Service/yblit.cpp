/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yblit.cpp
\ingroup Service
\brief 平台无关的图像块操作。
\version r1007
\author FrankHB <frankhb1989@gmail.com>
\since build 219
\par 创建时间:
	2011-06-16 19:45:32 +0800
\par 修改时间:
	2013-06-17 14:19 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YBlit
*/


#include "YSLib/Service/yblit.h"

using namespace ystdex;

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

namespace
{

//! \since build 370
inline SPos
blit_min(SPos d)
{
	return max<int>(0, -d);
}

inline SPos
blit_max(SPos s, SPos d, SDst sl, SDst dl, SDst cl)
{
	return min<int>(min<int>(dl - d, sl - s), cl);
}

} // unnamed namespace;

bool
BlitBounds(const Point& dp, const Point& sp,
	const Size& ds, const Size& ss, const Size& sc,
	int& min_x, int& min_y, int& delta_x, int& delta_y)
{
	yunseq(min_x = blit_min(dp.X), min_y = blit_min(dp.Y),
		delta_x = blit_max(sp.X, dp.X, ss.Width, ds.Width, sc.Width),
		delta_y = blit_max(sp.Y, dp.Y, ss.Height, ds.Height, sc.Height));
	if(min_x < delta_x && min_y < delta_y)
	{
		yunseq(delta_x -= min_x, delta_y -= min_y);
		return true;
	}
	return false;
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
	YAssert(dst.GetBufferPtr(), "Null pointer found.");
	YAssert(src.GetBufferPtr(), "Null pointer found.");
	YAssert(dst.GetSize() == src.GetSize(), "Source and destination sizes"
		"are not same.");

	if(YB_LIKELY(dst.GetBufferPtr() != src.GetBufferPtr()))
		std::copy_n(src.GetBufferPtr(), GetAreaOf(src.GetSize()),
			dst.GetBufferPtr());
}

void
ClearImage(const Graphics& g)
{
	ClearPixel(g.GetBufferPtr(), GetAreaOf(g.GetSize()));
}

void
Fill(const Graphics& g, Color c)
{
	FillPixel<PixelType>(g.GetBufferPtr(), GetAreaOf(g.GetSize()), c);
}

YSL_END_NAMESPACE(Drawing)

YSL_END

