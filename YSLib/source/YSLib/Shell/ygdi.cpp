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
\brief 平台无关的图形设备接口实现。
\version 0.3138;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-14 18:29:46 + 08:00;
\par 修改时间:
	2011-01-19 09:45 + 08:00;
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
	blit_min_x(SPOS sx, SPOS dx)
	{
		return sx + vmax<int>(0, -dx);
	}

	inline SPOS
	blit_min_y(SPOS sy, SPOS dy)
	{
		return sy + vmax<int>(0, -dy);
	}

	inline SPOS
	blit_max_x(SDST sw, SDST dw, SPOS sx, SPOS dx, SDST cw)
	{
		return sx + vmin<int>(
			vmin<int>(dw - dx, sw - sx), cw);
	}

	inline SPOS
	blit_max_y(SDST sh, SDST dh, SPOS sy, SPOS dy, SDST ch)
	{
		return sy + vmin<int>(
			vmin<int>(dh - dy, sh - sy), ch);
	}
}

void
BlitPosition(const Point& sp, const Point& dp,
	const Size& ss, const Size& ds, const Size& sc,
	int& min_x, int& min_y, int& max_x, int& max_y)
{
	min_x = blit_min_x(sp.X, dp.X);
	min_y = blit_min_y(sp.Y, dp.Y);
	max_x = blit_max_x(ss.Width, ds.Width, sp.X, dp.X, sc.Width);
	max_y = blit_max_y(ss.Height, ds.Height, sp.Y, dp.Y, sc.Height);
}

template<>
void
BlitScale<false, false>(const Point& sp, const Point& dp,
	const Size& ss, const Size& ds, const Size& sc,
	int& delta_x, int& delta_y,
	int& src_off, int& dst_off)
{
	int min_x, min_y, max_x, max_y;
	BlitPosition(sp, dp, ss, ds, sc, min_x, min_y, max_x, max_y);

	YAssert(!(max_x < min_x || max_x < min_x), "Error occured @@ BlitScale.");

	delta_x = max_x - min_x;
	delta_y = max_y - min_y;
	src_off = min_y * ss.Width + min_x;
	dst_off = vmax<int>(0, dp.Y) * ds.Width
		+ vmax<int>(0, dp.X);
}
template<>
void
BlitScale<true, false>(const Point& sp, const Point& dp,
	const Size& ss, const Size& ds, const Size& sc,
	int& delta_x, int& delta_y,
	int& src_off, int& dst_off)
{
	int min_x, min_y, max_x, max_y;
	BlitPosition(sp, dp, ss, ds, sc, min_x, min_y, max_x, max_y);

	YAssert(!(max_x < min_x || max_x < min_x), "Error occured @@ BlitScale.");

	delta_x = max_x - min_x;
	delta_y = max_y - min_y;
	src_off = min_y * ss.Width + min_x;
	dst_off = (vmax<int>(0, dp.Y) + delta_y - 1) * ds.Width
		+ vmax<int>(0, dp.X);
}
template<>
void
BlitScale<false, true>(const Point& sp, const Point& dp,
	const Size& ss, const Size& ds, const Size& sc,
	int& delta_x, int& delta_y,
	int& src_off, int& dst_off)
{
	int min_x, min_y, max_x, max_y;
	BlitPosition(sp, dp, ss, ds, sc, min_x, min_y, max_x, max_y);

	YAssert(!(max_x < min_x || max_x < min_x), "Error occured @@ BlitScale.");

	delta_x = max_x - min_x;
	delta_y = max_y - min_y;
	src_off = min_y * ss.Width + min_x;
	dst_off = vmax<int>(0, dp.Y) * ds.Width
		+ vmax<int>(0, dp.X) + delta_x - 1;
}
template<>
void
BlitScale<true, true>(const Point& sp, const Point& dp,
	const Size& ss, const Size& ds, const Size& sc,
	int& delta_x, int& delta_y,
	int& src_off, int& dst_off)
{
	int min_x, min_y, max_x, max_y;
	BlitPosition(sp, dp, ss, ds, sc, min_x, min_y, max_x, max_y);

	YAssert(!(max_x < min_x || max_x < min_x), "Error occured @@ BlitScale.");

	delta_x = max_x - min_x;
	delta_y = max_y - min_y;
	src_off = min_y * ss.Width + min_x;
	dst_off = (vmax<int>(0, dp.Y) + delta_y - 1) * ds.Width
		+ vmax<int>(0, dp.X) + delta_x - 1;
}


