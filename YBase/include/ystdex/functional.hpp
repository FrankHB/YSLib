/*
	© 2010-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file functional.hpp
\ingroup YStandardEx
\brief 函数和可调用对象。
\version r2269
\author FrankHB <frankhb1989@gmail.com>
\since build 333
\par 创建时间:
	2010-08-22 13:04:29 +0800
\par 修改时间:
	2015-05-02 13:29 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Functional
*/


#ifndef YB_INC_ystdex_functional_hpp_
#define YB_INC_ystdex_functional_hpp_ 1

#include "tuple.hpp" // for ../ydef.h, std::tuple_size, vseq::join_n_t,
//	make_index_sequence;
#include "ref.hpp" // for wrapped_traits;
#include <string> // for std::char_traits;

namespace ystdex
{

//! \since build 447
template<typename _tFrom, typename _tTo, typename... _tFromParams,
	typename... _tToParams>
struct is_covariant<_tFrom(_tFromParams...), _tTo(_tToParams...)>
	: is_covariant<_tFrom, _tTo>
{};

//! \since build 575
template<typename _tFrom, typename _tTo, typename... _tFromParams,
	typename... _tToParams>
struct is_covariant<std::function<_tFrom(_tFromParams...)>,
	std::function<_tTo(_tToParams...)>>
	: is_covariant<_tFrom(_tFromParams...), _tTo(_tToParams...)>
{};


//! \since build 447
template<typename _tResFrom, typename _tResTo, typename... _tFromParams,
	typename... _tToParams>
struct is_contravariant<_tResFrom(_tFromParams...), _tResTo(_tToParams...)>
	: is_contravariant<std::tuple<_tFromParams...>, std::tuple<_tToParams...>>
{};

//! \since build 575
template<typename _tResFrom, typename _tResTo, typename... _tFromParams,
	typename... _tToParams>
struct is_contravariant<std::function<_tResFrom(_tFromParams...)>,
	std::function<_tResTo(_tToParams...)>>
	: is_contravariant<_tResFrom(_tFromParams...), _tResTo(_tToParams...)>
{};


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


//! \see 关于调用参数类型： ISO C++11 30.3.1.2[thread.thread.constr] 。
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
using make_parameter_tuple_t = typename make_parameter_tuple<_fCallable>::type;

template<typename _tRet, typename... _tParams>
struct make_parameter_tuple<_tRet(_tParams...)>
{
	using type = std::tuple<_tParams...>;
};

template<typename _tRet, typename... _tParams>
struct make_parameter_tuple<_tRet(*)(_tParams...)>
{
	using type = std::tuple<_tParams...>;
};

template<typename _tRet, typename... _tParams>
struct make_parameter_tuple<_tRet(&)(_tParams...)>
{
	using type = std::tuple<_tParams...>;
};

template<typename _tRet, class _tClass, typename... _tParams>
struct make_parameter_tuple<_tRet(_tClass::*)(_tParams...)>
{
	using type = std::tuple<_tParams...>;
};

//! \since build 358
//@{
template<typename _tRet, class _tClass, typename... _tParams>
struct make_parameter_tuple<_tRet(_tClass::*)(_tParams...) const>
{
	using type = std::tuple<_tParams...>;
};

template<typename _tRet, class _tClass, typename... _tParams>
struct make_parameter_tuple<_tRet(_tClass::*)(_tParams...) volatile>
{
	using type = std::tuple<_tParams...>;
};

template<typename _tRet, class _tClass, typename... _tParams>
struct make_parameter_tuple<_tRet(_tClass::*)(_tParams...) const volatile>
{
	using type = std::tuple<_tParams...>;
};
//@}

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
using return_of_t = typename return_of<_fCallable>::type;

template<typename _tRet, typename... _tParams>
struct return_of<_tRet(_tParams...)>
{
	using type = _tRet;
};

template<typename _tRet, typename... _tParams>
struct return_of<_tRet(*)(_tParams...)>
{
	using type = _tRet;
};

template<typename _tRet, typename... _tParams>
struct return_of<_tRet(&)(_tParams...)>
{
	using type = _tRet;
};

template<typename _tRet, class _tClass, typename... _tParams>
struct return_of<_tRet(_tClass::*)(_tParams...)>
{
	using type = _tRet;
};

//! \since build 358
//@{
template<typename _tRet, class _tClass, typename... _tParams>
struct return_of<_tRet(_tClass::*)(_tParams...) const>
{
	using type = _tRet;
};

template<typename _tRet, class _tClass, typename... _tParams>
struct return_of<_tRet(_tClass::*)(_tParams...) volatile>
{
	using type = _tRet;
};

template<typename _tRet, class _tClass, typename... _tParams>
struct return_of<_tRet(_tClass::*)(_tParams...) const volatile>
{
	using type = _tRet;
};
//@}

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
		typename make_parameter_tuple<_fCallable>::type>;
};

