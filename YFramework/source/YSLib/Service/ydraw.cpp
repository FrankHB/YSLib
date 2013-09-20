/*
	Copyright by FrankHB 2009-2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ydraw.cpp
\ingroup Service
\brief 平台无关的二维图形光栅化。
\version r896
\author FrankHB <frankhb1989@gmail.com>
\since build 219
\par 创建时间:
	2011-06-16 19:45:33 +0800
\par 修改时间:
	2013-09-17 09:47 +0800
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
PlotHLineSeg(BitmapPtr dst, const Size& ds, SPos y, SPos x1, SPos x2, Color c)
{
	YAssert(dst, "Null pointer found.");

	if(IsInInterval<int>(y, ds.Height)
		&& !((x1 < 0 && x2 < 0) || (x1 >= ds.Width && x2 >= ds.Width)))
	{
		RestrictInInterval(x1, 0, ds.Width);
		RestrictInInterval(x2, 0, ds.Width);
		RestrictLessEqual(x1, x2);
		FillPixel<PixelType>(&dst[y * ds.Width + x1], x2 - x1, c);
		return true;
	}
	return false;
}

bool
PlotVLineSeg(BitmapPtr dst, const Size& ds, SPos x, SPos y1, SPos y2, Color c)
{
	YAssert(dst, "Null pointer found.");

	if(IsInInterval<int>(x, ds.Width)
		&& !((y1 < 0 && y2 < 0) || (y1 >= ds.Height && y2 >= ds.Height)))
	{
		RestrictInInterval(y1, 0, ds.Height);
		RestrictInInterval(y2, 0, ds.Height);
		RestrictLessEqual(y1, y2);
		FillVerticalLine<PixelType>(&dst[y1 * ds.Width + x], y2 - y1, ds.Width,
			c);
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

bool
PlotCircle(void(*plotter)(const Graphics&, SPos, SPos, SDst, SDst, Color),
	const Graphics& g, const Point& pt, SDst r, Color c)
{
	if(r == 0)
		return false;

	YAssert(plotter, "Null pointer found.");

	// Bresenham circle algorithm implementation.
	// See http://willperone.net/Code/codecircle.php .
	for(SPos x(0), y(r), p(3 - 2 * r); y >= x;
		p += p < 0 ? (4 * x++ + 6) : (4 * (x++ - y--) + 10))
		plotter(g, pt.X, pt.Y, x, y, c);
	return true;
}

} // unnamed namespace;

bool
DrawCircle(const Graphics& g, const Point& pt, SDst r, Color c)
{
	return PlotCircle([](const Graphics& g, SPos x, SPos y, SDst dx, SDst dy,
		Color c){
		using namespace std;
		using namespace placeholders;
		const auto plot(bind(PlotPixel, g.GetBufferPtr(), cref(g.GetSize()), _1,
			_2, c));

		plot(x + dx, y + dy),
		plot(x - dx, y + dy),
		plot(x - dx, y - dy),
		plot(x + dx, y - dy),
		plot(x + dy, y + dx),
		plot(x - dy, y + dx),
		plot(x - dy, y - dx),
		plot(x + dy, y - dx);
	}, g, pt, r, c);
}

bool
FillCircle(const Graphics& g, const Point& pt, SDst r, Color c)
{
	return PlotCircle([](const Graphics& g, SPos x, SPos y, SDst dx, SDst dy,
		Color c){
		using namespace std;
		using namespace placeholders;
		const auto plot(bind(PlotHLineSeg, g.GetBufferPtr(), cref(g.GetSize()),
			_1, _2, _3, c));

		plot(y + dy, x - dx, x + dx + 1),
		plot(y - dy, x - dx, x + dx + 1),
		plot(y + dx, x - dy, x + dy + 1),
		plot(y - dx, x - dy, x + dy + 1);
	}, g, pt, r, c);
}

} // namespace Drawing;

} // namespace YSLib;

