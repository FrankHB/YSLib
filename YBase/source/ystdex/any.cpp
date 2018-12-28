/*
	© 2012-2016, 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file any.cpp
\ingroup YStandardEx
\brief 动态泛型类型。
\version r384
\author FrankHB <frankhb1989@gmail.com>
\since build 352
\par 创建时间:
	2012-11-05 11:12:01 +0800
\par 修改时间:
	2018-12-20 07:10 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Any
*/


#include "ystdex/any.h" // for default_init;

namespace ystdex
{

namespace any_ops
{

holder::~holder() = default;

} // namespace any_ops;


bad_any_cast::~bad_any_cast() = default;

const char*
bad_any_cast::from() const ynothrow
{
	return
		from_type() == ystdex::type_id<void>() ? "unknown" : from_type().name();
}

const char*
bad_any_cast::to() const ynothrow
{
	return to_type() == ystdex::type_id<void>() ? "unknown" : to_type().name();
}

const char*
bad_any_cast::what() const ynothrow
{
	return "Failed conversion: any_cast.";
}

} // namespace ystdex;

