/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ydraw.h
\ingroup Service
\brief 平台无关的二维图形光栅化。
\version r1658;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-06-16 19:43:26 +0800;
\par 修改时间:
	2012-03-26 08:32 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Service::YDraw;
*/


#ifndef YSL_INC_SERVICE_YDRAW_H_
#define YSL_INC_SERVICE_YDRAW_H_

#include "../Core/ygdibase.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

//图形接口上下文操作：绘图。

/*!
\brief 绘制像素：(x, y) 。
\pre 断言 g.IsValid && Rect(g.GetSize()).Contains(x, y) 。
*/
inline void
PutPixel(const Graphics& g, SPos x, SPos y, Color c)
{
	YAssert(g.IsValid(),
		"In function \"inline void\n"
		"PutPixel(const Graphics& g, SPos x, SPos y, Color c)\": \n"
		"The graphics device context is invalid.");
	YAssert(Rect(g.GetSize()).Contains(x, y),
		"In function \"inline void\n"
		"PutPixel(const Graphics& g, SPos x, SPos y, Color c)\": \n"
		"The pixel is not in the device context buffer.");

	g.GetBufferPtr()[y * g.GetWidth() + x] = c;
}

/*!
\brief 绘制点：(x, y) 。
*/
inline bool
DrawPoint(const Graphics& g, SPos x, SPos y, Color c)
{
	if(YCL_LIKELY(g.IsValid()))
		if(Rect(g.GetSize()).Contains(x, y))
		{
			PutPixel(g, x, y, c);
			return true;
		}
	return false;
}
/*!
\brief 绘制点：pt 。
*/
inline bool
DrawPoint(const Graphics& g, const Point& pt, Color c)
{
	return DrawPoint(g, pt.X, pt.Y, c);
}

/*!
\brief 绘制水平线段：指定端点水平坐标 x1 、 x2 - 1 ，竖直坐标 y 。
\pre 断言：g.IsValid() 。
*/
bool
DrawHLineSeg(const Graphics& g, SPos y, SPos x1, SPos x2, Color c);

/*!
\brief 绘制竖直线段：指定竖直水平坐标 x ，竖直坐标 y1 - 1 、 y2 。
\pre 断言：g.IsValid() 。
*/
bool
DrawVLineSeg(const Graphics& g, SPos x, SPos y1, SPos y2, Color c);

/*!
\brief 绘制一般线段：端点 p1(x1, y1) 和 p2(x2, y2) 。
*/
bool
DrawLineSeg(const Graphics& g, SPos x1, SPos y1, SPos x2, SPos y2, Color c);
/*!
\brief 绘制一般线段：端点 p1, p2 。
*/
inline bool
DrawLineSeg(const Graphics& g, const Point& p1, const Point& p2, Color c)
{
	return DrawLineSeg(g, p1.X, p1.Y, p2.X, p2.Y, c);
}

/*!
\brief 绘制空心正则矩形。
\note 右下角顶点坐标 (pt.X + s.Width - 1, pt.Y + s.Height - 1) 。
*/
bool
DrawRect(const Graphics& g, const Point& pt, const Size& s, Color c);
/*!
\brief 绘制空心正则矩形。
\note 右下角顶点坐标 (r.X + r.Width - 1, r.Y + r.Height - 1) 。
*/
inline bool
DrawRect(const Graphics& g, const Rect& r, Color c)
{
	return DrawRect(g, r.GetPoint(), r.GetSize(), c);
}

/*!
\brief 绘制实心正则矩形。
\note 右下角顶点坐标 (pt.X + s.Width - 1, pt.Y + s.Height - 1) 。
*/
bool
FillRect(const Graphics& g, const Point& pt, const Size& s, Color c);
/*!
\brief 绘制实心正则矩形。
\note 右下角顶点坐标 (r.X + r.Width - 1, r.Y + r.Height - 1) 。
*/
inline bool
FillRect(const Graphics& g, const Rect& r, Color c)
{
	return FillRect(g, r.GetPoint(), r.GetSize(), c);
}

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

