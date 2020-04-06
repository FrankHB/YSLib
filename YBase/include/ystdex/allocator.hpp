/*
	© 2014-2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file allocator.hpp
\ingroup YStandardEx
\brief 分配器接口。
\version r5613
\author FrankHB <frankhb1989@gmail.com>
\since build 882
\par 创建时间:
	2020-02-10 21:34:28 +0800
\par 修改时间:
	2020-04-06 15:02 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Allocator

扩展标准库头 \c \<memory> ，提供对分配器使用相关的接口。
*/


#ifndef YB_INC_ystdex_allocator_hpp_
#define YB_INC_ystdex_allocator_hpp_ 1

#include "placement.hpp" // for internal "placement.hpp", <memory>,
//	std::pair, false_, true_, std::allocator_arg, is_constructible, not_, and_,
//	is_explicitly_constructible, is_implicitly_constructible, enable_if_t,
//	detected_or_t, is_unqualified_object, is_copy_constructible, is_class_type,
//	is_same, std::pointer_traits, std::declval, is_detected, detected_t,
//	is_detected_exact, cond, remove_cvref_t, bool_, is_lvalue_reference,
//	exclude_self_t;
#include "apply.hpp" // for std::tuple, std::forward_as_tuple,
//	std::piecewise_construct_t, std::piecewise_construct, std::make_tuple,
//	ystdex::apply;
#include "type_op.hpp" // for internal "type_op.hpp", cond_or_t;
#include "pointer.hpp" // for ystdex::swap_dependent, ystdex::to_address;
#include "exception.h" // for throw_invalid_construction;

/*!
\brief \c \<memory> 特性测试宏。
\see WG21 P0941R2 2.2 。
\see LWG 2108 。
\see LWG 2467 。
\see https://blogs.msdn.microsoft.com/vcblog/2016/01/22/vs-2015-update-2s-stl-is-c17-so-far-feature-complete/ 。
\since build 834
*/
//@{
#ifndef __cpp_lib_allocator_traits_is_always_equal
#	if YB_IMPL_MSCPP >= 1900 || __cplusplus >= 201411L
#		define __cpp_lib_allocator_traits_is_always_equal 201411L
#	endif
#endif
//@}

/*!	\defgroup allocators Allcators
\brief 分配器。
\see WG21 N4606 17.6.3.5 [allocator.requirements] 。
\since build 746
*/

namespace ystdex
{

//! \since build 830
#if __cpp_lib_allocator_traits_is_always_equal >= 201411L \
	|| __cplusplus >= 201411L
#	define YB_Impl_Has_allocator_traits_is_always_equal true
#else
#	define YB_Impl_Has_allocator_traits_is_always_equal false
#endif


#if __cplusplus > 201703L && (!defined(__GLIBCXX__) || __GLIBCXX__ > 20190731)
// XXX: See https://github.com/cplusplus/draft/issues/3111.
// XXX: See https://gcc.gnu.org/viewcvs/gcc/trunk/libstdc++-v3/include/std/memory?r1=273515&r2=273945.
//! \since build 863
//@{
using std::uses_allocator_construction_args;
using std::make_obj_using_allocator;
using std::uninitialized_construct_using_allocator;
//@}
#else
/*!
\see WG21 P0591R4 。
\since build 850
*/
//@{
namespace details
{

template<typename _type>
struct is_pair : false_
{};

template<typename _type1, typename _type2>
struct is_pair<std::pair<_type1, _type2>> : true_
{};


template<typename _type, class _tAlloc>
struct pair_has_allocator : std::uses_allocator<_type, _tAlloc>
{};

template<typename _type1, typename _type2, class _tAlloc>
struct pair_has_allocator<std::pair<_type1, _type2>, _tAlloc>
	: or_<pair_has_allocator<_type1, _tAlloc>,
	pair_has_allocator<_type2, _tAlloc>>
{};

template<typename, class, typename...>
struct uaca_res;

template<typename _type, class _tAlloc, typename... _tParams>
using uaca_res_t = _t<uaca_res<_type, _tAlloc, _tParams...>>;

template<typename _type, class _tAlloc>
struct uaa_func
{
	const _tAlloc& a;

