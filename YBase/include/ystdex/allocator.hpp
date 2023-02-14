/*
	© 2014-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file allocator.hpp
\ingroup YStandardEx
\brief 分配器接口。
\version r6218
\author FrankHB <frankhb1989@gmail.com>
\since build 882
\par 创建时间:
	2020-02-10 21:34:28 +0800
\par 修改时间:
	2023-02-13 20:35 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Allocator

扩展标准库头 \c \<memory> ，提供对分配器使用相关的接口。
*/


#ifndef YB_INC_ystdex_allocator_hpp_
#define YB_INC_ystdex_allocator_hpp_ 1

#include "compressed_pair.hpp" // for internal "placement.hpp", <memory>,
//	false_, std::pair, true_, std::uses_allocator, std::allocator_arg,
//	is_constructible, not_, and_, is_explicitly_constructible,
//	is_implicitly_constructible, enable_if_t, detected_or_t,
//	is_unqualified_object, is_copy_constructible, is_class_type, is_same,
//	std::pointer_traits, std::declval, is_detected, detected_t,
//	is_detected_exact, cond, remove_cvref_t, bool_, is_lvalue_reference, cond_t,
//	remove_reference_t, compressed_pair, compressed_pair_element,
//	enable_if_convertible_t, enable_if_constructible_t, is_assignable,
//	remove_cv_t, exclude_self_t, ystdex::copy_assign, is_bitwise_swappable,
//	is_nothrow_copy_constructible, is_nothrow_default_constructible;
#include "apply.hpp" // for std::tuple, std::forward_as_tuple,
//	std::piecewise_construct_t, std::piecewise_construct, std::make_tuple,
//	ystdex::apply;
#include "type_op.hpp" // for internal "type_op.hpp", cond_or_t;
#include "pointer.hpp" // for internal "pointer.hpp", ystdex::swap_dependent,
//	ystdex::to_address;
#include "exception.h" // for throw_invalid_construction;

/*!
\brief \c \<memory> 特性测试宏。
\see ISO C++20 [version.syn] 。
\see WG21 P0941R2 2.2 。
\see LWG 2108 。
\see LWG 2467 。
\see https://blogs.msdn.microsoft.com/vcblog/2016/01/22/vs-2015-update-2s-stl-is-c17-so-far-feature-complete/ 。
\since build 834
*/
//!@{
#ifndef __cpp_lib_allocator_traits_is_always_equal
#	if YB_IMPL_MSCPP >= 1900 || __cplusplus >= 201411L
#		define __cpp_lib_allocator_traits_is_always_equal 201411L
#	endif
#endif
//!@}

namespace ystdex
{

//! \since build 830
#if __cpp_lib_allocator_traits_is_always_equal >= 201411L \
	|| __cplusplus >= 201411L
#	define YB_Impl_has_allocator_traits_is_always_equal true
#else
#	define YB_Impl_has_allocator_traits_is_always_equal false
#endif


#if __cplusplus >= 202002L && (!defined(__GLIBCXX__) || __GLIBCXX__ > 20190731)
// XXX: See https://github.com/cplusplus/draft/issues/3111.
// XXX: See https://gcc.gnu.org/git/?p=gcc.git;a=blobdiff;f=libstdc%2B%2B-v3/include/std/memory;h=0a483d2d8d1a1287685cb5cc8a7d338a14e7fef3;hp=3036802f8c3eb1c3013dc1720ad85087e5202694;hb=3090082cbefd8b1374f237bd4242b554490b2933;hpb=e6c847fb8f90b1c119a677b81cfc294b13eb7772.
// XXX: The source of libstdc++ uses '__cpp_lib_make_obj_using_allocator'
//	internally in headers like <memory_resource> and <scoped_allocator> to
//	detect the existence of the features. This is not in ISO C++ or proposals
//	and not used here.
//! \since build 863
//!@{
using std::uses_allocator_construction_args;
using std::make_obj_using_allocator;
using std::uninitialized_construct_using_allocator;
//!@}
#else
/*!
\see WG21 P0591R4 。
\since build 850
*/
//!@{
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
	YB_ATTR_nodiscard yconstfn auto
	operator()(_tParams&&...) const -> uaca_res_t<_type, _tAlloc, _tParams...>;
};


template<typename, class _bPair, class _bPfx, class _tAlloc,
	typename... _tParams>
YB_ATTR_nodiscard yconstfn std::tuple<_tParams&&...>
uses_allocator_args(_bPair, false_, _bPfx, const _tAlloc&, _tParams&&... args)
{
	return std::forward_as_tuple(yforward(args)...);
}
template<typename, class _tAlloc, typename... _tParams>
YB_ATTR_nodiscard yconstfn
	std::tuple<const std::allocator_arg_t&, const _tAlloc&, _tParams&&...>
