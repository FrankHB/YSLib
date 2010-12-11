/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ygdi.cpp
\ingroup Shell
\brief 平台无关的图形设备接口实现。
\version 0.2601;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-14 18:29:46 + 08:00;
\par 修改时间:
	2010-12-08 22:34 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YGDI;
*/


#include "ygdi.h"
#include "../Core/yexcept.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

#define BLT_ALPHA_BITS	8
#define BLT_MAX_ALPHA	((1 << BLT_ALPHA_BITS) - 1)
#define BLT_ROUND		(1 << (BLT_ALPHA_BITS - 1))
#define BLT_THRESHOLD	8
#define BLT_THRESHOLD2	128

namespace
{
	inline SPOS
	blitMinX(SPOS sx, SPOS dx)
	{
		return sx + vmax<int>(0, -dx);
	}
	inline SPOS
	blitMinY(SPOS sy, SPOS dy)
	{
		return sy + vmax<int>(0, -dy);
	}
	inline SPOS
	blitMaxX(SPOS minX, SDST sw, SDST dw, SPOS sx, SPOS dx, SDST cw)
	{
		return sx + vmin<int>(vmin<int>(dw - dx, sw - sx), cw);
	}
	inline SPOS
	blitMaxY(SPOS minY, SDST sh, SDST dh, SPOS sy, SPOS dy, SDST ch)
	{
		return sy + vmin<int>(vmin<int>(dh - dy, sh - sy), ch);
	}
}


void
blitScale(const Point& sp, const Point& dp,
	const Size& ss, const Size& ds, const Size& sc,
	int& minX, int& minY, int& maxX, int& maxY)
{
	minX = blitMinX(sp.X, dp.X);
	minY = blitMinY(sp.Y, dp.Y);
	maxX = blitMaxX(minX, ss.Width, ds.Width, sp.X, dp.X, sc.Width);
	maxY = blitMaxY(minY, ss.Height, ds.Height, sp.Y, dp.Y, sc.Height);
}


static void
blitFor(int deltaX, int deltaY,
	BitmapPtr dc, ConstBitmapPtr sc,
	int dInc, int sInc)
{
	deltaX *= sizeof(PixelType);
	for(int y(0); y < deltaY; ++y)
	{
		memcpy(dc, sc, deltaX);
		sc += sInc;
		dc += dInc;
	}
}
void
blit(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);
	blitFor(maxX - minX, maxY - minY,
		dst + vmax<int>(0, dp.Y) * ds.Width + vmax<int>(0, dp.X),
		src + minY * ss.Width + minX,
		ds.Width, ss.Width);
}
void
blitH(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);
	blitFor(maxX - minX, maxY - minY,
		dst + (vmax<int>(0, dp.Y) + maxY - minY - 1) * ds.Width
		+ vmax<int>(0, dp.X),
		src + minY * ss.Width + minX,
		minX - maxX - ds.Width, ss.Width - maxX + minX);
}
static void
blitForU(int deltaX, int deltaY,
	BitmapPtr dc, ConstBitmapPtr sc,
	int dInc, int sInc)
{
	for(int y(0); y < deltaY; ++y)
	{
		for(int x(0); x < deltaX; ++x)
			*dc-- = *sc++;
		sc += sInc;
		dc -= dInc;
	}
}
void
blitV(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);
	blitForU(maxX - minX, maxY - minY,
		dst + vmax<int>(0, dp.Y) * ds.Width + vmax<int>(0, dp.X)
		+ maxX - minX - 1,
		src + minY * ss.Width + minX,
		minX - maxX - ds.Width, ss.Width - maxX + minX);
}
void
blitU(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);
	blitForU(maxX - minX, maxY - minY,
		dst + (vmax<int>(0, dp.Y) + maxY - minY - 1) * ds.Width
		+ vmax<int>(0, dp.X),
		src + minY * ss.Width + minX,
		ds.Width - maxX + minX, ss.Width - maxX + minX);
}

