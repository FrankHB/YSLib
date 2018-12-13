/*
	© 2012-2016, 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file function.hpp
\ingroup YStandardEx
\brief 函数基本操作和调用包装对象。
\version r4292
\author FrankHB <frankhb1989@gmail.com>
\since build 847
\par 创建时间:
	2018-12-13 01:24:06 +0800
\par 修改时间:
	2018-12-14 00:19 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Function
*/


#ifndef YB_INC_ystdex_function_hpp_
#define YB_INC_ystdex_function_hpp_ 1

#include "type_op.hpp" // for internal "type_op.hpp", index_sequence, true_,
//	std::tuple, is_convertible, vseq::at, bool_, index_sequence_for,
//	remove_cvref_t, _t, std::tuple_element, std::tuple_size, size_t_,
//	vseq::join_n_t, nullptr_t;

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


//! \since build 847
//@{
template<typename>
struct mk_ptuple;

#define YB_Impl_Functional_ptuple_spec(_exp, _p, _e) \
	template<typename _tRet, _exp typename... _tParams ynoexcept_qual(ne)> \
	struct mk_ptuple<_tRet _p (_tParams... YPP_Args _e) ynoexcept_qual(ne)> \
	{ \
		using type = std::tuple<_tParams...>; \
	};

YB_Impl_Functional_ptuple_spec(, , )
YB_Impl_Functional_ptuple_spec(, (*), )
YB_Impl_Functional_ptuple_spec(, , (, ...))
YB_Impl_Functional_ptuple_spec(, (*), (, ...))

#undef YB_Impl_Functional_ptuple_spec

template<typename _fSig>
struct mk_ptuple<std::function<_fSig>> : mk_ptuple<_fSig>
{};


template<typename>
struct ret_of;

#define YB_Impl_Functional_ret_spec(_exp, _p, _e) \
	template<typename _tRet, _exp typename... _tParams ynoexcept_qual(ne)> \
	struct ret_of<_tRet _p (_tParams... YPP_Args _e) ynoexcept_qual(ne)> \
	{ \
		using type = _tRet; \
	};

#define YB_Impl_Functional_ret_spec_f(_e) \
	YB_Impl_Functional_ret_spec(, , _e) \
	YB_Impl_Functional_ret_spec(, (*), _e)

YB_Impl_Functional_ret_spec_f()
YB_Impl_Functional_ret_spec_f((, ...))

#undef YB_Impl_Functional_ret_spec_f

#define YB_Impl_Functional_ret_spec_mf(_e) \
	YB_Impl_Functional_ret_spec(class _tClass YPP_Comma, (_tClass::*), _e)

YB_Impl_Functional_ret_spec_mf()
YB_Impl_Functional_ret_spec_mf((, ...))

#undef YB_Impl_Functional_ret_spec_mf

#undef YB_Impl_Functional_ret_spec

template<typename _tSig>
struct ret_of<std::function<_tSig>> : ret_of<_tSig>
{};
//@}

} // namespace details;

//! \ingroup binary_type_traits
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


//! \ingroup transformation_traits
//@{
//! \brief 取参数列表元组。
//@{
//! \since build 333
template<typename _fCallable>
struct make_parameter_tuple
	: details::mk_ptuple<remove_cvref_t<_fCallable>>
{};

//! \since build 447
template<typename _fCallable>
using make_parameter_tuple_t = _t<make_parameter_tuple<_fCallable>>;
//@}


//! \brief 取返回类型。
//@{
//! \since build 333
template<typename _fCallable>
struct return_of : details::ret_of<remove_cvref_t<_fCallable>>
{};

//! \since build 447
template<typename _fCallable>
using return_of_t = _t<return_of<_fCallable>>;
//@}


/*!
\brief 取指定索引的参数类型。
\since build 333
*/
//@{
template<size_t _vIdx, typename _fCallable>
struct parameter_of
{
	using type
		= _t<std::tuple_element<_vIdx, _t<make_parameter_tuple<_fCallable>>>>;
};

//! \since build 447
template<size_t _vIdx, typename _fCallable>
using parameter_of_t = _t<parameter_of<_vIdx, _fCallable>>;
//@}


/*!
\brief 取参数列表大小。
\since build 333
*/
template<typename _fCallable>
struct paramlist_size : size_t_<std::tuple_size<typename
	make_parameter_tuple<_fCallable>::type>::value>
{};


//! \since build 572
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
//@}

} // namespace ystdex;

#endif

