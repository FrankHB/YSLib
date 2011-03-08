/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yobject.cpp
\ingroup Core
\brief 平台无关的基础对象实现。
\version 0.1518;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2011-03-07 13:10 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YObject;
*/


#include "yobject.h"
#include "../Helper/yglobal.h"
#include "../Core/yshell.h"

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

const Rect Rect::Empty = Rect();
const Rect Rect::FullScreen = Rect(Point::Zero,
	Global::MainScreenWidth, Global::MainScreenHeight);


SPOS
SelectFrom(const BinaryGroup& o, bool is_1st)
{
	return is_1st ? o.X : o.Y;
}
SDST
SelectFrom(const Size& s, bool is_1st)
{
	return is_1st ? s.Width : s.Height;
}

SPOS&
SelectRefFrom(BinaryGroup& o, bool is_1st)
{
	return is_1st ? o.X : o.Y;
}
SDST&
SelectRefFrom(Size& s, bool is_1st)
{
	return is_1st ? s.Width : s.Height;
}

void
UpdateTo(BinaryGroup& o, SPOS v, bool is_1st)
{
	if(is_1st)
		o.X = v;
	else
		o.Y = v;
}

void
UpdateTo(Size& s, SDST v, bool is_1st)
{
	if(is_1st)
		s.Width = v;
	else
		s.Height = v;
}


BitmapPtr
Graphics::operator[](std::size_t r) const ythrow()
{
	YAssert(pBuffer, "Buffer pointer is null @@ Graphics::operator[];");
	YAssert(r < size.Height, "Access out of range @@ Graphics::operator[];");

	return pBuffer + r * size.Width;
}
BitmapPtr
Graphics::at(std::size_t r) const ythrow(std::runtime_error, std::out_of_range)
{
	if(!pBuffer)
		throw std::runtime_error(
			"Buffer pointer is null @@ Graphics::operator[];");
	if(r >= size.Height)
		throw std::out_of_range("Access out of range @@ Graphics::operator[];");

	return pBuffer + r * size.Width;
}


YSL_END_NAMESPACE(Drawing)

YSL_END

