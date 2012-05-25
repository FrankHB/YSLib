/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ygdibase.cpp
\ingroup Core
\brief 平台无关的基础图形学对象。
\version r1503;
\author FrankHB<frankhb1989@gmail.com>
\since build 206 。
\par 创建时间:
	2011-05-03 07:23:44 +0800;
\par 修改时间:
	2012-05-25 21:25 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Core::YGDIBase;
*/


#include "YSLib/Core/ygdibase.h"
#include "YSLib/Core/ycutil.h"
#include <ystdex/algorithm.hpp>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

const Size Size::Zero;
const Size Size::Invalid(std::numeric_limits<SDst>::lowest(),
		std::numeric_limits<SDst>::lowest());


const Rect Rect::Empty;
const Rect Rect::Invalid(Point::Zero, Size::Invalid);

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
	const SPos axm(a.X + a.Width);
	const SPos bxm(b.X + b.Width);
	const SPos aym(a.Y + a.Height);
	const SPos bym(b.Y + b.Height);
	const SDst dx(max(axm, bxm) - min(a.X, b.X)),
		dy(max(aym, bym) - min(a.Y, b.Y));

	//相离。
	if(a.Width + b.Width < dx || a.Height + b.Height < dy)
		return Rect::Empty;

	//优化：包含情况。
	if(dx == a.Width && dy == a.Height)
		return b;
	if(dx == b.Width && dy == b.Height)
		return a;

	SPos x1(max(a.X, b.X)), x2(min(axm, bxm)),
		y1(max(a.Y, b.Y)), y2(min(aym, bym));

	if(x2 < x1)
		std::swap(x1, x2);
	if(y2 < y1)
		std::swap(y1, y2);

	return Rect(x1, y1, x2 - x1, y2 - y1);
}

Rect
Unite(const Rect& a, const Rect& b)
{
	if(YCL_UNLIKELY(a.IsEmpty()))
		return b;
	if(YCL_UNLIKELY(b.IsEmpty()))
		return a;

	auto mx(min(a.X, b.X)), my(min(a.Y, b.Y));

	return Rect(mx, my, max<SPos>(a.X + a.Width, b.X + b.Width) - mx,
		max<SPos>(a.Y + a.Height, b.Y + b.Height) - my);
}


const Graphics Graphics::Invalid;

BitmapPtr
Graphics::operator[](size_t r) const ynothrow
{
	YAssert(pBuffer, "Null pointer found.");
	YAssert(r < size.Height, "Access out of range.");

	return pBuffer + r * size.Width;
}
BitmapPtr
Graphics::at(size_t r) const ythrow(GeneralEvent, std::out_of_range)
{
	if(YCL_UNLIKELY(!pBuffer))
		throw GeneralEvent("Null pointer found.");
	if(YCL_UNLIKELY(r >= size.Height))
		throw std::out_of_range("Access out of range.");

	return pBuffer + r * size.Width;
}


YSL_END_NAMESPACE(Drawing)

YSL_END

