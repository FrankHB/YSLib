/*
	© 2015-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file scope_guard.hpp
\ingroup YStandardEx
\brief 作用域守护。
\version r392
\author FrankHB <frankhb1989@gmail.com>
\since build 588
\par 创建时间:
	2015-03-29 00:54:19 +0800
\par 修改时间:
	2016-04-20 15:33 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::ScopeGuard

提供作用域守护的便利接口。
*/


#ifndef YB_INC_ystdex_scope_guard_hpp_
#define YB_INC_ystdex_scope_guard_hpp_ 1

#include "type_traits.hpp" // for is_constructible, is_reference,
//	is_nothrow_swappable, std::swap, std::declval, is_nothrow_copyable;
#include <memory> // for std::addressof;

namespace ystdex
{

//! \since build 605
//@{
/*!
\brief 作用域守护：析构时调用保存的函数对象或引用。
\note 不可复制，不提供其它状态。
*/
template<typename _func, bool _bNoThrow = true>
struct guard
{
	_func func;

	//! \since build 606
	template<typename... _tParams>
	guard(_tParams&&... args)
		ynoexcept(is_constructible<_func, _tParams&&...>::value)
		: func(yforward(args)...)
	{}
	guard(guard&&) = default;
	~guard() ynoexcept_spec(!_bNoThrow)
	{
		func();
	}

	guard&
	operator=(guard&&) = default;
};

/*!
\brief 创建作用域守护。
\relates guard
\since build 606
*/
//! \since build 649
template<typename _type, bool _bNoThrow = true, typename... _tParams>
guard<_type>
make_guard(_tParams&&... args) ynoexcept_spec(guard<_type, _bNoThrow>(
	guard<_type, _bNoThrow>(yforward(args)...)))
{
	return guard<_type, _bNoThrow>(yforward(args)...);
}
template<bool _bNoThrow = true, typename _type>
guard<_type>
make_guard(_type f)
	ynoexcept_spec(guard<_type, _bNoThrow>(guard<_type, _bNoThrow>(f)))
{
	return guard<_type, _bNoThrow>(f);
}

//! \since build 686
template<bool _bNoThrow = true, typename _type>
guard<one_shot<_type>>
unique_guard(_type f) ynoexcept_spec(
	guard<one_shot<_type>, _bNoThrow>(guard<one_shot<_type>, _bNoThrow>(f)))
{
	return guard<one_shot<_type>, _bNoThrow>(f, true);
}
//@}


/*!
\brief 创建共享作用域守护。
\since build 676
*/
template<typename _func, typename _type = void>
inline std::shared_ptr<_type>
share_guard(_func f, _type* p = {})
{
	return ystdex::share_raw(p, f);
}


namespace details
{

//! \since build 569
//@{
template<typename _type, typename _tToken,
	bool _bRef = is_reference<_tToken>::value>
struct state_guard_traits
{
	//! \since build 586
	//@{
	static void
	save(_tToken t, _type& val) ynoexcept_spec(t(true, val))
	{
		t(true, val);
	}

	static void
	restore(_tToken t, _type& val) ynoexcept_spec(t(false, val))
	{
		t(false, val);
	}
	//@}
};

template<typename _type, typename _tToken>
struct state_guard_traits<_type, _tToken, true>
{
	//! \since build 586
	//@{
	static void
	save(_tToken t, _type& val) ynoexcept(is_nothrow_swappable<_type>())
	{
		using std::swap;

		swap(val, static_cast<_type&>(t));
	}

	static void
	restore(_tToken t, _type& val) ynoexcept(is_nothrow_swappable<_type>())
	{
		using std::swap;

		swap(val, static_cast<_type&>(t));
	}
	//@}
};


template<typename _type, typename _tToken>
struct state_guard_impl : private state_guard_traits<_type, _tToken>
{
	using value_type = _type;
	using token_type = _tToken;

	token_type token;
	union
	{
		value_type value;
		byte data[sizeof(value_type)];
	};

	state_guard_impl(token_type t)
		: token(t)
	{}
	state_guard_impl(const state_guard_impl&) = delete;
	~state_guard_impl()
	{}

