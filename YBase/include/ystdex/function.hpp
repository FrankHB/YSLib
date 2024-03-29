﻿/*
	© 2012-2016, 2018-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file function.hpp
\ingroup YStandardEx
\brief 函数基本操作和调用包装对象。
\version r5053
\author FrankHB <frankhb1989@gmail.com>
\since build 847
\par 创建时间:
	2018-12-13 01:24:06 +0800
\par 修改时间:
	2022-04-29 00:50 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Function
*/


#ifndef YB_INC_ystdex_function_hpp_
#define YB_INC_ystdex_function_hpp_ 1

#include "any.h" // for internal "type_op.hpp", <functional>, index_sequence,
//	true_, std::tuple, is_convertible, vseq::at, bool_, index_sequence_for,
//	remove_cvref_t, _t, vseq::at_t, vseq::seq_size_t, enable_if_t,
//	vseq::join_n_t, yconstraint, is_function, any, nullptr_t, ystdex::invoke,
//	std::allocator_arg_t, exclude_self_t, is_invocable_r, trivial_swap_t,
//	trivial_swap, std::allocator_arg, std::reference_wrapper, std::swap,
//	is_bitwise_swappable;
#include "apply.hpp" // for internal "apply.hpp", call_projection;
#include "operators.hpp" // for equality_comparable;

