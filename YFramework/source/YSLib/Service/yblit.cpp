/*
	© 2011-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yblit.cpp
\ingroup Service
\brief 平台无关的图像块操作。
\version r1065
\author FrankHB <frankhb1989@gmail.com>
\since build 219
\par 创建时间:
	2011-06-16 19:45:32 +0800
\par 修改时间:
	2013-12-23 23:45 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YBlit
*/


#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_YBlit

using namespace ystdex;

namespace YSLib
{

namespace Drawing
{

namespace
{

//! \since build 446
inline SDst
blit_min(SPos s, SPos d)
{
	return max<SPos>(max<SPos>(0, s), s - d);
}

inline SPos
blit_max(SPos s, SPos d, SDst sl, SDst dl, SDst cl)
{
	return min<SPos>(min<SPos>(sl, s + cl), s + dl - d);
}

} // unnamed namespace;

bool
BlitBounds(const Point& dp, const Point& sp,
	const Size& ds, const Size& ss, const Size& sc,
	SDst& min_x, SDst& min_y, SDst& delta_x, SDst& delta_y)
{
	SPos max_x, max_y;

	yunseq(min_x = blit_min(sp.X, dp.X), min_y = blit_min(sp.Y, dp.Y),
		max_x = blit_max(sp.X, dp.X, ss.Width, ds.Width, sc.Width),
		max_y = blit_max(sp.Y, dp.Y, ss.Height, ds.Height, sc.Height));
	if(min_x < max_x && min_y < max_y)
	{
		yunseq(delta_x = max_x - min_x, delta_y = max_y - min_y);
		return true;
	}
	return false;
}


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

} // namespace Drawing;

} // namespace YSLib;