/*
void blit(u8* dst, SDST dw, SDST dh,
const u8* src, SDST sw, SDST sh,
const Point& sp, const Point& dp, const Size& sc)
{
const int minX = blitMinX(sx, dx),
minY = blitMinY(sy, dy),
maxX = blitMaxX(minX, sw, dw, sx, dx, cw),
maxY = blitMaxY(minY, sh, dh, sy, dy, ch);
const u8* sc = src + minY * ss.Width + minX;
u8* dc = dst + vmax<int>(0, dp.Y) * ds.Width + vmax<int>(0, dp.X);

for(int y = minY; y < maxY; y++)
{
memcpy(dc, sc, maxX - minX);
sc += sw;
dc += dw;
}
}
*/

static void
blit2For(int deltaX, int deltaY,
	BitmapPtr dc, ConstBitmapPtr sc,
	int dInc, int sInc)
{
	for(int y(0); y < deltaY; ++y)
	{
		for(int x(0); x < deltaX; ++x)
		{
			if(*sc & BITALPHA)
				*dc = *sc;
			++sc;
			++dc;
		}
		sc += sInc;
		dc += dInc;
	}
}
void
blit2(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);
	blit2For(maxX - minX, maxY - minY,
		dst + vmax<int>(0, dp.Y) * ds.Width + vmax<int>(0, dp.X),
		src + minY * ss.Width + minX,
		ds.Width - maxX + minX, ss.Width - maxX + minX);
}
void
blit2H(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);
	blit2For(maxX - minX, maxY - minY,
		dst + (vmax<int>(0, dp.Y) + maxY - minY - 1) * ds.Width
		+ vmax<int>(0, dp.X),
		src + minY * ss.Width + minX,
		minX - maxX - ds.Width, ss.Width - maxX + minX);
}
static void
blit2ForU(int deltaX, int deltaY,
	BitmapPtr dc, ConstBitmapPtr sc,
	int dInc, int sInc)
{
	for(int y(0); y < deltaY; ++y)
	{
		for(int x(0); x < deltaX; ++x)
		{
			if(*sc & BITALPHA)
				*dc = *sc;
			++sc;
			--dc;
		}
		sc += sInc;
		dc -= dInc;
	}
}
void
blit2V(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);
	blit2ForU(maxX - minX, maxY - minY,
		dst + vmax<int>(0, dp.Y) * ds.Width + vmax<int>(0, dp.X)
		+ maxX - minX - 1,
		src + minY * ss.Width + minX,
		minX - maxX - ds.Width, ss.Width - maxX + minX);
}
void
blit2U(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);
	blit2ForU(maxX - minX, maxY - minY,
		dst + (vmax<int>(0, dp.Y) + maxY - minY - 1) * ds.Width
		+ vmax<int>(0, dp.X),
		src + minY * ss.Width + minX,
		ds.Width - maxX + minX, ss.Width - maxX + minX);
}

static void
blit2For(int deltaX, int deltaY,
	BitmapPtr dc, ConstBitmapPtr sc, const u8* sa,
	int dInc, int sInc)
{
	for(int y(0); y < deltaY; ++y)
	{
		for(int x(0); x < deltaX; ++x)
		{
			*dc = ((*sa++ & 0x80) ? *sc : 0) | BITALPHA;
			++sc;
			++dc;
		}
		sc += sInc;
		dc += dInc;
	}
}
void
blit2(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);

	const int srcOffset(minY * ss.Width + minX);

	blit2For(maxX - minX, maxY - minY,
		dst + vmax<int>(0, dp.Y) * ds.Width + vmax<int>(0, dp.X),
		src + srcOffset,
		srcA + srcOffset,
		ds.Width - maxX + minX, ss.Width - maxX + minX);
}
void
blit2H(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);

	const int srcOffset(minY * ss.Width + minX);

	blit2For(maxX - minX, maxY - minY,
		dst + (vmax<int>(0, dp.Y) + maxY - minY - 1) * ds.Width
		+ vmax<int>(0, dp.X),
		src + srcOffset, srcA + srcOffset,
		minX - maxX - ds.Width, ss.Width - maxX + minX);
}
static void
blit2ForU(int deltaX, int deltaY,
	BitmapPtr dc, ConstBitmapPtr sc, const u8* sa,
	int dInc, int sInc)
{
	for(int y(0); y < deltaY; ++y)
	{
		for(int x(0); x < deltaX; ++x)
		{
			*dc = ((*sa++ & 0x80) ? *sc : 0) | BITALPHA;
			++sc;
			--dc;
		}
		sc += sInc;
		dc -= dInc;
	}
}
void
blit2V(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);

	const int srcOffset(minY * ss.Width + minX);

	blit2ForU(maxX - minX, maxY - minY,
		dst + vmax<int>(0, dp.Y) * ds.Width + vmax<int>(0, dp.X)
		+ maxX - minX - 1,
		src + srcOffset, srcA + srcOffset,
		minX - maxX - ds.Width, ss.Width - maxX + minX);
}
void
blit2U(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);

	const int srcOffset(minY * ss.Width + minX);

	blit2ForU(maxX - minX, maxY - minY,
		dst + (vmax<int>(0, dp.Y) + maxY - minY - 1) * ds.Width
		+ vmax<int>(0, dp.X),
		src + srcOffset, srcA + srcOffset,
		ds.Width - maxX + minX, ss.Width - maxX + minX);
}