namespace ystdex
{

//! \since build 447
//@{
namespace details
{

//! \since build 851
//@{
template<class, class, class>
struct elements_convertible;

template<class _type1, class _type2>
struct elements_convertible<_type1, _type2, index_sequence<>> : true_
{};

template<typename... _types1, typename... _types2, size_t... _vSeq,
	size_t _vHead>
struct elements_convertible<empty_base<_types1...>, empty_base<_types2...>,
	index_sequence<_vHead, _vSeq...>>
{
	static_assert(sizeof...(_types1) == sizeof...(_types2),
		"Mismatched sizes of tuple found.");

private:
	using t1 = empty_base<_types1...>;
	using t2 = empty_base<_types2...>;

public:
	static yconstexpr const bool value
		= is_convertible<vseq::at<t1, _vHead>, vseq::at<t2, _vHead>>::value
		&& elements_convertible<t1, t2, index_sequence<_vSeq...>>::value;
};
//@}


//! \since build 847
//@{
//! \since build 851
template<typename>
struct mk_plist;

#define YB_Impl_Functional_ptuple_spec(_exp, _p, _e) \
	template<typename _tRet, _exp typename... _tParams ynoexcept_param(ne)> \
	struct mk_plist<_tRet _p (_tParams... YPP_Args _e) ynoexcept_qual(ne)> \
	{ \
		using type = empty_base<_tParams...>; \
	};

YB_Impl_Functional_ptuple_spec(, , )
YB_Impl_Functional_ptuple_spec(, (*), )
YB_Impl_Functional_ptuple_spec(, , (, ...))
YB_Impl_Functional_ptuple_spec(, (*), (, ...))

#undef YB_Impl_Functional_ptuple_spec

//! \since build 851
template<typename _fSig>
struct mk_plist<std::function<_fSig>> : mk_plist<_fSig>
{};


template<typename>
struct ret_of;

#define YB_Impl_Functional_ret_spec(_exp, _p, _e) \
	template<typename _tRet, _exp typename... _tParams ynoexcept_param(ne)> \
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

template<typename _fSig>
struct ret_of<std::function<_fSig>> : ret_of<_fSig>
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

//! \since build 851
template<typename... _tFroms, typename... _tTos>
struct is_covariant<empty_base<_tFroms...>, empty_base<_tTos...>>
	: bool_<details::elements_convertible<empty_base<_tFroms...>,
	empty_base<_tTos...>, index_sequence_for<_tTos...>>::value>
{};

template<typename... _tFroms, typename... _tTos>
struct is_covariant<std::tuple<_tFroms...>, std::tuple<_tTos...>>
	: is_covariant<empty_base<_tFroms...>, empty_base<_tTos...>>
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
	: is_contravariant<empty_base<_tFromParams...>, empty_base<_tToParams...>>
{};

//! \since build 851
template<typename... _tFroms, typename... _tTos>
struct is_contravariant<empty_base<_tFroms...>, empty_base<_tTos...>>
	: bool_<details::elements_convertible<empty_base<_tTos...>,
	empty_base<_tFroms...>, index_sequence_for<_tTos...>>::value>
{};

template<typename... _tFroms, typename... _tTos>
struct is_contravariant<std::tuple<_tFroms...>, std::tuple<_tTos...>>
	: is_contravariant<empty_base<_tFroms...>, empty_base<_tTos...>>
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


//! \ingroup transformation_traits
//@{
//! \brief 取参数列表。
//@{
//! \since build 851
//@{
template<typename _fCallable>
struct make_parameter_list
	: details::mk_plist<remove_cvref_t<_fCallable>>
{};

template<typename _fCallable>
using make_parameter_list_t = _t<make_parameter_list<_fCallable>>;
//@}
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
	using type = vseq::at_t<make_parameter_list_t<_fCallable>, _vIdx>;
};

//! \since build 447
template<size_t _vIdx, typename _fCallable>
using parameter_of_t = _t<parameter_of<_vIdx, _fCallable>>;
//@}


/*!
\ingroup unary_type_traits
\brief 取参数列表大小。
\since build 333
*/
template<typename _fCallable>
struct paramlist_size : vseq::seq_size_t<make_parameter_list_t<_fCallable>>
{};


//! \since build 572
//@{
/*!
\ingroup binary_type_traits
\brief 取指定返回类型和元组指定参数类型的函数类型。
*/
//@{
template<typename, class>
struct make_function_type;

template<typename _tRet, class _tTuple>
using make_function_type_t = _t<make_function_type<_tRet, _tTuple>>;

template<typename _tRet, typename... _tParams>
struct make_function_type<_tRet, empty_base<_tParams...>>
{
	using type = _tRet(_tParams...);
};

template<typename _tRet, typename... _tParams>
struct make_function_type<_tRet, std::tuple<_tParams...>>
	: make_function_type<_tRet, empty_base<_tParams...>>
{};
//@}


//! \since build 448
template<typename _tRet, typename... _tParams, size_t... _vSeq>
struct call_projection<std::function<_tRet(_tParams...)>,
	index_sequence<_vSeq...>>
	: call_projection<_tRet(_tParams...), index_sequence<_vSeq...>>
{};


/*!
\ingroup unary_type_traits
\brief 取内建函数类型。
\since build 850
*/
template<typename _fCallable>
using as_function_type_t = make_function_type_t<return_of_t<_fCallable>,
	make_parameter_list_t<_fCallable>>;


/*!
\brief 启用备用重载。
\since build 651
*/
template<template<typename...> class _gOp, typename _func, typename... _tParams>
using enable_fallback_t = enable_if_t<!is_detected<_gOp, _tParams&&...>::value,
	decltype(std::declval<_func>()(std::declval<_tParams>()...))>;


//! \brief 取指定维数和指定参数类型的多元映射扩展恒等函数类型。
template<typename _type, size_t _vN = 1, typename _tParam = _type>
using id_func_t
	= make_function_type_t<_type, vseq::join_n_t<_vN, empty_base<_tParam>>>;

//! \brief 取指定维数和 const 左值引用参数类型的多元映射扩展恒等函数类型。
template<typename _type, size_t _vN = 1>
using id_func_clr_t = id_func_t<_type, _vN, const _type&>;

//! \brief 取指定维数和 const 右值引用参数类型的多元映射扩展恒等函数类型。
template<typename _type, size_t _vN = 1>
using id_func_rr_t = id_func_t<_type, _vN, _type&&>;
//@}
//@}


//! \since build 849
//@{
/*!
\brief 空函数策略。
*/
enum class empty_function_policy
{
	//! \brief 调用空值行为未定义。
	no_check,
	//! \brief 调用空值无作用。
	no_effect,
	//! \brief 调用空值抛出 std::bad_function_call 。
	throwing
};


/*!
\brief 函数包装特征。

用于定制函数包装模板实现的特征。
当前由空函数特化。
包含以下静态成员函数模板：
init_empty ：初始化指定空函数对象，保证不抛出异常；
call ：按策略调用函数。
成员用于实现，不保证稳定。
*/
//@{
template<typename, empty_function_policy>
struct function_traits;

template<typename _tRet, typename... _tParams>
struct function_traits<_tRet(_tParams...), empty_function_policy::no_check>
{
	template<class _tContent, typename _fInvoke>
	static yconstfn_relaxed void
	init_empty(const _tContent&, _fInvoke&) ynothrow
	{}

