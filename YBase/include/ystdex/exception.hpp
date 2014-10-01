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
\version r108
\author FrankHB <frankhb1989@gmail.com>
\since build 522
\par 创建时间:
	2014-07-25 20:14:51 +0800
\par 修改时间:
	2014-09-30 08:01 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Exception
*/


#ifndef YB_INC_ystdex_exception_hpp_
#define YB_INC_ystdex_exception_hpp_ 1

#include "type_op.hpp" // for ystdex::remove_cv_t;
#include <exception>
#include <stdexcept>
#include <memory> // for std::addressof;

namespace ystdex
{

/*!
\brief 迭代处理异常。
\since build 538
*/
template<typename _fCallable>
void
iterate_exceptions(_fCallable&& f,
	std::exception_ptr p = std::current_exception())
{
	while(p)
		p = yforward(f)(p);
}

/*!
\brief 取嵌套异常指针。
\return 若符合 \c std::rethrow_if_nested 抛出异常的条件则返回异常指针，否则为空。
\since build 538
*/
template<class _tEx>
inline std::exception_ptr
get_nested_exception_ptr(const _tEx& e)
{
	if(const auto p_cast
		= dynamic_cast<const std::nested_exception*>(std::addressof(e)))
		return p_cast->nested_ptr();
	return {};
}

/*!
\brief 引起异常：若当前存在正在处理的异常则抛出嵌套异常，否则抛出异常。
\since build 522
*/
template<typename _type>
YB_NORETURN inline void
raise_exception(const _type& e)
{
	if(std::current_exception())
		std::throw_with_nested(e);
	throw e;
}


/*!
\brief 处理嵌套异常。
\since build 539
*/
template<typename _tEx, typename _fCallable,
	typename _tExCaught = remove_cv_t<_tEx>&>
void
handle_nested(_tEx& e, _fCallable&& f)
{
	try
	{
		std::rethrow_if_nested(e);
	}
	catch(_tExCaught e)
	{
		yforward(f)(e);
	}
}

} // namespace ystdex;

#endif

