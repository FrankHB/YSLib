/*
	© 2015, 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file optional.cpp
\ingroup YStandardEx
\brief 可选值包装类型。
\version r49
\author FrankHB <frankhb1989@gmail.com>
\since build 590
\par 创建时间:
	2015-04-09 22:33:28 +0800
\par 修改时间:
	2018-08-06 18:38 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Optonal
*/


#include "ystdex/optional.h"

namespace ystdex
{

#if YB_Has_optional != 1
inline namespace cpp2017
{

bad_optional_access::~bad_optional_access() = default;

YB_PURE YB_ATTR_returns_nonnull const char*
bad_optional_access::what() const ynothrow
{
	return "bad optional access";
}

} // inline namespace cpp2017;
#endif

} // namespace ystdex;