static inline void
biltAlphaPoint(BitmapPtr dc, ConstBitmapPtr sc, const u8* sa)
{
	int a = *sa;

	if(a >= BLT_THRESHOLD)
	{
		int s = *sc, d = *dc;

		if(d & BITALPHA && a <= BLT_MAX_ALPHA - BLT_THRESHOLD)
		{
			int dbr = (d & 0x1F) | (d << 6 & 0x1F0000), dg = d & 0x3E0;

			dbr = (dbr + (((((s & 0x1F) | (s << 6 & 0x1F0000)) - dbr)
				* a + BLT_ROUND) >> BLT_ALPHA_BITS));
			dg  = (dg  + ((((s & 0x3E0) - dg ) * a + BLT_ROUND)
				>> BLT_ALPHA_BITS));
			*dc = (dbr & 0x1F) | (dg & 0x3E0) | (dbr >> 6 & 0x7C00) | BITALPHA;
		}
		else if(a >= BLT_THRESHOLD2)
			*dc = s | BITALPHA;
	}
}
static void
blitAlphaFor(int deltaX, int deltaY,
	BitmapPtr dc, ConstBitmapPtr sc, const u8* sa,
	int dInc, int sInc)
{
	for(int y(0); y < deltaY; ++y)
	{
		for(int x(0); x < deltaX; ++x)
		{
			biltAlphaPoint(dc, sc, sa);
			++sa;
			++sc;
			++dc;
		}
		sc += sInc;
		sa += sInc;
		dc += dInc;
	}
}
void
blitAlpha(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);

	const int srcOffset(minY * ss.Width + minX);

	blitAlphaFor(maxX - minX, maxY - minY,
		dst + vmax<int>(0, dp.Y) * ds.Width + vmax<int>(0, dp.X),
		src + srcOffset, srcA + srcOffset,
		ds.Width - maxX + minX, ss.Width - maxX + minX);
}
void
blitAlphaH(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);

	const int srcOffset(minY * ss.Width + minX);

	blitAlphaFor(maxX - minX, maxY - minY,
		dst + (vmax<int>(0, dp.Y) + maxY - minY - 1) * ds.Width
		+ vmax<int>(0, dp.X),
		src + srcOffset, srcA + srcOffset,
		minX - maxX - ds.Width, ss.Width - maxX + minX);
}
static void
blitAlphaForU(int deltaX, int deltaY,
	BitmapPtr dc, ConstBitmapPtr sc, const u8* sa,
	int dInc, int sInc)
{
	for(int y(0); y < deltaY; ++y)
	{
		for(int x(0); x < deltaX; ++x)
		{
			biltAlphaPoint(dc, sc, sa);
			++sa;
			++sc;
			--dc;
		}
		sc += sInc;
		sa += sInc;
		dc -= dInc;
	}
}
void
blitAlphaV(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);

	const int srcOffset(minY * ss.Width + minX);

	blitAlphaForU(maxX - minX, maxY - minY,
		dst + vmax<int>(0, dp.Y) * ds.Width + vmax<int>(0, dp.X)
		+ maxX - minX - 1,
		src + srcOffset, srcA + srcOffset,
		minX - maxX - ds.Width, ss.Width - maxX + minX);
}
void
blitAlphaU(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* srcA, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int minX, minY, maxX, maxY;

	blitScale(sp, dp, ss, ds, sc,
		minX, minY, maxX, maxY);

	const int srcOffset(minY * ss.Width + minX);

	blitAlphaForU(maxX - minX, maxY - minY,
		dst + (vmax<int>(0, dp.Y) + maxY - minY - 1) * ds.Width
		+ vmax<int>(0, dp.X)
		+ maxX - minX - 1, src + srcOffset, srcA + srcOffset,
		ds.Width - maxX + minX, ss.Width - maxX + minX);
}