	template<typename... _tParams>
	YB_ATTR_nodiscard YB_FLATTEN inline auto
	operator()(_tParams&&...) const -> uaca_res_t<_type, _tAlloc, _tParams...>;
};


template<typename, class _bPair, class _bPfx, class _tAlloc,
	typename... _tParams>
YB_ATTR_nodiscard YB_FLATTEN inline std::tuple<_tParams&&...>
uses_allocator_args(_bPair, false_, _bPfx, const _tAlloc&, _tParams&&... args)
{
	return std::forward_as_tuple(yforward(args)...);
}
template<typename, class _tAlloc, typename... _tParams>
YB_ATTR_nodiscard YB_FLATTEN inline
	std::tuple<const std::allocator_arg_t&, const _tAlloc&, _tParams&&...>
uses_allocator_args(false_, true_, true_, const _tAlloc& a, _tParams&&... args)
{
	return std::forward_as_tuple(std::allocator_arg, a, yforward(args)...);
}
template<typename, class _tAlloc, typename... _tParams>
YB_ATTR_nodiscard YB_FLATTEN inline
	std::tuple<_tParams&&..., const _tAlloc&>
uses_allocator_args(false_, true_, false_, const _tAlloc& a, _tParams&&... args)
{
	return std::forward_as_tuple(yforward(args)..., a);
}
template<class _type, class _tAlloc>
YB_ATTR_nodiscard YB_FLATTEN inline auto
uses_allocator_args(true_, true_, false_, const _tAlloc&)
	-> uaca_res_t<_type, _tAlloc, const std::piecewise_construct_t&,
	std::tuple<>, std::tuple<>>;
template<typename _type, class _tAlloc, typename _type1, typename _type2>
YB_ATTR_nodiscard YB_FLATTEN inline auto
uses_allocator_args(true_, true_, false_, const _tAlloc&,
	const std::pair<_type1, _type2>&)
	-> uaca_res_t<_type, _tAlloc, const std::piecewise_construct_t&,
	std::tuple<const _type1&>, std::tuple<const _type2&>>;
template<typename _type, class _tAlloc, typename _type1, typename _type2>
YB_ATTR_nodiscard YB_FLATTEN inline auto
uses_allocator_args(true_, true_, false_, const _tAlloc& a,
	std::pair<_type1, _type2>&&)
	-> uaca_res_t<_type, _tAlloc, const std::piecewise_construct_t&,
	std::tuple<_type1&&>, std::tuple<_type2&&>>;
template<typename _type, class _tAlloc, typename _type1, typename _type2>
YB_ATTR_nodiscard YB_FLATTEN inline auto
uses_allocator_args(true_, true_, false_, const _tAlloc& a, _type1&&, _type2&&)
	-> uaca_res_t<_type, _tAlloc, const std::piecewise_construct_t&,
	std::tuple<_type1&&>, std::tuple<_type2&&>>;
//! \since build 861
template<typename _type, class _tAlloc, class _tTuple1, class _tTuple2>
YB_ATTR_nodiscard YB_FLATTEN inline auto
uses_allocator_args(true_, true_, false_, const _tAlloc& a,
	std::piecewise_construct_t, _tTuple1&& x, _tTuple2&& y) -> decltype(
	std::make_tuple(std::piecewise_construct, ystdex::apply(
	uaa_func<remove_cv_t<typename _type::first_type>, _tAlloc>{a}, yforward(x)),
	ystdex::apply(uaa_func<remove_cv_t<typename _type::second_type>,
	_tAlloc>{a}, yforward(y))))
{
	return std::make_tuple(std::piecewise_construct, ystdex::apply(uaa_func<
		remove_cv_t<typename _type::first_type>, _tAlloc>{a}, yforward(x)),
		ystdex::apply(uaa_func<remove_cv_t<typename _type::second_type>,
		_tAlloc>{a}, yforward(y)));
}

} // unnamed namespace details;

// XXX: Blocked. 'yforward' in the trailing-return-type cause G++ 5.3 20151204
//	crash: internal compiler error: Segmentation fault.
template<typename _type, class _tAlloc, typename... _tParams>
YB_ATTR_nodiscard YB_FLATTEN inline auto
uses_allocator_construction_args(const _tAlloc& a, _tParams&&... args) -> yimpl(
	decltype(details::uses_allocator_args<_type>(details::is_pair<_type>(),
	details::pair_has_allocator<_type, _tAlloc>(), is_constructible<_type,
	std::allocator_arg_t, _tAlloc, _tParams...>(), a,
	std::forward<_tParams&&>(args)...)))
{
	return details::uses_allocator_args<_type>(details::is_pair<_type>(),
		details::pair_has_allocator<_type, _tAlloc>(), is_constructible<_type,
		std::allocator_arg_t, _tAlloc, _tParams...>(), a, yforward(args)...);
}

namespace details
{

template<typename _type, class _tAlloc, typename... _tParams>
struct uaca_res
{
	using type = decltype(ystdex::uses_allocator_construction_args<_type>(
		std::declval<const _tAlloc&>(), std::declval<_tParams>()...));
};

template<typename _type, class _tAlloc>
	template<typename... _tParams>
YB_ATTR_nodiscard YB_FLATTEN inline auto
uaa_func<_type, _tAlloc>::operator()(_tParams&&... args) const
	-> uaca_res_t<_type, _tAlloc, _tParams...>
{
	return
		ystdex::uses_allocator_construction_args<_type>(a, yforward(args)...);
}

template<class _type, class _tAlloc>
YB_ATTR_nodiscard YB_FLATTEN inline auto
uses_allocator_args(true_, true_, false_, const _tAlloc& a) -> uaca_res_t<_type,
	_tAlloc, const std::piecewise_construct_t&, std::tuple<>, std::tuple<>>
{
	return ystdex::uses_allocator_construction_args<_type>(a,
		std::piecewise_construct, std::tuple<>(), std::tuple<>());
}
template<typename _type, class _tAlloc, typename _type1, typename _type2>
YB_ATTR_nodiscard YB_FLATTEN inline auto
uses_allocator_args(true_, true_, false_, const _tAlloc& a,
	const std::pair<_type1, _type2>& arg)
	-> uaca_res_t<_type, _tAlloc, const std::piecewise_construct_t&,
	std::tuple<const _type1&>, std::tuple<const _type2&>>
{
	return ystdex::uses_allocator_construction_args<_type>(a,
		std::piecewise_construct, std::forward_as_tuple(arg.first),
		std::forward_as_tuple(arg.second));
}
template<typename _type, class _tAlloc, typename _type1, typename _type2>
YB_ATTR_nodiscard YB_FLATTEN inline auto
uses_allocator_args(true_, true_, false_, const _tAlloc& a,
	std::pair<_type1, _type2>&& arg)
	-> uaca_res_t<_type, _tAlloc, const std::piecewise_construct_t&,
	std::tuple<_type1&&>, std::tuple<_type2&&>>
{
	return ystdex::uses_allocator_construction_args<_type>(a,
		std::piecewise_construct, std::forward_as_tuple(yforward(arg.first)),
		std::forward_as_tuple(yforward(arg.second)));
}
template<typename _type, class _tAlloc, typename _type1, typename _type2>
YB_ATTR_nodiscard YB_FLATTEN inline auto
uses_allocator_args(true_, true_, false_, const _tAlloc& a, _type1&& arg1,
	_type2&& arg2)
	-> uaca_res_t<_type, _tAlloc, const std::piecewise_construct_t&,
	std::tuple<_type1&&>, std::tuple<_type2&&>>
{
	return ystdex::uses_allocator_construction_args<_type>(a,
		std::piecewise_construct, std::forward_as_tuple(yforward(arg1)),
		std::forward_as_tuple(yforward(arg2)));
}

template<typename _type, class _tAlloc>
struct ucua_func
{
	void* pv;

