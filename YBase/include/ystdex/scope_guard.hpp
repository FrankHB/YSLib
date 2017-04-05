/*
	© 2015-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file scope_guard.hpp
\ingroup YStandardEx
\brief 作用域守护。
\version r507
\author FrankHB <frankhb1989@gmail.com>
\since build 588
\par 创建时间:
	2015-03-29 00:54:19 +0800
\par 修改时间:
	2016-04-05 14:57 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::ScopeGuard

提供作用域守护的便利接口。
*/


#ifndef YB_INC_ystdex_scope_guard_hpp_
#define YB_INC_ystdex_scope_guard_hpp_ 1

#include "utility.hpp" // for is_constructible, is_reference,
//	is_nothrow_swappable, ystdex::vswap, std::declval, is_nothrow_copyable;
#include "base.h" // for noncopyable;
#include "placement.hpp" // for tagged_value;

namespace ystdex
{

/*!
\brief 作用域守护：析构时调用保存的函数对象或引用。
\note 不可复制，不提供其它状态。
\since build 605
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
//@{
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
//@}

//! \since build 779
//@{
template<typename _tState = bool, bool _bNoThrow = true, typename _func>
guard<one_shot<_func, void, _tState>>
unique_guard(_func f, _tState s = {}) ynoexcept_spec(
	guard<one_shot<_func, void, _tState>, _bNoThrow>(
	guard<one_shot<_func, void, _tState>, _bNoThrow>(f)))
{
	using guarded_type = one_shot<_func, void, _tState>;

	return guard<guarded_type, _bNoThrow>(f, s);
}

template<typename _tRes, typename _tState = bool, bool _bNoThrow = true,
	typename _func>
guard<one_shot<_func, _tRes, _tState>>
unique_state_guard(_func f, _tRes r = {}, _tState s = {}) ynoexcept_spec(
	guard<one_shot<_func, _tRes, _tState>, _bNoThrow>(
	guard<one_shot<_func, _tRes, _tState>, _bNoThrow>(f)))
{
	using guarded_type = one_shot<_func, _tRes, _tState>;

	return guard<guarded_type, _bNoThrow>(f, r, s);
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


/*!
\brief 解除操作。
\since build 702
*/
//@{
//! \brief 使用成员 \c dismiss 。
template<class _type>
yconstfn auto
dismiss(_type& gd) -> decltype(gd.dismiss())
{
	return gd.dismiss();
}
template<typename _func, typename _tRes, typename _tState>
inline void
dismiss(const one_shot<_func, _tRes, _tState>& gd)
{
	gd.fresh = {};
}
//! \brief 使用 ADL \c dismiss 。
template<typename _func, bool _bNoThrow>
yconstfn auto
dismiss(guard<_func, _bNoThrow>& gd) -> decltype(dismiss(gd.func))
{
	return dismiss(gd.func);
}
//@}


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
	//! \since build 704
	//@{
	static void
	save(_tToken t, _type& val) ynoexcept_spec(ystdex::vswap(val, t))
	{
		ystdex::vswap(val, t);
	}

	static void
	restore(_tToken t, _type& val) ynoexcept_spec(ystdex::vswap(val, t))
	{
		ystdex::vswap(val, t);
	}
	//@}
};


template<typename _type, typename _tToken>
struct state_guard_impl : public tagged_value<_tToken, _type>,
	private state_guard_traits<_type, _tToken>, private noncopyable
{
	//! \since build 718
	//@{
	using base = tagged_value<_tToken, _type>;
	using typename base::token_type;
	using typename base::value_type;

	using base::token;
	using base::value;
	//@}

	state_guard_impl(token_type t)
		: base(t)
	{}

	//! \since build 586
	template<typename... _tParams>
	void
	construct_and_save(_tParams&&... args)
		ynoexcept(noexcept(value_type(yforward(args)...))
		&& noexcept(std::declval<state_guard_impl&>().save()))
	{
		base::construct(yforward(args)...);
		save();
	}

	//! \since build 718
	using base::destroy;

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
\warning 非虚析构。
\todo 支持分配器。
\todo 支持有限的复制和转移。
*/
//@{
template<typename _type, typename _tCond = bool,
	typename _tToken = std::function<void(bool, _type&)>>
class state_guard : private details::state_guard_impl<_type, _tToken>
{
	static_assert(is_nothrow_copy_constructible<_tCond>(),
		"Invalid condition type found.");

private:
	using base = details::state_guard_impl<_type, _tToken>;

public:
	using typename base::value_type;
	using typename base::token_type;
	using condition_type = _tCond;

	using base::token;
	using base::value;

private:
	//! \since build 692
	condition_type enabled{};

public:
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
	//@}

	//! \since build 630
	void
	dismiss() ynoexcept(is_nothrow_copyable<condition_type>())
	{
		if(enabled)
			base::destroy();
		enabled = condition_type();
	}

	//! \since build 718
	condition_type
	has_value() const ynothrow
	{
		return base::engaged;
	}
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
\brief 使用 ystdex::vswap 调用暂存对象的作用域守护。
\since build 569
\todo 支持分配器。
\todo 支持有限的复制和转移。
*/
template<typename _type, typename _tCond = bool, typename _tReference = _type&>
using swap_guard = state_guard<_type, _tCond, _tReference>;

} // namespace ystdex;

#endif

