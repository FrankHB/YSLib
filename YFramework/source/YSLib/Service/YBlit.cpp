/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YBlit.cpp
\ingroup Service
\brief 平台无关的图像块操作。
\version r1088
\author FrankHB <frankhb1989@gmail.com>
\since build 219
\par 创建时间:
	2011-06-16 19:45:32 +0800
\par 修改时间:
	2015-08-19 10:27 +0800
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
	return SDst(max<SPos>(max(SPos(), s), s - d));
}

inline SPos
blit_max(SPos s, SPos d, SDst sl, SDst dl, SDst cl)
{
	// XXX: Conversion to 'SPos' might be implementation-defined.
	return min<SPos>(min<SPos>(SPos(sl), s + SPos(cl)), s + SPos(dl) - d);
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
	if(max_x >= 0 && max_y >= 0 && min_x < SDst(max_x) && min_y < SDst(max_y))
	{
		yunseq(delta_x = SDst(max_x) - min_x, delta_y = SDst(max_y) - min_y);
		return true;
	}
	return {};
}


void
CopyBuffer(const Graphics& dst, const ConstGraphics& src)
{
	YAssert(dst.GetSize() == src.GetSize(), "Source and destination sizes"
		"are not same.");

	if(YB_LIKELY(Nonnull(dst.GetBufferPtr()) != Nonnull(src.GetBufferPtr())))
		CopyBitmapBuffer(dst.GetBufferPtr(), src.GetBufferPtr(), src.GetSize());
}

void
ClearImage(const Graphics& g)
{
	ClearPixels(g.GetBufferPtr(), size_t(GetAreaOf(g.GetSize())));
}

void
Fill(const Graphics& g, Color c)
{
	FillPixels<Pixel>(g.GetBufferPtr(), size_t(GetAreaOf(g.GetSize())), c);
}

} // namespace Drawing;

} // namespace YSLib;