	template<typename... _tParams>
	_type*
	operator()(_tParams&&... args) const
		ynoexcept(is_nothrow_constructible<_type, _tParams...>())
	{
		return ::new(pv) _type(yforward(args)...);
	}
};

// TODO: Detect more implemenations?
// XXX: This is a workaround to implementation without support of WG21 N4387.
#	if (defined(__GLIBCXX__) && (__GLIBCXX__ <= 20150630 \
	|| YB_IMPL_GNUCPP < 60000)) && !(_LIBCPP_VERSION > 4000 \
	|| __cplusplus >= 201703L)
// NOTE: See See https://gcc.gnu.org/viewcvs/gcc/trunk/libstdc++-v3/include/bits/stl_pair.h?r1=225189&r2=225188&pathrev=225189,
//	and https://reviews.llvm.org/rL276605.
template<typename _type1, typename _type2, class _tAlloc>
struct ucua_func<std::pair<_type1, _type2>, _tAlloc>
{
	void* pv;

	template<typename... _tParams>
	auto
	operator()(_tParams&&... args) const
		-> decltype(::new(pv) std::pair<_type1, _type2>(yforward(args)...))
	{
		return ::new(pv) std::pair<_type1, _type2>(yforward(args)...);
	}
	template<typename _tParam1, typename _tParam2>
	enable_if_t<and_<not_<is_constructible<_tParam1, _tParam2>>,
		is_explicitly_constructible<_type1, _tParam1>,
		is_implicitly_constructible<_type2, _tParam2>>::value,
		std::pair<_type1, _type2>*>
	operator()(_tParam1&& arg1, _tParam2&& arg2) const
	{
		return ::new(pv)
			std::pair<_type1, _type2>(_type1(arg1), yforward(arg2));
	}
	template<typename _tParam1, typename _tParam2>
	enable_if_t<and_<not_<is_constructible<_tParam1, _tParam2>>,
		is_implicitly_constructible<_type1, _tParam1>,
		is_explicitly_constructible<_type2, _tParam2>>::value,
		std::pair<_type1, _type2>*>
	operator()(_tParam1&& arg1, _tParam2&& arg2) const
	{
		return ::new(pv)
			std::pair<_type1, _type2>(yforward(arg1), _type2(arg2));
	}
	template<typename _tParam1, typename _tParam2>
	enable_if_t<and_<not_<is_constructible<_tParam1, _tParam2>>,
		is_explicitly_constructible<_type1, _tParam1>,
		is_explicitly_constructible<_type2, _tParam2>>::value,
		std::pair<_type1, _type2>*>
	operator()(_tParam1&& arg1, _tParam2&& arg2) const
	{
		return ::new(pv)
			std::pair<_type1, _type2>(_type1(arg1), _type2(arg2));
	}
};
#	endif

} // namespace details;

