/*
	© 2014-2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file swap.hpp
\ingroup YStandardEx
\brief 交换操作。
\version r433
\author FrankHB <frankhb1989@gmail.com>
\since build 831
\par 创建时间:
	2018-07-12 16:38:36 +0800
\par 修改时间:
	2018-09-03 21:44 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Swap
*/


#ifndef YB_INC_ystdex_swap_hpp_
#define YB_INC_ystdex_swap_hpp_ 1

#include "type_pun.hpp" // for internal "type_pun.hpp", std::move, bool_,
//	false_, true_, is_class, is_nothrow_constructible, is_nothrow_assignable,
//	add_volatile_t, std::addressof, is_standard_layout, pun_storage_t,
//	aligned_replace_cast;

#if __cpp_lib_is_swappable >= 201603L
#	define YB_Impl_Swap_Traits true
#	define YB_Impl_Swap_ns std
#else
#	define YB_Impl_Swap_ns ystdex
#endif

namespace ystdex
{

//! \since build 834
//@{
//! \ingroup unary_type_traits
//@{
// XXX: No %tuple is instantiated in namespace %stdex, so no tuple check like
//	libstdc++.
template<typename _type>
using is_swap_movable = and_<is_move_constructible<_type>,
	is_move_assignable<_type>>;

template<typename _type>
using is_nothrow_swap_movable = and_<is_nothrow_move_constructible<_type>,
	is_nothrow_move_assignable<_type>>;
//@}

//! \ingroup transformation_traits
template<typename _tSwapped, typename _type = void>
using enable_if_swap_movable_t
	= enable_if_t<is_swap_movable<_tSwapped>::value, _type>;


#if !YB_Impl_Swap_Traits
inline namespace cpp2017
{

template<typename>
struct is_swappable;

template<typename>
struct is_nothrow_swappable;

} // inline namespace cpp2017;
#endif
//@}

} // namespace ystdex;

//! \since build 834
namespace ystdex_swap
{

yimpl(using ystdex::enable_if_swap_movable_t);
yimpl(using ystdex::is_nothrow_swap_movable);
yimpl(using ystdex::enable_if_t);
using YB_Impl_Swap_ns::is_swappable;
using YB_Impl_Swap_ns::is_nothrow_swappable;

// XXX: Function templates %ystdex::swap are defined in %swap.hpp.
template<typename _type>
yconstfn_relaxed yimpl(enable_if_swap_movable_t)<_type>
swap(_type&, _type&) ynoexcept(is_nothrow_swap_movable<_type>::value);

template<typename _type, size_t _vN>
yconstfn_relaxed yimpl(enable_if_t)<is_swappable<_type>::value>
swap(_type(&)[_vN], _type(&)[_vN])
	ynoexcept(is_nothrow_swappable<_type>::value);

/*!
\brief 为 ADL 提供重载的命名空间。
\note 在这个命名空间中 swap 不应指称 std 中的名称以避免歧义。
\since build 682
*/
namespace dep_swap
{

//! \since build 834
//@{
yimpl(using ystdex::nonesuch);
yimpl(using ystdex::any_constructible);
yimpl(using ystdex::is_same);

using ystdex_swap::swap;
//@}
//! \since build 496
nonesuch
swap(any_constructible, any_constructible);

template<typename _type, typename _type2>
struct yimpl(helper)
{
	static yconstexpr const bool value = !is_same<decltype(
		swap(std::declval<_type>(), std::declval<_type2>())), nonesuch>::value;

	//! \since build 694
	helper()
		ynoexcept_spec(swap(std::declval<_type>(), std::declval<_type2>()))
	{}
};

} // namespace dep_swap;

/*!
\brief 符合 ISO C++20 要求的 std::swap 替代实现。
\see WG21 P0185R1 。
\see WG21 P0879R0 。
\since build 834
*/
//@{
//! \see WG21 P0185R1 。
template<typename _type>
yconstfn_relaxed yimpl(enable_if_swap_movable_t)<_type>
swap(_type& a, _type& b) ynoexcept(is_nothrow_swap_movable<_type>::value)
{
	_type tmp = std::move(a);

	a = std::move(b);
	b = std::move(tmp);
}
//! \see LWG 809 。
template<typename _type, size_t _vN>
yconstfn_relaxed yimpl(enable_if_t)<is_swappable<_type>::value>
swap(_type(&a)[_vN], _type(&b)[_vN])
	ynoexcept(is_nothrow_swappable<_type>::value)
{
	for(size_t n(0); n < _vN; ++n)
		swap(a[n], b[n]);
}
//@}
/*!
\note 因为 std 和 ystdex 中并没有特化版本，需要此重载提供避免歧义。
\since build 837
*/
yconstfn_relaxed void
swap(ystdex::byte& x, ystdex::byte& y) ynothrow
{
	auto t(x);

	x = y;
	y = t;
}

} // namespace ystdex_swap;