namespace
{
	template<bool _bPositiveScan>
	void
	blit_line(BitmapPtr dst_iter, ConstBitmapPtr src_iter, int delta_x)
	{
		if(delta_x > 0)
			mmbcpy(dst_iter, src_iter, delta_x * sizeof(PixelType));
	}
	template<>
	void
	blit_line<false>(BitmapPtr dst_iter, ConstBitmapPtr src_iter, int delta_x)
	{
		for(int x(0); x < delta_x; ++x)
			*dst_iter-- = *src_iter++;
	}

	template<bool _bPositiveScan>
	void
	blit_for(int delta_x, int delta_y,
		BitmapPtr dst_iter, ConstBitmapPtr src_iter,
		int dst_inc, int src_inc)
	{
		for(int y(0); y < delta_y; ++y)
		{
			blit_line<_bPositiveScan>(dst_iter, src_iter, delta_x);
			src_iter += src_inc;
			delta_assignment<_bPositiveScan>(dst_iter, dst_inc);
		}
	}
}

void
Blit(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int delta_x, delta_y;
	int src_off, dst_off;

	BlitScale<false, false>(sp, dp, ss, ds, sc,
		delta_x, delta_y, src_off, dst_off);
	blit_for<true>(delta_x, delta_y, dst + dst_off, src + src_off,
		ds.Width, ss.Width);
}

void
BlitH(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int delta_x, delta_y;
	int src_off, dst_off;

	BlitScale<true, false>(sp, dp, ss, ds, sc,
		delta_x, delta_y, src_off, dst_off);
	blit_for<false>(delta_x, delta_y, dst + dst_off, src + src_off,
		- ds.Width - delta_x, ss.Width - delta_x);
}

void
BlitV(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int delta_x, delta_y;
	int src_off, dst_off;

	BlitScale<false, true>(sp, dp, ss, ds, sc,
		delta_x, delta_y, src_off, dst_off);
	blit_for<true>(delta_x, delta_y, dst + dst_off, src + src_off,
		- ds.Width - delta_x, ss.Width - delta_x);
}

void
BlitU(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int delta_x, delta_y;
	int src_off, dst_off;

	BlitScale<true, true>(sp, dp, ss, ds, sc,
		delta_x, delta_y, src_off, dst_off);
	blit_for<false>(delta_x, delta_y, dst + dst_off, src + src_off,
		ds.Width, ss.Width);
}

/*
void Blit(u8* dst, SDST dw, SDST dh,
const u8* src, SDST sw, SDST sh,
const Point& sp, const Point& dp, const Size& sc)
{
const int min_x = blit_min_x(sx, dx),
min_y = blit_min_y(sy, dy),
max_x = blit_max_x(sw, dw, sx, dx, cw),
max_y = blit_max_y(sh, dh, sy, dy, ch);
const u8* src_iter = src + min_y * ss.Width + min_x;
u8* dst_iter = dst + vmax<int>(0, dp.Y) * ds.Width + vmax<int>(0, dp.X);

for(int y = min_y; y < max_y; y++)
{
mmbcpy(dst_iter, src_iter, max_x - min_x);
src_iter += sw;
dst_iter += dw;
}
}
*/

namespace
{
	template<bool _bPositiveScan>
	void
	blit2_for(int delta_x, int delta_y,
		BitmapPtr dst_iter, ConstBitmapPtr src_iter,
		int dst_inc, int src_inc)
	{
		for(int y(0); y < delta_y; ++y)
		{
			for(int x(0); x < delta_x; ++x)
			{
				if(*src_iter & BITALPHA)
					*dst_iter = *src_iter;
				++src_iter;
				xcrease<_bPositiveScan>(dst_iter);
			}
			src_iter += src_inc;
			delta_assignment<_bPositiveScan>(dst_iter, dst_inc);
		}
	}
}

void
Blit2(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int delta_x, delta_y;
	int src_off, dst_off;

	BlitScale<false, false>(sp, dp, ss, ds, sc,
		delta_x, delta_y, src_off, dst_off);
	blit2_for<true>(delta_x, delta_y, dst + dst_off, src + src_off,
		ds.Width - delta_x, ss.Width - delta_x);
}