template<typename _type, class _tAlloc, typename... _tParams>
YB_ATTR_nodiscard _type
make_obj_using_allocator(const _tAlloc& a, _tParams&&... args)
{
	return ystdex::make_from_tuple<_type>(
		ystdex::uses_allocator_construction_args<_type>(a, yforward(args)...));
}

template<typename _type, class _tAlloc, typename... _tParams>
_type*
uninitialized_construct_using_allocator(_type* p, const _tAlloc& a,
	_tParams&&... args)
{
	return ystdex::apply(details::ucua_func<_type, _tAlloc>{p},
		ystdex::uses_allocator_construction_args<_type>(a, yforward(args)...));
}
//@}
#endif


//! \since build 595
namespace details
{

//! \since build 830
//@{
#if !YB_Impl_Has_allocator_traits_is_always_equal
template<typename _type, typename... _tParams>
using mem_is_always_equal_t = typename _type::is_always_equal;
#endif
//@}

} // namespace details;

//! \since build 595
inline namespace cpp2017
{

#if YB_Impl_Has_allocator_traits_is_always_equal
using std::allocator_traits;
#else
template<class _tAlloc>
struct allocator_traits : std::allocator_traits<_tAlloc>
{
	using is_always_equal = detected_or_t<is_empty<_tAlloc>,
		details::mem_is_always_equal_t, _tAlloc>;
};
#endif

} // inline namespace cpp2017;


/*!
\ingroup unary_type_traits
\since build 848
*/
template<class _tAlloc>
using alloc_value_t = typename allocator_traits<_tAlloc>::value_type;

//! \since build 867
//@{
template<class _tAlloc>
using alloc_pointer_t = typename allocator_traits<_tAlloc>::pointer;

template<class _tAlloc>
using alloc_const_pointer_t = typename allocator_traits<_tAlloc>::const_pointer;

template<class _tAlloc>
using alloc_size_type_t = typename allocator_traits<_tAlloc>::size_type;

template<class _tAlloc>
using alloc_difference_type_t
	= typename allocator_traits<_tAlloc>::difference_type;
//@}

/*!
\ingroup binary_type_traits
\since build 848
*/
//@{
template<class _tAlloc, typename _type>
using rebind_alloc_t
	= typename allocator_traits<_tAlloc>::template rebind_alloc<_type>;

template<class _tAlloc, typename _type>
using rebind_traits_t
	= typename allocator_traits<_tAlloc>::template rebind_traits<_type>;
//@}


/*!
\ingroup unary_type_traits
\brief 判断类型是否符合分配器要求的目标类型。
\note _type 可以是不完整类型。
\see LWG 2447 。
\since build 650
*/
template<typename _type>
struct is_allocatable : is_unqualified_object<_type>
{};