namespace ystdex
{

//! \since build 834
inline namespace cpp2017
{

/*!
\ingroup type_traits_operations
\see WG21 P0185R1 。
\see ISO C++17 [swappable.requirements] 。
*/
//@{
#if YB_Impl_Swap_Traits
using std::is_swappable_with;
using std::is_swappable;
using std::is_nothrow_swappable_with;
using std::is_nothrow_swappable;
#else
//! \since build 586
//@{
//! \brief 判断是否可以调用 \c swap 。
//@{
//! \ingroup binary_type_traits
template<typename _type, typename _type2>
struct is_swappable_with
	: bool_<yimpl(ystdex_swap::dep_swap::helper<_type, _type2>::value)>
{};


//! \ingroup unary_type_traits
template<typename _type>
struct is_swappable
	: and_<is_referenceable<_type>, is_swappable_with<_type&, _type&>>
{};
//@}


//! \brief 判断是否可以无抛出地调用 \c swap 。
//@{
template<typename _type, typename _type2>
struct is_nothrow_swappable_with : is_nothrow_default_constructible<
	yimpl(ystdex_swap::dep_swap::helper<_type, _type2>)>
{};


template<typename _type>
struct is_nothrow_swappable
	: and_<is_referenceable<_type>, is_nothrow_swappable_with<_type&, _type&>>
{};
//@}
//@}
#endif
//@}

} // inline namespace cpp2017;


//! \since build 837
namespace details
{

template<typename _type>
yconstfn_relaxed void
swap_dep(_type& x, _type& y, false_)
{
	ystdex_swap::swap(x, y);
}
template<typename _type>
yconstfn_relaxed void
swap_dep(_type& x, _type& y, true_)
{
	using ystdex_swap::swap;

	swap(x, y);
}

} // namespace details;

/*!
\brief 支持基于 ystdex_swap::swap 的 ADL 的左值交换。
\since build 831
\sa boost::swap
\sa std::iter_swap
\see LWG 809 。
\see WG21 P0934R0 。

在 swap 成员函数中无法直接使用 using 引入带有命名空间限定的 swap 。
注意，除非崔在对参数更特定的 swap 函数，以 ystdex_swap::swap 为名称的模板和
	ystdex_swap::swap 被 ADL 同时查找到后，重载时引起歧义。
若以 ystdex 中的类型作为模板参数，使用模板参数的依赖名称查找，只要包含头文件，
	实例化时总是不论顺序总是查找到 ystdex_swap::swap ，
	若此时重载候选中同时存在 std::swap ，则引起歧义。
由于 is_nothrow_swappable 的实现（不论在哪个命名空间）总是要求 std::swap 作为重载
	候选之一，和 ystdex::swap 共存时总是在调用 ystdex::swap （不论是否使用 ystdex::
	限定）的用户代码中引起歧义导致无法使用（没有更特定的 swap 函数作为候选时），
	或失去使用 ystdex::swap 模板代理 ADL 调用的意义（对每个类型都另行声明 swap 函数
	作为重载候选时），本模板不使用 ystdex::swap 作为名称。
非类类型不使用 ADL ，以免 std 或其它命名空间中的 swap 调用的歧义。
*/
template<typename _type>
yconstfn_relaxed void
swap_dependent(_type& x, _type& y) ynothrow(is_nothrow_swappable<_type>())
{
	details::swap_dep(x, y, is_class<_type>());
}

/*!
\brief 交换非 volatile 左值和 volatile 左值。
\pre 可赋值。
\since build 704
*/
//@{
template<typename _type>
yimpl(enable_if_t)<is_nothrow_constructible<_type, volatile _type&&>()
	&& is_nothrow_assignable<_type&, volatile _type&&>()
	&& is_nothrow_assignable<volatile _type&, _type&&>()>
swap_volatile(_type& x, volatile _type& y)
	ynoexcept(is_nothrow_constructible<_type, volatile _type&>()
	&& noexcept(x = std::move(y)) && noexcept(y = std::move(x)))
{
	auto t(std::move(x));

	x = std::move(y);
	y = std::move(t);
}
template<typename _type>
inline auto
swap_volatile(volatile _type& x, _type& y)
	ynoexcept_spec(ystdex::swap_volatile(y, x))
	-> decltype(ystdex::swap_volatile(y, x))
{
	ystdex::swap_volatile(y, x);
}
template<typename _type, size_t _vN>
void
swap_volatile(_type(&x)[_vN], add_volatile_t<_type[_vN]>& y)
	ynoexcept(noexcept(ystdex::swap_volatile(x[0], y[0])))
{
	auto first(std::addressof(x[0]));
	auto first2(std::addressof(y[0]));

	for(const auto last(first + _vN); first != last; yunseq(++first, ++first2))
		ystdex::swap_volatile(*first, *first2);
}
//@}

//! \since build 704
namespace details
{

template<typename _type, typename _type2>
using swap_volatile_avail = bool_<is_volatile<remove_reference_t<_type>>::value
	!= is_volatile<remove_reference_t<_type2>>::value>;

#if YB_HAS_NOEXCEPT
template<typename _type, typename _type2>
using swap_volatile_noexcept = and_<swap_volatile_avail<_type, _type2>, bool_<
	noexcept(swap_volatile(std::declval<_type>(), std::declval<_type2>()))>>;
#else
template<typename _type, typename _type2>
using swap_volatile_noexcept = swap_volatile_avail<_type, _type2>;
#endif

} // namespace details;

/*!
\brief 交换相同标准布局类型可修改左值的存储。
\since build 620
*/
template<typename _type>
void
swap_underlying(_type& x, _type& y) ynothrow
{
	static_assert(is_standard_layout<_type>(),
		"Invalid underlying type found.");
	using utype = pun_storage_t<_type>;

	ystdex_swap::swap(ystdex::aligned_replace_cast<utype&>(x),
		ystdex::aligned_replace_cast<utype&>(y));
}

/*!
\brief 交换可能是 volatile 的左值或右值。
\pre 参数类型可交换，或具有一个 volatile 类型可用 swap_volatie 交换。
\since build 704
*/
//@{
//! \note 使用 ADL swap 或 ystdex_swap::swap 。
template<typename _type, typename _type2, yimpl(typename
	= enable_if_t<!details::swap_volatile_avail<_type, _type2>::value>)>
void
vswap(_type&& x, _type2&& y)
	ynoexcept(is_nothrow_swappable_with<_type&&, _type2&&>())
{
	ystdex::swap_dependent(yforward(x), yforward(y));
}
//! \note 使用 ADL swap_volatile 或 ystdex::swap_volatile 。
//@{
template<typename _type, typename _type2>
inline auto
vswap(_type&& x, _type2&& y) ynoexcept(detected_or_t<false_,
	details::swap_volatile_noexcept, _type, _type2>())
	-> yimpl(enable_if_t)<details::swap_volatile_avail<_type, _type2>::value>
{
	swap_volatile(yforward(x), yforward(y));
}
//@}
//@}


//! \note 使用 ADL swap 或 ystdex_swap::swap 。
//@{
/*!
\brief 复制后交换。
\since build 768
*/
template<typename _type, typename _type2 = _type>
inline _type&
copy_and_swap(_type& obj, const _type2& new_val)
{
	auto t(new_val);

	ystdex::swap_dependent(t, obj);
	return obj;
}

/*!
\brief 转移后交换。
\since build 818
*/
template<typename _type, typename _type2 = _type>
inline _type&
move_and_swap(_type& obj, _type2&& new_val)
{
	auto t(std::move(new_val));

	ystdex::swap_dependent(t, obj);
	return obj;
}
//@}

inline namespace cpp2014
{

#if __cpp_lib_exchange_function >= 201304L
using std::exchange;
#else
/*!
\brief 交换值并返回旧值。
\return 被替换的原值。
\see WG21 N3797 20.2.3[utility.exchange] 。
\see http://www.open-std.org/JTC1/sc22/WG21/docs/papers/2013/n3668.html 。
\since build 517
*/
template<typename _type, typename _type2 = _type>
_type
exchange(_type& obj, _type2&& new_val)
{
	_type old_val = std::move(obj);

	obj = std::forward<_type2>(new_val);
	return old_val;
}
#endif

} // inline namespace cpp2014;

} // namespace ystdex;

#endif