//! \since build 447
template<size_t _vIdx, typename _fCallable>
using parameter_of_t = typename parameter_of<_vIdx, _fCallable>::type;
//@}


/*!
\ingroup metafunctions
\brief 取参数列表大小。
\since build 333
*/
template<typename _fCallable>
struct paramlist_size : integral_constant<size_t, std::tuple_size<typename
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
using make_function_type_t = typename make_function_type<_tRet, _tTuple>::type;

template<typename _tRet, typename... _tParams>
struct make_function_type<_tRet, std::tuple<_tParams...>>
{
	using type = _tRet(_tParams...);
};
//@}


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


//! \since build 537
//@{
//! \brief 复合函数。
template<typename _func1, typename _func2>
struct composed
{
	_func1 f;
	_func2 g;

	template<typename... _tParams>
	auto
	operator()(_tParams&&... args) const -> decltype(f(g(yforward(args))...))
	{
		return f(g(yforward(args))...);
	}
};

/*!
\brief 函数复合。
\note 第一个参数最后被调用，可以为多元函数；其它被复合的函数需要保证有一个参数。
\relates composed
\return 复合的可调用对象。
*/
template<typename _func1, typename _func2>
composed<_func1, _func2>
compose(_func1 f, _func2 g)
{
	return composed<_func1, _func2>{f, g};
}
template<typename _func1, typename _func2, typename... _funcs>
yconstfn auto
compose(_func1 f, _func2 g, _funcs... args)
	-> decltype(ystdex::compose(ystdex::compose(f, g), args...))
{
	return ystdex::compose(ystdex::compose(f, g), args...);
}
//@}


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
	//! \since build 547
	template<typename _func>
	static _tRet
	call(_func&& f, std::tuple<_tParams...>&& args, yimpl(decay_t<
		decltype(yforward(f)(yforward(std::get<_vSeq>(yforward(args)))...))>*
		= {}))
	{
		return yforward(f)(yforward(std::get<_vSeq>(yforward(args)))...);
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
};

//! \since build 547
template<typename... _tParams, size_t... _vSeq>
struct call_projection<std::tuple<_tParams...>, index_sequence<_vSeq...>>
{
	template<typename _func>
	static auto
	call(_func&& f, std::tuple<_tParams...>&& args)
		-> decltype(yforward(f)(std::get<_vSeq>(yforward(args))...))
	{
		return yforward(f)(std::get<_vSeq>(yforward(args))...);
	}
};
//@}


/*!
\brief 应用函数对象和参数元组。
\tparam _func 函数对象及其引用类型。
\tparam _tTuple 元组及其引用类型。
\see WG21/N3936 20.5.1[intseq.general] 。
\see http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4023.html#tuple.apply 。
\since build 547
*/
template<typename _func, class _tTuple>
inline auto
apply(_func&& f, _tTuple&& args)
	-> yimpl(decltype(call_projection<_tTuple, make_index_sequence<
	std::tuple_size<decay_t<_tTuple>>::value>>::call(yforward(f),
	yforward(args))))
{
	return call_projection<_tTuple, make_index_sequence<std::tuple_size<
		decay_t<_tTuple>>::value>>::call(yforward(f), yforward(args));
}


//! \since build 447
namespace details
{

//! \since build 448
template<typename _fCallable, size_t _vLen = paramlist_size<_fCallable>::value>
struct expand_proxy : private call_projection<_fCallable,
	make_index_sequence<_vLen>>, private expand_proxy<_fCallable, _vLen - 1>
{
	using call_projection<_fCallable, make_index_sequence<_vLen>>::call;
	using expand_proxy<_fCallable, _vLen - 1>::call;
};

//! \since build 448
template<typename _fCallable>
struct expand_proxy<_fCallable, 0>
	: private call_projection<_fCallable, index_sequence<>>
{
	using call_projection<_fCallable, index_sequence<>>::call;
};

} // namespace details;


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
		yimpl(typename = exclude_self_ctor_t<expanded_caller, _fCaller>)>
	expanded_caller(_fCaller&& f)
		: caller(yforward(f))
	{}

	template<typename... _tParams>
	auto
	operator()(_tParams&&... args)
		-> decltype(details::expand_proxy<_fHandler>::call(caller,
		std::forward_as_tuple(yforward(args)...)))
	{
		return details::expand_proxy<_fHandler>::call(caller,
			std::forward_as_tuple(yforward(args)...));
	}
	//! \since build 595
	template<typename... _tParams>
	auto
	operator()(_tParams&&... args) const
		-> decltype(details::expand_proxy<_fHandler>::call(caller,
		std::forward_as_tuple(yforward(args)...))) const
	{
		return details::expand_proxy<_fHandler>::call(caller,
			std::forward_as_tuple(yforward(args)...));
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

} // namespace ystdex;

#endif