//! \since build 595
namespace details
{

//! \since build 746
//@{
template<typename _type, typename... _tParams>
using mem_new_t
	= decltype(_type::operator new(std::declval<_tParams>()...));

template<typename _type, typename... _tParams>
using mem_delete_t
	= decltype(_type::operator delete(std::declval<_tParams>()...));
//@}

template<typename _type>
using is_copy_constructible_class
	= and_<is_copy_constructible<_type>, is_class_type<_type>>;

//! \since build 867
//@{
template<class _tAlloc>
using mem_pointer_t = typename _tAlloc::pointer;

template<class _tAlloc>
using mem_const_pointer_t = typename _tAlloc::const_pointer;

template<class _tAlloc>
using mem_const_pointer_conv_t = is_convertible<mem_pointer_t<_tAlloc>,
	mem_const_pointer_t<_tAlloc>>;

template<class _tAlloc>
using mem_void_pointer_t = typename _tAlloc::void_pointer;

template<class _tAlloc>
using mem_void_pointer_conv_t = is_convertible<mem_pointer_t<_tAlloc>,
	mem_void_pointer_t<_tAlloc>>;

template<class _tAlloc>
using mem_const_void_pointer_t = typename _tAlloc::const_void_pointer;

template<class _tAlloc>
using mem_const_void_pointer_conv_t = and_<is_convertible<mem_pointer_t<
	_tAlloc>, mem_const_void_pointer_t<_tAlloc>>,
	is_convertible<mem_const_pointer_t<_tAlloc>,
	mem_const_void_pointer_t<_tAlloc>>, is_convertible<
	mem_void_pointer_t<_tAlloc>, mem_const_void_pointer_t<_tAlloc>>>;

template<class _tAlloc>
using same_pointer_to_test_t
	= is_same<decltype(std::pointer_traits<mem_pointer_t<_tAlloc>>::pointer_to(
	*std::declval<mem_pointer_t<_tAlloc>>())), mem_pointer_t<_tAlloc>>;

template<class _tAlloc>
using same_pointer_to_t = cond_or_t<is_detected<mem_pointer_t, _tAlloc>, true_,
	same_pointer_to_test_t, _tAlloc>;

template<class _tAlloc>
using mem_allocate_t = decltype(std::declval<_tAlloc&>().allocate(size_t(0)));

template<class _tAlloc, typename _tPointer = decltype(
	std::declval<typename allocator_traits<_tAlloc>::const_void_pointer>())>
using ator_allocate_hint_t = decltype(allocator_traits<_tAlloc>::allocate(
	std::declval<_tAlloc&>(), size_t(0), std::declval<_tPointer>()));

template<class _tAlloc>
using mem_deallocate_t = decltype(std::declval<_tAlloc&>().deallocate(
	std::declval<mem_allocate_t<_tAlloc>>(), size_t(0)));

template<class _tAlloc>
using ator_max_size_t
	= decltype(allocator_traits<_tAlloc>::max_size(std::declval<_tAlloc&>()));

// XXX: The member %details::mem_value_type_t is in %type_op.hpp.
template<class _type, typename _tPointer = detected_t<alloc_pointer_t,
	_type>, typename _tSize = detected_t<alloc_size_type_t, _type>,
	typename _tDifference = detected_t<alloc_difference_type_t, _type>>
struct check_allocator_members : and_<cond_or_t<is_detected<
	mem_const_pointer_t, _type>, true_, mem_const_pointer_conv_t, _type>,
	cond_or_t<is_detected<mem_void_pointer_t, _type>, true_,
	mem_void_pointer_conv_t, _type>, cond_or_t<is_detected<
	mem_const_void_pointer_t, _type>, true_, mem_const_void_pointer_conv_t,
	_type>, is_unsigned<_tSize>, is_signed<_tDifference>,
	same_pointer_to_t<_type>,
	is_detected_exact<_tPointer, mem_allocate_t, _type>,
	is_detected_exact<_tPointer, ator_allocate_hint_t, _type>,
	is_detected<mem_deallocate_t, _type>,
	is_detected_exact<_tSize, ator_max_size_t, _type>>
{};

template<typename _type,
	typename _tValue = detected_or_t<void, mem_value_type_t, _type>>
struct is_allocator
	: cond_or_t<is_allocatable<_tValue>, false_, check_allocator_members, _type>
{};

template<typename _type, typename _tValue>
using same_alloc_value_t = is_same<alloc_value_t<_type>, _tValue>;
//@}

//! \since build 650
template<typename _type>
using nested_allocator_t = typename _type::allocator_type;

//! \since build 887
//@{
template<typename _type>
using mem_get_allocator_t
	= decltype(std::declval<const _type&>().get_allocator());

template<typename _type,
	typename _tAlloc = remove_cvref_t<mem_get_allocator_t<_type>>>
struct check_get_allocator : bool_<is_allocator<_tAlloc>::value
	&& noexcept(std::declval<const _type&>().get_allocator())>
{};
//@}

} // namespace details;

/*!
\ingroup type_traits_operations
\since build 746
*/
//@{
template<typename _type, typename... _tParams>
struct has_mem_new : is_detected<details::mem_new_t, _type, _tParams...>
{};

template<typename _type, typename... _tParams>
struct has_mem_delete : is_detected<details::mem_delete_t, _type, _tParams...>
{};
//@}


/*!
\pre _type 是非类类型或完整的类类型。
\note 可能有假阳性结果。
\see ISO C++17 [allocator.requirements] 。
\since build 867
*/
//@{
/*!
\ingroup unary_type_traits
\brief 判断类型是分配器。

判断分配器，检查符合以下要求：
成员类型 value_type 是没有被 cv 修饰的（可能不完整的）对象类型；
不涉及其它分配器、 操作符、容器传播或 is_always_equal 的其它表达式符合类型要求；
可以符合分配器要求的值调用 allocate 、deallocator 和 max_size 。
因为 ADL ，带有非限定形式操作符不能被可靠地检查而支持不完整的 value_type 。
*/
template<typename _type>
struct is_allocator : details::is_allocator<_type>
{};