	//! \since build 586
	template<typename... _tParams>
	void
	construct_and_save(_tParams&&... args)
		ynoexcept(noexcept(value_type(yforward(args)...))
		&& noexcept(std::declval<state_guard_impl&>().save()))
	{
		new(std::addressof(value)) value_type(yforward(args)...);
		save();
	}

	//! \since build 586
	void
	destroy() ynoexcept(is_nothrow_destructible<value_type>())
	{
		value.~value_type();
	}

	void
	save() ynoexcept(noexcept(state_guard_traits<value_type, token_type>
		::save(std::declval<token_type&>(), std::declval<value_type&>())))
	{
		state_guard_traits<value_type, token_type>::save(token, value);
	}

	void
	restore()
		ynoexcept_spec(state_guard_traits<value_type, token_type>::restore(
		std::declval<token_type&>(), std::declval<value_type&>()))
	{
		state_guard_traits<value_type, token_type>::restore(token, value);
	}

	void
	restore_and_destroy()
		ynoexcept_spec(state_guard_traits<value_type, token_type>::restore(
		std::declval<token_type&>(), std::declval<value_type&>()))
	{
		restore();
		destroy();
	}
};
//@}

} // namespace details;

/*!
\brief 使用临时状态暂存对象的作用域守护。
\since build 569
\todo 支持分配器。
\todo 支持有限的复制和转移。
*/
//@{
template<typename _type, typename _tCond = bool,
	typename _tToken = std::function<void(bool, _type&)>>
class state_guard : private details::state_guard_impl<_type, _tToken>
{
private:
	using base = details::state_guard_impl<_type, _tToken>;

public:
	using typename base::value_type;
	using typename base::token_type;
	using condition_type = _tCond;

	using base::token;
	using base::value;
	using base::data;
	mutable condition_type enabled{};

	//! \since build 586
	//@{
	template<typename... _tParams>
	state_guard(condition_type cond, token_type t, _tParams&&... args)
		ynoexcept(and_<is_nothrow_constructible<base, token_type>,
		is_nothrow_copy_constructible<condition_type>>()
		&& noexcept(std::declval<state_guard&>()
		.base::construct_and_save(yforward(args)...)))
		: base(t),
		enabled(cond)
	{
		if(enabled)
			base::construct_and_save(yforward(args)...);
	}
	~state_guard() ynoexcept(is_nothrow_copy_constructible<condition_type>()
		&& noexcept(std::declval<state_guard&>().base::restore_and_destroy()))
	{
		if(enabled)
			base::restore_and_destroy();
	}

	//! \since build 630
	void
	dismiss() ynoexcept(is_nothrow_copyable<condition_type>())
	{
		if(enabled)
			base::destroy();
		enabled = condition_type();
	}
	//@}
};

template<typename _type, typename _tToken>
class state_guard<_type, void, _tToken>
	: private details::state_guard_impl<_type, _tToken>
{
private:
	using base = details::state_guard_impl<_type, _tToken>;

public:
	using typename base::value_type;
	using typename base::token_type;
	using condition_type = void;

	using base::token;
	using base::value;
	using base::data;

	template<typename... _tParams>
	state_guard(token_type t, _tParams&&... args) ynoexcept(
		is_nothrow_constructible<base, token_type>() && noexcept(std::declval<
		state_guard&>().base::construct_and_save(yforward(args)...)))
		: base(t)
	{
		base::construct_and_save(yforward(args)...);
	}
	//! \since build 586
	~state_guard()
		ynoexcept_spec(std::declval<state_guard&>().base::restore_and_destroy())
	{
		base::restore_and_destroy();
	}
};
//@}


/*!
\brief 使用 ADL swap 调用暂存对象的作用域守护。
\since build 569
\todo 支持分配器。
\todo 支持有限的复制和转移。
*/
template<typename _type, typename _tCond = bool, typename _tReference = _type&>
using swap_guard = state_guard<_type, _tCond, _tReference>;

} // namespace ystdex;

#endif

