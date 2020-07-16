/*
	© 2010-2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file invoke.hpp
\ingroup YStandardEx
\brief 可调用对象和调用包装接口。
\version r4617
\author FrankHB <frankhb1989@gmail.com>
\since build 832
\par 创建时间:
	2018-07-24 05:03:12 +0800
\par 修改时间:
	2020-07-17 01:25 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Invoke
*/


#ifndef YB_INC_ystdex_invoke_hpp_
#define YB_INC_ystdex_invoke_hpp_ 1

#include "meta.hpp" // for "meta.hpp", <type_traits>, std::declval,
//	__cpp_lib_is_invocable, void_t, false_, true_, remove_cvref, is_base_of,
//	or_, is_void, is_same_or_convertible, is_implicitly_nothrow_constructible,
//	__cpp_lib_invoke, and_;
#include "variadic.hpp" // for vseq::apply_t, vseq::_a, conditional_t;
#include <functional> // for <functional>, std::reference_wrapper;
#include "cassert.h" // for yconstraint;

/*!
\brief \<functional\> 特性测试宏。
\see WG21 P0941R2 2.2 。
\see https://blogs.msdn.microsoft.com/vcblog/2015/06/19/c111417-features-in-vs-2015-rtm/ 。
\since build 679
*/
//@{
#ifndef __cpp_lib_transparent_operators
#	if YB_IMPL_MSCPP >= 1800 || __cplusplus >= 201210L
#		define __cpp_lib_transparent_operators 201210L
#	endif
#endif
#ifndef __cpp_lib_invoke
#	if YB_IMPL_MSCPP >= 1900 || __cplusplus >= 201411L
#		define __cpp_lib_invoke 201411L
#	endif
#endif
//@}

namespace ystdex
{

/*!
\brief 伪输出对象。
\note 吸收所有赋值操作。
\since build 273
*/
struct pseudo_output
{
	//! \since build 690
	//@{
	template<typename... _tParams>
	yconstfn
	pseudo_output(_tParams&&...) ynothrow
	{}