/*!
\ingroup binary_type_traits
\pre _tValue 是（可能不完整的）对象类型。
\brief 判断类型是指定值类型的分配器。
*/
template<typename _type, typename _tValue>
struct is_allocator_for : cond_or_t<is_allocator<_type>, false_,
	details::same_alloc_value_t, _type, _tValue>
{};


/*!
\ingroup unary_type_traits
\brief 判断类型是 byte 分配器。
*/
template<typename _type>
struct is_byte_allocator : is_allocator_for<_type, byte>
{};
//@}


//! \since build 650
//@{
/*!
\ingroup unary_type_traits
\brief 判断类型具有嵌套的成员 allocator_type 指称一个构造器类型。
\note 若嵌套的类型是满足具有 value_type 的可复制构造类类型，则视为构造器类型。
*/
template<typename _type>
struct has_nested_allocator
	: is_allocator<detected_t<details::nested_allocator_t, _type>>
{};


/*!
\ingroup metafunctions
\brief 取嵌套成员分配器类型，若不存在则使用第二模板参数指定的默认类型。
*/
template<typename _type, class _tDefault = std::allocator<_type>>
struct nested_allocator : cond<has_nested_allocator<_type>,
	detected_t<details::nested_allocator_t, _type>, _tDefault>
{};
//@}


/*!
\brief 启用 allocator_type 的基类。
\warning 非虚析构。
\since build 844
*/
//@{
template<class _tCon, bool = has_nested_allocator<_tCon>::value>
struct nested_allocator_base
{
	using allocator_type = typename _tCon::allocator_type;
};

template<class _tCon>
struct nested_allocator_base<_tCon, false>
{};
//@}


/*!
\ingroup unary_type_traits
\brief 判断类型具有返回分配器的无异常抛出的 get_allocator 成员函数。
\since build 887
*/
template<typename _type>
struct has_get_allocator : cond_or_t<is_detected<details::mem_get_allocator_t>,
	false_, details::check_get_allocator, _type>
{};


namespace details
{

//! \since build 846
template<class _tAlloc>
class allocator_delete_base
{
protected:
	using ator_traits = allocator_traits<remove_cvref_t<_tAlloc>>;
	using stored_ator_t
		= cond_t<is_lvalue_reference<_tAlloc>, lref<_tAlloc>, _tAlloc>;

	//! \since build 843
	mutable stored_ator_t alloc;

	template<class _tParam,
		yimpl(typename = exclude_self_t<allocator_delete_base, _tParam>)>
	allocator_delete_base(_tParam&& a) ynothrow
		: alloc(yforward(a))
	{}
	allocator_delete_base(const allocator_delete_base&) = default;
	allocator_delete_base(allocator_delete_base&&) = default;

	allocator_delete_base&
	operator=(const allocator_delete_base&) = default;
	allocator_delete_base&
	operator=(allocator_delete_base&&) = default;

	//! \since build 887
	YB_ATTR_nodiscard YB_PURE _tAlloc&
	get_allocator() const ynothrow
	{
		return alloc;
	}
};


//! \since build 843
//@{
template<typename _tAlloc>
yconstfn_relaxed void
do_alloc_on_copy(_tAlloc&, const _tAlloc&, false_) ynothrow
{}
template<typename _tAlloc>
yconstfn_relaxed void
do_alloc_on_copy(_tAlloc& x, const _tAlloc& y, true_) ynoexcept_spec(x = y)
{
	x = y;
}

template<typename _tAlloc>
yconstfn_relaxed void
do_alloc_on_move(_tAlloc&, _tAlloc&, false_) ynothrow
{}
/*!
\pre 参数指定的类型不在转移时抛出异常而退出。
\since build 865
*/
template<typename _tAlloc>
yconstfn_relaxed void
do_alloc_on_move(_tAlloc& x, _tAlloc& y, true_) ynothrow
{
	// NOTE: As per ISO C++17 [allocator.requirements], this shall exit via
	//	an exception.
	x = std::move(y);
}

template<typename _tAlloc>
yconstfn_relaxed void
do_alloc_on_swap(_tAlloc&, _tAlloc&, false_) ynothrow
{}
template<typename _tAlloc>
yconstfn_relaxed void
do_alloc_on_swap(_tAlloc& x, _tAlloc& y, true_)
	ynoexcept_spec(ystdex::swap_dependent(x, y))
{
	ystdex::swap_dependent(x, y);
}
//@}

} // namespace details;


