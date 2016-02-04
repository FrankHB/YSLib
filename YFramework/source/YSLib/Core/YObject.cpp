/*
	© 2009-2013, 2015-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YObject.cpp
\ingroup Core
\brief 平台无关的基础对象。
\version r804
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2016-02-04 18:30 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YObject
*/


#include "YSLib/Core/YModules.h"
#include YFM_YSLib_Core_YObject
#include <ystdex/cast.hpp> // for ystdex::polymorphic_downcast;

namespace YSLib
{

ImplDeDtor(IValueHolder)


bool
ValueObject::operator==(const ValueObject& vo) const
{
	const auto x(content.get_holder());
	const auto y(vo.content.get_holder());

	return x && y && ystdex::polymorphic_downcast<const IValueHolder&>(
		*x) == ystdex::polymorphic_downcast<const IValueHolder&>(*y);
}

} // namespace YSLib;

