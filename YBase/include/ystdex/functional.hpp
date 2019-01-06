/*
	© 2010-2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file functional.hpp
\ingroup YStandardEx
\brief 函数和可调用对象。
\version r3914
\author FrankHB <frankhb1989@gmail.com>
\since build 333
\par 创建时间:
	2010-08-22 13:04:29 +0800
\par 修改时间:
	2019-01-06 13:21 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Functional
*/


#ifndef YB_INC_ystdex_functional_hpp_
#define YB_INC_ystdex_functional_hpp_ 1

#include "functor.hpp" // for "ref.hpp", "invoke.hpp", <functional>,
//	std::function, addressof_op, less, mem_get;
#include "function.hpp" // for "function.hpp", std::tuple, index_sequence,
//	index_sequence_for, is_nothrow_swappable, exclude_self_t, true_, false_,
//	is_void, common_nonvoid_t, make_index_sequence;
#include "swap.hpp" // for "swap.hpp", ystdex::swap_dependent,
//	ystdex::move_and_swap;
#include <numeric> // for std::accumulate;

namespace ystdex
{

/*!
\brief 复合调用 std::bind 和 std::placeholders::_1 。
\note ISO C++ 要求 std::placeholders::_1 被实现支持。
*/
//@{
//! \since build 628
template<typename _func, typename... _tParams>
YB_ATTR_nodiscard YB_PURE inline auto
bind1(_func&& f, _tParams&&... args) -> decltype(
	std::bind(yforward(f), std::placeholders::_1, yforward(args)...))
{
	return std::bind(yforward(f), std::placeholders::_1, yforward(args)...);
}
//! \since build 653
template<typename _tRes, typename _func, typename... _tParams>
YB_ATTR_nodiscard YB_PURE inline auto
bind1(_func&& f, _tParams&&... args) -> decltype(
	std::bind<_tRes>(yforward(f), std::placeholders::_1, yforward(args)...))
{
	return
		std::bind<_tRes>(yforward(f), std::placeholders::_1, yforward(args)...);
}
//@}

/*!
\brief 复合调用 ystdex::bind1 和 std::placeholders::_2 以实现值的设置。
\note 从右到左逐个应用参数。
\note ISO C++ 要求 std::placeholders::_2 被实现支持。
\since build 651
*/
template<typename _func, typename _func2, typename... _tParams>
YB_ATTR_nodiscard YB_PURE inline auto
bind_forward(_func&& f, _func2&& f2, _tParams&&... args)
	-> decltype(ystdex::bind1(yforward(f), std::bind(yforward(f2),
	std::placeholders::_2, yforward(args)...)))
{
	return ystdex::bind1(yforward(f), std::bind(yforward(f2),
		std::placeholders::_2, yforward(args)...));
}


//! \since build 537
//@{
//! \brief 复合函数。
template<typename _func, typename _func2>
struct composed
{
	_func f;
	_func2 g;

	/*!
	\note 每个函数只在函数调用表达式中出现一次。
	\since build 735
	*/
	template<typename... _tParams>
	yconstfn auto
	operator()(_tParams&&... args) const ynoexcept_spec(f(g(yforward(args)...)))
		-> decltype(f(g(yforward(args)...)))
	{
		return f(g(yforward(args)...));
	}
};

/*!
\brief 函数复合。
\note 最后一个参数最先被调用，可以为多元函数；其它被复合的函数需要保证有一个参数。
\relates composed
\return 复合的可调用对象。
*/
//@{
template<typename _func, typename _func2>
YB_ATTR_nodiscard YB_PURE yconstfn composed<_func, _func2>
compose(_func f, _func2 g)
{
	return composed<_func, _func2>{f, g};
}
//! \since build 731
template<typename _func, typename _func2, typename _func3, typename... _funcs>
YB_ATTR_nodiscard YB_PURE yconstfn auto
compose(_func f, _func2 g, _func3 h, _funcs... args)
	-> decltype(ystdex::compose(ystdex::compose(f, g), h, args...))
{
	return ystdex::compose(ystdex::compose(f, g), h, args...);
}
//@}
//@}


//! \since build 735
//@{
//! \brief 多元分发的复合函数。
template<typename _func, typename _func2>
struct composed_n
{
	_func f;
	_func2 g;