/*!
\note 模板参数可能是引用。
\see $2018-12 @ %Documentation::Workflow.
*/
//@{
/*!
\brief 释放分配器分配的存储资源的删除器。
\since build 846
*/
template<class _tAlloc>
class allocator_guard_delete : private details::allocator_delete_base<_tAlloc>
{
private:
	//! \since build 846
	using base = details::allocator_delete_base<_tAlloc>;
	//! \since build 846
	using typename base::ator_traits;

public:
	//! \since build 595
	using pointer = typename ator_traits::pointer;
	//! \since build 595
	using size_type = typename ator_traits::size_type;

private:
	//! \since build 843
	size_type count;

public:
	//! \since build 843
	allocator_guard_delete(_tAlloc a, size_type n = 1) ynothrow
		: base(yforward(a)), count(n)
	{}
	allocator_guard_delete(const allocator_guard_delete&) = default;
	allocator_guard_delete(allocator_guard_delete&&) = default;

	allocator_guard_delete&
	operator=(const allocator_guard_delete&) = default;
	allocator_guard_delete&
	operator=(allocator_guard_delete&&) = default;

	//! \since build 595
	void
	operator()(pointer p) const ynothrowv
	{
		ator_traits::deallocate(base::alloc, p, count);
	}

	//! \since build 846
	yimpl(using) base::get_allocator;

	//! \since build 887
	YB_ATTR_nodiscard YB_PURE size_type
	get_count() const ynothrow
	{
		return count;
	}
};


/*!
\brief 释放分配器分配的对象的删除器。
\since build 595
*/
template<class _tAlloc>
class allocator_delete : private details::allocator_delete_base<_tAlloc>
{
private:
	//! \since build 846
	using base = details::allocator_delete_base<_tAlloc>;
	//! \since build 846
	using typename base::ator_traits;

public:
	//! \since build 595
	using pointer = typename ator_traits::pointer;

	//! \since build 847
	template<class _tParam,
		yimpl(typename = exclude_self_t<allocator_delete, _tParam>)>
	allocator_delete(_tParam&& a) ynothrow
		: base(yforward(a))
	{}
	//! \since build 843
	//@{
	allocator_delete(const allocator_delete&) = default;
	allocator_delete(allocator_delete&&) = default;

	allocator_delete&
	operator=(const allocator_delete&) = default;
	allocator_delete&
	operator=(allocator_delete&&) = default;
	//@}

	void
	operator()(pointer p) const ynothrowv
	{
		ator_traits::destroy(base::alloc, ystdex::to_address(p));
		ator_traits::deallocate(base::alloc, p, 1);
	}

	//! \since build 846
	yimpl(using) base::get_allocator;
};
//@}


/*!
\brief 分配器守卫。
\since build 843
*/
template<class _tAlloc>
using allocator_guard
	= std::unique_ptr<alloc_value_t<_tAlloc>, allocator_guard_delete<_tAlloc>>;


/*!
\brief 构造分配器守卫。
\since build 843
*/
template<class _tAlloc>
YB_ATTR_nodiscard inline allocator_guard<_tAlloc>
make_allocator_guard(_tAlloc& a,
	typename allocator_traits<_tAlloc>::size_type n = 1)
{
	return allocator_guard<_tAlloc>(allocator_traits<_tAlloc>::allocate(a, n),
		allocator_guard_delete<_tAlloc>(a, n));
}


/*!
\brief 使用分配器创建对象。
\return 分配的对象指针。
\since build 849
*/
template<typename _type, class _tAlloc, typename... _tParams>
YB_ATTR_nodiscard auto
create_with_allocator(_tAlloc&& a, _tParams&&... args)
	-> std::unique_ptr<_type, allocator_delete<remove_cvref_t<_tAlloc>>>
{
	using ator_t = remove_cvref_t<_tAlloc>;
	using ator_del_t = allocator_delete<ator_t>;
	auto gd(ystdex::make_allocator_guard(a));

	allocator_traits<ator_t>::construct(a, gd.get(), yforward(args)...);
	return std::unique_ptr<_type, ator_del_t>(gd.release(), ator_del_t(a));
}


/*!
\brief 使用分配器创建 std::unique_ptr 。
\see WG21 P0316R0 。
\since build 847
*/
template<typename _type, class _tAlloc, typename... _tParams>
YB_ATTR_nodiscard auto
allocate_unique(const _tAlloc& alloc, _tParams&&... args)
	-> std::unique_ptr<_type, allocator_delete<rebind_alloc_t<_tAlloc, _type>>>
{
	rebind_alloc_t<_tAlloc, _type> a(alloc);

	return ystdex::create_with_allocator<_type>(a, yforward(args)...);
}
/*!
\brief 使用分配器和初始化列表参数创建 std::unique_ptr 。
\since build 851
*/
template<typename _type, class _tAlloc, typename _tValue>
YB_ATTR_nodiscard auto
allocate_unique(const _tAlloc& alloc, std::initializer_list<_tValue> il)
	-> std::unique_ptr<_type, allocator_delete<rebind_alloc_t<_tAlloc, _type>>>
{
	rebind_alloc_t<_tAlloc, _type> a(alloc);

	return ystdex::create_with_allocator<_type>(a, il);
}

