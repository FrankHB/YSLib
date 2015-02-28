/*
	© 2012-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YBrush.cpp
\ingroup UI
\brief 画刷。
\version r262
\author FrankHB <frankhb1989@gmail.com>
\since build 293
\par 创建时间:
	2012-01-10 19:56:59 +0800
\par 修改时间:
	2015-02-27 19:06 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YBrush
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_Service_YBrush
#include YFM_YSLib_Service_YDraw

namespace YSLib
{

namespace Drawing
{

void
SolidBrush::operator()(const PaintContext& pc) const
{
	FillRect(pc.Target, pc.ClipArea, Color);
}


void
ImageBrush::operator()(const PaintContext& pc) const
{
	if(ImagePtr)
		Update(pc, *ImagePtr, DstOffset, SrcOffset);
}

void
ImageBrush::DefaultUpdate(const PaintContext& pc, const Image& img,
	const Point& dst_offset, const Point& src_offset)
{
	const auto& g(pc.Target);
	const Rect& bounds(pc.ClipArea);
	const auto& src(img.GetContext());

//	CopyTo(g.GetBufferPtr(), src, g.GetSize(), bounds.GetPoint(), Offset,
//		bounds.GetSize());
	BlitLines<false, false>(CopyLine<true>(), g.GetBufferPtr(),
		src.GetBufferPtr(), g.GetSize(), src.GetSize(), bounds.GetPoint(),
		bounds.GetPoint() + src_offset - dst_offset, bounds.GetSize());
}

} // namespace Drawing;

} // namespace YSLib;