	//! \note 第二函数会被分发：多次出现在函数调用表达式中。
	template<typename... _tParams>
	yconstfn auto
	operator()(_tParams&&... args) const ynoexcept_spec(f(g(yforward(args))...))
		-> decltype(f(g(yforward(args))...))
	{
		return f(g(yforward(args))...);
	}
};

/*!
\brief 单一分派的多元函数复合。
\note 第一参数最后被调用，可以为多元函数；其它被复合的函数需要保证有一个参数。
\relates composed_n
\return 单一分派的多元复合的可调用对象。
*/
//@{
//! \since build 740
template<typename _func, typename _func2>
yconstfn composed_n<_func, _func2>
compose_n(_func f, _func2 g)
{
	return composed_n<_func, _func2>{f, g};
}
template<typename _func, typename _func2, typename _func3, typename... _funcs>
yconstfn auto
compose_n(_func f, _func2 g, _func3 h, _funcs... args)
	-> decltype(ystdex::compose_n(ystdex::compose_n(f, g), h, args...))
{
	return ystdex::compose_n(ystdex::compose_n(f, g), h, args...);
}
//@}


//! \brief 多元复合函数。
template<typename _func, typename... _funcs>
struct generalized_composed
{
	_func f;
	std::tuple<_funcs...> g;

	template<typename... _tParams>
	yconstfn auto
	operator()(_tParams&&... args) const ynoexcept_spec(yimpl(call(
		index_sequence_for<_tParams...>(), yforward(args)...))) -> decltype(
		yimpl(call(index_sequence_for<_tParams...>(), yforward(args)...)))
	{
		return call(index_sequence_for<_tParams...>(), yforward(args)...);
	}

private:
	template<size_t... _vSeq, typename... _tParams>
	yconstfn auto
	call(index_sequence<_vSeq...>, _tParams&&... args) const
		ynoexcept_spec(f(std::get<_vSeq>(g)(yforward(args))...))
		-> decltype(f(std::get<_vSeq>(g)(yforward(args))...))
	{
		return f(std::get<_vSeq>(g)(yforward(args))...);
	}
};

/*!
\brief 多元函数复合。
\relates generalized_composed
\return 以多元函数复合的可调用对象。
*/
template<typename _func, typename... _funcs>
yconstfn generalized_composed<_func, std::tuple<_funcs...>>
generalized_compose(_func f, _funcs... args)
{
	return generalized_composed<_func,
		std::tuple<_funcs...>>{f, make_tuple(args...)};
}
//@}


/*!
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
		return ystdex::move_and_swap(*this, std::move(f));
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
		return ystdex::move_and_swap(*this, std::move(f));
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
\ingroup functors
\since build 824
*/
//@{
//! \brief get 成员等于仿函数。
//@{
template<typename _type = void>
struct get_equal_to
	: composed_n<equal_to<_type*>, composed<addressof_op<_type>, mem_get<>>>
{};

template<>
struct get_equal_to<void>
	: composed_n<equal_to<>, composed<addressof_op<>, mem_get<>>>
{};
//@}

//! \brief get 成员小于仿函数。
//@{
template<typename _type = void>
struct get_less
	: composed_n<less<_type*>, composed<addressof_op<_type>, mem_get<>>>
{};

template<>
struct get_less<void>
	: composed_n<less<>, composed<addressof_op<>, mem_get<>>>
{};
//@}
//@}


//! \since build 606
namespace details
{

//! \since build 832
template<typename _type, typename _fCallable, typename... _tParams>
YB_ATTR_nodiscard _type
invoke_for_value(true_, _type&& val, _fCallable&& f, _tParams&&... args)
{
	ystdex::invoke(yforward(f), yforward(args)...);
	return yforward(val);
}

//! \since build 832
template<typename _type, typename _fCallable, typename... _tParams>
YB_ATTR_nodiscard auto
invoke_for_value(false_, _type&&, _fCallable&& f, _tParams&&... args)
	-> invoke_result_t<_fCallable, _tParams...>
{
	return ystdex::invoke(yforward(f), yforward(args)...);
}

} // unnamed namespace;

/*!
\brief 调用第二参数起指定的函数对象，若返回空类型则使用第一参数的值为返回值。
\since build 832
*/
template<typename _type, typename _fCallable, typename... _tParams>
YB_ATTR_nodiscard auto
invoke_for_value(_type&& val, _fCallable&& f, _tParams&&... args)
	-> common_nonvoid_t<invoke_result_t<_fCallable, _tParams...>, _type>
{
	return details::invoke_for_value(is_void<invoke_result_t<_fCallable,
		_tParams...>>(), yforward(val), yforward(f), yforward(args)...);
}


/*!
\brief 调用投影：向原调用传递序列指定的位置的参数。
\since build 447
*/
//@{
template<class, class>
struct call_projection;

template<typename _tRet, typename... _tParams, size_t... _vSeq>
struct call_projection<_tRet(_tParams...), index_sequence<_vSeq...>>
{
	//! \since build 810
	//@{
	template<typename _func>
	static yconstfn auto
	apply_call(_func&& f, std::tuple<_tParams...>&& args, yimpl(decay_t<
		decltype(yforward(f)(std::get<_vSeq>(yforward(args))...))>* = {}))
		-> yimpl(decltype(yforward(f)(std::get<_vSeq>(yforward(args))...)))
	{
		return yforward(f)(std::get<_vSeq>(yforward(args))...);
	}

