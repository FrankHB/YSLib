/*
	© 2014-2018, 2020-2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file exception.h
\ingroup YStandardEx
\brief 标准库异常扩展接口。
\version r339
\author FrankHB <frankhb1989@gmail.com>
\since build 522
\par 创建时间:
	2014-07-25 20:14:51 +0800
\par 修改时间:
	2021-12-21 20:52 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Exception
*/


#ifndef YB_INC_ystdex_exception_h_
#define YB_INC_ystdex_exception_h_ 1

#include <libdefect/exception.h> // for std::exception_ptr,
//	std::nested_exception;
#include "deref_op.hpp" // for call_value_or, remove_cv_t;
#include <functional> // for std::mem_fn;
#include <memory> // for std::addressof;
#include <stdexcept> // for std::logic_error;
#include <system_error> // for std::system_error, std::generic_category,
//	std::error_category;

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
	return
		ystdex::call_value_or(std::mem_fn(&std::nested_exception::nested_ptr),
		dynamic_cast<const std::nested_exception*>(std::addressof(e)));
}

/*!
\brief 引发异常：若当前存在正在处理的异常则抛出嵌套异常，否则抛出异常。
\since build 657
*/
template<typename _type>
YB_NORETURN inline void
raise_exception(_type&& e)
{
	if(std::current_exception())
		std::throw_with_nested(yforward(e));
	throw yforward(e);
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


/*!
\brief 异常：未实现的操作。
\note 和 \c std::system_error 中的错误条件不同，一般不表示依赖外部确定支持条件。
*/
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


/*!
\brief 异常：无法表示数值范围的转换。
\since build 703
*/
class YB_API narrowing_error : public std::logic_error
{
public:
	narrowing_error()
		: logic_error("Norrowing found.")
	{}
	using logic_error::logic_error;
	narrowing_error(const narrowing_error&) = default;
	//! \brief 虚析构：类定义外默认实现。
	~narrowing_error() override;
};
//@}


//! \since build 686
//@{
//! \brief 使用不满足构造限制检查导致的异常。
class YB_API invalid_construction : public std::invalid_argument
{
public:
	invalid_construction();
	//! \since build 689
	invalid_construction(const invalid_construction&) = default;

	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 689
	*/
	~invalid_construction() override;
};

/*!
\brief 抛出 invalid_construction 异常。
\throw invalid_construction
\relates invalid_construction
*/
YB_NORETURN YB_API void
throw_invalid_construction();
//@}


//! \since build 848
//@{
/*!
\brief 不满足分配器兼容性要求导致的异常。
\sa alloc_rebind_t
\see WG21 P0043R0 。

兼容的分配器可重绑定不同的类型进行分配。
分配器一致保证可替换。通常使用相等性检查一致性。
一致性蕴含兼容性，即兼容性是一致性的必要非充分条件。
若分配器类型动态确定（如持有具有目标类型擦除的类型），兼容性不适合使用
	narrow contract 约束。此时适合使用异常代替。
*/
class YB_API allocator_mismatch_error : public std::invalid_argument
{
public:
	allocator_mismatch_error();
	allocator_mismatch_error(const allocator_mismatch_error&) = default;

	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 689
	*/
	~allocator_mismatch_error() override;
};

/*!
\brief 抛出 allocator_mismatch_error 异常。
\throw allocator_mismatch_error
\relates allocator_mismatch_error
*/
YB_NORETURN YB_API void
throw_allocator_mismatch_error();
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

