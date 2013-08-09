/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ydraw.cpp
\ingroup Service
\brief 平台无关的二维图形光栅化。
\version r835
\author FrankHB <frankhb1989@gmail.com>
\since build 219
\par 创建时间:
	2011-06-16 19:45:33 +0800
\par 修改时间:
	2013-08-05 21:30 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YDraw
*/


#include "YSLib/Service/ydraw.h"
#include "YSLib/Service/yblit.h"

namespace YSLib
{

namespace Drawing
{

bool
DrawHLineSeg(const Graphics& g, SPos y, SPos x1, SPos x2, Color c)
{
	YAssert(bool(g), "Invalid graphics context found.");

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
	YAssert(bool(g), "Invalid graphics context found.");

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
		"Not drawing an oblique line: the line is horizontal.");
	YAssert(x1 != x2, "Not drawing an oblique line: the line is vertical.");

	if(Rect(g.GetSize()).Contains(x1, y1)
		&& Rect(g.GetSize()).Contains(x2, y2))
	{
		/*
		一般 Bresenham 算法：实现自
		http://cg.sjtu.edu.cn/lecture_site/chap2/mainframe212.htm 伪代码。
		*/
		//起点 (x1, y1) 和终点 (x2, y2) 不同。

		const s8 sx(FetchSign(x2 - x1)), sy(FetchSign(y2 - y1));
		SDst dx(std::abs(x2 - x1)), dy(std::abs(y2 - y1));
		bool f(dy > dx);

		if(f)
			std::swap(dx, dy);

		//初始化误差项以补偿非零截断。
		const SDst dx2(dx << 1), dy2(dy << 1);
		int e(dy2 - dx);

		//主循环。
		while(dx-- != 0)
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

} // unnamed namespace;

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
DrawRect(const Graphics& g, const Point& pt, const Size& s, Color c)
{
	const SPos x1(pt.X), y1(pt.Y), x2(x1 + s.Width - 1), y2(y1 + s.Height - 1);

	if(YB_LIKELY(x1 < x2 && y1 < y2))
	{
		bool b(DrawVLineSeg(g, x1, y1, y2, c));

		b |= DrawHLineSeg(g, y2, x1, x2 + 1, c);
		b |= DrawVLineSeg(g, x2, y1, y2, c);
		b |= DrawHLineSeg(g, y1, x1, x2, c);
		return b;
	}
	return false;
}

bool
FillRect(const Graphics& g, const Point& pt, const Size& s, Color c)
{
	if(YB_LIKELY(g))
	{
		FillRect<PixelType>(g.GetBufferPtr(), g.GetSize(), pt, s, c);
		return true;
	}
	return false;
}

namespace
{

//! \since build 394
void
Draw8Points(BitmapPtr dst, const Size& s, SPos x0, SPos y0, SPos x, SPos y,
	Color c)
{
	PlotPixel(dst, s, x0 + x, y0 + y, c),
	PlotPixel(dst, s, x0 - x, y0 + y, c),
	PlotPixel(dst, s, x0 - x, y0 - y, c),
	PlotPixel(dst, s, x0 + x, y0 - y, c),
	PlotPixel(dst, s, x0 + y, y0 + x, c),
	PlotPixel(dst, s, x0 - y, y0 + x, c),
	PlotPixel(dst, s, x0 - y, y0 - x, c),
	PlotPixel(dst, s, x0 + y, y0 - x, c);
}

} // unnamed namespace;

bool
DrawCircle(const Graphics& g, const Point& pt, SDst r, Color c)
{
	if(r == 0)
		return false;

	// Midpoint circle algorithm implementation.
	// See http://cg.sjtu.edu.cn/lecture_site/chap2/mainframe224.htm .
	const auto dst(g.GetBufferPtr());
	const Size& s(g.GetSize());
	SPos x(0), y(r), d(1 - r);

	for(SDst dt(2), db(4); x <= y; yunseq(dt += 2, db += 2, ++x))
	{
		Draw8Points(dst, s, pt.X, pt.Y, x, y, c);
		if(d < 0)
			d += dt;
		else
		{
			d += db;
			yunseq(db += 2, --y);
		}
	}
	return true;
}

} // namespace Drawing;

} // namespace YSLib;

