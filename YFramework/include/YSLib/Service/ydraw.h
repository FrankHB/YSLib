/*
	© 2011-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ydraw.h
\ingroup Service
\brief 平台无关的二维图形光栅化。
\version r1134
\author FrankHB <frankhb1989@gmail.com>
\since build 219
\par 创建时间:
	2011-06-16 19:43:26 +0800
\par 修改时间:
	2014-05-23 09:22 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YDraw
*/


#ifndef YSL_INC_Service_ydraw_h_
#define YSL_INC_Service_ydraw_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YGDIBase

namespace YSLib
{

namespace Drawing
{

/*!	\defgroup Graphics2D Function for 2D Drawing
\brief 2D 图形操作。
\since build 452

更改 2D 光栅图形相关数据结构的状态以显示图形。
光栅图形以表示像素缓冲区的 BitmapPtr 或 Graphics& 类型表示。
包括以下接口：
	Put* 为基本状态修改操作；
	Plot* 以 BitmapPtr 和 const Rect& 为首参数，为带边界检查的绘制操作；
	Draw* 以 Graphics& 为首参数，第二个参数非表示边界的 const Rect& ，
		为带 Graphics 边界检查的描画操作；
	Draw* 以 Graphics& 和表示边界的 const Rect& 为首参数，
		为带一般边界检查的描画操作；
	Fill* 参数同 Draw* ，为带边界检查的填充操作。
	以上边界检查指运行时忽略越界的状态修改，要求指定的边界必须包含于缓冲区。
*/

/*!
\ingroup Graphics2D
\brief 修改指定位置的像素：(x, y) 。
\pre 断言 <tt>dst</tt> 。
\since build 394
*/
inline void
PutPixel(BitmapPtr dst, SDst w, SPos x, SPos y, Color c)
{
	YAssertNonnull(dst);
	dst[y * w + x] = c;
}
/*!
\ingroup Graphics2D
\brief 修改指定位置的像素：(x, y) 。
\pre 断言 <tt>Rect(g.GetSize()).Contains(x, y)</tt> 。
*/
inline void
PutPixel(const Graphics& g, SPos x, SPos y, Color c)
{
	YAssert(Rect(g.GetSize()).Contains(x, y),
		"The pixel is out of the buffer.");
	PutPixel(g.GetBufferPtr(), g.GetWidth(), x, y, c);
}

/*!
\ingroup Graphics2D
\since build 452
*/
//@{
//! \brief 绘制指定位置的像素：(x, y) 。
inline void
PlotPixel(BitmapPtr dst, const Rect& bounds, SDst w, SPos x, SPos y, Color c)
{
	if(bounds.Contains(x, y))
		PutPixel(dst, w, x, y, c);
}

//! \brief 描画点。
//@{
inline void
DrawPoint(const Graphics& g, const Rect& bounds, SPos x, SPos y, Color c)
{
	YAssert(bounds.IsUnstrictlyEmpty() || Rect(g.GetSize()).Contains(bounds),
		"The boundary is out of the buffer.");
	PlotPixel(g.GetBufferPtr(), bounds, g.GetWidth(), x, y, c);
}
inline void
DrawPoint(const Graphics& g, const Rect& bounds, const Point& pt, Color c)
{
	DrawPoint(g, bounds, pt.X, pt.Y, c);
}
//@}


/*!
\brief 绘制水平线段：在宽 w 的缓冲区内的区域 bounds 绘制指定端点水平坐标 x1 、
	x2 - 1 ，竖直坐标 y 。
\pre 断言： <tt>dst</tt> 。
*/
YF_API void
PlotHLineSeg(BitmapPtr dst, const Rect& bounds, SDst w, SPos y, SPos x1,
	SPos x2, Color);

/*!
\brief 描画水平线段。
\pre 断言： <tt>bool(g)</tt> 。
\pre 断言： bounds 在 g 指定的边界内。
\sa PlotHLineSeg
*/
inline void
DrawHLineSeg(const Graphics& g, const Rect& bounds, SPos y, SPos x1, SPos x2,
	Color c)
{
	YAssert(bool(g), "Invalid graphics context found."),
	YAssert(bounds.IsUnstrictlyEmpty() || Rect(g.GetSize()).Contains(bounds),
		"The boundary is out of the buffer.");
	PlotHLineSeg(g.GetBufferPtr(), bounds, g.GetWidth(), y, x1, x2, c);
}

/*!
\brief 绘制竖直线段：在宽 w 的缓冲区内的区域 bounds 绘制指定竖直水平坐标 x ，
	竖直坐标 y1 - 1 、 y2 。
\pre 断言： <tt>dst</tt> 。
*/
YF_API void
PlotVLineSeg(BitmapPtr dst, const Rect& bounds, SDst w, SPos x, SPos y1,
	SPos y2, Color);

/*!
\brief 描画竖直线段。
\pre 断言： <tt>bool(g)</tt> 。
\pre 断言： bounds 在 g 指定的边界内。
\sa PlotVLineSeg
*/
inline void
DrawVLineSeg(const Graphics& g, const Rect& bounds, SPos x, SPos y1, SPos y2,
	Color c)
{
	YAssert(bool(g), "Invalid graphics context found."),
	YAssert(bounds.IsUnstrictlyEmpty() || Rect(g.GetSize()).Contains(bounds),
		"The boundary is out of the buffer.");
	PlotVLineSeg(g.GetBufferPtr(), bounds, g.GetWidth(), x, y1, y2, c);
}

/*!
\brief 绘制线段：在宽 w 的缓冲区内的区域 bounds 绘制端点为 p1(x1, y1)
	和 p2(x2, y2) 的线段。
\pre 间接断言： <tt>dst</tt> 。
*/
YF_API void
PlotLineSeg(BitmapPtr dst, const Rect& bounds, SDst w, SPos x1, SPos y1,
	SPos x2, SPos y2, Color);

//! \brief 描画线段：在区域 ds 绘制端点为 p1(x1, y1) 和 p2(x2, y2) 的线段。
//@{
/*
\pre 断言： <tt>bool(g)</tt> 。
\pre 断言： bounds 在 g 指定的边界内。
*/
inline void
DrawLineSeg(const Graphics& g, const Rect& bounds, SPos x1, SPos y1, SPos x2,
	SPos y2, Color c)
{
	YAssert(bool(g), "Invalid graphics context found."),
	YAssert(bounds.IsUnstrictlyEmpty() || Rect(g.GetSize()).Contains(bounds),
		"The boundary is out of the buffer.");
	PlotLineSeg(g.GetBufferPtr(), bounds, g.GetWidth(), x1, y1, x2, y2, c);
}
inline PDefH(void, DrawLineSeg, const Graphics& g, const Rect& bounds,
	const Point& p1, const Point& p2, Color c)
	ImplExpr(DrawLineSeg(g, bounds, p1.X, p1.Y, p2.X, p2.Y, c))
//@}


/*!
\brief 描画标准矩形。
\pre 间接断言：图形接口上下文有效。
*/
//@{
//! \note 右下角顶点坐标 (pt.X + s.Width - 1, pt.Y + s.Height - 1) 。
YF_API void
DrawRect(const Graphics& g, const Rect& bounds, const Point& pt,
	const Size& s, Color c);
//! \note 右下角顶点坐标 (r.X + r.Width - 1, r.Y + r.Height - 1) 。
inline void
DrawRect(const Graphics& g, const Rect& bounds, const Rect& r, Color c)
{
	DrawRect(g, bounds, r.GetPoint(), r.GetSize(), c);
}
//@}

/*!
\brief 填充标准矩形。
\pre 断言：图形接口上下文有效。
*/
//@{
//! \note 右下角顶点坐标 (pt.X + s.Width - 1, pt.Y + s.Height - 1) 。
//@{
YF_API void
FillRect(const Graphics& g, const Rect&, Color c);
inline PDefH(void, FillRect, const Graphics& g, const Rect& bounds,
	const Rect& r, Color c)
	ImplExpr(FillRect(g, bounds & r, c))
//@}
//@}


//! \pre 间接断言：图形接口缓冲区指针非空。
//@{
//! \brief 描画圆形。
YF_API void
DrawCircle(const Graphics&, const Rect&, const Point&, SDst, Color c);

//! \brief 填充圆形。
YF_API void
FillCircle(const Graphics&, const Rect&, const Point&, SDst, Color c);
//@}


//! \brief 描画多边形。
template<typename _tIn>
void
DrawPolygon(Graphics& g, const Rect& bounds, _tIn first, _tIn last, Color c)
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

			DrawLineSeg(g, bounds, *first, *mid, c);
			yunseq(++first, ++mid);
		}
		YAssert(!is_undereferenceable(first), "Invalid iterator found.");
		YAssert(!is_undereferenceable(old), "Invalid iterator found.");
		DrawLineSeg(g, bounds, *first, *old, c);
	}
}
//@}

} // namespace Drawing;

} // namespace YSLib;

#endif