bool
DrawHLineSeg(const Graphics& g, SPOS y, SPOS x1, SPOS x2, Color c)
{
	YAssert(g.IsValid(),
		"In function \"void\n"
		"DrawHLineSeg(const Graphics& g, SPOS y, SPOS x1, SPOS x2, Color c)"
		"\": \n"
		"The graphics device context is invalid.");

	if(IsInInterval<int>(y, g.GetHeight())
		&& !((x1 < 0 && x2 < 0) || (x1 >= g.GetWidth() && x2 >= g.GetWidth())))
	{
		RestrictInInterval(x1, 0, g.GetWidth());
		RestrictInInterval(x2, 0, g.GetWidth());
		RestrictLessEqual(x1, x2);
		FillSeq<PixelType>(&g.GetBufferPtr()[y * g.GetWidth() + x1],
			x2 - x1, c);
		return true;
	}
	return false;
}

bool
DrawVLineSeg(const Graphics& g, SPOS x, SPOS y1, SPOS y2, Color c)
{
	YAssert(g.IsValid(),
		"In function \"void\n"
		"DrawVLineSeg(const Graphics& g, SPOS x, SPOS y1, SPOS y2, Color c)"
		"\": \n"
		"The graphics device context is invalid.");

	if(IsInInterval<int>(x, g.GetWidth())
		&& !((y1 < 0 && y2 < 0)
		|| (y1 >= g.GetHeight() && y2 >= g.GetHeight())))
	{
		RestrictInInterval(y1, 0, g.GetHeight());
		RestrictInInterval(y2, 0, g.GetHeight());
		RestrictLessEqual(y1, y2);
		FillVLine<PixelType>(&g.GetBufferPtr()[y1 * g.GetWidth() + x],
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
	DrawObliqueLine(const Graphics& g, SPOS x1, SPOS y1, SPOS x2, SPOS y2,
		Color c)
	{
		YAssert(y1 != y2,
			"In function \"static void\n"
			"DrawObliqueLine(const Graphics& g, SPOS x1, SPOS y1,"
			" SPOS x2, SPOS y2, Color c)\": \n"
			"Not drawing an oblique line: the line is horizontal.");
		YAssert(x1 != x2,
			"In function \"static void\n"
			"DrawObliqueLine(const Graphics& g, SPOS x1, SPOS y1,"
			" SPOS x2, SPOS y2, Color c)\": \n"
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
			SDST dx(abs(x2 - x1)), dy(abs(y2 - y1));
			bool f(dy > dx);

			if(f)
				std::swap(dx, dy);

			//初始化误差项以补偿非零截断。
			const SDST dx2(dx << 1), dy2(dy << 1);
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
DrawLineSeg(const Graphics& g, SPOS x1, SPOS y1, SPOS x2, SPOS y2, Color c)
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
	SPOS x1(p.X), y1(p.Y),
		x2(x1 + vmax<SPOS>(s.Width, 0)),
		y2(y1 + vmax<SPOS>(s.Height, 0));
	bool b(DrawVLineSeg(g, x1, y1, y2, c));
	b |= DrawHLineSeg(g, y2, x1, x2 + 1, c);
	b |= DrawVLineSeg(g, x2, y2 + 1, y1, c);
	b |= DrawHLineSeg(g, y1, x2, x1, c);

	return b;
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


Padding::Padding(SDST l, SDST r, SDST t, SDST b)
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
SetAllTo(Padding& p, SDST l, SDST r, SDST t, SDST b)
{
	p.Left = l;
	p.Right = r;
	p.Top = t;
	p.Bottom = b;
}


BitmapBuffer::BitmapBuffer(ConstBitmapPtr i, SDST w, SDST h)
	: Size(w, h),
	img(NULL)
{
	SetSize(w, h);
	if(i)
		std::memcpy(img, i, sizeof(PixelType) * GetAreaFrom(*this));
}

void
BitmapBuffer::SetSize(SDST w, SDST h)
{
	u32 s(w * h);

	if(s == 0)
	{
		ydelete_array(img);
		img = NULL;
	}
	else if(GetAreaFrom(*this) < s)
		ydelete_array(img);
		try
		{
			img = ynew PixelType[s];
		}
		catch(std::bad_alloc&)
		{
			throw LoggedEvent("Allocation failed"
				" @@ BitmapBuffer::SetSize(SDST, SDST);", 1);
		}
	Width = w;
	Height = h;
	ClearImage();
}
void
BitmapBuffer::SetSizeSwap()
{
	std::swap(Width, Height);
	ClearImage();
}

void
BitmapBuffer::ClearImage() const
{
	ClearPixel(img, GetAreaFrom(*this));
}

void
BitmapBuffer::BeFilledWith(Color c) const
{
	FillSeq<PixelType>(img, GetAreaFrom(*this), c);
}

void
BitmapBuffer::CopyToBuffer(BitmapPtr dst, const ROT rot, const Size& ds,
	const Point& sp, const Point& dp, const Size& sc) const
{
	if(~rot & 1 && dst && img)
	{
		if(rot)
			blitU(dst, ds,
			img, *this,
			sp, dp, sc);
		else
			blit(dst, ds,
			img, *this,
			sp, dp, sc);
	}
}


BitmapBufferEx::BitmapBufferEx(ConstBitmapPtr i, SDST w, SDST h)
	: BitmapBuffer(i, w, h), imgAlpha(NULL)
{
	SetSize(w, h);
	if(i)
		std::memcpy(img, i, sizeof(PixelType) * GetAreaFrom(*this));
}

void
BitmapBufferEx::SetSize(SDST w, SDST h)
{
	u32 s(w * h);

	if(s == 0)
	{
		ydelete_array(img);
		img = NULL;
		ydelete_array(imgAlpha);
		imgAlpha = NULL;
	}
	else if(GetAreaFrom(*this) < s)
		try
		{
			ydelete_array(img);
			img = ynew PixelType[s];
			ydelete_array(imgAlpha);
			imgAlpha = ynew u8[s];
		}
		catch(std::bad_alloc&)
		{
			throw LoggedEvent("Allocation failed"
				" @@ BitmapBufferEx::SetSize(SDST, SDST);", 1);
		}
	Width = w;
	Height = h;
	ClearImage();
}

void
BitmapBufferEx::ClearImage() const
{
	const u32 t = GetAreaFrom(*this);

	ClearPixel(img, t);
	ClearPixel(imgAlpha, t);
}

void
BitmapBufferEx::CopyToBuffer(BitmapPtr dst, ROT rot, const Size& ds,
	const Point& sp, const Point& dp, const Size& sc) const
{
	if(~rot & 1 && dst && img)
	{
		if(rot)
			blit2U(dst, ds,
			img, imgAlpha, *this,
			sp, dp, sc);
		else
			blit2(dst, ds,
			img, imgAlpha, *this,
			sp, dp, sc);
	}
}

void
BitmapBufferEx::BlitToBuffer(BitmapPtr dst, ROT rot, const Size& ds,
	const Point& sp, const Point& dp, const Size& sc) const
{
	if(~rot & 1 && dst && img)
	{
		if(rot)
			blitAlphaU(dst, ds,
			img, imgAlpha, *this,
			sp, dp, sc);
		else
			blitAlpha(dst, ds,
			img, imgAlpha, *this,
			sp, dp, sc);
	}
}

YSL_END_NAMESPACE(Drawing)

YSL_END

