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
\version 0.1329;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-05-03 07:23:44 +0800;
\par 修改时间:
	2011-05-31 23:32 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YGDIBase;
*/


#include "ygdibase.h"
#include "../Helper/yglobal.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

const Point Point::Zero = Point();
const Point Point::FullScreen = Point(Global::MainScreenWidth,
	Global::MainScreenHeight);

const Vec Vec::Zero = Vec();
const Vec Vec::FullScreen = Vec(Global::MainScreenWidth,
	Global::MainScreenHeight);


const Size Size::Zero = Size();
const Size Size::FullScreen = Size(Global::MainScreenWidth,
	Global::MainScreenHeight);


SPos
SelectFrom(const BinaryGroup& o, bool is_1st)
{
	return is_1st ? o.X : o.Y;
}
SDst
SelectFrom(const Size& s, bool is_1st)
{
	return is_1st ? s.Width : s.Height;
}

SPos&
SelectRefFrom(BinaryGroup& o, bool is_1st)
{
	return is_1st ? o.X : o.Y;
}
SDst&
SelectRefFrom(Size& s, bool is_1st)
{
	return is_1st ? s.Width : s.Height;
}

void
UpdateTo(BinaryGroup& o, SPos v, bool is_1st)
{
	if(is_1st)
		o.X = v;
	else
		o.Y = v;
}
void
UpdateTo(Size& s, SDst v, bool is_1st)
{
	if(is_1st)
		s.Width = v;
	else
		s.Height = v;
}


const Rect Rect::Empty = Rect();
const Rect Rect::FullScreen = Rect(Point::Zero,
	Global::MainScreenWidth, Global::MainScreenHeight);

bool
Rect::Contains(int px, int py) const
{
	return Width > 0 && Height > 0 && IsInInterval<int>(px - X, Width)
		&& IsInInterval<int>(py - Y, Height);
}

bool
Rect::ContainsStrict(int px, int py) const
{
	return Width > 1 && Height > 1 && IsInOpenInterval<int>(px - X, Width - 1)
		&& IsInOpenInterval<int>(py - Y, Height - 1);
}


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