	template<class _tContent, typename _fInvoke>
	static _tRet
	call(const _tContent& content, const _fInvoke& f, _tParams&&... args)
	{
		yconstraint(bool(content.has_value()));
		yassume(f);
		return f(content, yforward(args)...);
	}
};

template<typename _tRet, typename... _tParams>
struct function_traits<_tRet(_tParams...), empty_function_policy::no_effect>
{
	// NOTE: See $2020-01 @ %Documentation::Workflow.
	//! \since build 878
	template<class _tContent>
#if YB_IMPL_GNUCPP && defined(NDEBUG) \
	&& !(defined(YB_DLL) || defined(YB_BUILD_DLL))
	YB_ATTR(noinline) YB_ATTR(optimize("O2"))
#endif
	static _tRet
	invoke_empty(const _tContent&, _tParams...)
		ynoexcept(is_nothrow_constructible<_tRet>())
	{
		return _tRet();
	}

	template<class _tContent, typename _fInvoke>
	static yconstfn_relaxed void
	init_empty(const _tContent&, _fInvoke& f) ynothrow
	{
		static_assert(is_nothrow_copy_assignable<_fInvoke>(),
			"Invalid invoker type found.");

		// XXX: Here lambda-expression is buggy in G++ LTO.
		f = &invoke_empty<_tContent>;
	}

	template<class _tContent, typename _fInvoke>
	static _tRet
	call(const _tContent& content, const _fInvoke& f, _tParams&&... args)
	{
		yassume(f);
		return f(content, yforward(args)...);
	}
};

template<typename _tRet, typename... _tParams>
struct function_traits<_tRet(_tParams...), empty_function_policy::throwing>
{
	// NOTE: See $2020-01 @ %Documentation::Workflow.
	//! \since build 878
	template<class _tContent>
#if YB_IMPL_GNUCPP && defined(NDEBUG) \
	&& !(defined(YB_DLL) || defined(YB_BUILD_DLL))
	YB_ATTR(noinline) YB_ATTR(optimize("O2"))
#endif
	YB_NORETURN static _tRet
	invoke_empty(const _tContent&, _tParams...)
	{
		// TODO: Use a function call to throw?
		throw std::bad_function_call();
	}

	template<class _tContent, typename _fInvoke>
	static yconstfn_relaxed void
	init_empty(const _tContent&, _fInvoke& f)
	{
		// XXX: Here lambda-expression is buggy in G++ LTO.
		f = &invoke_empty<_tContent>;
	}