uses_allocator_args(false_, true_, true_, const _tAlloc& a, _tParams&&... args)
{
	return std::forward_as_tuple(std::allocator_arg, a, yforward(args)...);
}
template<typename, class _tAlloc, typename... _tParams>
YB_ATTR_nodiscard yconstfn
	std::tuple<_tParams&&..., const _tAlloc&>
uses_allocator_args(false_, true_, false_, const _tAlloc& a, _tParams&&... args)
{
	return std::forward_as_tuple(yforward(args)..., a);
}
template<class _type, class _tAlloc>
YB_ATTR_nodiscard yconstfn auto
uses_allocator_args(true_, true_, false_, const _tAlloc&)
	-> uaca_res_t<_type, _tAlloc, const std::piecewise_construct_t&,
	std::tuple<>, std::tuple<>>;
template<typename _type, class _tAlloc, typename _type1, typename _type2>
YB_ATTR_nodiscard yconstfn auto
uses_allocator_args(true_, true_, false_, const _tAlloc&,
	const std::pair<_type1, _type2>&)
	-> uaca_res_t<_type, _tAlloc, const std::piecewise_construct_t&,
	std::tuple<const _type1&>, std::tuple<const _type2&>>;
template<typename _type, class _tAlloc, typename _type1, typename _type2>
YB_ATTR_nodiscard yconstfn auto
uses_allocator_args(true_, true_, false_, const _tAlloc& a,
	std::pair<_type1, _type2>&&)
	-> uaca_res_t<_type, _tAlloc, const std::piecewise_construct_t&,
	std::tuple<_type1&&>, std::tuple<_type2&&>>;
template<typename _type, class _tAlloc, typename _type1, typename _type2>
YB_ATTR_nodiscard yconstfn auto
uses_allocator_args(true_, true_, false_, const _tAlloc& a, _type1&&, _type2&&)
	-> uaca_res_t<_type, _tAlloc, const std::piecewise_construct_t&,
	std::tuple<_type1&&>, std::tuple<_type2&&>>;
//! \since build 861
template<typename _type, class _tAlloc, class _tTuple1, class _tTuple2>
YB_ATTR_nodiscard yconstfn auto
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

} // namespace details;

// XXX: Blocked. 'yforward' in the trailing-return-type cause G++ 5.3 20151204
//	crash: internal compiler error: Segmentation fault.
template<typename _type, class _tAlloc, typename... _tParams>
YB_ATTR_nodiscard yconstfn auto
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
YB_ATTR_nodiscard yconstfn auto
uaa_func<_type, _tAlloc>::operator()(_tParams&&... args) const
	-> uaca_res_t<_type, _tAlloc, _tParams...>
{
	return
		ystdex::uses_allocator_construction_args<_type>(a, yforward(args)...);
}

template<class _type, class _tAlloc>
YB_ATTR_nodiscard yconstfn auto
uses_allocator_args(true_, true_, false_, const _tAlloc& a) -> uaca_res_t<_type,
	_tAlloc, const std::piecewise_construct_t&, std::tuple<>, std::tuple<>>
{
	return ystdex::uses_allocator_construction_args<_type>(a,
		std::piecewise_construct, std::tuple<>(), std::tuple<>());
}
template<typename _type, class _tAlloc, typename _type1, typename _type2>
YB_ATTR_nodiscard yconstfn auto
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
YB_ATTR_nodiscard yconstfn auto
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
YB_ATTR_nodiscard yconstfn auto
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
	yconstfn _type*
	operator()(_tParams&&... args) const
		ynoexcept(is_nothrow_constructible<_type, _tParams...>())
	{
		return ::new(pv) _type(yforward(args)...);
	}
};

// XXX: This is a workaround to implementation without support of WG21 N4387.
#	if (defined(__GLIBCXX__) && (__GLIBCXX__ <= 20150630 \
	|| YB_IMPL_GNUCPP < 60000)) && !(_LIBCPP_VERSION > 4000 \
	|| __cplusplus >= 201703L)
// NOTE: See https://gcc.gnu.org/git/?p=gcc.git;a=blobdiff;f=libstdc%2B%2B-v3/include/bits/stl_pair.h;h=6672ecb227377d199df830bc475f6ebecd03beed;hp=490b00565a0560f9f15f86bb706fa8f3800ee026;hb=bf7818bfb0c70e626f8b71501ada3263f306a476;hpb=ddb63209a8dc059a7b2a137d7a1859222bb43dd6,
//	and https://reviews.llvm.org/rL276605.
template<typename _type1, typename _type2, class _tAlloc>
struct ucua_func<std::pair<_type1, _type2>, _tAlloc>
{
	void* pv;