void
Blit2H(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int delta_x, delta_y;
	int src_off, dst_off;

	BlitScale<true, false>(sp, dp, ss, ds, sc,
		delta_x, delta_y, src_off, dst_off);
	blit2_for<false>(delta_x, delta_y, dst + dst_off, src + src_off,
		- ds.Width - delta_x, ss.Width - delta_x);
}

void
Blit2V(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int delta_x, delta_y;
	int src_off, dst_off;

	BlitScale<false, true>(sp, dp, ss, ds, sc,
		delta_x, delta_y, src_off, dst_off);
	blit2_for<true>(delta_x, delta_y, dst + dst_off, src + src_off,
		- ds.Width - delta_x, ss.Width - delta_x);
}

void
Blit2U(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int delta_x, delta_y;
	int src_off, dst_off;

	BlitScale<true, true>(sp, dp, ss, ds, sc,
		delta_x, delta_y, src_off, dst_off);
	blit2_for<false>(delta_x, delta_y, dst + dst_off, src + src_off,
		ds.Width - delta_x, ss.Width - delta_x);
}

namespace
{
	template<bool _bPositiveScan>
	void
	blit2_for(int delta_x, int delta_y,
		BitmapPtr dst_iter, ConstBitmapPtr src_iter, const u8* src_alpha,
		int dst_inc, int src_inc)
	{
		for(int y(0); y < delta_y; ++y)
		{
			for(int x(0); x < delta_x; ++x)
			{
				*dst_iter = ((*src_alpha++ & 0x80) ? *src_iter : 0) | BITALPHA;
				++src_iter;
				xcrease<_bPositiveScan>(dst_iter);
			}
			src_iter += src_inc;
			delta_assignment<_bPositiveScan>(dst_iter, dst_inc);
		}
	}
}

void
Blit2(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* src_alpha, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int delta_x, delta_y;
	int src_off, dst_off;

	BlitScale<false, false>(sp, dp, ss, ds, sc,
		delta_x, delta_y, src_off, dst_off);
	blit2_for<true>(delta_x, delta_y, dst + dst_off, src + src_off, src_alpha + src_off,
		ds.Width - delta_x, ss.Width - delta_x);
}

void
Blit2H(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* src_alpha, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int delta_x, delta_y;
	int src_off, dst_off;

	BlitScale<true, false>(sp, dp, ss, ds, sc,
		delta_x, delta_y, src_off, dst_off);
	blit2_for<false>(delta_x, delta_y,
		dst + dst_off, src + src_off, src_alpha + src_off,
		- ds.Width - delta_x, ss.Width - delta_x);
}

void
Blit2V(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* src_alpha, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int delta_x, delta_y;
	int src_off, dst_off;

	BlitScale<false, true>(sp, dp, ss, ds, sc,
		delta_x, delta_y, src_off, dst_off);
	blit2_for<true>(delta_x, delta_y,
		dst + dst_off, src + src_off, src_alpha + src_off,
		- ds.Width - delta_x, ss.Width - delta_x);
}

void
Blit2U(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* src_alpha, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int delta_x, delta_y;
	int src_off, dst_off;

	BlitScale<true, true>(sp, dp, ss, ds, sc,
		delta_x, delta_y, src_off, dst_off);
	blit2_for<false>(delta_x, delta_y,
		dst + dst_off, src + src_off, src_alpha + src_off,
		ds.Width - delta_x, ss.Width - delta_x);
}

namespace
{
//#define YSL_FAST_BLIT

#ifdef YSL_FAST_BLIT

	//测试用，不使用 Alpha 混合的快速算法。
	inline void
	biltAlphaPoint(BitmapPtr dst_iter, ConstBitmapPtr src_iter, const u8* src_alpha)
	{
		if(*src_alpha >= BLT_THRESHOLD2)
			*dst_iter = *src_iter | BITALPHA;
	}

#else

