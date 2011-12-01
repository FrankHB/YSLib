/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ygdibase.cpp
\ingroup Core
\brief 平台无关的基础图形学对象。
\version r1469;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-05-03 07:23:44 +0800;
\par 修改时间:
	2011-12-01 08:36 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YGDIBase;
*/


#include "YSLib/Core/ygdibase.h"
#include "YSLib/Helper/yglobal.h"
#include <ystdex/algorithm.hpp>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

const Size Size::Zero = Size();
const Size Size::FullScreen = Size(MainScreenWidth, MainScreenHeight);


SDst
SelectFrom(const Size& s, bool is_1st)
{
	return is_1st ? s.Width : s.Height;
}

SDst&
SelectRefFrom(Size& s, bool is_1st)
{
	return is_1st ? s.Width : s.Height;
}

void
UpdateTo(Size& s, SDst val, bool is_1st)
{
	if(is_1st)
		s.Width = val;
	else
		s.Height = val;
}


const Rect Rect::Empty = Rect();
const Rect Rect::FullScreen = Rect(Point::Zero,
	MainScreenWidth, MainScreenHeight);

bool
Rect::Contains(int px, int py) const
{
	return Width > 0 && Height > 0 && IsInInterval<int>(px - X, Width)
		&& IsInInterval<int>(py - Y, Height);
}
bool
Rect::Contains(const Rect& r) const
{
	return Contains(r.GetPoint()) && Contains(r.GetPoint() + r.GetSize());
}

bool
Rect::ContainsStrict(int px, int py) const
{
	return Width > 1 && Height > 1 && IsInOpenInterval<int>(px - X,
		Width - 1) && IsInOpenInterval<int>(py - Y, Height - 1);
}
bool
Rect::ContainsStrict(const Rect& r) const
{
	return ContainsStrict(r.GetPoint())
		&& ContainsStrict(r.GetPoint() + r.GetSize());
}


Rect
Intersect(const Rect& a, const Rect& b)
{
	const SPos xmin(min(a.X, b.X)), ymin(min(a.Y, b.Y)),
		xmax(max(a.X + a.Width, b.X + b.Width)),
		ymax(max(a.Y + a.Height, b.Y + b.Height));
	const SDst dx(xmax - xmin), dy(ymax - ymin);

	//相离。
	if(a.Width + b.Width < dx || a.Height + b.Height < dy)
		return Rect::Empty;

	//优化：包含情况。
	if(dx == a.Width && dy == a.Height)
		return b;
	if(dx == b.Width && dy == a.Height)
		return a;

	const SPos x1(max(a.X, b.X)), x2(min(a.X + a.Width, b.X + b.Width)),
		y1(max(a.Y, b.Y)), y2(min(a.Y + a.Height, b.Y + b.Height));

	return Rect(min(x1, x2), min(y1, y2),
		std::abs(x2 - x1), std::abs(y2 - y1));
}

Rect
Unite(const Rect& a, const Rect& b)
{
	if(a.IsEmpty())
		return b;
	if(b.IsEmpty())
		return a;

	auto mx(min(a.X, b.X)), my(min(a.Y, b.Y));

	return Rect(mx, my, max<SPos>(a.X + a.Width, b.X + b.Width) - mx,
		max<SPos>(a.Y + a.Height, b.Y + b.Height) - my);
}


const Graphics Graphics::Invalid;

BitmapPtr
Graphics::operator[](size_t r) const ynothrow
{
	YAssert(pBuffer, "Buffer pointer is null @ Graphics::operator[];");
	YAssert(r < size.Height, "Access out of range @ Graphics::operator[];");

	return pBuffer + r * size.Width;
}
BitmapPtr
Graphics::at(size_t r) const ythrow(GeneralEvent, std::out_of_range)
{
	if(!pBuffer)
		throw GeneralEvent("Buffer pointer is null @ Graphics::operator[];");
	if(r >= size.Height)
		throw std::out_of_range("Access out of range @ Graphics::operator[];");

	return pBuffer + r * size.Width;
}


YSL_END_NAMESPACE(Drawing)

YSL_END

