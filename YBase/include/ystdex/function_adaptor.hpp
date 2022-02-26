/*
	© 2012, 2016, 2018, 2020-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file function_adaptor.hpp
\ingroup YStandardEx
\brief 函数适配器。
\version r5130
\author FrankHB <frankhb1989@gmail.com>
\since build 939
\par 创建时间:
	2022-02-14 06:37:03 +0800
\par 修改时间:
	2022-02-15 18:04 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::FunctionAdaptor
*/


#ifndef YB_INC_ystdex_function_adaptor_hpp_
#define YB_INC_ystdex_function_adaptor_hpp_ 1

#include "bind.hpp" // for "bind.hpp", <functional>, std::move, std::bind;
#include "swap.hpp" // for internal "swap.hpp", std::allocator_arg,
//	trivial_swap_t, ystdex::swap_dependent, is_nothrow_swappable,
//	exclude_self_t, is_bitwise_swappable, and_, trivial_swap;
#include "compose.hpp" // for "compose.hpp";

namespace ystdex
{

/*!
\brief 更新间接调用对象。
\note 可配合跳板实现异步调用。
*/
//@{
//! \since build 881
template<class _tThunk, typename _func>
inline void
update_thunk(_tThunk& thunk, _func&& f)
{
	// TODO: Blocked. Use C++14 lambda initializers to simplify the
	//	implementation.
	thunk = std::bind([&thunk](_tThunk& tnk, const _func& cur){
		thunk = std::move(tnk);
		cur();
	}, std::move(thunk), yforward(f));
}
//! \since build 928
//@{
template<class _tThunk, typename _func>
inline void
update_thunk(_tThunk& thunk, trivial_swap_t, _func&& f)
{
	// TODO: Blocked. Use C++14 lambda initializers to simplify the
	//	implementation.
	thunk = _tThunk(trivial_swap,
		std::bind([&thunk](_tThunk& tnk, const _func& cur){
		thunk = std::move(tnk);
		cur();
	}, std::move(thunk), yforward(f)));
}
template<class _tThunk, class _tAlloc, typename _func>
inline void
update_thunk(_tThunk& thunk, const _tAlloc& a, _func&& f)
{
	// TODO: Blocked. Use C++14 lambda initializers to simplify the
	//	implementation.
	thunk = _tThunk(std::allocator_arg, a,
		std::bind([&thunk](_tThunk& tnk, const _func& cur){
		thunk = std::move(tnk);
		cur();
	}, std::move(thunk), yforward(f)));
}
template<class _tThunk, class _tAlloc, typename _func>
inline void
update_thunk(_tThunk& thunk, const _tAlloc& a, trivial_swap_t, _func&& f)
{
	// TODO: Blocked. Use C++14 lambda initializers to simplify the
	//	implementation.
	thunk = _tThunk(std::allocator_arg, a, trivial_swap,
		std::bind([&thunk](_tThunk& tnk, const _func& cur){
		thunk = std::move(tnk);
		cur();
	}, std::move(thunk), yforward(f)));
}
//@}
//@}


/*!
\ingroup functors
\brief 调用一次的函数包装模板。
\pre 静态断言：函数对象和结果转移以及默认状态构造和状态交换不抛出异常。
\since build 686
\todo 优化 std::function 等可空类型的实现。
\todo 复用静态断言。
\todo 简化转移实现。
*/
//@{
template<typename _func, typename _tRes = void, typename _tState = bool>
struct one_shot
{
	static_assert(is_nothrow_move_constructible<_func>(),
		"Invalid target type found.");
	static_assert(is_nothrow_move_constructible<_tRes>(),
		"Invalid result type found.");
	static_assert(is_nothrow_swappable<_tState>(),
		"Invalid state type found.");

	_func func;
	mutable _tRes result;
	mutable _tState fresh{};

	//! \since build 820
	template<typename _type, typename _type2 = _tState,
		yimpl(typename = exclude_self_t<one_shot, _type>)>
	one_shot(_type&& f, _tRes r = {}, _type2&& s = {})
		: func(yforward(f)), result(r), fresh(yforward(s))
	{}
	one_shot(one_shot&& f) ynothrow
		: func(std::move(f.func)), result(std::move(f.result))
	{
		ystdex::swap_dependent(fresh, f.fresh);
	}

