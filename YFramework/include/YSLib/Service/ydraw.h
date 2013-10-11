/*
	© 2011-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ydraw.h
\ingroup Service
\brief 平台无关的二维图形光栅化。
\version r800
\author FrankHB <frankhb1989@gmail.com>
\since build 219
\par 创建时间:
	2011-06-16 19:43:26 +0800
\par 修改时间:
	2013-10-04 13:16 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YDraw
*/


#ifndef YSL_INC_Service_ydraw_h_
#define YSL_INC_Service_ydraw_h_ 1

#include "../Core/ygdibase.h"

namespace YSLib
{

namespace Drawing
{

//图形接口上下文操作：绘图。

/*!
\brief 修改指定位置的像素：(x, y) 。
\pre 断言 <tt>dst</tt> 。
\since build 394
*/
inline void
PutPixel(BitmapPtr dst, SDst w, SPos x, SPos y, Color c)
{
	YAssert(dst, "Null pointer found.");

	dst[y * w + x] = c;
}
/*!
\brief 修改指定位置的像素：(x, y) 。
\pre 断言 <tt>Rect(g.GetSize()).Contains(x, y)</tt> 。
*/
inline void
PutPixel(const Graphics& g, SPos x, SPos y, Color c)
{
	YAssert(Rect(g.GetSize()).Contains(x, y),
		"The pixel is not in the device context buffer.");

	PutPixel(g.GetBufferPtr(), g.GetWidth(), x, y, c);
}

/*!
\brief 绘制指定位置的像素：(x, y) 。
\since build 446
*/
inline bool
PlotPixel(BitmapPtr dst, const Rect& s, SPos x, SPos y, Color c)
{
	if(Rect(s).Contains(x, y))
	{
		PutPixel(dst, s.Width, x, y, c);
		return true;
	}
	return false;
}

/*!
\brief 描画点。
*/
//@{
inline bool
DrawPoint(const Graphics& g, SPos x, SPos y, Color c)
{
	if(YB_LIKELY(g))
		return PlotPixel(g.GetBufferPtr(), g.GetSize(), x, y, c);
	return false;
}
inline bool
DrawPoint(const Graphics& g, const Point& pt, Color c)
{
	return DrawPoint(g, pt.X, pt.Y, c);
}
//@}


/*!
\brief 绘制水平线段：在区域 ds 绘制指定端点水平坐标 x1 、 x2 - 1 ，竖直坐标 y 。
\pre 断言： <tt>dst</tt> 。
\since build 446
*/
YF_API bool
PlotHLineSeg(BitmapPtr dst, const Size& ds, SPos y, SPos x1, SPos x2, Color);

/*!
\brief 描画水平线段。
\pre 断言： <tt>bool(g)</tt> 。
\sa PlotHLineSeg
*/
inline bool
DrawHLineSeg(const Graphics& g, SPos y, SPos x1, SPos x2, Color c)
{
	YAssert(bool(g), "Invalid graphics context found.");

	return PlotHLineSeg(g.GetBufferPtr(), g.GetSize(), y, x1, x2, c);
}

/*!
\brief 绘制竖直线段：在区域 ds 绘制指定竖直水平坐标 x ，竖直坐标 y1 - 1 、 y2 。
\pre 断言： <tt>dst</tt> 。
\since build 446
*/
YF_API bool
PlotVLineSeg(BitmapPtr dst, const Size& ds, SPos x, SPos y1, SPos y2, Color);

/*!
\brief 描画竖直线段。
\pre 断言： <tt>bool(g)</tt> 。
\sa PlotVLineSeg
*/
inline bool
DrawVLineSeg(const Graphics& g, SPos x, SPos y1, SPos y2, Color c)
{
	YAssert(bool(g), "Invalid graphics context found.");

	return PlotVLineSeg(g.GetBufferPtr(), g.GetSize(), x, y1, y2, c);
}

/*!
\brief 描画线段：在区域 ds 绘制端点为 p1(x1, y1) 和 p2(x2, y2) 的线段。
*/
//@{
YF_API bool
DrawLineSeg(const Graphics& g, SPos x1, SPos y1, SPos x2, SPos y2, Color c);
inline bool
DrawLineSeg(const Graphics& g, const Point& p1, const Point& p2, Color c)
{
	return DrawLineSeg(g, p1.X, p1.Y, p2.X, p2.Y, c);
}
//@}


/*!
\brief 描画标准矩形。
\note 右下角顶点坐标 (pt.X + s.Width - 1, pt.Y + s.Height - 1) 。
*/
YF_API bool
DrawRect(const Graphics& g, const Point& pt, const Size& s, Color c);
/*!
\brief 描画标准矩形。
\note 右下角顶点坐标 (r.X + r.Width - 1, r.Y + r.Height - 1) 。
*/
inline bool
DrawRect(const Graphics& g, const Rect& r, Color c)
{
	return DrawRect(g, r.GetPoint(), r.GetSize(), c);
}

/*!
\brief 填充标准矩形。
\note 右下角顶点坐标 (pt.X + s.Width - 1, pt.Y + s.Height - 1) 。
*/
YF_API bool
FillRect(const Graphics& g, const Point& pt, const Size& s, Color c);
/*!
\brief 填充标准矩形。
\note 右下角顶点坐标 (r.X + r.Width - 1, r.Y + r.Height - 1) 。
*/
inline bool
FillRect(const Graphics& g, const Rect& r, Color c)
{
	return FillRect(g, r.GetPoint(), r.GetSize(), c);
}


/*!
\brief 描画圆形。
\since build 394
*/
YF_API bool
DrawCircle(const Graphics&, const Point&, SDst, Color c);

/*!
\brief 填充圆形。
\since build 446
*/
YF_API bool
FillCircle(const Graphics&, const Point&, SDst, Color c);


/*!
\brief 描画多边形。
\since build 449
*/
template<typename _tIn>
void
DrawPolygon(Graphics& g, _tIn first, _tIn last, Color c)
{
	if(YB_LIKELY(first != last))
	{
		using ystdex::is_undereferenceable;
		const _tIn old(first);
		_tIn mid(first);

		++mid;
		while(mid != last)
		{
			YAssert(!is_undereferenceable(first), "Invalid iterator found.");
			YAssert(!is_undereferenceable(mid), "Invalid iterator found.");

			DrawLineSeg(g, *first, *mid, c);
			yunseq(++first, ++mid);
		}

		YAssert(!is_undereferenceable(first), "Invalid iterator found.");
		YAssert(!is_undereferenceable(old), "Invalid iterator found.");

		DrawLineSeg(g, *first, *old, c);
	}
}

} // namespace Drawing;

} // namespace YSLib;

#endif