	template<class _tContent, typename _fInvoke>
	static _tRet
	call(const _tContent& content, const _fInvoke& f, _tParams&&... args)
	{
		yassume(f);
		return f(content, yforward(args)...);
	}
};
//@}
//@}


//! \since build 848
//@{
/*!
\ingroup functors
\brief 函数包装类模板。
\since build 849
*/
template<class, typename>
class function_base;


/*!
\brief 判断是否符合 std::function 实现初始化为空函数对象的条件。
\note 同时支持对 std::function 和 ystdex 中的实现的判断。
\see ISO C++17 [func.wrap.func.con]/9 。
\since build 848
*/
//@{
template<typename _fSig>
YB_ATTR_nodiscard YB_PURE static bool
function_not_empty(const std::function<_fSig>& f) ynothrow
{
	return bool(f);
}
//! \since build 849
template<class _tTraits, typename _fSig>
YB_ATTR_nodiscard YB_PURE static bool
function_not_empty(const function_base<_tTraits, _fSig>& f) ynothrow
{
	return bool(f);
}
template<typename _func,
	yimpl(typename = enable_if_t<is_function<_func>::value>)>
YB_ATTR_nodiscard YB_PURE static bool
function_not_empty(_func* p) ynothrow
{
	return p;
}
template<typename _tRet, class _tClass>
YB_ATTR_nodiscard YB_PURE static bool
function_not_empty(_tRet _tClass::* p) ynothrow
{
	return p;
}
template<typename _type>
YB_ATTR_nodiscard YB_STATELESS static yconstfn bool
function_not_empty(const _type&) ynothrow
{
	return true;
}
//@}


/*!
\brief 函数包装类模板特化。
\sa any

提供支持不同动态类型的调用包装。
和 ISO C++17 std::function 特化对应的接口相同，除以下特性：
允许不满足 CopyConstructible 的类型作为目标；
复制不满足 CopyConstructible 的目标时抛出异常；
模板参数中包含特化调用行为的特征；
调用没有目标的空对象的行为由特征决定，不一定抛出异常，行为可能未定义；
保证转移构造无异常抛出（参见 WG21 P0043R0 ）；
构造函数支持分配器（类似 WG21 P0302R1 移除的接口；另见 WG21 P0043R0 ）；
不提供 assign（可直接使用构造后转移赋值代替）。
分配器支持具有以下特性：
相等的分配器随复制和转移构造传播（同 WG21 P0043R0 ）；
没有使用分配器的初始化不保证等价使用默认分配器（和 any 设计的策略一致）；
没有 std::uses_allocator 特化（同 ISO C++17 和 WG21 P0043R0 ）。
*/
template<class _tTraits, typename _tRet, typename... _tParams>
class function_base<_tTraits, _tRet(_tParams...)> : private equality_comparable<
	function_base<_tTraits, _tRet(_tParams...)>, nullptr_t>
{
public:
	//! \since build 848
	//@{
	using result_type = _tRet;

private:
	template<class _tHandler>
	struct invoker
	{
		static _tRet
		invoke(const any& a, _tParams... args)
		// TODO: Exception specification?
		{
			return static_cast<_tRet>(ystdex::invoke(_tHandler::get_reference(
				a.get_storage()), yforward(args)...));
		}
	};

	// XXX: The empty state is solely determined by %content, having nothing to
	//	do with %p_invoke.
	any content{default_init};
	// XXX: The invoker pointer outside %content would cause the %function
	//	object not fit in %any, as well as cost of copy, move and swap. This
	//	is reasonable at the cost for fast invocation in %operator().
	_tRet(*p_invoke)(const any&, _tParams...);

public:
	function_base() ynothrow
	{
		_tTraits::init_empty(content, p_invoke);
	}
	//! \ingroup YBase_replacement_extensions
	template<class _tAlloc>
	inline
	function_base(std::allocator_arg_t, const _tAlloc&) ynothrow
		: function_base()
	{}
	function_base(nullptr_t) ynothrow
		: function_base()
	{}
	//! \ingroup YBase_replacement_extensions
	template<class _tAlloc>
	inline
	function_base(std::allocator_arg_t, const _tAlloc&, nullptr_t,
		const _tAlloc&) ynothrow
		: function_base()
	{}
	/*!
	\see LWG 2781 。
	\see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=65760 。
	\see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66284 。
	*/
	template<typename _fCallable, yimpl(typename
		= exclude_self_t<function_base, _fCallable>), yimpl(typename
		= enable_if_t<is_invocable_r<_tRet, _fCallable&, _tParams...>::value>)>
	function_base(_fCallable f)
	{
		if(ystdex::function_not_empty(f))
			yunseq(content = any(std::move(f)),
				// XXX: Here lambda-expression is buggy in G++ LTO.
				p_invoke = invoker<any_ops::value_handler<_fCallable>>::invoke);
		else
			_tTraits::init_empty(content, p_invoke);
	}
	//! \ingroup YBase_replacement_extensions
	//@{
	//! \since build 926
	template<typename _fCallable, yimpl(typename
		= exclude_self_t<function_base, _fCallable>), yimpl(typename
		= enable_if_t<is_invocable_r<_tRet, _fCallable&, _tParams...>::value>)>
	function_base(trivial_swap_t, _fCallable f)
	{
		if(ystdex::function_not_empty(f))
			yunseq(content = any(trivial_swap, std::move(f)),
				// XXX: Here lambda-expression is buggy in G++ LTO.
				p_invoke = invoker<any_ops::value_handler<_fCallable,
				any_ops::is_in_place_storable<_fCallable, true_>>>::invoke);
		else
			_tTraits::init_empty(content, p_invoke);
	}
	template<typename _fCallable, class _tAlloc, yimpl(typename
		= exclude_self_t<function_base, _fCallable>), yimpl(typename
		= enable_if_t<is_invocable_r<_tRet, _fCallable&, _tParams...>::value>)>
	function_base(std::allocator_arg_t, const _tAlloc& a, _fCallable f)
	{
		if(ystdex::function_not_empty(f))
			yunseq(content = any(std::allocator_arg, a, std::move(f)),
				// XXX: Here lambda-expression is buggy in G++ LTO.
				p_invoke = invoker<any::allocated_value_handler_t<_tAlloc,
				_fCallable>>::invoke);
		else
			_tTraits::init_empty(content, p_invoke);
	}
	//! \since build 926
	template<typename _fCallable, class _tAlloc, yimpl(typename
		= exclude_self_t<function_base, _fCallable>), yimpl(typename
		= enable_if_t<is_invocable_r<_tRet, _fCallable&, _tParams...>::value>)>
	function_base(std::allocator_arg_t, const _tAlloc& a, trivial_swap_t,
		_fCallable f)
	{
		if(ystdex::function_not_empty(f))
			yunseq(content = any(std::allocator_arg, a, trivial_swap,
				std::move(f)),
				// XXX: Here lambda-expression is buggy in G++ LTO.
				p_invoke = invoker<any::allocated_value_handler_t<_tAlloc,
				_fCallable, true_>>::invoke);
		else
			_tTraits::init_empty(content, p_invoke);
	}
	//@}
	function_base(const function_base&) = default;
	/*!
	\ingroup YBase_replacement_extensions
	\throw allocator_mismatch_error 分配器不兼容。
	*/
	template<class _tAlloc>
	function_base(std::allocator_arg_t, const _tAlloc& a,
		const function_base& x)
		: content(std::allocator_arg, a, x), p_invoke(x.p_invoke)
	{}
	// NOTE: The 'ynothrow' is presented as a conforming extension to ISO C++17,
	//	as well as a conforming extension of WG21 P0288.
	// XXX: The moved-from value is valid but unspecified. It is empty here to
	//	simplify the implementation (as libstdc++ does), which is still not
	//	guaranteed by the interface.
	function_base(function_base&&) ynothrow = default;
	/*!
	\ingroup YBase_replacement_extensions
	\throw allocator_mismatch_error 分配器不兼容。
	*/
	template<class _tAlloc>
	function_base(std::allocator_arg_t, const _tAlloc& a, function_base&& x)
		: content(std::allocator_arg, a, std::move(x)), p_invoke(x.p_invoke)
	{}

	function_base&
	operator=(nullptr_t) ynothrow
	{
		content = any();
		_tTraits::init_empty(content, p_invoke);
		return *this;
	}
	//! \since build 849
	template<typename _fCallable, yimpl(typename
		= exclude_self_t<function_base, _fCallable>), yimpl(typename
		= enable_if_t<is_invocable_r<_tRet, _fCallable&, _tParams...>::value>)>
	function_base&
	operator=(_fCallable&& f)
	{
		// NOTE: Efficiency consideration is simliar to %any::operator=, as
		//	construction of %p_invoke here is simple enough.
		return *this = function_base(std::move(f));
	}
	template<typename _fCallable>
	function_base&
	operator=(std::reference_wrapper<_fCallable> f) ynothrow
	{
		// NOTE: Ditto.
		return *this = function_base(f);
	}
	// NOTE: Ditto.
	function_base&
	operator=(const function_base&) yimpl(= default);
	// NOTE: Ditto.
	function_base&
	operator=(function_base&& f) ynothrow yimpl(= default);

	YB_ATTR_nodiscard YB_PURE friend bool
	operator==(const function_base& f, nullptr_t) ynothrow
	{
		return !f;
	}

	/*!
	\pre 策略指定不检查时断言： \c bool(*this) 。
	\throw std::bad_function_call ：策略指定使用异常，且对象表示空函数。
	*/
	_tRet
	operator()(_tParams... args) const
	{
		return _tTraits::call(content, p_invoke, yforward(args)...);
	}

	YB_ATTR_nodiscard YB_PURE explicit
	operator bool() const ynothrow
	{
		return content.has_value();
	}

	//! \see LWG 2062 。
	void
	swap(function_base& f) ynothrow
	{
		std::swap(p_invoke, f.p_invoke),
		content.swap(f.content);
	}
	friend void
	swap(function_base& x, function_base& y) ynothrow
	{
		return x.swap(y);
	}

	template<typename _type>
	YB_ATTR_nodiscard YB_PURE _type*
	target() ynothrow
	{
		return content.template try_get_object_ptr<_type>();
	}
	template<typename _type>
	YB_ATTR_nodiscard YB_PURE const _type*
	target() const ynothrow
	{
		return content.template try_get_object_ptr<_type>();
	}

	YB_ATTR_nodiscard YB_PURE const type_info&
	target_type() const ynothrow
	{
		return content.type();
	}
	//@}
};

//! \relates function_base
//@{
//! \since build 851
template<class _tTraits, typename _fSig>
struct make_parameter_list<function_base<_tTraits, _fSig>>
	: make_parameter_list<_fSig>
{};

template<class _tTraits, typename _fSig>
struct return_of<function_base<_tTraits, _fSig>> : return_of<_fSig>
{};

//! \since build 851
//@{
template<class _tTraits, typename _tFrom, typename _tTo,
	typename... _tFromParams, typename... _tToParams>
struct is_covariant<function_base<_tTraits, _tFrom(_tFromParams...)>,
	function_base<_tTraits, _tTo(_tToParams...)>>
	: is_covariant<_tFrom(_tFromParams...), _tTo(_tToParams...)>
{};

template<class _tTraits, typename _tResFrom, typename _tResTo,
	typename... _tFromParams, typename... _tToParams>
struct is_contravariant<function_base<_tTraits, _tResFrom(_tFromParams...)>,
	function_base<_tTraits, _tResTo(_tToParams...)>>
	: is_contravariant<_tResFrom(_tFromParams...), _tResTo(_tToParams...)>
{};
//@}

//! \since build 878
template<class _tTraits, typename _tRet, typename _tUnused,
	typename... _tParams>
struct make_function_type<_tRet, function_base<_tTraits, _tUnused(_tParams...)>>
	: make_function_type<_tRet, empty_base<_tParams...>>
{};

//! \since build 849
template<class _tTraits, typename _tRet, typename... _tParams, size_t... _vSeq>
struct call_projection<function_base<_tTraits, _tRet(_tParams...)>,
	index_sequence<_vSeq...>>
	: call_projection<_tRet(_tParams...), index_sequence<_vSeq...>>
{};
//@}

//! \since build 926
template<class _tTraits, typename _tRet, typename... _tParams>
struct is_bitwise_swappable<function_base<_tTraits, _tRet(_tParams...)>> : true_
{};
//@}


//! \since build 849
//@{
template<typename _fSig>
using unchecked_function = function_base<function_traits<_fSig,
	empty_function_policy::no_check>, _fSig>;

template<typename _fSig>
using optional_function = function_base<function_traits<_fSig,
	empty_function_policy::no_effect>, _fSig>;

/*!
\ingroup YBase_replacement_features
\brief 函数包装类模板。
\see ISO C++17 [func.wrap.func] 。
\see WG21 P0288 。
*/
template<typename _fSig>
using function = function_base<function_traits<_fSig,
	empty_function_policy::throwing>, _fSig>;
//@}

} // namespace ystdex;

#endif

