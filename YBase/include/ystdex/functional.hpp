/*
	© 2010-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file functional.hpp
\ingroup YStandardEx
\brief 函数和可调用对象。
\version r3178
\author FrankHB <frankhb1989@gmail.com>
\since build 333
\par 创建时间:
	2010-08-22 13:04:29 +0800
\par 修改时间:
	2017-04-05 15:08 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Functional
*/


#ifndef YB_INC_ystdex_functional_hpp_
#define YB_INC_ystdex_functional_hpp_ 1

#include "type_op.hpp" // for "tuple.hpp", true_, std::tuple,
//	is_convertible, vseq::at, bool_, index_sequence_for, member_target_type_t,
//	false_, is_void, _t, size_t_, std::tuple_size, vseq::join_n_t, std::swap,
//	common_nonvoid_t, is_nothrow_swappable, make_index_sequence, exclude_self_t;
#include "functor.hpp" // for "ref.hpp", <functional>, std::function,
//	__cpp_lib_invoke, less, addressof_op, mem_get;
#include "cassert.h" // for yconstraint;
#include <numeric> // for std::accumulate;

namespace ystdex
{

//! \since build 447
//@{
namespace details
{

template<class, class, class>
struct tuple_element_convertible;

template<class _type1, class _type2>
struct tuple_element_convertible<_type1, _type2, index_sequence<>> : true_
{};

template<typename... _types1, typename... _types2, size_t... _vSeq,
	size_t _vHead>
struct tuple_element_convertible<std::tuple<_types1...>, std::tuple<_types2...>,
	index_sequence<_vHead, _vSeq...>>
{
	static_assert(sizeof...(_types1) == sizeof...(_types2),
		"Mismatched sizes of tuple found.");

private:
	using t1 = std::tuple<_types1...>;
	using t2 = std::tuple<_types2...>;

public:
	static yconstexpr const bool value
		= is_convertible<vseq::at<t1, _vHead>, vseq::at<t2, _vHead>>::value
		&& tuple_element_convertible<t1, t2, index_sequence<_vSeq...>>::value;
};

} // namespace details;

/*!
\ingroup binary_type_traits
\since build 447
*/
//@{
//! \brief 判断指定类型之间是否协变。
//@{
template<typename _tFrom, typename _tTo>
struct is_covariant : is_convertible<_tFrom, _tTo>
{};

template<typename _tFrom, typename _tTo, typename... _tFromParams,
	typename... _tToParams>
struct is_covariant<_tFrom(_tFromParams...), _tTo(_tToParams...)>
	: is_covariant<_tFrom, _tTo>
{};

template<typename... _tFroms, typename... _tTos>
struct is_covariant<std::tuple<_tFroms...>, std::tuple<_tTos...>>
	: bool_<details::tuple_element_convertible<std::tuple<_tFroms...>,
	std::tuple<_tTos...>, index_sequence_for<_tTos...>>::value>
{};

//! \since build 575
template<typename _tFrom, typename _tTo, typename... _tFromParams,
	typename... _tToParams>
struct is_covariant<std::function<_tFrom(_tFromParams...)>,
	std::function<_tTo(_tToParams...)>>
	: is_covariant<_tFrom(_tFromParams...), _tTo(_tToParams...)>
{};
//@}


//! \brief 判断指定类型之间是否逆变。
//@{
template<typename _tFrom, typename _tTo>
struct is_contravariant : is_convertible<_tTo, _tFrom>
{};

template<typename _tResFrom, typename _tResTo, typename... _tFromParams,
	typename... _tToParams>
struct is_contravariant<_tResFrom(_tFromParams...), _tResTo(_tToParams...)>
	: is_contravariant<std::tuple<_tFromParams...>, std::tuple<_tToParams...>>
{};

template<typename... _tFroms, typename... _tTos>
struct is_contravariant<std::tuple<_tFroms...>, std::tuple<_tTos...>>
	: bool_<details::tuple_element_convertible<std::tuple<_tTos...>,
	std::tuple<_tFroms...>, index_sequence_for<_tTos...>>::value>
{};

//! \since build 575
template<typename _tResFrom, typename _tResTo, typename... _tFromParams,
	typename... _tToParams>
struct is_contravariant<std::function<_tResFrom(_tFromParams...)>,
	std::function<_tResTo(_tToParams...)>>
	: is_contravariant<_tResFrom(_tFromParams...), _tResTo(_tToParams...)>
{};
//@}
//@}


//! \since build 594
//@{
//! \brief 统计函数参数列表中的参数个数。
template<typename... _tParams>
yconstfn size_t
sizeof_params(_tParams&&...) ynothrow
{
	return sizeof...(_tParams);
}


//! \since build 412
//@{
//! \brief 变长参数操作模板。
//@{
template<size_t _vN>
struct variadic_param
{
	//! \since build 594
	template<typename _type, typename... _tParams>
	static yconstfn auto
	get(_type&&, _tParams&&... args) ynothrow
		-> decltype(variadic_param<_vN - 1>::get(yforward(args)...))
	{
		static_assert(sizeof...(args) == _vN,
			"Wrong variadic arguments number found.");

		return variadic_param<_vN - 1>::get(yforward(args)...);
	}
};

template<>
struct variadic_param<0U>
{
	//! \since build 594
	template<typename _type>
	static yconstfn auto
	get(_type&& arg) ynothrow -> decltype(yforward(arg))
	{
		return yforward(arg);
	}
};
//@}


/*!
\brief 取指定位置的变长参数。
\tparam _vN 表示参数位置的非负数，从左开始计数，第一个参数为 0 。
*/
template<size_t _vN, typename... _tParams>
yconstfn auto
varg(_tParams&&... args) ynothrow
	-> decltype(variadic_param<_vN>::get(yforward(args)...))
{
	static_assert(_vN < sizeof...(args),
		"Out-of-range index of variadic argument found.");

	return variadic_param<_vN>::get(yforward(args)...);
}
//@}


//! \see 关于调用参数类型： ISO C++11 30.3.1.2 [thread.thread.constr] 。
//@{
//! \brief 顺序链式调用。
//@{
template<typename _func>
inline void
chain_apply(_func&& f) ynothrow
{
	return yforward(f);
}
template<typename _func, typename _type, typename... _tParams>
inline void
chain_apply(_func&& f, _type&& arg, _tParams&&... args)
	ynoexcept_spec(ystdex::chain_apply(
	yforward(yforward(f)(yforward(arg))), yforward(args)...))
{
	return ystdex::chain_apply(yforward(yforward(f)(yforward(arg))),
		yforward(args)...);
}
//@}

//! \brief 顺序递归调用。
//@{
template<typename _func>
inline void
seq_apply(_func&&) ynothrow
{}
//! \since build 595
template<typename _func, typename _type, typename... _tParams>
inline void
seq_apply(_func&& f, _type&& arg, _tParams&&... args)
	ynoexcept_spec(yimpl(yunseq(0, (void(yforward(f)(yforward(args))), 0)...)))
{
	yforward(f)(yforward(arg));
	ystdex::seq_apply(yforward(f), yforward(args)...);
}
//@}

//! \brief 无序调用。
template<typename _func, typename... _tParams>
inline void
unseq_apply(_func&& f, _tParams&&... args)
	ynoexcept_spec(yimpl(yunseq((void(yforward(f)(yforward(args))), 0)...)))
{
	yunseq((void(yforward(f)(yforward(args))), 0)...);
}
//@}
//@}

// TODO: Blocked. Wait for upcoming ISO C++17 for %__cplusplus.
#if __cpp_lib_invoke >= 201411
//! \since build 617
using std::invoke;
#else
//! \since build 612
namespace details
{

template<typename _type, typename _tCallable>
struct is_callable_target
	: is_base_of<member_target_type_t<_tCallable>, decay_t<_type>>
{};

template<typename _fCallable, typename _type>
struct is_callable_case1 : and_<is_member_function_pointer<_fCallable>,
	is_callable_target<_type, _fCallable>>
{};

template<typename _fCallable, typename _type>
struct is_callable_case2 : and_<is_member_function_pointer<_fCallable>,
	not_<is_callable_target<_type, _fCallable>>>
{};

template<typename _fCallable, typename _type>
struct is_callable_case3 : and_<is_member_object_pointer<_fCallable>,
	is_callable_target<_type, _fCallable>>
{};

template<typename _fCallable, typename _type>
struct is_callable_case4 : and_<is_member_object_pointer<_fCallable>,
	not_<is_callable_target<_type, _fCallable>>>
{};

//! \since build 763
template<typename _fCallable, typename _type, typename... _tParams>
yconstfn auto
invoke_impl(_fCallable&& f, _type&& obj, _tParams&&... args)
	-> enable_if_t<is_callable_case1<decay_t<_fCallable>, _type>::value,
	decltype((yforward(obj).*f)(yforward(args)...))>
{
	return yconstraint(f), (yforward(obj).*f)(yforward(args)...);
}
template<typename _fCallable, typename _type, typename... _tParams>
yconstfn auto
invoke_impl(_fCallable&& f, _type&& obj, _tParams&&... args)
	-> enable_if_t<is_callable_case2<decay_t<_fCallable>, _type>::value,
	decltype(((*yforward(obj)).*f)(yforward(args)...))>
{
	return yconstraint(f), ((*yforward(obj)).*f)(yforward(args)...);
}
//! \since build 763
template<typename _fCallable, typename _type>
yconstfn auto
invoke_impl(_fCallable&& f, _type&& obj)
	-> enable_if_t<is_callable_case3<decay_t<_fCallable>, _type>::value,
	decltype(yforward(obj).*f)>
{
	return yconstraint(f), yforward(obj).*f;
}
template<typename _fCallable, typename _type>
yconstfn auto
invoke_impl(_fCallable&& f, _type&& obj)
	-> enable_if_t<is_callable_case4<decay_t<_fCallable>, _type>::value,
	decltype((*yforward(obj)).*f)>
{
	return yconstraint(f), (*yforward(obj)).*f;
}
template<typename _func, typename... _tParams>
yconstfn auto
invoke_impl(_func&& f, _tParams&&... args)
	-> enable_if_t<!is_member_pointer<decay_t<_func>>::value,
	decltype(yforward(f)(yforward(args)...))>
{
	return yforward(f)(yforward(args)...);
}

} // namespace details;

/*!
\brief 调用可调用对象。
\sa http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4169.html
\see WG21 N4527 20.9.2[func.require] ， WG21 N4527 20.9.3[func.invoke] 。
\see LWG 2013 。
\see CWG 1581 。
\see https://llvm.org/bugs/show_bug.cgi?id=23141 。
\since build 612
\todo 支持引用包装。
*/
template<typename _fCallable, typename... _tParams>
yimpl(yconstfn) result_of_t<_fCallable&&(_tParams&&...)>
invoke(_fCallable&& f, _tParams&&... args)
{
	return details::invoke_impl(yforward(f), yforward(args)...);
}
#endif

namespace details
{

//! \since build 729
template<typename _fCallable, typename... _tParams>
yconstfn pseudo_output
invoke_nonvoid_impl(true_, _fCallable&& f, _tParams&&... args)
{
	return ystdex::invoke(yforward(f), yforward(args)...), pseudo_output();
}
//! \since build 729
template<typename _fCallable, typename... _tParams>
inline result_of_t<_fCallable&&(_tParams&&...)>
invoke_nonvoid_impl(false_, _fCallable&& f, _tParams&&... args)
{
	return ystdex::invoke(yforward(f), yforward(args)...);
}

} // namespace details;

/*!
\brief 调用可调用对象，保证返回值非空。
\since build 635
\todo 支持引用包装。
*/
template<typename _fCallable, typename... _tParams>
yimpl(yconstfn) nonvoid_result_t<result_of_t<_fCallable&&(_tParams&&...)>>
invoke_nonvoid(_fCallable&& f, _tParams&&... args)
{
	return details::invoke_nonvoid_impl(is_void<result_of_t<
		_fCallable&&(_tParams&&...)>>(), yforward(f), yforward(args)...);
}


/*!
\brief 使用 invoke 调用非空值或取默认值。
\sa ystdex::call_value_or
\sa ystdex::invoke
\since build 767
*/
//@{
template<typename _type, typename _func>
yconstfn auto
invoke_value_or(_func f, _type&& p)
	-> decay_t<decltype(ystdex::invoke(f, *yforward(p)))>
{
	return p ? ystdex::invoke(f, *yforward(p))
		: decay_t<decltype(ystdex::invoke(f, *yforward(p)))>();
}
template<typename _tOther, typename _type, typename _func>
yconstfn auto
invoke_value_or(_func f, _type&& p, _tOther&& other)
	-> yimpl(decltype(p ? ystdex::invoke(f, *yforward(p)) : yforward(other)))
{
	return p ? ystdex::invoke(f, *yforward(p)) : yforward(other);
}
template<typename _tOther, typename _type, typename _func,
	typename _tSentinal = nullptr_t>
yconstfn auto
invoke_value_or(_func f, _type&& p, _tOther&& other, _tSentinal&& last)
	-> yimpl(decltype(!bool(p == yforward(last))
	? ystdex::invoke(f, *yforward(p)) : yforward(other)))
{
	return !bool(p == yforward(last)) ? ystdex::invoke(f, *yforward(p))
		: yforward(other);
}
//@}


/*!
\ingroup metafunctions
\brief 取参数列表元组。
\since build 333
*/
//@{
template<typename>
struct make_parameter_tuple;

//! \since build 447
template<typename _fCallable>
using make_parameter_tuple_t = _t<make_parameter_tuple<_fCallable>>;

//! \since build 675
template<typename _fCallable>
struct make_parameter_tuple<_fCallable&> : make_parameter_tuple<_fCallable>
{};

//! \since build 675
template<typename _fCallable>
struct make_parameter_tuple<_fCallable&&> : make_parameter_tuple<_fCallable>
{};

#define YB_Impl_Functional_ptuple_spec(_exp, _p, _q) \
	template<typename _tRet, _exp typename... _tParams> \
	struct make_parameter_tuple<_tRet _p (_tParams...) _q> \
	{ \
		using type = std::tuple<_tParams...>; \
	};

YB_Impl_Functional_ptuple_spec(, , )
YB_Impl_Functional_ptuple_spec(, (*), )

#define YB_Impl_Functional_ptuple_spec_mf(_q) \
	YB_Impl_Functional_ptuple_spec(class _tClass YPP_Comma, (_tClass::*), _q)

YB_Impl_Functional_ptuple_spec_mf()
//! \since build 358
//@{
YB_Impl_Functional_ptuple_spec_mf(const)
YB_Impl_Functional_ptuple_spec_mf(volatile)
YB_Impl_Functional_ptuple_spec_mf(const volatile)
//@}

#undef YB_Impl_Functional_ptuple_spec_mf

#undef YB_Impl_Functional_ptuple_spec

//! \since build 447
template<typename _tRet, typename... _tParams>
struct make_parameter_tuple<std::function<_tRet(_tParams...)>>
{
	using type = std::tuple<_tParams...>;
};
//@}


/*!
\ingroup metafunctions
\brief 取返回类型。
\since build 333
*/
//@{
template<typename>
struct return_of;

//! \since build 447
template<typename _fCallable>
using return_of_t = _t<return_of<_fCallable>>;

//! \since build 675
template<typename _fCallable>
struct return_of<_fCallable&> : return_of<_fCallable>
{};

//! \since build 675
template<typename _fCallable>
struct return_of<_fCallable&&> : return_of<_fCallable>
{};

#define YB_Impl_Functional_ret_spec(_exp, _p, _e, _q) \
	template<typename _tRet, _exp typename... _tParams> \
	struct return_of<_tRet _p (_tParams... YPP_Args _e) _q> \
	{ \
		using type = _tRet; \
	};

#define YB_Impl_Functional_ret_spec_f(_e) \
	YB_Impl_Functional_ret_spec(, , _e, ) \
	YB_Impl_Functional_ret_spec(, (*), _e, )

YB_Impl_Functional_ret_spec_f()
//! \since build 675
YB_Impl_Functional_ret_spec_f((, ...))

#undef YB_Impl_Functional_ret_spec_f

#define YB_Impl_Functional_ret_spec_mf(_e, _q) \
	YB_Impl_Functional_ret_spec(class _tClass YPP_Comma, (_tClass::*), \
		_e, _q)

#define YB_Impl_Functional_ret_spec_mfq(_e) \
	YB_Impl_Functional_ret_spec_mf(_e, ) \
	YB_Impl_Functional_ret_spec_mf(_e, const) \
	YB_Impl_Functional_ret_spec_mf(_e, volatile) \
	YB_Impl_Functional_ret_spec_mf(_e, const volatile)


YB_Impl_Functional_ret_spec_mfq()
//! \since build 675
YB_Impl_Functional_ret_spec_mfq((, ...))

#undef YB_Impl_Functional_ret_spec_mfq
#undef YB_Impl_Functional_ret_spec_mf

#undef YB_Impl_Functional_ret_spec

//! \since build 447
template<typename _tRet, typename... _tParams>
struct return_of<std::function<_tRet(_tParams...)>>
{
	using type = _tRet;
};
//@}


/*!
\ingroup metafunctions
\brief 取指定索引的参数类型。
\since build 333
*/
//@{
template<size_t _vIdx, typename _fCallable>
struct parameter_of
{
	using type = tuple_element_t<_vIdx,
		_t<make_parameter_tuple<_fCallable>>>;
};

//! \since build 447
template<size_t _vIdx, typename _fCallable>
using parameter_of_t = _t<parameter_of<_vIdx, _fCallable>>;
//@}


/*!
\ingroup metafunctions
\brief 取参数列表大小。
\since build 333
*/
template<typename _fCallable>
struct paramlist_size : size_t_<std::tuple_size<typename
	make_parameter_tuple<_fCallable>::type>::value>
{};


/*!
\ingroup metafunctions
\since build 572
*/
//@{
//! \brief 取指定返回类型和元组指定参数类型的函数类型。
//@{
template<typename, class>
struct make_function_type;

template<typename _tRet, class _tTuple>
using make_function_type_t = _t<make_function_type<_tRet, _tTuple>>;

template<typename _tRet, typename... _tParams>
struct make_function_type<_tRet, std::tuple<_tParams...>>
{
	using type = _tRet(_tParams...);
};
//@}


/*!
\brief 启用备用重载。
\since build 651
*/
template<template<typename...> class _gOp, typename _func, typename... _tParams>
using enable_fallback_t = enable_if_t<!is_detected<_gOp, _tParams&&...>::value,
	decltype(std::declval<_func>()(std::declval<_tParams&&>()...))>;


//! \brief 取指定维数和指定参数类型的多元映射扩展恒等函数类型。
template<typename _type, size_t _vN = 1, typename _tParam = _type>
using id_func_t
	= make_function_type_t<_type, vseq::join_n_t<_vN, std::tuple<_tParam>>>;

//! \brief 取指定维数和 const 左值引用参数类型的多元映射扩展恒等函数类型。
template<typename _type, size_t _vN = 1>
using id_func_clr_t = id_func_t<_type, _vN, const _type&>;

//! \brief 取指定维数和 const 右值引用参数类型的多元映射扩展恒等函数类型。
template<typename _type, size_t _vN = 1>
using id_func_rr_t = id_func_t<_type, _vN, _type&&>;
//@}


/*!
\brief 复合调用 std::bind 和 std::placeholders::_1 。
\note ISO C++ 要求 std::placeholders::_1 被实现支持。
*/
//@{
//! \since build 628
template<typename _func, typename... _tParams>
inline auto
bind1(_func&& f, _tParams&&... args) -> decltype(
	std::bind(yforward(f), std::placeholders::_1, yforward(args)...))
{
	return std::bind(yforward(f), std::placeholders::_1, yforward(args)...);
}
//! \since build 653
template<typename _tRes, typename _func, typename... _tParams>
inline auto
bind1(_func&& f, _tParams&&... args) -> decltype(
	std::bind<_tRes>(yforward(f), std::placeholders::_1, yforward(args)...))
{
	return std::bind<_tRes>(yforward(f), std::placeholders::_1, yforward(args)...);
}
//@}

/*!
\brief 复合调用 ystdex::bind1 和 std::placeholders::_2 以实现值的设置。
\note 从右到左逐个应用参数。
\note ISO C++ 要求 std::placeholders::_2 被实现支持。
\since build 651
*/
template<typename _func, typename _func2, typename... _tParams>
inline auto
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
yconstfn composed<_func, _func2>
compose(_func f, _func2 g)
{
	return composed<_func, _func2>{f, g};
}
//! \since build 731
template<typename _func, typename _func2, typename _func3, typename... _funcs>
yconstfn auto
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
\note 第一个参数最后被调用，可以为多元函数；其它被复合的函数需要保证有一个参数。
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

