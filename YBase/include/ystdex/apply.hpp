/*
	© 2013-2016, 2018-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file apply.hpp
\ingroup YStandardEx
\brief 元组和函数应用操作。
\version r1102
\author FrankHB <frankhb1989@gmail.com>
\since build 333
\par 创建时间:
	2019-01-11 19:43:23 +0800
\par 修改时间:
	2022-03-21 12:08 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Apply
*/


#ifndef YB_INC_ystdex_apply_hpp_
#define YB_INC_ystdex_apply_hpp_ 1

#include "invoke.hpp" // for "invoke.hpp", ystdex::invoke, decay_t;
#include "integer_sequence.hpp" // for index_sequence, make_index_sequence;
#include <tuple> // for <tuple>, __cpp_lib_apply, __cpp_lib_tuple_element_t,
//	__cpp_lib_make_from_tuple, std::tuple, std::get, std::forward_as_tuple,
//	std::tuple_size, std::make_from_tuple;

/*!
\brief \c \<tuple> 特性测试宏。
\see ISO C++20 [version.syn] 。
\see WG21 P0941R2 2.2 。
\see https://docs.microsoft.com/cpp/visual-cpp-language-conformance 。
\since build 833
*/
//@{
#ifndef __cpp_lib_apply
#	if YB_IMPL_MSCPP >= 1910 || __cplusplus >= 201603L
#		define __cpp_lib_apply 201603L
#	endif
#endif
#ifndef __cpp_lib_tuple_element_t
#	if YB_IMPL_MSCPP >= 1900 || __cplusplus >= 201402L
#		define __cpp_lib_tuple_element_t 201402L
#	endif
#endif
#ifndef __cpp_lib_make_from_tuple
#	if YB_IMPL_MSCPP >= 1910 || __cplusplus >= 201606L
#		define __cpp_lib_make_from_tuple 201606L
#	endif
#endif
//@}

namespace ystdex
{

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

	//! \since build 634
	template<typename _func>
	static yconstfn auto
	call(_func&& f, _tParams&&... args) -> yimpl(decltype(
		apply_call(yforward(f), std::forward_as_tuple(yforward(args)...))))
	{
		return
			apply_call(yforward(f), std::forward_as_tuple(yforward(args)...));
	}

	//! \since build 810
	template<typename _fCallable>
	static yconstfn auto
	invoke(_fCallable&& f, _tParams&&... args) -> yimpl(decltype(
		apply_invoke(yforward(f), std::forward_as_tuple(yforward(args)...))))
	{
		return
			apply_invoke(yforward(f), std::forward_as_tuple(yforward(args)...));
	}
};

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
	apply_invoke(_fCallable&& f, std::tuple<_tParams...>&& args)
		-> yimpl(decltype(ystdex::invoke(yforward(f),
		std::get<_vSeq>(yforward(args))...)))
	{
		return ystdex::invoke(yforward(f), std::get<_vSeq>(yforward(args))...);
	}

	//! \since build 751
	template<typename _func>
	static yconstfn auto
	call(_func&& f, _tParams&&... args) -> decltype(
		apply_call(yforward(f), std::forward_as_tuple(yforward(args)...)))
	{
		return
			apply_call(yforward(f), std::forward_as_tuple(yforward(args)...));
	}

	//! \since build 810
	template<typename _fCallable>
	static yconstfn auto
	invoke(_fCallable&& f, _tParams&&... args) -> yimpl(decltype(
		apply_invoke(yforward(f), std::forward_as_tuple(yforward(args)...))))
	{
		return apply_invoke(yforward(f),
			std::forward_as_tuple(yforward(args)...));
	}
};
//@}

#if !(__cpp_lib_make_from_tuple >= 201606L)
//! \since build 833
namespace details
{

template<typename _type, typename _tTuple, size_t... _vSeq>
yconstfn _type
make_from_tuple_impl(_tTuple&& t, index_sequence<_vSeq...>)
{
	return _type(std::get<_vSeq>(yforward(t))...);
}

} // namespace details;
#endif

inline namespace cpp2017
{

/*!
\tparam _tTuple 元组及其引用类型。
\see ISO C++17 [tuple.apply] 。
*/
//@{
#if __cpp_lib_apply >= 201603L
using std::apply;
#else
/*!
\brief 应用函数对象和参数元组。
\tparam _func 函数对象及其引用类型。
\see ISO C++17 [tuple.apply]/1 。
\see WG21 P0220R1 。
\since build 547
*/
template<typename _func, class _tTuple>
yconstfn auto
apply(_func&& f, _tTuple&& t)
	-> yimpl(decltype(call_projection<_tTuple, make_index_sequence<
	std::tuple_size<decay_t<_tTuple>>::value>>::apply_invoke(yforward(f),
	yforward(t))))
{
	return call_projection<_tTuple, make_index_sequence<std::tuple_size<
		decay_t<_tTuple>>::value>>::apply_invoke(yforward(f), yforward(t));
}
#endif

#if __cpp_lib_make_from_tuple >= 201606L
using std::make_from_tuple;
#else
/*!
\brief 从元组构造指定类型的值。
\tparam _type 被构造的对象类型。
\see WG21 N4606 20.5.2.5[tuple.apply]/2 。
\see WG21 P0209R2 。
\since build 735
*/
template<typename _type, class _tTuple>
yconstfn _type
make_from_tuple(_tTuple&& t)
{
	return details::make_from_tuple_impl<_type>(yforward(t),
		make_index_sequence<std::tuple_size<decay_t<_tTuple>>::value>());
}
#endif
//@}

} // inline namespace cpp2017;

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
/*!
\brief 变长参数操作模板。
\warning 非虚析构。
*/
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
\tparam _vN 表示参数位置的非负数，从左开始计数，第一参数为 0 。
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

} // namespace ystdex;

#endif

