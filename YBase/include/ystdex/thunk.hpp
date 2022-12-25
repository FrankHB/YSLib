/*
	© 2014-2016, 2018-2019, 2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file thunk.hpp
\ingroup YStandardEx
\brief 间接和惰性求值。
\version r240
\author FrankHB <frankhb1989@gmail.com>
\since build 588
\par 创建时间:
	2015-03-28 22:32:13 +0800
\par 修改时间:
	2022-11-28 19:55 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Thunk
*/


#ifndef YB_INC_ystdex_thunk_hpp_
#define YB_INC_ystdex_thunk_hpp_ 1

#include "ref.hpp" // for unwrap_reference_t, decay_t, std::forward,
//	is_reference_wrapper, enable_if_t, exclude_self_t, is_same,
//	remove_reference_t, std::move, enable_if_convertible_t, ystdex::invoke,
//	invoke_result_t;
#include "function.hpp" // for function;

namespace ystdex
{

namespace details
{

//! \since build 526
template<typename _type>
struct thunk_call_proxy
{
	_type arg;

	_type
	operator()() const
	{
		return arg;
	}
};

//! \since build 526
template<typename _tRet, typename _fCallable>
struct thunk_caller
{
	//! \since build 529
	static_assert(is_decayed<_fCallable>(), "Invalid type found.");

	//! \since build 541
	using caller_type = _fCallable;
	using return_type = _tRet;
	using value_type = decay_t<unwrap_reference_t<_tRet>>;

	//! \since build 541
	caller_type caller;

	thunk_caller(caller_type f)
		: caller(f)
	{}
	//! \todo 使用 ISO C++14 通用 lambda 表达式以支持转移构造，避免不必要的复制。
	thunk_caller(const value_type& arg)
		: caller([arg]{
			return std::forward<return_type>(arg);
		})
	{}
	//! \since build 675
	template<typename _type, yimpl(typename = exclude_self_t<thunk_caller,
		_type>, typename = enable_if_t<is_reference_wrapper<_type>::value
		&& is_same<remove_reference_t<unwrap_reference_t<_type>>,
		value_type>::value>)>
	thunk_caller(_type arg)
		: caller([arg]() ynoexcept(std::is_nothrow_constructible<return_type,
			_type>::value) -> return_type{
			return arg;
		})
	{}
	thunk_caller(const thunk_caller&) = default;
	thunk_caller(thunk_caller&&) = default;

	thunk_caller&
	operator=(const thunk_caller&) = default;
	thunk_caller&
	operator=(thunk_caller&&) = default;
};

} // namespace details;


/*!
\brief 包装惰性求值的过程。
\see http://c2.com/cgi/wiki?ProcedureWithNoArguments 。
\since build 732
*/
template<typename _tRet, typename _fCallable
	= function<remove_reference_t<unwrap_reference_t<_tRet>>()>>
class thunk : private details::thunk_caller<_tRet, decay_t<_fCallable>>
{
private:
	using base = details::thunk_caller<_tRet, decay_t<_fCallable>>;

public:
	//! \since build 541
	using typename base::caller_type;
	using typename base::return_type;
	using typename base::value_type;

	using base::caller;

	thunk(const value_type& arg)
		: base(arg)
	{}
	//! \since build 554
	template<typename _type>
	thunk(lref<_type> arg)
		: base(lref<value_type>(arg.get()))
	{}
	//! \since build 527
	template<typename _fCaller, yimpl(
		typename = exclude_self_t<thunk, _fCaller>, typename
		= enable_if_convertible_t<_fCaller&&, caller_type>)>
	thunk(_fCaller&& f)
		: base(std::move(caller_type(f)))
	{}
	thunk(const thunk&) = default;
	thunk(thunk&&) = default;

	thunk&
	operator=(const thunk&) = default;
	thunk&
	operator=(thunk&&) = default;

	//! \since build 526
	return_type
	operator()() const
	{
		return ystdex::invoke(caller);
	}

	//! \since build 526
	operator return_type() const
	{
		return operator();
	}
};

/*!
\brief 构造延迟调用对象。
\relates thunk
\since build 832
*/
//@{
template<typename _fCallable>
thunk<invoke_result_t<_fCallable>, decay_t<_fCallable>>
make_thunk(_fCallable&& f)
{
	return thunk<invoke_result_t<_fCallable>, decay_t<_fCallable>>(yforward(f));
}
//! \todo 使用 ISO C++14 返回值推导，直接以 lambda 表达式实现。
template<typename _type>
yimpl(enable_if_t<sizeof(invoke_result_t<_type>) != 0,
	details::thunk_call_proxy<_type>>)
make_thunk(const _type& obj)
{
	return ystdex::make_thunk(details::thunk_call_proxy<_type>{obj});
}
//@}

} // namespace ystdex;

#endif

