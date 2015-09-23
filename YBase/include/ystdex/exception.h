/*
	© 2014-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file exception.h
\ingroup YStandardEx
\brief 标准库异常扩展接口。
\version r238
\author FrankHB <frankhb1989@gmail.com>
\since build 522
\par 创建时间:
	2014-07-25 20:14:51 +0800
\par 修改时间:
	2015-09-23 13:15 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Exception
*/


#ifndef YB_INC_ystdex_exception_hpp_
#define YB_INC_ystdex_exception_hpp_ 1

#include "type_op.hpp" // for remove_cv_t;
#include <libdefect/exception.h>
#include <stdexcept> // for std::logic_error;
#include <memory> // for std::addressof;
#include <system_error> // for std::error_category, std::generic_category,
//	std::system_error, std::errc;

namespace ystdex
{

/*!	\defgroup exceptions Exceptions
\brief 异常类型。
\since build 590
*/


/*!
\brief 迭代处理异常。
\since build 538
*/
template<typename _func>
void
iterate_exceptions(_func&& f, std::exception_ptr p = std::current_exception())
{
	while(p)
		p = yforward(f)(p);
}

/*!
\brief 取嵌套异常指针。
\return 若符合 std::rethrow_if_nested 抛出异常的条件则返回异常指针，否则为空。
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
\brief 引发异常：若当前存在正在处理的异常则抛出嵌套异常，否则抛出异常。
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
template<typename _tEx, typename _func,
	typename _tExCaught = remove_cv_t<_tEx>&>
void
handle_nested(_tEx& e, _func&& f)
{
	try
	{
		std::rethrow_if_nested(e);
	}
	catch(_tExCaught ex)
	{
		yforward(f)(ex);
	}
}


/*!
\since build 475
\ingroup exceptions
*/
//@{
//! \brief 异常：不支持的操作。
class YB_API unsupported : public std::logic_error
{
public:
	unsupported()
		: logic_error("Unsupported operation found.")
	{}
	//! \since build 558
	using logic_error::logic_error;
	//! \since build 586
	unsupported(const unsupported&) = default;
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 556
	*/
	~unsupported() override;
};


//! \brief 异常：未实现的操作。
class YB_API unimplemented : public unsupported
{
public:
	unimplemented()
		: unsupported("Unimplemented operation found.")
	{}
	//! \since build 558
	using unsupported::unsupported;
	//! \since build 586
	unimplemented(const unimplemented&) = default;
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 556
	*/
	~unimplemented() override;
};
//@}


/*!
\brief 抛出错误： std::system_error 或允许相同构造函数参数的异常。
\throw _type 使用参数构造的异常。
\since build 637
*/
//@{
template<class _type = std::system_error, typename... _tParams>
YB_NORETURN inline void
throw_error(std::error_code ec, _tParams&&... args)
{
	throw _type(ec, yforward(args)...);
}
template<class _type = std::system_error, typename... _tParams>
YB_NORETURN inline void
throw_error(std::error_condition cond, _tParams&&... args)
{
	throw _type(cond.value(), cond.category(), yforward(args)...);
}
template<class _type = std::system_error, typename... _tParams>
YB_NORETURN inline void
throw_error(int ev, _tParams&&... args)
{
	throw _type(ev, std::generic_category(), yforward(args)...);
}
template<class _type = std::system_error, typename... _tParams>
YB_NORETURN inline void
throw_error(int ev, const std::error_category& ecat, _tParams&&... args)
{
	throw _type(ev, ecat, yforward(args)...);
}
//@}

} // namespace ystdex;

#endif

