/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ygdi.cpp
\ingroup Service
\brief 平台无关的图形设备接口。
\version r2848
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-14 18:29:46 +0800
\par 修改时间:
	2013-08-21 21:40 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YGDI
*/


#include "YSLib/Service/ygdi.h"
#include "YSLib/Service/yblit.h"

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
	if(!clip.IsUnstrictlyEmpty() && Clip(clip, bounds)
		&& !clip.IsUnstrictlyEmpty())
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


CompactPixmap::CompactPixmap(ConstBitmapPtr i, SDst w, SDst h)
	: BasicImage()
	//不能提前初始化 size ，否则指针非空和面积非零状态不一致。
{
	SetSize(w, h);
	if(i)
		std::copy_n(i, GetAreaOf(GetSize()), pBuffer);
}
CompactPixmap::CompactPixmap(unique_ptr<PixelType[]> p, const Size& s) ynothrow
	: BasicImage(Graphics(p.release(), s))
{}
CompactPixmap::CompactPixmap(const CompactPixmap& buf)
	: BasicImage()
{
	SetSize(buf.GetSize());
	if(const auto p = buf.GetBufferPtr())
		std::copy_n(p, GetAreaOf(GetSize()), pBuffer);
}
CompactPixmap::CompactPixmap(CompactPixmap&& buf) ynothrow
	: BasicImage(buf)
{
	buf.pBuffer = {};
}

void
CompactPixmap::SetContent(ConstBitmapPtr s, SDst w, SDst h)
{
	SetSize(w, h);
	if(YB_LIKELY(pBuffer && s))
		std::copy_n(s, GetAreaOf(GetSize()), pBuffer);
}
void
CompactPixmap::SetSize(const Size& s)
{
	const auto area(GetAreaOf(s));

	try
	{
		unique_ptr<PixelType[]> p_new(YB_LIKELY(area != 0)
			? new PixelType[area] : nullptr);
		unique_ptr<PixelType[]> p_old(pBuffer);

		pBuffer = p_new.release();
	}
	catch(std::bad_alloc&)
	{
		throw LoggedEvent("BitmapBuffer allocation failed.", Alert);
	}

	YAssert(bool(pBuffer) == (area != 0), "Buffer corruptied.");

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
	Drawing::ClearImage(*this);
}


CompactPixmapEx::CompactPixmapEx(ConstBitmapPtr i, SDst w, SDst h)
	: CompactPixmap(), pBufferAlpha()
{
	SetSize(w, h);
	if(i)
		std::copy_n(i, GetAreaOf(GetSize()), pBuffer);
}
CompactPixmapEx::CompactPixmapEx(const CompactPixmapEx& buf)
	: CompactPixmap(), pBufferAlpha()
{
	SetSize(buf.GetSize());
	if(const auto p = buf.GetBufferPtr())
	{
		std::copy_n(p, GetAreaOf(GetSize()), pBuffer),
		std::copy_n(buf.GetBufferAlphaPtr(), GetAreaOf(GetSize()),
			pBufferAlpha);
	}
}
CompactPixmapEx::CompactPixmapEx(CompactPixmapEx&& buf) ynothrow
	: CompactPixmap(std::move(buf)), pBufferAlpha(buf.GetBufferAlphaPtr())
{
	buf.pBufferAlpha = {};
}

void
CompactPixmapEx::SetSize(const Size& s)
{
	const auto area(GetAreaOf(s));

	try
	{
		unique_ptr<PixelType[]> p_new(YB_LIKELY(area != 0) ? new PixelType[area]
			: nullptr);
		unique_ptr<AlphaType[]> p_new_alpha(YB_LIKELY(area != 0)
			? new AlphaType[area] : nullptr);

		unique_ptr<PixelType[]> p_old(pBuffer);
		unique_ptr<AlphaType[]> p_old_alpha(pBufferAlpha);

		pBuffer = p_new.release();
		pBufferAlpha = p_new_alpha.release();
	}
	catch(std::bad_alloc&)
	{
		throw LoggedEvent("CompactPixmapEx allocation failed.", Alert);
	}

	YAssert(bool(pBuffer) == (area != 0), "Buffer corruptied.");
	YAssert(bool(pBufferAlpha) == (area != 0), "Buffer corruptied.");

	sGraphics = s,
	ClearImage();
}

void
CompactPixmapEx::ClearImage() const
{
	const u32 t = GetAreaOf(sGraphics);

	ClearPixel(pBuffer, t);
	ClearPixel(pBufferAlpha, t);
}

bool
CopyTo(BitmapPtr dst, const Graphics& g, const Size& ds,
	const Point& dp, const Point& sp, const Size& sc, Rotation rot)
{
	if(~rot & 1 && dst && bool(g))
	{
		if(rot == RDeg0)
			BlitLines<false, false, BitmapPtr, ConstBitmapPtr>(BlitLine<true>,
				dst, g.GetBufferPtr(), ds, g.GetSize(), dp, sp, sc);
		else
			BlitLines<true, true, BitmapPtr, ConstBitmapPtr>(BlitLine<false>,
				dst, g.GetBufferPtr(), ds, g.GetSize(), dp, sp, sc);
		return true;
	}
	return false;
}
bool
CopyTo(BitmapPtr dst, const CompactPixmapEx& buf, const Size& ds,
	const Point& dp, const Point& sp, const Size& sc, Rotation rot)
{
	if(~rot & 1 && dst && bool(buf))
	{
		if(rot == RDeg0)
			BlitLines<false, false>(BlitTransparentLine<true>(), dst,
				IteratorPair(buf.GetBufferPtr(), buf.GetBufferAlphaPtr()), ds,
				buf.GetSize(), dp, sp, sc);
		else
			BlitLines<true, true>(BlitTransparentLine<false>(), dst,
				IteratorPair(buf.GetBufferPtr(), buf.GetBufferAlphaPtr()), ds,
				buf.GetSize(), dp, sp, sc);
		return true;
	}
	return false;
}

bool
BlitTo(BitmapPtr dst, const CompactPixmapEx& buf, const Size& ds,
	const Point& dp, const Point& sp, const Size& sc, Rotation rot)
{
	if(~rot & 1 && dst && bool(buf))
	{
		if(rot == RDeg0)
			BlitLines<false, false>(BlitBlendLine<true>(), dst,
				IteratorPair(buf.GetBufferPtr(), buf.GetBufferAlphaPtr()), ds,
				buf.GetSize(), dp, sp, sc);
		else
			BlitLines<true, true>(BlitBlendLine<false>(), dst,
				IteratorPair(buf.GetBufferPtr(), buf.GetBufferAlphaPtr()), ds,
				buf.GetSize(), dp, sp, sc);
		return true;
	}
	return false;
}

} // namespace Drawing;

} // namespace YSLib;