	//! \since build 818
	one_shot&
	operator=(one_shot&& f) ynothrow
	{
		ystdex::swap_dependent(*this, f);
		return *this;
	}

	template<typename... _tParams>
	yconstfn_relaxed auto
	operator()(_tParams&&... args) const
		ynoexcept(noexcept(func(yforward(args)...)))
		-> decltype(func(yforward(args)...))
	{
		if(fresh)
		{
			result = func(yforward(args)...);
			fresh = {};
		}
		return std::forward<_tRes>(result);
	}
};

template<typename _func, typename _tState>
struct one_shot<_func, void, _tState>
{
	static_assert(is_nothrow_move_constructible<_func>(),
		"Invalid target type found.");
	static_assert(is_nothrow_swappable<_tState>(),
		"Invalid state type found.");

	_func func;
	mutable _tState fresh{};

	//! \since build 820
	template<typename _type, typename _type2 = _tState,
		yimpl(typename = exclude_self_t<one_shot, _type>)>
	one_shot(_type&& f, _type2&& s = {})
		: func(yforward(f)), fresh(yforward(s))
	{}
	one_shot(one_shot&& f) ynothrow
		: func(std::move(f.func))
	{
		ystdex::swap_dependent(fresh, f.fresh);
	}

	//! \since build 818
	one_shot&
	operator=(one_shot&& f) ynothrow
	{
		ystdex::swap_dependent(*this, f);
		return *this;
	}

	template<typename... _tParams>
	yconstfn_relaxed void
	operator()(_tParams&&... args) const
		ynoexcept(noexcept(func(yforward(args)...)))
	{
		if(fresh)
		{
			func(yforward(args)...);
			fresh = {};
		}
	}
};

template<typename _func, typename _tRes>
struct one_shot<_func, _tRes, void>
{
	static_assert(is_nothrow_move_constructible<_func>(),
		"Invalid target type found.");
	static_assert(is_nothrow_move_constructible<_tRes>(),
		"Invalid result type found.");

	mutable _func func;
	mutable _tRes result;

	one_shot(_func f, _tRes r = {})
		: func(f), result(r)
	{}

	template<typename... _tParams>
	yconstfn_relaxed auto
	operator()(_tParams&&... args) const
		ynoexcept(noexcept(func(yforward(args)...)))
		-> decltype(func(yforward(args)...) && noexcept(func = {}))
	{
		if(func)
		{
			result = func(yforward(args)...);
			func = {};
		}
		return std::forward<_tRes>(result);
	}
};

template<typename _func>
struct one_shot<_func, void, void>
{
	static_assert(is_nothrow_move_constructible<_func>(),
		"Invalid target type found.");

	mutable _func func;

	one_shot(_func f)
		: func(f)
	{}

	template<typename... _tParams>
	yconstfn_relaxed void
	operator()(_tParams&&... args) const
		ynoexcept(noexcept(func(yforward(args)...) && noexcept(func = {})))
	{
		if(func)
		{
			func(yforward(args)...);
			func = {};
		}
	}
};
//@}

/*!
\relates one_shot
\since build 926
*/
//@{
template<typename _func, typename _tRes, typename _tState>
struct is_bitwise_swappable<one_shot<_func, _tRes, _tState>>
	: and_<is_bitwise_swappable<_func>, is_bitwise_swappable<_tRes>,
		is_bitwise_swappable<_tState>>
{};

template<typename _func, typename _tState>
struct is_bitwise_swappable<one_shot<_func, void, _tState>>
	: and_<is_bitwise_swappable<_func>, is_bitwise_swappable<_tState>>
{};

template<typename _func, typename _tRes>
struct is_bitwise_swappable<one_shot<_func, _tRes, void>>
	: and_<is_bitwise_swappable<_func>, is_bitwise_swappable<_tRes>>
{};
//@}

} // namespace ystdex;

#endif