	template<typename _fCallable>
	static yconstfn auto
	apply_invoke(_fCallable&& f, std::tuple<_tParams...>&& args,
		yimpl(decay_t<decltype(ystdex::invoke(yforward(f),
		std::get<_vSeq>(yforward(args))...))>* = {})) -> yimpl(decltype(
		ystdex::invoke(yforward(f), std::get<_vSeq>(yforward(args))...)))
	{
		return ystdex::invoke(yforward(f), std::get<_vSeq>(yforward(args))...);
	}
	//@}

	template<typename _func>
	static yconstfn auto
	call(_func&& f, _tParams&&... args)
		-> yimpl(decltype(call_projection::apply_call(yforward(f),
		std::forward_as_tuple(yforward(args)...))))
	{
		return call_projection::apply_call(yforward(f),
			std::forward_as_tuple(yforward(args)...));
	}

	//! \since build 810
	template<typename _fCallable>
	static yconstfn auto
	invoke(_fCallable&& f, _tParams&&... args)
		-> yimpl(decltype(call_projection::apply_invoke(yforward(f),
		std::forward_as_tuple(yforward(args)...))))
	{
		return call_projection::apply_invoke(yforward(f),
			std::forward_as_tuple(yforward(args)...));
	}
};

//! \since build 448
template<typename _tRet, typename... _tParams, size_t... _vSeq>
struct call_projection<std::function<_tRet(_tParams...)>,
	index_sequence<_vSeq...>>
	: call_projection<_tRet(_tParams...), index_sequence<_vSeq...>>
{};

//! \since build 849
template<class _tTraits, typename _tRet, typename... _tParams, size_t... _vSeq>
struct call_projection<function_base<_tTraits, _tRet(_tParams...)>,
	index_sequence<_vSeq...>>
	: call_projection<_tRet(_tParams...), index_sequence<_vSeq...>>
{};

/*!
\note 不需要显式指定返回类型。
\since build 547
*/
template<typename... _tParams, size_t... _vSeq>
struct call_projection<std::tuple<_tParams...>, index_sequence<_vSeq...>>
{
	//! \since build 810
	template<typename _func>
	static yconstfn auto
	apply_call(_func&& f, std::tuple<_tParams...>&& args)
		-> yimpl(decltype(yforward(f)(std::get<_vSeq>(yforward(args))...)))
	{
		return yforward(f)(std::get<_vSeq>(yforward(args))...);
	}

	//! \since build 810
	template<typename _fCallable>
	static yconstfn auto
	apply_invoke(_fCallable&& f, std::tuple<_tParams...>&& args) -> yimpl(
		decltype(ystdex::invoke(yforward(f), std::get<_vSeq>(args)...)))
	{
		return ystdex::invoke(yforward(f), std::get<_vSeq>(args)...);
	}

	//! \since build 751
	template<typename _func>
	static yconstfn auto
	call(_func&& f, _tParams&&... args)
		-> decltype(call_projection::apply_call(yforward(f),
		std::forward_as_tuple(yforward(yforward(args))...)))
	{
		return call_projection::apply_call(yforward(f),
			std::forward_as_tuple(yforward(yforward(args))...));
	}

