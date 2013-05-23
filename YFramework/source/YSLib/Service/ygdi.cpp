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
\version r2784
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-14 18:29:46 +0800
\par 修改时间:
	2013-05-22 01:45 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YGDI
*/


#include "YSLib/Service/ygdi.h"
#include "YSLib/Service/yblit.h"

using namespace ystdex;

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

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


BitmapBuffer::BitmapBuffer(ConstBitmapPtr i, SDst w, SDst h)
	: BasicImage()
	//不能提前初始化 size ，否则指针非空和面积非零状态不一致。
{
	SetSize(w, h);
	if(i)
		std::copy_n(i, GetAreaOf(GetSize()), pBuffer);
}
BitmapBuffer::BitmapBuffer(const BitmapBuffer& buf)
	: BasicImage()
{
	SetSize(buf.GetSize());
	if(const auto p = buf.GetBufferPtr())
		std::copy_n(p, GetAreaOf(GetSize()), pBuffer);
}
BitmapBuffer::BitmapBuffer(BitmapBuffer&& buf) ynothrow
	: BasicImage(buf)
{
	buf.pBuffer = nullptr;
}

void
BitmapBuffer::SetContent(ConstBitmapPtr s, SDst w, SDst h)
{
	SetSize(w, h);
	if(YB_LIKELY(pBuffer && s))
		std::copy_n(s, GetAreaOf(GetSize()), pBuffer);
}
void
BitmapBuffer::SetSize(const Size& s)
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
		throw LoggedEvent("%BitmapBuffer allocation failed.", 1);
	}

	YAssert(!((pBuffer != nullptr) ^ (area != 0)), "Buffer corruptied.");

	sGraphics = s,
	ClearImage();
}
void
BitmapBuffer::SetSizeSwap()
{
	std::swap(sGraphics.Width, sGraphics.Height);
	ClearImage();
}

void
BitmapBuffer::ClearImage() const
{
	Drawing::ClearImage(*this);
}


BitmapBufferEx::BitmapBufferEx(ConstBitmapPtr i, SDst w, SDst h)
	: BitmapBuffer(), pBufferAlpha()
{
	SetSize(w, h);
	if(i)
		std::copy_n(i, GetAreaOf(GetSize()), pBuffer);
}
BitmapBufferEx::BitmapBufferEx(const BitmapBufferEx& buf)
	: BitmapBuffer(), pBufferAlpha()
{
	SetSize(buf.GetSize());
	if(const auto p = buf.GetBufferPtr())
	{
		std::copy_n(p, GetAreaOf(GetSize()), pBuffer),
		std::copy_n(buf.GetBufferAlphaPtr(), GetAreaOf(GetSize()),
			pBufferAlpha);
	}
}
BitmapBufferEx::BitmapBufferEx(BitmapBufferEx&& buf) ynothrow
	: BitmapBuffer(std::move(buf)), pBufferAlpha(buf.GetBufferAlphaPtr())
{
	buf.pBufferAlpha = nullptr;
}

void
BitmapBufferEx::SetSize(const Size& s)
{
	const auto area(GetAreaOf(s));

	try
	{
		unique_ptr<PixelType[]> p_new(YB_LIKELY(area != 0)
			? new PixelType[area] : nullptr);
		unique_ptr<Color::AlphaType[]> p_new_alpha(YB_LIKELY(area != 0)
			? new Color::AlphaType[area] : nullptr);

		unique_ptr<PixelType[]> p_old(pBuffer);
		unique_ptr<Color::AlphaType[]> p_old_alpha(pBufferAlpha);

		pBuffer = p_new.release();
		pBufferAlpha = p_new_alpha.release();
	}
	catch(std::bad_alloc&)
	{
		throw LoggedEvent("%BitmapBufferEx allocation failed.", 1);
	}

	YAssert(!((pBuffer != nullptr) ^ (area != 0)), "Buffer corruptied.");
	YAssert(!((pBufferAlpha != nullptr) ^ (area != 0)), "Buffer corruptied.");

	sGraphics = s,
	ClearImage();
}

void
BitmapBufferEx::ClearImage() const
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
		(rot == RDeg0
			? Blit<BlitLoop, false, false, BitmapPtr, ConstBitmapPtr>
			: Blit<BlitLoop, true, true, BitmapPtr, ConstBitmapPtr>)(
			dst, ds, g.GetBufferPtr(), g.GetSize(), dp, sp, sc);
		return true;
	}
	return false;
}
bool
CopyTo(BitmapPtr dst, const BitmapBufferEx& buf, const Size& ds,
	const Point& dp, const Point& sp, const Size& sc, Rotation rot)
{
	if(~rot & 1 && dst && bool(buf))
	{
		(rot == RDeg0
			? Blit<BlitTransparentLoop, false, false, BitmapPtr, IteratorPair>
			: Blit<BlitTransparentLoop, true, true, BitmapPtr, IteratorPair>)(
			dst, ds, IteratorPair(buf.GetBufferPtr(), buf.GetBufferAlphaPtr()),
			buf.GetSize(), dp, sp, sc);
		return true;
	}
	return false;
}

bool
BlitTo(BitmapPtr dst, const BitmapBufferEx& buf, const Size& ds,
	const Point& dp, const Point& sp, const Size& sc, Rotation rot)
{
	if(~rot & 1 && dst && bool(buf))
	{
		(rot == RDeg0
			? Blit<BlitBlendLoop, false, false, BitmapPtr, IteratorPair>
			: Blit<BlitBlendLoop, true, true, BitmapPtr, IteratorPair>)(
			dst, ds, IteratorPair(buf.GetBufferPtr(), buf.GetBufferAlphaPtr()),
			buf.GetSize(), dp, sp, sc);
		return true;
	}
	return false;
}

YSL_END_NAMESPACE(Drawing)

YSL_END

