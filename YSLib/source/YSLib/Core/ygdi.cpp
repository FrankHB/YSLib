/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ygdi.cpp
\ingroup Shell
\brief 平台无关的图形设备接口。
\version 0.3479;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-14 18:29:46 +0800;
\par 修改时间:
	2011-05-31 04:56 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YGDI;
*/


#include "ygdi.h"

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


bool
DrawHLineSeg(const Graphics& g, SPos y, SPos x1, SPos x2, Color c)
{
	YAssert(g.IsValid(),
		"In function \"void\n"
		"DrawHLineSeg(const Graphics& g, SPos y, SPos x1, SPos x2, Color c)"
		"\": \n"
		"The graphics device context is invalid.");

	if(IsInInterval<int>(y, g.GetHeight())
		&& !((x1 < 0 && x2 < 0) || (x1 >= g.GetWidth() && x2 >= g.GetWidth())))
	{
		RestrictInInterval(x1, 0, g.GetWidth());
		RestrictInInterval(x2, 0, g.GetWidth());
		RestrictLessEqual(x1, x2);
		FillPixel<PixelType>(&g.GetBufferPtr()[y * g.GetWidth() + x1],
			x2 - x1, c);
		return true;
	}
	return false;
}

bool
DrawVLineSeg(const Graphics& g, SPos x, SPos y1, SPos y2, Color c)
{
	YAssert(g.IsValid(),
		"In function \"void\n"
		"DrawVLineSeg(const Graphics& g, SPos x, SPos y1, SPos y2, Color c)"
		"\": \n"
		"The graphics device context is invalid.");

	if(IsInInterval<int>(x, g.GetWidth())
		&& !((y1 < 0 && y2 < 0) || (y1 >= g.GetHeight()
		&& y2 >= g.GetHeight())))
	{
		RestrictInInterval(y1, 0, g.GetHeight());
		RestrictInInterval(y2, 0, g.GetHeight());
		RestrictLessEqual(y1, y2);
		FillVerticalLine<PixelType>(&g.GetBufferPtr()[y1 * g.GetWidth() + x],
			y2 - y1, g.GetWidth(), c);
		return true;
	}
	return false;
}

namespace
{
	/*!
	\brief 倾斜直线光栅化函数。
	\pre 断言：y1 != y2 。
	\pre 断言：x1 != x2 。
	*/
	bool
	DrawObliqueLine(const Graphics& g, SPos x1, SPos y1, SPos x2, SPos y2,
		Color c)
	{
		YAssert(y1 != y2,
			"In function \"static void\n"
			"DrawObliqueLine(const Graphics& g, SPos x1, SPos y1,"
			" SPos x2, SPos y2, Color c)\": \n"
			"Not drawing an oblique line: the line is horizontal.");
		YAssert(x1 != x2,
			"In function \"static void\n"
			"DrawObliqueLine(const Graphics& g, SPos x1, SPos y1,"
			" SPos x2, SPos y2, Color c)\": \n"
			"Not drawing an oblique line: the line is vertical.");

		if(Rect(g.GetSize()).Contains(x1, y1)
			&& Rect(g.GetSize()).Contains(x2, y2))
		{
			/*
			一般 Bresenham 算法：实现自
			http://cg.sjtu.edu.cn/lecture_site/chap2/mainframe212.htm 伪代码。
			*/
			//起点 (x1, y1) 和终点 (x2, y2) 不同。

			const s8 sx(sgn(x2 - x1)), sy(sgn(y2 - y1));
			SDst dx(abs(x2 - x1)), dy(abs(y2 - y1));
			bool f(dy > dx);

			if(f)
				std::swap(dx, dy);

			//初始化误差项以补偿非零截断。
			const SDst dx2(dx << 1), dy2(dy << 1);
			int e(dy2 - dx);

			//主循环。
			while(dx--)
			{
				PutPixel(g, x1, y1, c);
				if(e >= 0)
				{
					if(f)
						x1 += sx;
					else
						y1 += sy;
					e -= dx2;
				}
				if(f)
					y1 += sy;
				else
					x1 += sx;
				e += dy2;
			}
			return true;
		}
		return false;
	}
}

bool
DrawLineSeg(const Graphics& g, SPos x1, SPos y1, SPos x2, SPos y2, Color c)
{
	if(y1 == y2)
		return DrawHLineSeg(g, y1, x1, x2 + 1, c);
	else if(x1 == x2)
		return DrawVLineSeg(g, x1, y1, y2 + 1, c);
	else
		return DrawObliqueLine(g, x1, y1, x2, y2, c);
}

bool
DrawRect(const Graphics& g, const Point& p, const Size& s, Color c)
{
	const SPos x1(p.X), y1(p.Y), x2(x1 + s.Width - 1), y2(y1 + s.Height - 1);
	if(x1 < x2 && y1 < y2)
	{
		bool b(DrawVLineSeg(g, x1, y1, y2, c));

		b |= DrawHLineSeg(g, y2, x1, x2, c);
		b |= DrawVLineSeg(g, x2, y1, y2 + 1, c);
		b |= DrawHLineSeg(g, y1, x1, x2, c);

		return b;
	}
	return false;
}

