/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file exception.h
\ingroup YStandardEx
\brief 标准库异常扩展接口。
\version r53
\author FrankHB <frankhb1989@gmail.com>
\since build 522
\par 创建时间:
	2014-07-25 20:14:51 +0800
\par 修改时间:
	2014-07-25 20:38 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Exception
*/


#ifndef YB_INC_ystdex_exception_hpp_
#define YB_INC_ystdex_exception_hpp_ 1

#include "../ydef.h"
#include <exception>
#include <stdexcept>

namespace ystdex
{

/*!
\brief 引起异常：若当前存在未捕获的异常则抛出嵌套异常，否则抛出异常。
\since build 522
*/
template<typename _type>
YB_NORETURN inline void
raise_exception(const _type& e)
{
	if(std::uncaught_exception())
		std::throw_with_nested(e);
	throw e;
}

} // namespace ystdex;

#endif

