/*
	© 2011-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ydraw.cpp
\ingroup Service
\brief 平台无关的二维图形光栅化。
\version r1017
\author FrankHB <frankhb1989@gmail.com>
\since build 219
\par 创建时间:
	2011-06-16 19:45:33 +0800
\par 修改时间:
	2013-10-19 04:07 +0800
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

void
PlotHLineSeg(BitmapPtr dst, const Rect& bounds, SDst w, SPos y, SPos x1,
	SPos x2, Color c)
{
	YAssert(dst, "Null pointer found."),
	YAssert(bounds.Width <= w, "Wrong boundary or width found.");

	if(!bounds.IsUnstrictlyEmpty()
		&& IsInInterval<SPos>(y - bounds.Y, bounds.Height))
	{
		const auto bx(bounds.X);
		const auto bxw(bx + bounds.Width);

		if(!((x1 < bx && x2 < bx) || (x1 >= bxw && x2 >= bxw)))
		{
			RestrictInInterval(x1, bx, bxw),
			RestrictInInterval(x2, bx, bxw + 1);
			RestrictLessEqual(x1, x2);
			FillPixel<PixelType>(&dst[y * w + x1], x2 - x1, c);
		}
	}
}

void
PlotVLineSeg(BitmapPtr dst, const Rect& bounds, SDst w, SPos x, SPos y1,
	SPos y2, Color c)
{
	YAssert(dst, "Null pointer found."),
	YAssert(bounds.Width <= w, "Wrong boundary or width found.");

	if(!bounds.IsUnstrictlyEmpty()
		&& IsInInterval<SPos>(x - bounds.X, bounds.Width))
	{
		const auto by(bounds.Y);
		const auto byh(by + bounds.Height);

		if(!((y1 < by && y2 < by) || (y1 >= byh && y2 >= byh)))
		{
			RestrictInInterval(y1, by, byh),
			RestrictInInterval(y2, by, byh + 1);
			RestrictLessEqual(y1, y2);
			FillVerticalLine<PixelType>(&dst[y1 * w + x], y2 - y1, w, c);
		}
	}
}

void
PlotLineSeg(BitmapPtr dst, const Rect& bounds, SDst w, SPos x1, SPos y1,
	SPos x2, SPos y2, Color c)
{
	if(y1 == y2)
		PlotHLineSeg(dst, bounds, w, y1, x1, x2 + 1, c);
	else if(x1 == x2)
		PlotVLineSeg(dst, bounds, w, x1, y1, y2 + 1, c);
	else
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
			PlotPixel(dst, bounds, w, x1, y1, c);
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
	}
}

void
DrawRect(const Graphics& g, const Rect& bounds, const Point& pt, const Size& s,
	Color c)
{
	const SPos x1(pt.X), y1(pt.Y), x2(x1 + s.Width - 1), y2(y1 + s.Height - 1);

	if(YB_LIKELY(x1 < x2 && y1 < y2))
	{
		DrawVLineSeg(g, bounds, x1, y1, y2, c),
		DrawHLineSeg(g, bounds, y2, x1, x2 + 1, c),
		DrawVLineSeg(g, bounds, x2, y1, y2, c),
		DrawHLineSeg(g, bounds, y1, x1, x2, c);
	}
}

void
FillRect(const Graphics& g, const Rect& bounds, Color c)
{
	FillRectRaw<PixelType>(g.GetBufferPtr(), g.GetSize(), bounds, c);
}

namespace
{

//! \since build 452
void
PlotCircle(void(*plotter)(const Graphics&, const Rect&, SPos, SPos, SDst, SDst,
	Color), const Graphics& g, const Rect& bounds, const Point& pt, SDst r,
	Color c)
{
	YAssert(plotter, "Null pointer found.");

	if(r != 0)
	{
		// Bresenham circle algorithm implementation.
		// See http://willperone.net/Code/codecircle.php .
		for(SPos x(0), y(r), p(3 - 2 * r); y >= x;
			p += p < 0 ? (4 * x++ + 6) : (4 * (x++ - y--) + 10))
			plotter(g, bounds, pt.X, pt.Y, x, y, c);
	}
}

} // unnamed namespace;

void
DrawCircle(const Graphics& g, const Rect& bounds, const Point& pt, SDst r,
	Color c)
{
	PlotCircle([](const Graphics& g, const Rect& bounds, SPos x, SPos y,
		SDst dx, SDst dy, Color c){
		using namespace std;
		using namespace placeholders;
		const auto plot(bind(PlotPixel, g.GetBufferPtr(), cref(bounds),
			g.GetWidth(), _1, _2, c));

		plot(x + dx, y + dy),
		plot(x - dx, y + dy),
		plot(x - dx, y - dy),
		plot(x + dx, y - dy),
		plot(x + dy, y + dx),
		plot(x - dy, y + dx),
		plot(x - dy, y - dx),
		plot(x + dy, y - dx);
	}, g, bounds, pt, r, c);
}

void
FillCircle(const Graphics& g, const Rect& bounds, const Point& pt, SDst r,
	Color c)
{
	PlotCircle([](const Graphics& g, const Rect& bounds, SPos x, SPos y,
		SDst dx, SDst dy, Color c){
		using namespace std;
		using namespace placeholders;
		const auto plot(bind(PlotHLineSeg, g.GetBufferPtr(), cref(bounds),
			g.GetWidth(), _1, _2, _3, c));

		plot(y + dy, x - dx, x + dx + 1),
		plot(y - dy, x - dx, x + dx + 1),
		plot(y + dx, x - dy, x + dy + 1),
		plot(y - dx, x - dy, x + dy + 1);
	}, g, bounds, pt, r, c);
}

} // namespace Drawing;

} // namespace YSLib;