	one_shot(_func f, _tRes r = {}, _tState s = {})
		: func(f), result(r), fresh(s)
	{}
	one_shot(one_shot&& f) ynothrow
		: func(std::move(f.func)), result(std::move(f.result))
	{
		using std::swap;

		swap(fresh, f.fresh);
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

	one_shot(_func f, _tState s = {})
		: func(f), fresh(s)
	{}
	one_shot(one_shot&& f) ynothrow
		: func(std::move(f.func))
	{
		using std::swap;

		swap(fresh, f.fresh);
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
\brief get 成员小于仿函数。
\since build 672
*/
template<typename _type>
using get_less
	= composed_n<less<_type*>, composed<addressof_op<_type>, mem_get<>>>;


//! \since build 606
namespace details
{

template<typename _type, typename _fCallable, typename... _tParams>
_type
call_for_value(true_, _type&& val, _fCallable&& f, _tParams&&... args)
{
	ystdex::invoke(yforward(f), yforward(args)...);
	return yforward(val);
}

template<typename _type, typename _fCallable, typename... _tParams>
auto
call_for_value(false_, _type&&, _fCallable&& f, _tParams&&... args)
	-> result_of_t<_fCallable&&(_tParams&&...)>
{
	return ystdex::invoke(yforward(f), yforward(args)...);
}

} // unnamed namespace;

/*!
\brief 调用第二个参数起指定的函数对象，若返回空类型则使用第一个参数的值为返回值。
\since build 606
*/
template<typename _type, typename _fCallable, typename... _tParams>
auto
call_for_value(_type&& val, _fCallable&& f, _tParams&&... args)
	-> common_nonvoid_t<result_of_t<_fCallable&&(_tParams&&...)>, _type>
{
	return details::call_for_value(is_void<result_of_t<_fCallable&&(
		_tParams&&...)>>(), yforward(val), yforward(f), yforward(args)...);
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
	//! \since build 751
	template<typename _func>
	static yconstfn auto
	call(_func&& f, std::tuple<_tParams...>&& args, yimpl(decay_t<
		decltype(yforward(f)(std::get<_vSeq>(yforward(args))...))>* = {}))
		-> yimpl(decltype(yforward(f)(std::get<_vSeq>(yforward(args))...)))
	{
		return yforward(f)(std::get<_vSeq>(yforward(args))...);
	}
	//! \since build 634
	template<typename _func>
	static yconstfn auto
	call(_func&& f, _tParams&&... args)
		-> decltype(call_projection::call(yforward(f),
		std::forward_as_tuple(yforward(args)...)))
	{
		return call_projection::call(yforward(f),
			std::forward_as_tuple(yforward(args)...));
	}

	//! \since build 751
	template<typename _fCallable>
	static yconstfn auto
	invoke(_fCallable&& f, std::tuple<_tParams...>&& args,
		yimpl(decay_t<decltype(ystdex::invoke(yforward(f),
		std::get<_vSeq>(yforward(args))...))>* = {})) -> yimpl(decltype(
		ystdex::invoke(yforward(f), std::get<_vSeq>(yforward(args))...)))
	{
		return ystdex::invoke(yforward(f), std::get<_vSeq>(yforward(args))...);
	}
	//! \since build 634
	template<typename _func>
	static yconstfn auto
	invoke(_func&& f, _tParams&&... args)
		-> decltype(call_projection::invoke(yforward(f),
		std::forward_as_tuple(yforward(args)...)))
	{
		return call_projection::invoke(yforward(f),
			std::forward_as_tuple(yforward(args)...));
	}
};

//! \since build 448
template<typename _tRet, typename... _tParams, size_t... _vSeq>
struct call_projection<std::function<_tRet(_tParams...)>,
	index_sequence<_vSeq...>> : private
	call_projection<_tRet(_tParams...), index_sequence<_vSeq...>>
{
	//! \since build 589
	using call_projection<_tRet(_tParams...), index_sequence<_vSeq...>>::call;
	//! \since build 634
	using
		call_projection<_tRet(_tParams...), index_sequence<_vSeq...>>::invoke;
};

/*!
\note 不需要显式指定返回类型。
\since build 547
*/
template<typename... _tParams, size_t... _vSeq>
struct call_projection<std::tuple<_tParams...>, index_sequence<_vSeq...>>
{
	//! \since build 751
	template<typename _func>
	static yconstfn auto
	call(_func&& f, std::tuple<_tParams...>&& args)
		-> yimpl(decltype(yforward(f)(std::get<_vSeq>(yforward(args))...)))
	{
		return yforward(f)(std::get<_vSeq>(yforward(args))...);
	}

	//! \since build 751
	template<typename _func>
	static yconstfn auto
	call(_func&& f, _tParams&&... args)
		-> decltype(call_projection::call(yforward(f),
		std::forward_as_tuple(yforward(yforward(args))...)))
	{
		return call_projection::call(yforward(f),
			std::forward_as_tuple(yforward(yforward(args))...));
	}

	//! \since build 634
	template<typename _fCallable>
	static yconstfn auto
	invoke(_fCallable&& f, std::tuple<_tParams...>&& args) -> yimpl(
		decltype(ystdex::invoke(yforward(f), std::get<_vSeq>(args)...)))
	{
		return ystdex::invoke(yforward(f), std::get<_vSeq>(args)...);
	}
	template<typename _func>
	static yconstfn auto
	invoke(_func&& f, _tParams&&... args) -> decltype(call_projection::invoke(
		yforward(f), std::forward_as_tuple(yforward(args)...)))
	{
		return call_projection::invoke(yforward(f),
			std::forward_as_tuple(yforward(args)...));
	}
};
//@}


/*!
\brief 应用函数对象和参数元组。
\tparam _func 函数对象及其引用类型。
\tparam _tTuple 元组及其引用类型。
\see WG21 N4606 20.5.2.5[tuple.apply]/1 。
\see http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4023.html#tuple.apply 。
\since build 547
*/
template<typename _func, class _tTuple>
yconstfn auto
apply(_func&& f, _tTuple&& args)
	-> yimpl(decltype(call_projection<_tTuple, make_index_sequence<
	std::tuple_size<decay_t<_tTuple>>::value>>::call(yforward(f),
	yforward(args))))
{
	return call_projection<_tTuple, make_index_sequence<std::tuple_size<
		decay_t<_tTuple>>::value>>::call(yforward(f), yforward(args));
}


//! \since build 634
//@{
template<typename _fCallable, size_t _vLen = paramlist_size<_fCallable>::value>
struct expand_proxy : private call_projection<_fCallable,
	make_index_sequence<_vLen>>, private expand_proxy<_fCallable, _vLen - 1>
{
	/*!
	\see CWG 1393 。
	\see EWG 102 。
	*/
	using call_projection<_fCallable, make_index_sequence<_vLen>>::call;
	/*!
	\note 为避免歧义，不直接使用 using 声明。
	\since build 657
	*/
	template<typename... _tParams>
	static auto
	call(_tParams&&... args) -> decltype(
		expand_proxy<_fCallable, _vLen - 1>::call(yforward(args)...))
	{
		return expand_proxy<_fCallable, _vLen - 1>::call(yforward(args)...);
	}
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
\since build 634
\sa object_result_t
*/
template<typename _fCond, typename _fCallable, typename... _tParams>
result_of_t<_fCallable&&(_tParams&&...)>
retry_on_cond(_fCond cond, _fCallable&& f, _tParams&&... args)
{
	using res_t = result_of_t<_fCallable&&(_tParams&&...)>;
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
\todo 支持 ref-qualifier 。
*/
template<typename _fHandler, typename _fCallable>
struct expanded_caller
{
	//! \since build 448
	static_assert(is_object<_fCallable>(), "Callable object type is needed.");

	//! \since build 525
	_fCallable caller;

	//! \since build 448
	template<typename _fCaller,
		yimpl(typename = exclude_self_t<expanded_caller, _fCaller>)>
	expanded_caller(_fCaller&& f)
		: caller(yforward(f))
	{}

	//! \since build 640
	template<typename... _tParams>
	auto
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
\since build 448
*/
template<typename _fHandler, typename _fCallable>
yconstfn expanded_caller<_fHandler, decay_t<_fCallable>>
make_expanded(_fCallable&& f)
{
	return expanded_caller<_fHandler, decay_t<_fCallable>>(yforward(f));
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