	inline void
	biltAlphaPoint(BitmapPtr dst_iter, ConstBitmapPtr src_iter,
		const u8* src_alpha)
	{
		register u32 a = *src_alpha;

		if(a >= BLT_THRESHOLD)
		{
			/*
			格式： 16 位 ARGB1555 。
			算法示意：
								arrrrrgggggbbbbb
				0000000000arrrrrgggggbbbbb000000
				00000000000111110000000000000000
				00000000000rrrrr0000000000000000
				00000000000rrrrr00000000000bbbbb : dbr
				0000000000000000000000ggggg00000 : dg
			分解红色和蓝色分量至 32 位寄存器以减少乘法次数。
			使用下列 Alpha 混合公式（其中 alpha = a / BLT_MAX_ALPHA）：
			dst_iter = (1 - alpha) * d + alpha * s
			= ((BLT_MAX_ALPHA - a) * d + a * s) >> BLT_ALPHA_BITS
			= ((d << BLT_ALPHA_BITS) + BLT_ROUND + a * (s - d))
				>> BLT_ALPHA_BITS;
			可进一步近似为 d + ((a * (s - d)) >> BLT_ALPHA_BITS)，但有额外损失。
			*/
			register u32 s = *src_iter, d = *dst_iter;

			if(d & BITALPHA && a <= BLT_MAX_ALPHA - BLT_THRESHOLD)
			{
				register u32 dbr = (d & 0x1F) | (d << 6 & 0x1F0000),
					dg = d & 0x3E0;

				dbr = (dbr + (((((s & 0x1F) | (s << 6 & 0x1F0000)) - dbr)
					* a + BLT_ROUND) >> BLT_ALPHA_BITS));
				dg  = (dg  + ((((s & 0x3E0) - dg) * a + BLT_ROUND)
					>> BLT_ALPHA_BITS));
				*dst_iter = (dbr & 0x1F) | (dg & 0x3E0)
					| (dbr >> 6 & 0x7C00) | BITALPHA;
			}
			else if(a >= BLT_THRESHOLD2)
				*dst_iter = s | BITALPHA;
		}
	}

#endif

	template<bool _bPositiveScan>
	void
	blitAlpha_for(int delta_x, int delta_y,
		BitmapPtr dst_iter, ConstBitmapPtr src_iter, const u8* src_alpha,
		int dst_inc, int src_inc)
	{
		for(int y(0); y < delta_y; ++y)
		{
			for(int x(0); x < delta_x; ++x)
			{
				biltAlphaPoint(dst_iter, src_iter, src_alpha);
				++src_alpha;
				++src_iter;
				xcrease<_bPositiveScan>(dst_iter);
			}
			src_iter += src_inc;
			src_alpha += src_inc;
			delta_assignment<_bPositiveScan>(dst_iter, dst_inc);
		}
	}
}

void
BlitAlpha(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* src_alpha, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int delta_x, delta_y;
	int src_off, dst_off;

	BlitScale<false, false>(sp, dp, ss, ds, sc,
		delta_x, delta_y, src_off, dst_off);
	blitAlpha_for<true>(delta_x, delta_y,
		dst + dst_off, src + src_off, src_alpha + src_off,
		ds.Width - delta_x, ss.Width - delta_x);
}

void
BlitAlphaH(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* src_alpha, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int delta_x, delta_y;
	int src_off, dst_off;

	BlitScale<true, false>(sp, dp, ss, ds, sc,
		delta_x, delta_y, src_off, dst_off);
	blitAlpha_for<false>(delta_x, delta_y,
		dst + dst_off, src + src_off, src_alpha + src_off,
		- ds.Width - delta_x, ss.Width - delta_x);
}

void
BlitAlphaV(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* src_alpha, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int delta_x, delta_y;
	int src_off, dst_off;

	BlitScale<false, true>(sp, dp, ss, ds, sc,
		delta_x, delta_y, src_off, dst_off);
	blitAlpha_for<true>(delta_x, delta_y,
		dst + dst_off, src + src_off, src_alpha + src_off,
		- ds.Width - delta_x, ss.Width - delta_x);
}

