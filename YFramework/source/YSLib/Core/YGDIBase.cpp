/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YGDIBase.cpp
\ingroup Core
\brief 平台无关的基础图形学对象。
\version r709
\author FrankHB <frankhb1989@gmail.com>
\since build 206
\par 创建时间:
	2011-05-03 07:23:44 +0800
\par 修改时间:
	2015-03-25 15:46 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YGDIBase
*/


#include "YSLib/Core/YModules.h"
#include YFM_YSLib_Core_YGDIBase
#include YFM_YSLib_Core_YCoreUtilities

namespace YSLib
{

namespace Drawing
{

const Size Size::Invalid(std::numeric_limits<SDst>::lowest(),
	std::numeric_limits<SDst>::lowest());

//! \since build 453
namespace
{

//! \since build 528
bool
RectContainsRaw(const Rect& r, SPos px, SPos py) ynothrow
{
	YAssert(r.Width > 0, "Invalid width found."),
	YAssert(r.Height > 0, "Invalid height found.");

	// XXX: Conversion to 'SPos' might be implementation-defined.
	return IsInInterval<SPos>(px - r.X, SPos(r.Width))
		&& IsInInterval<SPos>(py - r.Y, SPos(r.Height));
}
inline bool
RectContainsRaw(const Rect& r, const Point& pt) ynothrow
{
	return RectContainsRaw(r, pt.X, pt.Y);
}

bool
RectContainsStrictRaw(const Rect& r, SPos px, SPos py) ynothrow
{
	YAssert(r.Width > 1, "Invalid width found."),
	YAssert(r.Height > 1, "Invalid height found.");
	// XXX: Conversion to 'SPos' might be implementation-defined.
	return IsInOpenInterval<SPos>(px - r.X, SPos(r.Width - 1))
		&& IsInOpenInterval<SPos>(py - r.Y, SPos(r.Height - 1));
}
inline bool
RectContainsStrictRaw(const Rect& r, const Point& pt) ynothrow
{
	return RectContainsStrictRaw(r, pt.X, pt.Y);
}

} // unnamed namespace;

const Rect Rect::Invalid(Size::Invalid);

bool
Rect::Contains(SPos px, SPos py) const ynothrow
{
	return !IsUnstrictlyEmpty() && RectContainsRaw(*this, px, py);
}
bool
Rect::Contains(const Rect& r) const ynothrow
{
	return !IsUnstrictlyEmpty() && RectContainsRaw(*this, r.GetPoint())
		&& RectContainsRaw(*this, r.GetPoint() + r.GetSize() - Vec(1, 1));
}

bool
Rect::ContainsStrict(SPos px, SPos py) const ynothrow
{
	return Width > 1 && Height > 1 && RectContainsStrictRaw(*this, px, py);
}
bool
Rect::ContainsStrict(const Rect& r) const ynothrow
{
	return Width > 1 && Height > 1 && !r.IsUnstrictlyEmpty()
		&& RectContainsStrictRaw(*this, r.GetPoint())
		&& RectContainsStrictRaw(*this, r.GetPoint() + r.GetSize() - Vec(1, 1));
}

Rect&
Rect::operator&=(const Rect& r) ynothrow
{
	const SPos x1(max(X, r.X)), x2(min(GetRight(), r.GetRight())),
		y1(max(Y, r.Y)), y2(min(GetBottom(), r.GetBottom()));

	return *this = x2 < x1 || y2 < y1 ? Rect()
		: Rect(x1, y1, SDst(x2 - x1), SDst(y2 - y1));
}

Rect&
Rect::operator|=(const Rect& r) ynothrow
{
	if(!*this)
		return *this = r;
	if(!r)
		return *this;

	const SPos mx(min(X, r.X)), my(min(Y, r.Y));

	return *this = Rect(mx, my, SDst(max(GetRight(), r.GetRight())
		- mx), SDst(max(GetBottom(), r.GetBottom()) - my));
}

} // namespace Drawing;

} // namespace YSLib;