bool
FillRect(const Graphics& g, const Point& p, const Size& s, Color c)
{
	if(g.IsValid())
	{
		// TODO : 矩形跨立实验。
		FillRect<PixelType>(g.GetBufferPtr(), g.GetSize(), p, s, c);
		return true;
	}
	return false;
}

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


Padding::Padding(SDst l, SDst r, SDst t, SDst b)
	: Left(l), Right(r), Top(t), Bottom(b)
{}
/*Padding::Padding(u64 m)
	: Left(m >> 48), Right((m >> 32) & 0xFFFF),
	Top((m >> 16) & 0xFFFF), Bottom(m & 0xFFFF)
{}*/

Padding&
Padding::operator+=(const Padding& m)
{
	Left += m.Left;
	Right += m.Right;
	Top += m.Top;
	Bottom += m.Bottom;
	return *this;
}


Padding
operator+(const Padding& a, const Padding& b)
{
	return Padding(a.Left + b.Left, a.Right + b.Right,
		a.Top + b.Top, a.Bottom + b.Bottom);
}


u64
GetAllFrom(Padding& p)
{
	u64 r = (p.Left << 16) | p.Right;

	r = (r << 32) | (p.Top << 16) | p.Bottom;
	return r;
}

void
SetAllTo(Padding& p, SDst l, SDst r, SDst t, SDst b)
{
	p.Left = l;
	p.Right = r;
	p.Top = t;
	p.Bottom = b;
}


Padding
FetchMargin(const Rect& r, const Size& s)
{
	return Padding(vmax<int>(0, r.X), vmax<int>(0, s.Width - r.X - r.Width),
		vmax<int>(0, r.Y), vmax<int>(0, s.Height - r.Y - r.Height));
}


BitmapBuffer::BitmapBuffer(ConstBitmapPtr i, SDst w, SDst h)
	: Graphics()
	//不能提前初始化 size ，否则指针非空和面积非零状态不一致。 
{
	SetSize(w, h);
	if(i)
		mmbcpy(pBuffer, i, GetSizeOfBuffer());
}

void
BitmapBuffer::SetSize(SDst w, SDst h)
{
	u32 s(w * h);

	if(s == 0)
	{
		ydelete_array(pBuffer);
		pBuffer = nullptr;
	}
	else if(GetAreaFrom(GetSize()) < s)
		try
		{
			BitmapPtr pBufferNew(ynew PixelType[s]);

			std::swap(pBuffer, pBufferNew);
			ydelete_array(pBufferNew);
		}
		catch(std::bad_alloc&)
		{
			throw LoggedEvent("Allocation failed @ BitmapBuffer::SetSize;", 1);
		}

	YAssert(!((pBuffer != nullptr) ^ (s != 0)), "Buffer corruptied"
		" @ BitmapBuffer::SetSize;");

	size.Width = w;
	size.Height = h;
	ClearImage();
}
void
BitmapBuffer::SetSizeSwap()
{
	std::swap(size.Width, size.Height);
	ClearImage();
}

void
BitmapBuffer::ClearImage() const
{
	Drawing::ClearImage(*this);
}

void
BitmapBuffer::BeFilledWith(Color c) const
{
	Fill(*this, c);
}


BitmapBufferEx::BitmapBufferEx(ConstBitmapPtr i, SDst w, SDst h)
	: BitmapBuffer(), pBufferAlpha()
{
	SetSize(w, h);
	if(i)
		mmbcpy(pBuffer, i, GetSizeOfBuffer());
}

void
BitmapBufferEx::SetSize(SDst w, SDst h)
{
	u32 s(w * h);

	if(s == 0)
	{
		ydelete_array(pBuffer);
		pBuffer = nullptr;
		ydelete_array(pBufferAlpha);
		pBufferAlpha = nullptr;
	}
	else if(GetAreaFrom(size) < s)
	{
		BitmapPtr pBufferNew(nullptr);

		try
		{
			pBufferNew = ynew PixelType[s];

			u8* pBufferAlphaNew(ynew u8[s]);

			std::swap(pBuffer, pBufferNew);
			ydelete_array(pBufferNew);
			std::swap(pBufferAlpha, pBufferAlphaNew);
			ydelete_array(pBufferAlphaNew);
		}
		catch(std::bad_alloc&)
		{
			ydelete_array(pBufferNew);
			throw LoggedEvent("Allocation failed"
				" @ BitmapBufferEx::SetSize;", 1);
		}
	}

	YAssert(!((pBuffer != nullptr) ^ (s != 0)), "Buffer corruptied"
		" @ BitmapBufferEx::SetSize;");
	YAssert(!((pBufferAlpha != nullptr) ^ (s != 0)), "Buffer corruptied"
		" @ BitmapBufferEx::SetSize;");

	size.Width = w;
	size.Height = h;
	ClearImage();
}

void
BitmapBufferEx::ClearImage() const
{
	const u32 t = GetAreaFrom(size);

	ClearPixel(pBuffer, t);
	ClearPixel(pBufferAlpha, t);
}

bool
CopyTo(BitmapPtr dst, const Graphics& g, const Size& ds,
	const Point& dp, const Point& sp, const Size& sc, Rotation rot)
{
	if(~rot & 1 && dst && g.IsValid())
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
	if(~rot & 1 && dst && buf.IsValid())
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
	if(~rot & 1 && dst && buf.IsValid())
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