void
BlitAlphaU(BitmapPtr dst, const Size& ds,
	ConstBitmapPtr src, const u8* src_alpha, const Size& ss,
	const Point& sp, const Point& dp, const Size& sc)
{
	int delta_x, delta_y;
	int src_off, dst_off;

	BlitScale<true, true>(sp, dp, ss, ds, sc,
		delta_x, delta_y, src_off, dst_off);
	blitAlpha_for<false>(delta_x, delta_y,
		dst + dst_off, src + src_off, src_alpha + src_off,
		ds.Width - delta_x, ss.Width - delta_x);
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
		FillPixel<PixelType>(&g.GetBufferPtr()[y * g.GetWidth() + x1],
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

void
CopyBuffer(const Graphics& dst, const Graphics& src)
{
	YAssert(dst.GetBufferPtr(), "Destination bitmap pointer is null.");
	YAssert(src.GetBufferPtr(), "Source bitmap pointer is null.");
	YAssert(dst.GetSize() == src.GetSize(), "Source and destination sizes"
		"are not same.");

	mmbcpy(dst.GetBufferPtr(), src.GetBufferPtr(),
		src.GetSizeOfBuffer());
}

void
CopyTo(const Graphics& dst, const Graphics& src,
	const Point& dp, Rotation rot)
{
	if(~rot & 1 && dst.IsValid() && src.IsValid())
		(rot ? BlitU : Blit)(dst.GetBufferPtr(), dst.GetSize(),
			src.GetBufferPtr(), src.GetSize(),
			Point::Zero, dp, src.GetSize());
}
void
CopyTo(BitmapPtr dst, const Graphics& g, Rotation rot, const Size& ds,
	const Point& sp, const Point& dp, const Size& sc)
{
	if(~rot & 1 && dst && g.GetBufferPtr())
		(rot ? BlitU : Blit)(dst, ds,
			g.GetBufferPtr(), g.GetSize(), sp, dp, sc);
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
	: Graphics()
	//不能提前初始化 size ，否则指针非空和面积非零状态不一致。 
{
	SetSize(w, h);
	if(i)
		mmbcpy(pBuffer, i, GetSizeOfBuffer());
}

void
BitmapBuffer::SetSize(SDST w, SDST h)
{
	u32 s(w * h);

	if(s == 0)
	{
		ydelete_array(pBuffer);
		pBuffer = NULL;
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
			throw LoggedEvent("Allocation failed"
				" @@ BitmapBuffer::SetSize(SDST, SDST);", 1);
		}

	YAssert(!((pBuffer != NULL) ^ (s != 0)), "Buffer corruptied"
		" @@ BitmapBuffer::SetSize(SDST, SDST);");

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


BitmapBufferEx::BitmapBufferEx(ConstBitmapPtr i, SDST w, SDST h)
	: BitmapBuffer(), pBufferAlpha(NULL)
{
	SetSize(w, h);
	if(i)
		mmbcpy(pBuffer, i, GetSizeOfBuffer());
}

void
BitmapBufferEx::SetSize(SDST w, SDST h)
{
	u32 s(w * h);

	if(s == 0)
	{
		ydelete_array(pBuffer);
		pBuffer = NULL;
		ydelete_array(pBufferAlpha);
		pBufferAlpha = NULL;
	}
	else if(GetAreaFrom(size) < s)
	{
		BitmapPtr pBufferNew(NULL);

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
				" @@ BitmapBufferEx::SetSize(SDST, SDST);", 1);
		}
	}

	YAssert(!((pBuffer != NULL) ^ (s != 0)), "Buffer corruptied"
		" @@ BitmapBufferEx::SetSize(SDST, SDST);");
	YAssert(!((pBufferAlpha != NULL) ^ (s != 0)), "Buffer corruptied"
		" @@ BitmapBufferEx::SetSize(SDST, SDST);");

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

void
BitmapBufferEx::Flush(BitmapPtr dst, const Size& ds,
	const Point& sp, const Point& dp, const Size& sc, Rotation rot) const
{
	if(~rot & 1 && dst && pBuffer)
	{
		if(rot)
			Blit2U(dst, ds,
			pBuffer, pBufferAlpha, GetSize(), sp, dp, sc);
		else
			Blit2(dst, ds, pBuffer, pBufferAlpha, GetSize(), sp, dp, sc);
	}
}

void
BitmapBufferEx::BlitTo(BitmapPtr dst, const Size& ds,
	const Point& sp, const Point& dp, const Size& sc, Rotation rot) const
{
	if(~rot & 1 && dst && pBuffer)
	{
		if(rot)
			BlitAlphaU(dst, ds, pBuffer, pBufferAlpha, GetSize(), sp, dp, sc);
		else
			BlitAlpha(dst, ds, pBuffer, pBufferAlpha, GetSize(), sp, dp, sc);
	}
}

YSL_END_NAMESPACE(Drawing)

YSL_END

