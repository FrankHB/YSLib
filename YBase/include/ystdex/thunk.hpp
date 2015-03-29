/*
	© 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file thunk.hpp
\ingroup YStandardEx
\brief 间接和惰性求值。
\version r177
\author FrankHB <frankhb1989@gmail.com>
\since build 588
\par 创建时间:
	2015-03-28 22:32:13 +0800
\par 修改时间:
	2015-03-28 22:19 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Thunk
*/


#ifndef YB_INC_ystdex_thunk_hpp_
#define YB_INC_ystdex_thunk_hpp_ 1

#include "type_op.hpp" // for std::forward, std::move, ystdex::result_of_t,
//	ystdex::decay_t, ystdex::enable_if_t;
#include "ref.hpp" // for ystdex::wrapped_traits, std::reference_wrapper;

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
template<typename _tRet, typename _func>
struct thunk_caller
{
	//! \since build 529
	static_assert(is_decayed<_func>::value, "Invalid type found.");

	//! \since build 541
	using caller_type = _func;
	using return_type = _tRet;
	using value_type = decay_t<wrapped_traits_t<_tRet>>;

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
	//! \since build 527
	thunk_caller(std::reference_wrapper<value_type> arg)
		: caller([arg]()->return_type{
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
\since build 526
*/
template<typename _tRet,
	typename _func = std::function<wrapped_traits_t<_tRet>()>>
class thunk : private details::thunk_caller<_tRet, decay_t<_func>>
{
private:
	using base = details::thunk_caller<_tRet, decay_t<_func>>;

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
	template<typename _fCaller, yimpl(typename
		= exclude_self_ctor_t<thunk, _fCaller>, typename
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
		return caller();
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
\since build 526
*/
//@{
template<typename _func>
thunk<result_of_t<_func()>, decay_t<_func>>
make_thunk(_func&& f)
{
	return thunk<result_of_t<_func()>, decay_t<_func>>(yforward(f));
}
//! \todo 使用 ISO C++14 返回值推导，直接以 lambda 表达式实现。
template<typename _type>
yimpl(enable_if_t<sizeof(result_of_t<_type()>) != 0,
	details::thunk_call_proxy<_type>>)
make_thunk(const _type& obj)
{
	return ystdex::make_thunk(details::thunk_call_proxy<_type>{obj});
}
//@}

} // namespace ystdex;

#endif

