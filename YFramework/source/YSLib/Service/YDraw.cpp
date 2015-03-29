/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YDraw.cpp
\ingroup Service
\brief 平台无关的二维图形光栅化。
\version r1111
\author FrankHB <frankhb1989@gmail.com>
\since build 219
\par 创建时间:
	2011-06-16 19:45:33 +0800
\par 修改时间:
	2015-03-25 15:36 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YDraw
*/


#include "YSLib/Service/YModules.h"
#include YFM_YSLib_Service_YDraw
#include YFM_YSLib_Service_YBlit

namespace YSLib
{

namespace Drawing
{

void
PlotHLineSeg(BitmapPtr dst, const Rect& bounds, SDst w, SPos y, SPos x1,
	SPos x2, Color c)
{
	YAssert(bounds.Width <= w, "Wrong boundary or width found.");
	// XXX: Conversion to 'SPos' might be implementation-defined.
	if(!bounds.IsUnstrictlyEmpty()
		&& IsInInterval(SPos(y - bounds.Y), SPos(bounds.Height)))
	{
		const SPos bx(bounds.X), bxw(bounds.GetRight());

		if(!((x1 < bx && x2 < bx)
			|| (x1 >= 0 && x2 >= 0 && x1 >= bxw && x2 >= bxw)))
		{
			RestrictInInterval(x1, bx, bxw),
			RestrictInInterval(x2, bx, SPos(bxw + 1));
			RestrictLessEqual(x1, x2);
			// XXX: Conversion to 'SPos' might be implementation-defined.
			FillPixel<Pixel>(&Nonnull(dst)[y * ptrdiff_t(w) + x1],
				SDst(x2 - x1), c);
		}
	}
}

void
PlotVLineSeg(BitmapPtr dst, const Rect& bounds, SDst w, SPos x, SPos y1,
	SPos y2, Color c)
{
	YAssert(bounds.Width <= w, "Wrong boundary or width found.");
	// XXX: Conversion to 'SPos' might be implementation-defined.
	if(!bounds.IsUnstrictlyEmpty()
		&& IsInInterval(SPos(x - bounds.X), SPos(bounds.Width)))
	{
		const SPos by(bounds.Y), byh(bounds.GetBottom());

		if(!((y1 < by && y2 < by)
			|| (y1 >= 0 && y2 >= 0 && y1 >= byh && y2 >= byh)))
		{
			RestrictInInterval(y1, by, byh),
			RestrictInInterval(y2, by, SPos(byh + 1));
			RestrictLessEqual(y1, y2);
			// XXX: Conversion to 'SPos' might be implementation-defined.
			FillVerticalLine<Pixel>(&Nonnull(dst)[y1 * ptrdiff_t(w) + x],
				SDst(y2 - y1), w, c);
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

		const std::int8_t sx(FetchSign(x2 - x1)), sy(FetchSign(y2 - y1));
		SDst dx(SDst(std::abs(x2 - x1))), dy(SDst(std::abs(y2 - y1)));
		bool f(dy > dx);

		if(f)
			std::swap(dx, dy);

		//初始化误差项以补偿非零截断。
		// XXX: Conversion to 'SPos' might be implementation-defined.
		const SPos dx2(SPos(dx << 1)), dy2(SPos(dy << 1));
		SPos e(dy2 - SPos(dx));

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
DrawRect(const Graphics& g, const Rect& bounds, const Rect& r, Color c)
{
	const SPos x1(r.X), y1(r.Y), x2(r.GetRight() - 1), y2(r.GetBottom() - 1);

	if(YB_LIKELY(x1 < x2 && y1 < y2))
	{
		DrawVLineSeg(g, bounds, x1, y1, y2, c),
		DrawHLineSeg(g, bounds, y2, x1, x2 + 1, c),
		DrawVLineSeg(g, bounds, x2, y1, y2, c),
		DrawHLineSeg(g, bounds, y1, x1, x2, c);
	}
}

void
FillRect(const Graphics& g, const Rect& r, Color c)
{
	YAssert(bool(g), "Invalid graphics context found.");
	FillRectRaw(g.GetBufferPtr(), Pixel(c), g.GetSize(), r);
}

namespace
{

//! \since build 587
void
PlotCircle(void(*plotter)(const Graphics&, const Rect&, SPos, SPos, SPos, SPos,
	Color), const Graphics& g, const Rect& bounds, const Point& pt, SDst r,
	Color c)
{
	YAssertNonnull(plotter),
	YAssert(bounds.IsUnstrictlyEmpty() || Rect(g.GetSize()).Contains(bounds),
		"The boundary is out of the buffer.");
	if(r != 0)
		// Bresenham circle algorithm implementation.
		// See http://willperone.net/Code/codecircle.php .
		// XXX: Conversion to 'SPos' might be implementation-defined.
		for(SPos x(0), y((SPos(r))), p(SPos(3 - 2 * r)); y >= x;
			p += p < 0 ? (4 * x++ + 6) : (4 * (x++ - y--) + 10))
			plotter(g, bounds, pt.X, pt.Y, x, y, c);
}

} // unnamed namespace;

void
DrawCircle(const Graphics& g_, const Rect& bounds_, const Point& pt, SDst r,
	Color c_)
{
	PlotCircle([](const Graphics& g, const Rect& bounds, SPos x, SPos y,
		SPos dx, SPos dy, Color c){
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
	}, g_, bounds_, pt, r, c_);
}

void
FillCircle(const Graphics& g_, const Rect& bounds_, const Point& pt, SDst r,
	Color c_)
{
	PlotCircle([](const Graphics& g, const Rect& bounds, SPos x, SPos y,
		SPos dx, SPos dy, Color c){
		using namespace std;
		using namespace placeholders;
		const auto plot(bind(PlotHLineSeg, g.GetBufferPtr(), cref(bounds),
			g.GetWidth(), _1, _2, _3, c));

		plot(y + dy, x - dx, x + dx + 1),
		plot(y - dy, x - dx, x + dx + 1),
		plot(y + dx, x - dy, x + dy + 1),
		plot(y - dx, x - dy, x + dy + 1);
	}, g_, bounds_, pt, r, c_);
}

} // namespace Drawing;

} // namespace YSLib;