	template<typename _tParam,
		yimpl(exclude_self_t<pseudo_output, _tParam>)>
	yconstfn const pseudo_output&
	operator=(_tParam&&) const ynothrow
	{
		return *this;
	}
	template<typename... _tParams>
	yconstfn const pseudo_output&
	operator()(_tParams&&...) const ynothrow
	{
		return *this;
	}
	//@}
};


/*!
\ingroup metafunctions
\since build 636
\see 关于相关的核心语言特性： WG21 P0146R0 。
*/
//@{
//! \brief 若类型不是空类型则取后备结果类型（默认为 pseudo_output ）。
template<typename _type, typename _tRes = pseudo_output>
using nonvoid_result_t = cond_t<not_<is_void<_type>>, _type, pseudo_output>;

//! \brief 若类型不是对象类型则取后备结果类型（默认为 pseudo_output ）。
template<typename _type, typename _tRes = pseudo_output>
using object_result_t = cond_t<is_object<_type>, _type, pseudo_output>;
//@}


//! \since build 832
namespace details
{

//! \since build 612
//@{
template<typename _type>
struct member_target_type_impl
{
	using type = void;
};

template<class _tClass, typename _type>
struct member_target_type_impl<_type _tClass::*>
{
	using type = _tClass;
};
//@}

} // namespace details;

//! \ingroup transformation_traits
//@{
/*!
\brief 取成员指针类型指向的类类型。
\since build 612
*/
template<typename _type>
using member_target_type_t = _t<details::member_target_type_impl<_type>>;

/*!
\brief INVOKE 调用分派。
\note 不含 std::reference_wrapper 处理。
\see ISO C++17 [func.require] 。
\since build 832
*/
//@{
template<typename _fCallable, typename _type, typename... _tParams>
using invoke_mem_fn_ref_t = decltype((std::declval<_type>().*std::declval<
	_fCallable>())(std::declval<_tParams>()...));

template<typename _fCallable, typename _type, typename... _tParams>
using invoke_mem_fn_deref_t = decltype(((*std::declval<_type>()).*std::declval<
	_fCallable>())(std::declval<_tParams>()...));

template<typename _fCallable, typename _type>
using invoke_mem_obj_ref_t
	= decltype(std::declval<_type>().*std::declval<_fCallable>());

template<typename _fCallable, typename _type>
using invoke_mem_obj_deref_t
	= decltype((*std::declval<_type>()).*std::declval<_fCallable>());

template<typename _fCallable, typename... _tParams>
using invoke_other_t
	= decltype(std::declval<_fCallable>()(std::declval<_tParams>()...));
//@}
//@}


/*!
\ingroup unary_type_traits
\brief 判断模板参数指定的类型是否为标注库引用包装的实例。
\since build 832
*/
//@{
template<typename>
struct is_std_reference_wrapper : false_type
{};

template<typename _type>
struct is_std_reference_wrapper<std::reference_wrapper<_type>> : true_type
{};
//@}


#if !(__cpp_lib_is_invocable >= 201703L)
//! \since build 832
namespace details
{

template<typename _type, typename _type2 = decay_t<_type>>
struct inv_unwrap
{
	using type = _type;
};

template<typename _type, typename _type2>
struct inv_unwrap<_type, std::reference_wrapper<_type2>>
{
	using type = _type2&;
};

template<typename _type>
using inv_unwrap_t = _t<inv_unwrap<_type>>;


// XXX: The 'ref' and 'deref' variants of tempaltes cannot be combined because
//	it might cause redefinitions of specializations. Note the bug can be hardly
//	detected, however 'clang++-7 -std=gnu++11' (not '-std=c++11') complaints
//	this even when not collided anyway, perhaps due to missing of resolution of
//	CWG 1558.
//! \since build 845
//@{
template<typename, typename>
struct inv_is_mem_ref;

template<typename _tRet, class _tClass, typename _tParam>
struct inv_is_mem_ref<_tRet _tClass::*, _tParam>
	: is_base_of<_tClass, remove_cvref_t<_tParam>>
{};


//! \since build 850
template<typename _fCallable, typename... _tParams>
using fproto = empty_base<_fCallable, _tParams...>;

template<class, bool, bool, typename = void>
struct inv_mem_ptr
{
	//! \since build 850
	static const yconstexpr bool noexcept_v = {};
};

//! \since build 850
//@{
template<typename _tMemPtr, typename _type, typename... _tParams>
struct inv_mem_ptr<fproto<_tMemPtr, _type, _tParams...>, true, true,
	void_t<invoke_mem_fn_ref_t<_tMemPtr, _type, _tParams...>>>
	: identity<invoke_mem_fn_ref_t<_tMemPtr, _type, _tParams...>>
{
	static const yconstexpr bool noexcept_v = ynoexcept((std::declval<
		inv_unwrap_t<_type>>().*std::declval<_tMemPtr>())(
		std::declval<_tParams>()...));
};

template<typename _tMemPtr, typename _type, typename... _tParams>
struct inv_mem_ptr<fproto<_tMemPtr, _type, _tParams...>, true, false,
	void_t<invoke_mem_fn_deref_t<_tMemPtr, _type, _tParams...>>>
	: identity<invoke_mem_fn_deref_t<_tMemPtr, _type, _tParams...>>
{
	static const yconstexpr bool noexcept_v = ynoexcept(((*std::declval<
		_type>()).*std::declval<_tMemPtr>())(std::declval<_tParams>()...));
};

template<typename _tMemPtr, typename _type>
struct inv_mem_ptr<fproto<_tMemPtr, _type>, false, true,
	void_t<invoke_mem_obj_ref_t<_tMemPtr, _type>>>
	: identity<invoke_mem_obj_ref_t<_tMemPtr, _type>>
{
	static const yconstexpr bool noexcept_v = ynoexcept(
		std::declval<inv_unwrap_t<_type>>().*std::declval<_tMemPtr>());
};

template<typename _tMemPtr, typename _type>
struct inv_mem_ptr<fproto<_tMemPtr, _type>, false, false,
	void_t<invoke_mem_obj_deref_t<_tMemPtr, _type>>>
	: identity<invoke_mem_obj_deref_t<_tMemPtr, _type>>
{
	static const yconstexpr bool noexcept_v
		= ynoexcept((*std::declval<_type>()).*std::declval<_tMemPtr>());
};
//@}
//@}

template<class, typename = void>
struct inv_other
{
	//! \since build 850
	static const yconstexpr bool noexcept_v = {};
};

template<typename _fCallable, typename... _tParams>
struct inv_other<fproto<_fCallable, _tParams...>,
	void_t<invoke_other_t<_fCallable, _tParams...>>>
	: identity<invoke_other_t<_fCallable, _tParams...>>
{
	static const yconstexpr bool noexcept_v
		= ynoexcept(std::declval<_fCallable>()(std::declval<_tParams>()...));
};


//! \since build 845
//@{
template<typename _tMemPtr, typename _tParam, typename... _tParams>
struct inv_mem : inv_mem_ptr<fproto<_tMemPtr, inv_unwrap_t<_tParam>,
	_tParams...>, is_member_function_pointer<_tMemPtr>::value,
	inv_is_mem_ref<_tMemPtr, _tParam>::value>
{};


template<bool, typename _fCallable, typename... _tParams>
struct inv_test : inv_other<fproto<_fCallable, _tParams...>>
{};

template<typename _fCallable, typename _tParam, typename... _tParams>
struct inv_test<true, _fCallable, _tParam, _tParams...>
	: inv_mem<decay_t<_fCallable>, _tParam, _tParams...>
{};
//@}


template<typename _fCallable, typename... _tParams>
using inv_result = inv_test<is_member_pointer<
	remove_reference_t<_fCallable>>::value, _fCallable, _tParams...>;

template<typename _fCallable, typename... _tParams>
using inv_result_t = _t<inv_result<_fCallable, _tParams...>>;


template<typename, class, typename = void>
struct inv_enabled : false_
{};

//! \since build 850
template<typename _tRet, typename _fCallable, typename... _tParams>
struct inv_enabled<_fCallable, fproto<_tRet, _tParams...>,
	void_t<inv_result_t<_fCallable, _tParams...>>>
	: or_<is_void<_tRet>,
	is_same_or_convertible<inv_result_t<_fCallable, _tParams...>, _tRet>>
{};

template<typename, typename, typename = void>
struct inv_enabled_nt : false_
{};

//! \since build 850
template<typename _tRet, typename _fCallable, typename... _tParams>
struct inv_enabled_nt<_fCallable, fproto<_tRet, _tParams...>,
	void_t<inv_result_t<_fCallable, _tParams...>>>
	: or_<is_void<_tRet>, is_implicitly_nothrow_constructible<_tRet,
	inv_result_t<_fCallable, _tParams...>>>
{};

} // namespace details;
#endif

inline namespace cpp2017
{

//! \since build 832
//@{
#if __cpp_lib_is_invocable >= 201703L
using std::is_invocable;
using std::is_invocable_r;
using std::is_nothrow_invocable;
using std::is_nothrow_invocable_r;

using std::invoke_result;
using std::invoke_result_t;
#else
//! \ingroup binary_type_traits
//@{
template<typename _fCallable, typename... _tParams>
struct is_invocable
	: details::inv_enabled<_fCallable, details::fproto<void, _tParams...>>
{};

template<typename _tRet, typename _fCallable, typename... _tParams>
struct is_invocable_r
	: details::inv_enabled<_fCallable, details::fproto<_tRet, _tParams...>>
{};

template<typename _fCallable, typename... _tParams>
struct is_nothrow_invocable
	: bool_constant<is_invocable<_fCallable, _tParams...>::value
	&& details::inv_result<_fCallable, _tParams...>::noexcept_v>
{};

template<typename _tRet, typename _fCallable, typename... _tParams>
struct is_nothrow_invocable_r : bool_constant<details::inv_enabled_nt<
	_fCallable, details::fproto<_tRet, _tParams...>>::value
	&& details::inv_result<_fCallable, _tParams...>::noexcept_v>
{};
//@}


//! \ingroup transformation_traits
//@{
template<typename _fCallable, typename... _tParams>
struct invoke_result
	: details::inv_result<_fCallable, _tParams...>
{};

template<typename _fCallable, typename... _tParams>
using invoke_result_t = _t<invoke_result<_fCallable, _tParams...>>;
//@}
#endif
//@}

} // inline namespace cpp2017;


#if __cpp_lib_invoke >= 201411L
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

//! \since build 832
template<typename _fCallable, typename _type>
struct is_callable_case2a : and_<is_callable_case2<_fCallable, _type>,
	is_std_reference_wrapper<decay_t<_type>>>
{};

//! \since build 832
template<typename _fCallable, typename _type>
struct is_callable_case2b : and_<is_callable_case2<_fCallable, _type>,
	not_<is_std_reference_wrapper<decay_t<_type>>>>
{};

template<typename _fCallable, typename _type>
struct is_callable_case3 : and_<is_member_object_pointer<_fCallable>,
	is_callable_target<_type, _fCallable>>
{};

template<typename _fCallable, typename _type>
struct is_callable_case4 : and_<is_member_object_pointer<_fCallable>,
	not_<is_callable_target<_type, _fCallable>>>
{};

//! \since build 832
template<typename _fCallable, typename _type>
struct is_callable_case4a : and_<is_callable_case4<_fCallable, _type>,
	is_std_reference_wrapper<decay_t<_type>>>
{};

//! \since build 832
template<typename _fCallable, typename _type>
struct is_callable_case4b : and_<is_callable_case4<_fCallable, _type>,
	not_<is_std_reference_wrapper<decay_t<_type>>>>
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
//! \since build 832
template<typename _fCallable, typename _type, typename... _tParams>
yconstfn auto
invoke_impl(_fCallable&& f, _type&& obj, _tParams&&... args)
	-> enable_if_t<is_callable_case2a<decay_t<_fCallable>, _type>::value,
	decltype((obj.get().*f)(yforward(args)...))>
{
	return yconstraint(f), (obj.get().*f)(yforward(args)...);
}
//! \since build 832
template<typename _fCallable, typename _type, typename... _tParams>
yconstfn auto
invoke_impl(_fCallable&& f, _type&& obj, _tParams&&... args)
	-> enable_if_t<is_callable_case2b<decay_t<_fCallable>, _type>::value,
	decltype(((*yforward(obj)).*f)(yforward(args)...))>
{
	return yconstraint(f), ((*yforward(obj)).*f)(yforward(args)...);
}
//! \since build 763
template<typename _fCallable, typename _type>
YB_PURE yconstfn auto
invoke_impl(_fCallable&& f, _type&& obj)
	-> enable_if_t<is_callable_case3<decay_t<_fCallable>, _type>::value,
	decltype(yforward(obj).*f)>
{
	return yconstraint(f), yforward(obj).*f;
}
//! \since build 832
template<typename _fCallable, typename _type>
yconstfn auto
invoke_impl(_fCallable&& f, _type&& obj)
	-> enable_if_t<is_callable_case4a<decay_t<_fCallable>, _type>::value,
	decltype(obj.get().*f)>
{
	return yconstraint(f), obj.get().*f;
}
//! \since build 832
template<typename _fCallable, typename _type>
yconstfn auto
invoke_impl(_fCallable&& f, _type&& obj)
	-> enable_if_t<is_callable_case4b<decay_t<_fCallable>, _type>::value,
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
\note 和 ISO C++17 的 std::invoke 兼容，只处理 std::wrapper_reference 为引用包装。
\sa http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4169.html
\see ISO C++17 [func.require] ， ISO C++ 17 [func.invoke] 。
\see CWG 1581 。
\see LWG 2013 。
\see LWG 2219 。
\see WG21 P0604R0 。
\see https://llvm.org/bugs/show_bug.cgi?id=23141 。
\since build 832
*/
template<typename _fCallable, typename... _tParams>
yimpl(yconstfn) invoke_result_t<_fCallable, _tParams...>
invoke(_fCallable&& f, _tParams&&... args)
	ynoexcept(is_nothrow_invocable<_fCallable, _tParams...>::value)
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
//! \since build 832
template<typename _fCallable, typename... _tParams>
inline invoke_result_t<_fCallable, _tParams...>
invoke_nonvoid_impl(false_, _fCallable&& f, _tParams&&... args)
{
	return ystdex::invoke(yforward(f), yforward(args)...);
}

} // namespace details;

/*!
\brief 调用可调用对象，保证返回值非空。
\since build 832
\todo 支持引用包装。
*/
template<typename _fCallable, typename... _tParams>
yimpl(yconstfn) nonvoid_result_t<invoke_result_t<_fCallable, _tParams...>>
invoke_nonvoid(_fCallable&& f, _tParams&&... args)
{
	return details::invoke_nonvoid_impl(is_void<invoke_result_t<
		_fCallable, _tParams...>>(), yforward(f), yforward(args)...);
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

} // namespace ystdex;

#endif

