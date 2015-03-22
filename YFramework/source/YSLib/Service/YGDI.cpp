﻿/*
	© 2009-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YGDI.cpp
\ingroup Service
\brief 平台无关的图形设备接口。
\version r2949
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-14 18:29:46 +0800
\par 修改时间:
	2015-03-21 23:36 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YGDI
*/


#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_YGDI
#include YFM_YSLib_Service_YBlend

using namespace ystdex;

namespace YSLib
{

namespace Drawing
{

Rect
operator+(const Rect& r, const Padding& m)
{
	return Rect(r.X + m.Left, r.Y + m.Top,
		max<int>(0, r.Width - m.Left - m.Right),
		max<int>(0, r.Height - m.Top - m.Bottom));
}


Padding
FetchMargin(const Rect& r, const Size& s)
{
	return Padding(r.X, s.Width - r.X - r.Width,
		r.Y, s.Height - r.Y - r.Height);
}


Point
ClipBounds(Rect& clip, const Rect& bounds)
{
	if(!clip.IsUnstrictlyEmpty() && Clip(clip, bounds))
		return clip.GetPoint() - bounds.GetPoint();
	clip.GetSizeRef() = {};
	return {};
}

Point
ClipMargin(PaintContext& pc, const Padding& m, const Size& ss)
{
	const Size& ds(pc.Target.GetSize());

	if(GetHorizontalOf(m) < ds.Width && GetVerticalOf(m) < ds.Height)
	{
		const auto& pt(pc.Location);
		const Point dp(max<int>(m.Left, pt.X), max<int>(m.Top, pt.Y));
		const Point sp(dp - pt);
		const auto scx(min<int>(ss.Width, ds.Width - m.Right - dp.X) - sp.X),
			scy(min<int>(ss.Height, ds.Height - m.Bottom - dp.Y) - sp.Y);

		if(scx > 0 && scy > 0)
		{
			pc.ClipArea &= Rect(dp, scx, scy);
			return pc.ClipArea.GetPoint() - pt;
		}
	}
	pc.ClipArea.GetSizeRef() = {};
	return {};
}


ImplDeDtor(BasicImage)


CompactPixmap::CompactPixmap(ConstBitmapPtr i, SDst w, SDst h)
	: BaseType()
	// NOTE: The size member cannot be initialized just here, or it would be
	//	inconsistent with buffer pointer.
{
	SetSize(w, h);
	if(i)
		std::copy_n(i, GetAreaOf(GetSize()), GetBufferPtr());
}
CompactPixmap::CompactPixmap(unique_ptr<Pixel[]> p, const Size& s) ynothrow
	: BaseType(std::move(p), s)
{}
CompactPixmap::CompactPixmap(const CompactPixmap& buf)
	// XXX: $2015-03 @ %Documentation::Workflow::Annual2015.
	: BaseType(), IImage()
{
	SetSize(buf.GetSize());
	if(const auto p = buf.GetBufferPtr())
		std::copy_n(p, GetAreaOf(GetSize()), GetBufferPtr());
}

void
CompactPixmap::SetContent(ConstBitmapPtr s, SDst w, SDst h)
{
	SetSize(w, h);
	if(YB_LIKELY(pBuffer && s))
		std::copy_n(s, GetAreaOf(GetSize()), pBuffer.get());
}
void
CompactPixmap::SetSize(const Size& s)
{
	const auto area(GetAreaOf(s));

	TryExpr(pBuffer.reset(YB_LIKELY(area != 0) ? new Pixel[area] : nullptr))
	CatchThrow(std::bad_alloc&,
		LoggedEvent("BitmapBuffer allocation failed.", Alert))
	YAssert(bool(pBuffer) == (area != 0), "Buffer corrupted.");
	sGraphics = s,
	ClearImage();
}
void
CompactPixmap::SetSizeSwap()
{
	std::swap(sGraphics.Width, sGraphics.Height);
	ClearImage();
}

void
CompactPixmap::ClearImage() const
{
	Drawing::ClearImage(GetContext());
}


CompactPixmapEx::CompactPixmapEx(ConstBitmapPtr i, SDst w, SDst h)
	: CompactPixmapEx()
{
	SetSize(w, h);
	if(i)
		std::copy_n(i, GetAreaOf(GetSize()), pBuffer.get());
}
CompactPixmapEx::CompactPixmapEx(const CompactPixmapEx& buf)
	: CompactPixmapEx()
{
	SetSize(buf.GetSize());
	if(const auto p = buf.GetBufferPtr())
	{
		std::copy_n(p, GetAreaOf(GetSize()), pBuffer.get()),
		std::copy_n(buf.GetBufferAlphaPtr(), GetAreaOf(GetSize()),
			pBufferAlpha.get());
	}
}

void
CompactPixmapEx::SetSize(const Size& s)
{
	const auto area(GetAreaOf(s));

	try
	{
		unique_ptr<Pixel[]> p_new(YB_LIKELY(area != 0) ? new Pixel[area]
			: nullptr);
		unique_ptr<AlphaType[]> p_new_alpha(YB_LIKELY(area != 0)
			? new AlphaType[area] : nullptr);

		yunseq(pBuffer = std::move(p_new),
			pBufferAlpha = std::move(p_new_alpha));
	}
	CatchThrow(std::bad_alloc&,
		LoggedEvent("CompactPixmapEx allocation failed.", Alert))
	YAssert(bool(pBuffer) == (area != 0), "Buffer corrupted."),
	YAssert(bool(pBufferAlpha) == (area != 0), "Alpha buffer corrupted.");
	sGraphics = s,
	ClearImage();
}

void
CompactPixmapEx::ClearImage() const
{
	const std::uint32_t t = GetAreaOf(sGraphics);

	ClearPixel(pBuffer.get(), t);
	ClearPixel(pBufferAlpha.get(), t);
}

bool
CopyTo(BitmapPtr dst, const ConstGraphics& g, const Size& ds,
	const Point& dp, const Point& sp, const Size& sc, Rotation rot)
{
	if(~rot & 1 && dst && bool(g))
	{
		if(rot == RDeg0)
			BlitLines<false, false>(CopyLine<true>(), dst, g.GetBufferPtr(),
				ds, g.GetSize(), dp, sp, sc);
		else
			BlitLines<true, true>(CopyLine<false>(), dst, g.GetBufferPtr(),
				ds, g.GetSize(), dp, sp, sc);
		return true;
	}
	return {};
}
bool
CopyTo(BitmapPtr dst, const CompactPixmapEx& buf, const Size& ds,
	const Point& dp, const Point& sp, const Size& sc, Rotation rot)
{
	if(~rot & 1 && dst && bool(buf))
	{
		if(rot == RDeg0)
			BlitPixels<false, false>(Shaders::BlitTransparentPoint(), dst,
				IteratorPair(buf.GetBufferPtr(), buf.GetBufferAlphaPtr()), ds,
				buf.GetSize(), dp, sp, sc);
		else
			BlitPixels<true, true>(Shaders::BlitTransparentPoint(), dst,
				IteratorPair(buf.GetBufferPtr(), buf.GetBufferAlphaPtr()), ds,
				buf.GetSize(), dp, sp, sc);
		return true;
	}
	return {};
}

bool
BlitTo(BitmapPtr dst, const CompactPixmapEx& buf, const Size& ds,
	const Point& dp, const Point& sp, const Size& sc, Rotation rot)
{
	if(~rot & 1 && dst && bool(buf))
	{
		if(rot == RDeg0)
			BlitPixels<false, false>(Shaders::BlitAlphaPoint(), dst,
				IteratorPair(buf.GetBufferPtr(), buf.GetBufferAlphaPtr()), ds,
				buf.GetSize(), dp, sp, sc);
		else
			BlitPixels<true, true>(Shaders::BlitAlphaPoint(), dst,
				IteratorPair(buf.GetBufferPtr(), buf.GetBufferAlphaPtr()), ds,
				buf.GetSize(), dp, sp, sc);
		return true;
	}
	return {};
}

} // namespace Drawing;

} // namespace YSLib;