	//! \since build 810
	template<typename _fCallable>
	static yconstfn auto
	invoke(_fCallable&& f, _tParams&&... args)
		-> yimpl(decltype(call_projection::apply_invoke(
		yforward(f), std::forward_as_tuple(yforward(args)...))))
	{
		return call_projection::apply_invoke(yforward(f),
			std::forward_as_tuple(yforward(args)...));
	}
};
//@}


//! \since build 634
//@{
template<typename _fCallable, size_t _vLen = paramlist_size<_fCallable>::value>
struct expand_proxy : private call_projection<_fCallable,
	make_index_sequence<_vLen>>, private expand_proxy<_fCallable, _vLen - 1>
{
	/*!
	\note 为避免歧义，不直接使用 using 声明。
	\see CWG 1393 。
	\see EWG 102 。
	*/
	//@{
	//! \since build 810
	//@{
	using call_projection<_fCallable, make_index_sequence<_vLen>>::apply_call;
	template<typename... _tParams>
	static auto
	apply_call(_tParams&&... args) -> decltype(
		expand_proxy<_fCallable, _vLen - 1>::apply_call(yforward(args)...))
	{
		return expand_proxy<_fCallable, _vLen - 1>::apply_call(
			yforward(args)...);
	}

	using call_projection<_fCallable, make_index_sequence<_vLen>>::apply_invoke;
	template<typename... _tParams>
	static auto
	apply_invoke(_tParams&&... args) -> decltype(
		expand_proxy<_fCallable, _vLen - 1>::apply_invoke(yforward(args)...))
	{
		return expand_proxy<_fCallable, _vLen - 1>::apply_invoke(
			yforward(args)...);
	}
	//@}

	using call_projection<_fCallable, make_index_sequence<_vLen>>::call;
	//! \since build 657
	template<typename... _tParams>
	static auto
	call(_tParams&&... args) -> decltype(
		expand_proxy<_fCallable, _vLen - 1>::call(yforward(args)...))
	{
		return expand_proxy<_fCallable, _vLen - 1>::call(yforward(args)...);
	}

	//! \since build 810
	//@{
	using call_projection<_fCallable, make_index_sequence<_vLen>>::invoke;
	template<typename... _tParams>
	static auto
	invoke(_tParams&&... args) -> decltype(
		expand_proxy<_fCallable, _vLen - 1>::invoke(yforward(args)...))
	{
		return expand_proxy<_fCallable, _vLen - 1>::invoke(yforward(args)...);
	}
	//@}
	//@}
};

template<typename _fCallable>
struct expand_proxy<_fCallable, 0>
	: private call_projection<_fCallable, index_sequence<>>
{
	using call_projection<_fCallable, index_sequence<>>::call;
};
//@}


/*!
\brief 循环重复调用：代替直接使用 do-while 语句以避免过多引入作用域外的变量。
\tparam _fCond 判断条件。
\tparam _fCallable 可调用对象类型。
\tparam _tParams 参数类型。
\note 条件接受调用结果或没有参数。
\since build 832
\sa object_result_t
*/
template<typename _fCond, typename _fCallable, typename... _tParams>
invoke_result_t<_fCallable, _tParams...>
retry_on_cond(_fCond cond, _fCallable&& f, _tParams&&... args)
{
	using res_t = invoke_result_t<_fCallable, _tParams...>;
	using obj_t = object_result_t<res_t>;
	obj_t res;

	do
		res = ystdex::invoke_nonvoid(yforward(f), yforward(args)...);
	while(expand_proxy<bool(obj_t&)>::call(cond, res));
	return res_t(res);
}


/*!
\brief 接受冗余参数的可调用对象。
\since build 447
\todo 支持 const 以外的限定符和 ref-qualifier 。
*/
template<typename _fHandler, typename _fCaller>
struct expanded_caller
{
	//! \since build 448
	static_assert(is_object<_fCaller>(), "Callable object type is needed.");

	//! \since build 525
	_fCaller caller;

	//! \since build 849
	//@{
	expanded_caller() = default;
	template<typename _func,
		yimpl(typename = exclude_self_t<expanded_caller, _fCaller>)>
	expanded_caller(_func f)
		: caller(yforward(f))
	{}
	expanded_caller(const expanded_caller&) = default;
	expanded_caller(expanded_caller&&) = default;

	expanded_caller&
	operator=(const expanded_caller&) = default;
	expanded_caller&
	operator=(expanded_caller&&) = default;
	//@}

	//! \since build 640
	template<typename... _tParams>
	inline auto
	operator()(_tParams&&... args) const -> decltype(
		expand_proxy<_fHandler>::call(caller, yforward(args)...))
	{
		return expand_proxy<_fHandler>::call(caller, yforward(args)...);
	}
};

/*!
\ingroup helper_functions
\brief 构造接受冗余参数的可调用对象。
\relates expanded_caller
\since build 849
*/
template<typename _fHandler, typename _fCallable>
YB_ATTR_nodiscard YB_PURE inline expanded_caller<_fHandler, _fCallable>
make_expanded(_fCallable f)
{
	return expanded_caller<_fHandler, _fCallable>(std::move(f));
}


/*!
\brief 合并值序列。
\note 语义同 Boost.Signal2 的 \c boost::last_value 但对非 \c void 类型使用默认值。
\since build 675
*/
//@{
template<typename _type>
struct default_last_value
{
	template<typename _tIn>
	_type
	operator()(_tIn first, _tIn last, const _type& val = {}) const
	{
		return std::accumulate(first, last, val,
			[](_type&, decltype(*first) v){
			return yforward(v);
		});
	}
};

template<>
struct default_last_value<void>
{
	template<typename _tIn>
	void
	operator()(_tIn first, _tIn last) const
	{
		for(; first != last; ++first)
			*first;
	}
};
//@}

} // namespace ystdex;

#endif

