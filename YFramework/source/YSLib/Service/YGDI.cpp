/*
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
\version r2968
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-14 18:29:46 +0800
\par 修改时间:
	2015-04-26 03:02 +0800
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
	// XXX: Conversion to 'SPos' might be implementation-defined.
	return Rect(r.X + m.Left, r.Y + m.Top,
		SDst(max<SPos>(0, SPos(r.Width) - m.Left - m.Right)),
		SDst(max<SPos>(0, SPos(r.Height) - m.Top - m.Bottom)));
}


Padding
FetchMargin(const Rect& r, const Size& s)
{
	// XXX: Conversion to 'SPos' might be implementation-defined.
	return Padding(r.X, SPos(s.Width) - r.X - SPos(r.Width),
		r.Y, SPos(s.Height) - r.Y - SPos(r.Height));
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
		// XXX: Conversion to 'SPos' might be implementation-defined.
		const SPos scx(min<SPos>(SPos(ss.Width), SPos(ds.Width) - m.Right
			- dp.X) - sp.X), scy(min<SPos>(SPos(ss.Height), SPos(ds.Height)
			- m.Bottom - dp.Y) - sp.Y);

		if(scx > 0 && scy > 0)
		{
			pc.ClipArea &= Rect(dp, SDst(scx), SDst(scy));
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
		CopyBitmapBuffer(GetBufferPtr(), i, GetSize());
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
		CopyBitmapBuffer(GetBufferPtr(), p, GetSize());
}

void
CompactPixmap::SetContent(ConstBitmapPtr s, SDst w, SDst h)
{
	SetSize(w, h);
	if(YB_LIKELY(pBuffer && s))
		CopyBitmapBuffer(pBuffer.get(), s, GetSize());
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
		CopyBitmapBuffer(pBuffer.get(), i, GetSize());
}
CompactPixmapEx::CompactPixmapEx(const CompactPixmapEx& buf)
	: CompactPixmapEx()
{
	SetSize(buf.GetSize());
	if(const auto p = buf.GetBufferPtr())
	{
		CopyBitmapBuffer(pBuffer.get(), p, GetSize()),
		CopyBitmapBuffer(pBufferAlpha.get(), buf.GetBufferAlphaPtr(),
			GetSize());
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
	const auto t(size_t(GetAreaOf(sGraphics)));

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