	template<typename... _tParams>
	yconstfn auto
	operator()(_tParams&&... args) const
		-> decltype(::new(pv) std::pair<_type1, _type2>(yforward(args)...))
	{
		return ::new(pv) std::pair<_type1, _type2>(yforward(args)...);
	}
	template<typename _tParam1, typename _tParam2>
	yconstfn enable_if_t<and_<not_<is_constructible<_tParam1, _tParam2>>,
		is_explicitly_constructible<_type1, _tParam1>,
		is_implicitly_constructible<_type2, _tParam2>>{},
		std::pair<_type1, _type2>*>
	operator()(_tParam1&& arg1, _tParam2&& arg2) const
	{
		return ::new(pv)
			std::pair<_type1, _type2>(_type1(arg1), yforward(arg2));
	}
	template<typename _tParam1, typename _tParam2>
	yconstfn enable_if_t<and_<not_<is_constructible<_tParam1, _tParam2>>,
		is_implicitly_constructible<_type1, _tParam1>,
		is_explicitly_constructible<_type2, _tParam2>>{},
		std::pair<_type1, _type2>*>
	operator()(_tParam1&& arg1, _tParam2&& arg2) const
	{
		return ::new(pv)
			std::pair<_type1, _type2>(yforward(arg1), _type2(arg2));
	}
	template<typename _tParam1, typename _tParam2>
	yconstfn enable_if_t<and_<not_<is_constructible<_tParam1, _tParam2>>,
		is_explicitly_constructible<_type1, _tParam1>,
		is_explicitly_constructible<_type2, _tParam2>>{},
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
YB_ATTR_nodiscard yconstfn _type
make_obj_using_allocator(const _tAlloc& a, _tParams&&... args)
{
	return ystdex::make_from_tuple<_type>(
		ystdex::uses_allocator_construction_args<_type>(a, yforward(args)...));
}

template<typename _type, class _tAlloc, typename... _tParams>
yconstfn _type*
uninitialized_construct_using_allocator(_type* p, const _tAlloc& a,
	_tParams&&... args)
{
	return ystdex::apply(details::ucua_func<_type, _tAlloc>{p},
		ystdex::uses_allocator_construction_args<_type>(a, yforward(args)...));
}
//!@}
#endif


//! \since build 595
namespace details
{

//! \since build 830
//!@{
#if !YB_Impl_has_allocator_traits_is_always_equal
template<typename _type, typename... _tParams>
using mem_is_always_equal_t = typename _type::is_always_equal;
#endif
//!@}

} // namespace details;

//! \since build 595
inline namespace cpp2017
{

#if YB_Impl_has_allocator_traits_is_always_equal
using std::allocator_traits;
#else
template<class _tAlloc>
struct allocator_traits : std::allocator_traits<_tAlloc>
{
	//! \see WG21 N4258 。
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
//!@{
template<class _tAlloc>
using alloc_pointer_t = typename allocator_traits<_tAlloc>::pointer;

template<class _tAlloc>
using alloc_const_pointer_t = typename allocator_traits<_tAlloc>::const_pointer;

template<class _tAlloc>
using alloc_size_type_t = typename allocator_traits<_tAlloc>::size_type;

template<class _tAlloc>
using alloc_difference_type_t
	= typename allocator_traits<_tAlloc>::difference_type;
//!@}

/*!
\ingroup binary_type_traits
\since build 848
*/
//!@{
template<class _tAlloc, typename _type>
using rebind_alloc_t
	= typename allocator_traits<_tAlloc>::template rebind_alloc<_type>;

template<class _tAlloc, typename _type>
using rebind_traits_t
	= typename allocator_traits<_tAlloc>::template rebind_traits<_type>;
//!@}


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
//!@{
template<typename _type, typename... _tParams>
using mem_new_t
	= decltype(_type::operator new(std::declval<_tParams>()...));

template<typename _type, typename... _tParams>
using mem_delete_t
	= decltype(_type::operator delete(std::declval<_tParams>()...));
//!@}

template<typename _type>
using is_copy_constructible_class
	= and_<is_copy_constructible<_type>, is_class_type<_type>>;

//! \since build 867
//!@{
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
//!@}

//! \since build 650
template<typename _type>
using nested_allocator_t = typename _type::allocator_type;

//! \since build 887
//!@{
template<typename _type>
using mem_get_allocator_t
	= decltype(std::declval<const _type&>().get_allocator());

template<typename _type,
	typename _tAlloc = remove_cvref_t<mem_get_allocator_t<_type>>>
struct check_get_allocator : bool_<is_allocator<_tAlloc>{}
	&& noexcept(std::declval<const _type&>().get_allocator())>
{};
//!@}

} // namespace details;

/*!
\ingroup type_traits_operations
\since build 746
*/
//!@{
template<typename _type, typename... _tParams>
struct has_mem_new : is_detected<details::mem_new_t, _type, _tParams...>
{};

template<typename _type, typename... _tParams>
struct has_mem_delete : is_detected<details::mem_delete_t, _type, _tParams...>
{};
//!@}


/*!
\pre _type 是非类类型或完整的类类型。
\note 不排除假阳性结果。
\see ISO C++17 [allocator.requirements] 。
\since build 867
*/
//!@{
/*!
\ingroup unary_type_traits
\brief 判断类型是分配器。

判断分配器，检查符合以下要求：
成员类型 value_type 是没有被 cv 修饰的（可能不完整的）对象类型；
不涉及其它分配器、操作符、容器传播或 is_always_equal 的其它表达式符合类型要求；
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
//!@}


//! \since build 650
//!@{
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
//!@}


/*!
\brief 启用 allocator_type 的基类。
\warning 非虚析构。
\since build 844
*/
//!@{
template<class _tCon, bool = has_nested_allocator<_tCon>{}>
struct nested_allocator_base
{
	using allocator_type = typename _tCon::allocator_type;
};

template<class _tCon>
struct nested_allocator_base<_tCon, false>
{};
//!@}


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

//! \since build 940
template<typename _tAlloc>
struct allocator_deleter_traits : allocator_traits<remove_cvref_t<_tAlloc>>
{
	// XXX: Rvalue references are not supported.
	using stored_type = cond_t<is_lvalue_reference<_tAlloc>,
		lref<remove_reference_t<_tAlloc>>, _tAlloc>;
	using size_type = typename
		allocator_traits<remove_cvref_t<_tAlloc>>::size_type;
	using guard_delete_base = compressed_pair<stored_type, size_type>;
	using delete_base = compressed_pair_element<stored_type>;
};


//! \since build 967
template<class _type, class _type2>
void
expects_equal_allocators(const _type& x, const _type2& y, false_) ynothrowv
{
	yunused(x), yunused(y);
	yverify(x.get_allocator() == y.get_allocator());
}
//! \since build 967
template<class _type, class _type2>
void
expects_equal_allocators(const _type&, const _type2&, true_) ynothrow
{}


//! \since build 843
//!@{
template<class _tAlloc>
yconstfn_relaxed void
do_alloc_on_copy(_tAlloc&, const _tAlloc&, false_) ynothrow
{}
template<class _tAlloc>
yconstfn_relaxed void
do_alloc_on_copy(_tAlloc& x, const _tAlloc& y, true_) ynoexcept_spec(x = y)
{
	x = y;
}

template<class _tAlloc>
yconstfn_relaxed void
do_alloc_on_move(_tAlloc&, _tAlloc&, false_) ynothrow
{}
/*!
\pre 参数指定的类型不在转移时抛出异常而退出。
\since build 865
*/
template<class _tAlloc>
yconstfn_relaxed void
do_alloc_on_move(_tAlloc& x, _tAlloc& y, true_) ynothrow
{
	// NOTE: As per ISO C++17 [allocator.requirements], this shall exit via
	//	an exception.
	x = std::move(y);
}

template<class _tAlloc>
yconstfn_relaxed void
do_alloc_on_swap(_tAlloc&, _tAlloc&, false_) ynothrow
{}
template<class _tAlloc>
yconstfn_relaxed void
do_alloc_on_swap(_tAlloc& x, _tAlloc& y, true_)
	ynoexcept_spec(ystdex::swap_dependent(x, y))
{
	ystdex::swap_dependent(x, y);
}
//!@}

} // namespace details;


/*!
\note 模板参数可能是引用。
\see $2018-12 @ %Documentation::Workflow.
*/
//!@{
/*!
\brief 释放分配器分配的存储资源的删除器。
\since build 846
*/
template<typename _tAlloc>
class allocator_guard_delete
	: private details::allocator_deleter_traits<_tAlloc>::guard_delete_base
{
private:
	//! \since build 965
	template<class>
	friend class allocator_guard_delete;
	//! \since build 940
	using traits_type = typename details::allocator_deleter_traits<_tAlloc>;
	//! \since build 846
	using base = typename traits_type::guard_delete_base;
	//! \since build 965
	template<class _tOtherAlloc>
	using enable_ptr_conv_t = enable_if_convertible_t<typename
		allocator_traits<_tOtherAlloc>::value_type*,
		typename traits_type::value_type*>;
	//! \since build 965
	template<typename _tParam>
	using enable_alloc_init_t = enable_if_constructible_t<_tAlloc, _tParam>;
	//! \since build 965
	template<typename _tParam>
	using enable_alloc_assign_t
		= enable_if_t<is_assignable<_tAlloc, _tParam>{}>;

public:
	//! \since build 965
	using allocator_type = remove_cv_t<_tAlloc>;
	//! \since build 595
	using pointer = typename traits_type::pointer;
	//! \since build 595
	using size_type = typename traits_type::size_type;

	//! \since build 965
	//!@{
	allocator_guard_delete() = default;
	//! \since build 937
	template<class _tParam,
		yimpl(typename = exclude_self_t<allocator_guard_delete, _tParam>)>
	inline
	allocator_guard_delete(_tParam&& a, size_type n = 1) ynothrow
		: base(yforward(a), n)
	{}
	allocator_guard_delete(const allocator_guard_delete&) = default;
	allocator_guard_delete(allocator_guard_delete&&) ynothrow = default;
	template<class _tOtherAlloc,
		yimpl(typename = enable_ptr_conv_t<_tOtherAlloc>, typename
		= enable_alloc_init_t<const remove_reference_t<_tOtherAlloc>&>)>
	inline
	allocator_guard_delete(const allocator_guard_delete<_tOtherAlloc>& other)
		ynothrow
		: base(other.first(), other.second())
	{}
	template<class _tOtherAlloc,
		yimpl(typename = enable_ptr_conv_t<_tOtherAlloc>,
		typename = enable_alloc_init_t<remove_reference_t<_tOtherAlloc>&&>)>
	inline
	allocator_guard_delete(allocator_guard_delete<_tOtherAlloc>&& other)
		ynothrow
		: base(std::move(other.first()), other.second())
	{}

	// NOTE: All assignment operators shall not throw, as required by ISO C++
	//	[unique.ptr.single.asgn]. So all assignment are specified 'ynothrow'
	//	regardless of the underlying allocator assignment, except for the
	//	explicitly defaulted move assignment operator. The defaulted one should
	//	not be declared with explicit noexcept specification, to allow it
	//	always well-formed without relying on the resolution of CWG 1778, as
	//	well as the case where it has a throwing exception but actually
	//	non-throwing.
	//! \since build 967
	//!@{
	// NOTE: The allocator may have a deleted copy %operator=.
	YB_ATTR_always_inline allocator_guard_delete&
	operator=(const allocator_guard_delete& d) ynothrow
	{
		ystdex::copy_assign(get_allocator(), d.first());
		return *this;			
	}
	// XXX: No 'ynothrow' is specified because the underlying assignment
	//	operator might be deleted.
	allocator_guard_delete&
	operator=(allocator_guard_delete&&) = default;
	// NOTE: Ditto.
	template<class _tOtherAlloc,
		yimpl(typename = enable_ptr_conv_t<_tOtherAlloc>, typename
		= enable_alloc_assign_t<const remove_reference_t<_tOtherAlloc>&>)>
	YB_ATTR_always_inline inline allocator_guard_delete&
	operator=(const allocator_guard_delete<_tOtherAlloc>& other) ynothrow
	{
		ystdex::copy_assign(get_allocator(), other.first());
		base::second_base().get_mutable() = other.second();
		return *this;
	}
	//!@}
	template<class _tOtherAlloc,
		yimpl(typename = enable_ptr_conv_t<_tOtherAlloc>, typename
		= enable_alloc_assign_t<remove_reference_t<_tOtherAlloc>&&>)>
	inline allocator_guard_delete&
	operator=(allocator_guard_delete<_tOtherAlloc>&& other) ynothrow
	{
		return *this = base(std::move(other.first()), other.second());
	}
	//!@}

	//! \since build 595
	void
	operator()(pointer p) const ynothrowv
	{
		traits_type::deallocate(get_allocator(), p, get_count());
	}

	//! \since build 846
	YB_ATTR_nodiscard YB_PURE _tAlloc&
	get_allocator() const ynothrow
	{
		return base::first_base().get_mutable();
	}

	//! \since build 887
	YB_ATTR_nodiscard YB_PURE size_type
	get_count() const ynothrow
	{
		return base::second();
	}

	// NOTE: %swap is deliberatedly omitted to allow preference on %std::swap
	//	which can support the case where the allocator may have a deleted copy
	//	%operator=.
};

/*!
\relates allocator_guard_delete
\since build 926
*/
template<class _tAlloc>
struct is_bitwise_swappable<allocator_guard_delete<_tAlloc>>
	: is_bitwise_swappable<
	typename details::allocator_deleter_traits<_tAlloc>::guard_delete_base>
{};


/*!
\brief 释放分配器分配的对象的删除器。
\note 类似 WG21 P0316R0 ，但不提供值模板类型参数和显式特化，且支持默认构造。
\since build 595
*/
template<typename _tAlloc>
class allocator_delete
	: private details::allocator_deleter_traits<_tAlloc>::delete_base
{
private:
	//! \since build 965
	template<class>
	friend class allocator_delete;
	//! \since build 940
	using traits_type = typename details::allocator_deleter_traits<_tAlloc>;
	//! \since build 846
	using base = typename traits_type::delete_base;
	//! \since build 965
	template<class _tOtherAlloc>
	using enable_ptr_conv_t = enable_if_convertible_t<typename
		allocator_traits<_tOtherAlloc>::value_type*,
		typename traits_type::value_type*>;
	//! \since build 965
	template<typename _tParam>
	using enable_alloc_init_t = enable_if_constructible_t<_tAlloc, _tParam>;
	//! \since build 965
	template<typename _tParam>
	using enable_alloc_assign_t
		= enable_if_t<is_assignable<_tAlloc, _tParam>{}>;

public:
	//! \since build 965
	using allocator_type = remove_cv_t<_tAlloc>;
	//! \since build 595
	using pointer = typename traits_type::pointer;

	//! \since build 965
	//!@{
	allocator_delete() = default;
	//! \since build 847
	template<class _tParam,
		yimpl(typename = exclude_self_t<allocator_delete, _tParam>)>
	inline
	allocator_delete(_tParam&& a) ynothrow
		: base(yforward(a))
	{}
	allocator_delete(const allocator_delete&) = default;
	allocator_delete(allocator_delete&&) ynothrow = default;
	template<class _tOtherAlloc,
		yimpl(typename = enable_ptr_conv_t<_tOtherAlloc>, typename
		= enable_alloc_init_t<const remove_reference_t<_tOtherAlloc>&>)>
	inline
	allocator_delete(const allocator_delete<_tOtherAlloc>& other) ynothrow
		: base(other.get())
	{}
	template<class _tOtherAlloc,
		yimpl(typename = enable_ptr_conv_t<_tOtherAlloc>,
		typename = enable_alloc_init_t<remove_reference_t<_tOtherAlloc>&&>)>
	inline
	allocator_delete(allocator_delete<_tOtherAlloc>&& other) ynothrow
		: base(std::move(other.get_mutable()))
	{}

	// NOTE: As %allocator_guard_delete.
	//! \since build 967
	//!@{
	// NOTE: The allocator may have a deleted copy %operator=.
	YB_ATTR_always_inline allocator_delete&
	operator=(const allocator_delete& d) ynothrow
	{
		ystdex::copy_assign(get_allocator(), d.get());
		return *this;			
	}
	allocator_delete&
	operator=(allocator_delete&&) = default;
	// NOTE: Ditto.
	template<class _tOtherAlloc,
		yimpl(typename = enable_ptr_conv_t<_tOtherAlloc>, typename
		= enable_alloc_assign_t<const remove_reference_t<_tOtherAlloc>&>)>
	YB_ATTR_always_inline inline allocator_delete&
	operator=(const allocator_delete<_tOtherAlloc>& other) ynothrow
	{
		ystdex::copy_assign(get_allocator(), other.get());
		return *this;
	}
	//!@}
	template<class _tOtherAlloc,
		yimpl(typename = enable_ptr_conv_t<_tOtherAlloc>, typename
		= enable_alloc_assign_t<remove_reference_t<_tOtherAlloc>&&>)>
	inline allocator_delete&
	operator=(allocator_delete<_tOtherAlloc>&& other) ynothrow
	{
		return *this = base(std::move(other.get_mutable()));
	}
	//!@}

	void
	operator()(pointer p) const ynothrowv
	{
		traits_type::destroy(get_allocator(), ystdex::to_address(p));
		traits_type::deallocate(get_allocator(), p, 1);
	}

	//! \since build 846
	YB_ATTR_nodiscard YB_PURE _tAlloc&
	get_allocator() const ynothrow
	{
		return base::get_mutable();
	}

	// NOTE: As %allocator_guard_delete, %swap is omitted. This is not the same
	//	to WG21 P0316R0.
};
//!@}

/*!
\relates allocator_guard_delete
\since build 926
*/
template<typename _tAlloc>
struct is_bitwise_swappable<allocator_delete<_tAlloc>> : is_bitwise_swappable<
	typename details::allocator_deleter_traits<_tAlloc>::delete_base>
{};


/*!
\brief 分配器守卫。
\since build 843
*/
template<typename _tAlloc>
using allocator_guard = std::unique_ptr<alloc_value_t<
	remove_reference_t<_tAlloc>>, allocator_guard_delete<_tAlloc>>;


/*!
\brief 构造分配器守卫。
\since build 843
*/
template<typename _tAlloc>
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
template<typename _type, typename _tAlloc, typename... _tParams>
YB_ATTR_nodiscard auto
create_with_allocator(_tAlloc&& a, _tParams&&... args)
	-> std::unique_ptr<_type, allocator_delete<remove_cvref_t<_tAlloc>>>
{
	using ator_t = remove_cvref_t<_tAlloc>;
	using ator_del_t = allocator_delete<ator_t>;
	auto gd(ystdex::make_allocator_guard(a));

	allocator_traits<ator_t>::construct(a, ystdex::to_address(gd.get()),
		yforward(args)...);
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
\brief 使用分配器创建 \c std::shared_ptr 实例对象。
\note 仅实现 WG21 P0674R1 和 ISO C++17 的交集，类似 libstdc++ 。
\see LWG 2070 。
\see https://developercommunity.visualstudio.com/content/problem/417142/lwg-2070p0674r1-stdallocate-shared-is-not-conformi.html
\since build 849
\todo 实现 WG21 P0674R1 。
*/
#if __cplusplus >= 202002L || (__GLIBCXX__ && !(__GLIBCXX__ <= 20111108))
// NOTE: See https://gcc.gnu.org/git/?p=gcc.git;a=blobdiff;f=libstdc%2B%2B-v3/include/bits/shared_ptr_base.h;h=fbbadd1aaaac3d7529a4cee844942e94941234d8;hp=a06f5b99f1827ca1934210b6b855540b408ab10a;hb=a58a38b32c7ed8b4843e8d2b2658323204fa96ed;hpb=3096f51bf6988fb20b0d5107322a2aebda25a50c.
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
\brief 使用分配器和初始化列表参数创建 \c std::shared_ptr 实例对象。
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
//!@{
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
//!@}


/*!
\brief 断言检查参数的分配器相等。
\sa yverify
\since build 967
*/
template<class _type, class _type2>
void
expects_equal_allocators(const _type& x, const _type2& y) ynothrowv
{
	using ator_t = remove_reference_t<decltype(x.get_allocator())>;

	details::expects_equal_allocators(x, y, and_<is_same<ator_t,
		remove_reference_t<decltype(y.get_allocator())>>,
		typename allocator_traits<ator_t>::is_always_equal>());
}


//! \since build 830
//!@{
//! \brief 按分配器特征在传播容器时复制赋值分配器。
template<class _tAlloc>
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
template<class _tAlloc>
YB_ATTR_nodiscard yconstfn _tAlloc
alloc_on_copy(const _tAlloc& a)
{
	// NOTE: As per ISO C++17 [allocator.requirements]/4, this shall not throw
	// exceptions.
	return allocator_traits<_tAlloc>::select_on_container_copy_construction(a);
}

//! \brief 按分配器特征在传播容器时转移分配器。
template<class _tAlloc>
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
template<class _tAlloc>
yconstfn_relaxed void
alloc_on_swap(_tAlloc& x, _tAlloc& y) ynothrow
{
	// NOTE: As per ISO C++17 [allocator.requirements]/4, this shall not throw
	// exceptions.
	details::do_alloc_on_swap(x, y,
		typename allocator_traits<_tAlloc>::propagate_on_container_swap());
}
//!@}

#undef YB_Impl_has_allocator_traits_is_always_equal


//! \since build 941
//!@{
/*!
\ingroup customization_points
\ingroup traits
\note 可定制和分配器的定义中不同的操作。
*/
//! \brief 分配器在容器中的使用复制构造的传播特征。
template<bool _bPOCCA = true, bool _bPOCMA = true, bool _bPOCS = true>
struct propagate_copy
{
	using on_copy_assignment = bool_<_bPOCCA>;
	using on_move_assignment = bool_<_bPOCMA>;
	using on_swap = bool_<_bPOCS>;

	template<class _tAlloc>
	YB_ATTR_nodiscard YB_ATTR_always_inline static yconstfn _tAlloc
	copy(const _tAlloc& a) ynoexcept(is_nothrow_copy_constructible<_tAlloc>())
	{
		return a;
	}
};


//! \brief 分配器在容器中使用默认构造的的传播特征。
template<bool _bPOCCA = false, bool _bPOCMA = false, bool _bPOCS = false>
struct propagate_default
{
	using on_copy_assignment = bool_<_bPOCCA>;
	using on_move_assignment = bool_<_bPOCMA>;
	using on_swap = bool_<_bPOCS>;

	template<class _tAlloc>
	YB_ATTR_nodiscard static YB_ATTR_always_inline yconstfn _tAlloc
	copy(const _tAlloc&)
		ynoexcept(is_nothrow_default_constructible<_tAlloc>())
	{
		return {};
	}
};


/*!
\ingroup allocators
\brief 传播分配器适配器。
\pre 第一参数是可被继承的分配器类型。
\pre 分配器类型可复制构造或被用户程序特化。
*/
template<class _tAlloc, class _tTraits = propagate_copy<>>
class propagating_allocator_adaptor : public _tAlloc
{
	static_assert(is_allocator<_tAlloc>(), "Invalid type found.");
	static_assert(is_copy_constructible<_tAlloc>(), "Invalid type found.");

public:
	using allocator_type = _tAlloc;
	using propagating_traits_type = _tTraits;
	using traits_type = std::allocator_traits<allocator_type>;
	//! \ingroup functors
	template<typename _tOther>
	struct rebind
	{
		using other = propagating_allocator_adaptor<rebind_alloc_t<
			allocator_type, _tOther>, propagating_traits_type>;
	};
	using propagate_on_container_copy_assignment
		= typename propagating_traits_type::on_copy_assignment;
	using propagate_on_container_move_assignment
		= typename propagating_traits_type::on_move_assignment;
	using propagate_on_container_swap
		= typename propagating_traits_type::on_swap;
	using is_always_equal = typename traits_type::is_always_equal;

	/*!
	\warning 语义视基类对象构造函数的形式可能不同。
	\sa __cpp_inheriting_constructors
	\see WG21 P0136R1 。
	*/
	using allocator_type::allocator_type;
	propagating_allocator_adaptor(const allocator_type& a)
		: allocator_type(a)
	{}
	propagating_allocator_adaptor(const propagating_allocator_adaptor&)
		= default;
	propagating_allocator_adaptor(propagating_allocator_adaptor&&) = default;
	template<class _tOther, class _tOtherTraits>
	propagating_allocator_adaptor(const
		propagating_allocator_adaptor<_tOther, _tOtherTraits>& other)
		ynoexcept_spec(allocator_type(other))
		: allocator_type(other)
	{}
	template<class _tOther, class _tOtherTraits>
	propagating_allocator_adaptor(
		propagating_allocator_adaptor<_tOther, _tOtherTraits>&& other)
		ynoexcept_spec(allocator_type(std::move(other)))
		: allocator_type(std::move(other))
	{}

	// NOTE: The base allocator may have a deleted copy %operator=.
	YB_ATTR_always_inline propagating_allocator_adaptor&
	operator=(const propagating_allocator_adaptor& a) ynoexcept_spec(
		ystdex::copy_assign(std::declval<allocator_type&>(),
		std::declval<const allocator_type&>()))
	{
		ystdex::copy_assign(static_cast<allocator_type&>(*this),
			static_cast<const allocator_type&>(a));
		return *this;
	}
	propagating_allocator_adaptor&
	operator=(propagating_allocator_adaptor&&) = default;

	YB_ATTR_nodiscard YB_ATTR_always_inline propagating_allocator_adaptor
	select_on_container_copy_construction() const
	{
		return propagating_traits_type::copy(*this);
	}
};

//! \relates propagating_allocator_adaptor
//!@{
template<class _tAlloc1, class _tTraits1, class _tAlloc2, class _tTraits2>
YB_ATTR_nodiscard YB_ATTR_always_inline inline bool
operator==(const propagating_allocator_adaptor<_tAlloc1, _tTraits1>& x,
	const propagating_allocator_adaptor<_tAlloc2, _tTraits2>& y) ynothrow
{
	return static_cast<const _tAlloc1&>(x) == static_cast<const _tAlloc2&>(y);
}

template<class _tAlloc1, class _tTraits1, class _tAlloc2, class _tTraits2>
YB_ATTR_nodiscard YB_ATTR_always_inline inline bool
operator!=(const propagating_allocator_adaptor<_tAlloc1, _tTraits1>& x,
	const propagating_allocator_adaptor<_tAlloc2, _tTraits2>& y) ynothrow
{
	return !(x == y);
}

template<class _tAlloc, class _tTraits>
struct is_bitwise_swappable<propagating_allocator_adaptor<_tAlloc, _tTraits>>
	: is_bitwise_swappable<_tAlloc>
{};
//!@}
//!@}

} // namespace ystdex;

#endif