/*!
\brief 使用分配器创建 std::shared_ptr 。
\note 仅实现 WG21 P0674R1 和 ISO C++17 的交集，类似 libstdc++ 。
\see LWG 2070 。
\see https://developercommunity.visualstudio.com/content/problem/417142/lwg-2070p0674r1-stdallocate-shared-is-not-conformi.html
\since build 849
\todo 实现 WG21 P0674R1 。
\todo 在 ISO C++20 出版后更新 __cplusplus 的值。
*/
#if __cplusplus > 201703L || (__GLIBCXX__ && !(__GLIBCXX__ <= 20111108))
// NOTE: See https://gcc.gnu.org/viewcvs/gcc/trunk/libstdc++-v3/include/bits/shared_ptr_base.h?r1=181171&r2=181170&pathrev=181171.
// NOTE: LWG 2070 is not implemented by libc++ yet.
using std::allocate_shared;
#else
template<typename _type, class _tAlloc, typename... _tParams>
YB_ATTR_nodiscard std::shared_ptr<_type>
allocate_shared(const _tAlloc& alloc, _tParams&&... args)
{
	return std::shared_ptr<_type>(
		ystdex::allocate_unique<_type>(alloc, yforward(args)...));
}
#endif
/*!
\brief 使用分配器和初始化列表参数创建 std::shared_ptr 。
\since build 851
*/
template<typename _type, class _tAlloc, typename _tValue>
YB_ATTR_nodiscard std::shared_ptr<_type>
allocate_shared(const _tAlloc& alloc, std::initializer_list<_tValue> il)
{
	return std::shared_ptr<_type>(ystdex::allocate_unique<_type>(alloc, il));
}


/*!
\ingroup allocators
\since build 746
*/
//@{
/*!
\brief 类分配器。
\warning 非虚析构。

和 std::allocator 类似但自动检查类的成员代替 ::operator new/::operator delete 。
*/
template<class _type>
struct class_allocator : std::allocator<_type>
{
	class_allocator() = default;
	using std::allocator<_type>::allocator;
	class_allocator(const class_allocator&) = default;

	YB_ALLOCATOR YB_ATTR_returns_nonnull _type*
	allocate(size_t n)
	{
		return _type::operator new(n * sizeof(_type));
	}

	//! \since build 778
	void
	deallocate(_type* p, size_t) ynothrow
	{
		// TODO: What if the size hint is supported by %_type?
		return _type::operator delete(p);
	}
};


/*!
\brief 局部分配器。

对支持类作用域分配的类类型为 class_allocator ，否则为 std::allocator 。
*/
template<typename _type>
using local_allocator = cond_t<and_<has_mem_new<_type, size_t>,
	has_mem_delete<_type*>>, class_allocator<_type>, std::allocator<_type>>;
//@}


//! \since build 830
//@{
//! \brief 按分配器特征在传播容器时复制赋值分配器。
template<typename _tAlloc>
yconstfn_relaxed void
alloc_on_copy(_tAlloc& x, const _tAlloc& y) ynothrow
{
	details::do_alloc_on_copy(x, y, typename allocator_traits<
		_tAlloc>::propagate_on_container_copy_assignment());
}
/*!
\brief 按分配器特征在传播容器时复制分配器。
\since build 830
*/
template<typename _tAlloc>
YB_ATTR_nodiscard yconstfn _tAlloc
alloc_on_copy(const _tAlloc& a)
{
	// NOTE: As per ISO C++17 [allocator.requirements]/4, this shall not throw
	// exceptions.
	return allocator_traits<_tAlloc>::select_on_container_copy_construction(a);
}

//! \brief 按分配器特征在传播容器时转移分配器。
template<typename _tAlloc>
yconstfn_relaxed void
alloc_on_move(_tAlloc& x, _tAlloc& y) ynothrow
{
	// NOTE: As per ISO C++17 [allocator.requirements]/4, this shall not throw
	// exceptions. Even without this requirement, it still shall not exit via an
	//	exception. See %details::do_alloc_on_move for the rationale.
	details::do_alloc_on_move(x, y, typename
		allocator_traits<_tAlloc>::propagate_on_container_move_assignment());
}

//! \brief 按分配器特征在传播容器时交换分配器。
template<typename _tAlloc>
yconstfn_relaxed void
alloc_on_swap(_tAlloc& x, _tAlloc& y) ynothrow
{
	// NOTE: As per ISO C++17 [allocator.requirements]/4, this shall not throw
	// exceptions.
	details::do_alloc_on_swap(x, y,
		typename allocator_traits<_tAlloc>::propagate_on_container_swap());
}
//@}

#undef YB_Impl_Has_allocator_traits_is_always_equal

} // namespace ystdex;

#endif

