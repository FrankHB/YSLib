/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file type_traits.hpp
\ingroup YStandardEx
\brief ISO C++ 类型特征扩展。
\version r550
\author FrankHB <frankhb1989@gmail.com>
\since build 201
\par 创建时间:
	2015-11-04 09:34:17 +0800
\par 修改时间:
	2015-11-04 11:26 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::TypeTraits
*/


#ifndef YB_INC_ystdex_type_traits_hpp_
#define YB_INC_ystdex_type_traits_hpp_ 1

#include "../ydef.h" // for <type_traits>;

namespace ystdex
{

/*!	\defgroup template_meta_programing Template Meta Programing
\brief 模板元编程。
\note 以下类别中的接口包括类模板和对应的别名模板。
\since build 288
*/

/*!	\defgroup meta_types Meta Types
\ingroup template_meta_programing
\brief 元类型。
\since build 288
*/

/*!	\defgroup meta_operations Meta Operations
\ingroup template_meta_programing
\brief 元操作。
\since build 288
*/

/*!	\defgroup metafunctions Metafunctions
\ingroup meta_operations
\brief 元函数。
\see http://www.boost.org/doc/libs/1_50_0/libs/mpl/doc/refmanual/metafunction.html 。
\since build 333
*/

/*!	\defgroup type_traits_operations Type Traits Operations
\ingroup metafunctions
\brief 类型特征操作。
\since build 306
*/

/*!	\defgroup unary_type_traits Unary Type Traits
\ingroup type_traits_operations
\brief 一元类型特征。
\see ISO C++11 20.9.1 [meta.rqmts] 。
\since build 306
*/

/*!	\defgroup binary_type_traits Binary Type Traits
\ingroup type_traits_operations
\brief 二元类型特征。
\see ISO C++11 20.9.1 [meta.rqmts] 。
\since build 306
*/

/*!	\defgroup transformation_traits Transformation Traits
\ingroup type_traits_operations
\brief 变换类型特征。
\see ISO C++11 20.9.1 [meta.rqmts] 。
\since build 590
*/


/*!
\ingroup metafunctions
\brief 嵌套类型别名。
\since build 649
*/
template<class _type>
using _t = typename _type::type;

/*!
\brief 包含 ISO C++ 2011 引入的名称的命名空间。
\since build 607
*/
inline namespace cpp2011
{

//! \since build 245
//@{
using std::integral_constant;
using std::true_type;
using std::false_type;

using std::is_void;
using std::is_integral;
using std::is_floating_point;
using std::is_array;
using std::is_pointer;
using std::is_lvalue_reference;
using std::is_rvalue_reference;
using std::is_member_object_pointer;
using std::is_member_function_pointer;
using std::is_enum;
using std::is_class;
using std::is_union;
using std::is_function;

using std::is_reference;
using std::is_arithmetic;
using std::is_fundamental;
using std::is_object;
using std::is_scalar;
using std::is_compound;
using std::is_member_pointer;

using std::is_const;
using std::is_volatile;
using std::is_trivial;
//	using std::is_trivially_copyable;
using std::is_standard_layout;
using std::is_pod;
using std::is_literal_type;
using std::is_empty;
using std::is_polymorphic;
using std::is_abstract;

using std::is_signed;
using std::is_unsigned;

using std::is_constructible;
//! \since build 551
//@{
using std::is_default_constructible;
using std::is_copy_constructible;
using std::is_move_constructible;

using std::is_assignable;
using std::is_copy_assignable;
using std::is_move_assignable;

using std::is_destructible;
//@}

#	if !YB_IMPL_GNUC || YB_IMPL_GNUCPP >= 50000
//! \since build 595
//@{
using std::is_trivially_constructible;
using std::is_trivially_default_constructible;
using std::is_trivially_copy_constructible;
using std::is_trivially_move_constructible;

using std::is_trivially_assignable;
using std::is_trivially_copy_assignable;
using std::is_trivially_move_assignable;
//@}
#	endif
//! \since build 591
using std::is_trivially_destructible;

//! \since build 551
//@{
using std::is_nothrow_constructible;
using std::is_nothrow_default_constructible;
using std::is_nothrow_copy_constructible;
using std::is_nothrow_move_constructible;

using std::is_nothrow_assignable;
using std::is_nothrow_copy_assignable;
using std::is_nothrow_move_assignable;

using std::is_nothrow_destructible;
//@}

using std::has_virtual_destructor;

using std::alignment_of;
using std::rank;
using std::extent;

using std::is_same;
using std::is_base_of;
using std::is_convertible;

using std::remove_const;
using std::remove_volatile;
using std::remove_cv;
using std::add_const;
using std::add_volatile;
using std::add_cv;

using std::remove_reference;
using std::add_lvalue_reference;
using std::add_rvalue_reference;

using std::make_signed;
using std::make_unsigned;

using std::remove_extent;
using std::remove_all_extents;

using std::remove_pointer;
using std::add_pointer;

using std::aligned_storage;
#	if !YB_IMPL_GNUC || YB_IMPL_GNUCPP >= 50000
//! \since build 595
using std::aligned_union;
#	endif
using std::decay;
using std::enable_if;
using std::conditional;
using std::common_type;
//! \since build 439
using std::underlying_type;
using std::result_of;
//@}

} // inline namespace cpp2011;


/*!
\brief 包含 ISO C++ 2014 引入的名称的命名空间。
\since build 607
*/
inline namespace cpp2014
{

/*!
\ingroup transformation_traits
\brief ISO C++ 14 兼容类型操作别名。
*/
//@{
#if __cpp_lib_transformation_trait_aliases >= 201304 || __cplusplus > 201103L
using std::remove_const_t;
using std::remove_volatile_t;
using std::remove_cv_t;
using std::add_const_t;
using std::add_volatile_t;
using std::add_cv_t;

using std::remove_reference_t;
using std::add_lvalue_reference_t;
using std::add_rvalue_reference_t;

using std::make_signed_t;
using std::make_unsigned_t;

using std::remove_extent_t;
using std::remove_all_extents_t;

using std::remove_pointer_t;
using std::add_pointer_t;

using std::aligned_storage_t;
#	if !YB_IMPL_GNUC || YB_IMPL_GNUCPP >= 50000
using std::aligned_union_t;
#	endif
using std::decay_t;
using std::enable_if_t;
using std::conditional_t;
using std::common_type_t;
using std::underlying_type_t;
using std::result_of_t;
#else
//! \since build 340
//@{
template<typename _type>
using remove_const_t = typename remove_const<_type>::type;

template<typename _type>
using remove_volatile_t = typename remove_volatile<_type>::type;

template<typename _type>
using remove_cv_t = typename remove_cv<_type>::type;

template<typename _type>
using add_const_t = typename add_const<_type>::type;

template<typename _type>
using add_volatile_t = typename add_volatile<_type>::type;

template<typename _type>
using add_cv_t = typename add_cv<_type>::type;


template<typename _type>
using remove_reference_t = typename remove_reference<_type>::type;

template<typename _type>
using add_lvalue_reference_t = typename add_lvalue_reference<_type>::type;

template<typename _type>
using add_rvalue_reference_t = typename add_rvalue_reference<_type>::type;


template<typename _type>
using make_signed_t = typename make_signed<_type>::type;

template<typename _type>
using make_unsigned_t = typename make_unsigned<_type>::type;


template<typename _type>
using remove_extent_t = typename remove_extent<_type>::type;

template<typename _type>
using remove_all_extents_t = typename remove_all_extents<_type>::type;


template<typename _type>
using remove_pointer_t = typename remove_pointer<_type>::type;

template<typename _type>
using add_pointer_t = typename add_pointer<_type>::type;


template<size_t _vLen,
	size_t _vAlign = yalignof(typename aligned_storage<_vLen>::type)>
using aligned_storage_t = typename aligned_storage<_vLen, _vAlign>::type;
//@}

//! \since build 339
//@{
#	if !YB_IMPL_GNUC || YB_IMPL_GNUCPP >= 50000
template<size_t _vLen, typename... _types>
using aligned_union_t = typename aligned_union<_vLen, _types...>::type;
#	endif

template<typename _type>
using decay_t = typename decay<_type>::type;

template<bool _bCond, typename _type = void>
using enable_if_t = typename enable_if<_bCond, _type>::type;

template<bool _bCond, typename _type, typename _type2>
using conditional_t = typename conditional<_bCond, _type, _type2>::type;

template<typename... _types>
using common_type_t = typename common_type<_types...>::type;

template<typename _type>
using underlying_type_t = typename underlying_type<_type>::type;

template<typename _type>
using result_of_t = typename result_of<_type>::type;
//@}
#endif
//@}

} // inline namespace cpp2014;


/*!
\ingroup meta_types
\brief bool 常量。
\sa http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4389.html
\see WG21/N4527 20.10.3[meta.help] 。
\since build 617
*/
#if YB_IMPL_MSCPP >= 1900
using std::bool_constant;
#else
template<bool _b>
using bool_constant = integral_constant<bool, _b>;
#endif


//! \ingroup metafunctions
//@{
//! \brief 表达式 SFINAE 别名模板。
//@{
/*!
\see WG21/N3911 。
\see WG21/N4296 20.10.2[meta.type.synop] 。
\since build 591
*/
template<typename...>
using void_t = void;

/*!
\sa enable_if_t
\sa void_t
\since build 649
*/
template<bool _bCond>
using enable_when = void_t<enable_if_t<_bCond>>;
//@}


/*!
\brief 逻辑操作元函数。
\note 和 libstdc++ 实现以及 Boost.MPL 兼容。
\since build 578
*/
//@{
template<typename...>
struct and_;

template<>
struct and_<> : true_type
{};

template<typename _b1>
struct and_<_b1> : _b1
{};

template<typename _b1, typename _b2>
struct and_<_b1, _b2> : conditional_t<_b1::value, _b2, _b1>
{};

template<typename _b1, typename _b2, typename _b3, typename... _bn>
struct and_<_b1, _b2, _b3, _bn...>
	: conditional_t<_b1::value, and_<_b2, _b3, _bn...>, _b1>
{};


template<typename...>
struct or_;

template<>
struct or_<> : false_type
{};

template<typename _b1>
struct or_<_b1> : _b1
{};

template<typename _b1, typename _b2>
struct or_<_b1, _b2> : conditional_t<_b1::value, _b1, _b2>
{};

template<typename _b1, typename _b2, typename _b3, typename... _bn>
struct or_<_b1, _b2, _b3, _bn...>
	: conditional_t<_b1::value, _b1, or_<_b2, _b3, _bn...>>
{};


template<typename _b>
struct not_ : bool_constant<!_b::value>
{};
//@}
//@}


/*!
\brief 接受任意参数类型构造的类型。
\since build 586
*/
struct any_constructible
{
	any_constructible(...);
};


/*!
\brief 不接受任意参数类型构造的类型。
\see WG21/N4502 6.6 。
\since build 649
*/
struct nonesuch
{
	nonesuch() = delete;
	nonesuch(const nonesuch&) = delete;
	~nonesuch() = delete;

	void
	operator=(const nonesuch&) = delete;
};


/*!
\ingroup metafunctions
\since build 649
\see WG21/N4502 。
*/
//@{
namespace details
{

template<typename _tDefault, typename, template<typename...> class, typename...>
struct detector
{
	using value_t = false_type;
	using type = _tDefault;
};

template<typename _tDefault, template<typename...> class _gOp,
	typename... _tParams>
struct detector<_tDefault, void_t<_gOp<_tParams...>>, _gOp, _tParams...>
{
	using value_t = true_type;
	using type = _gOp<_tParams...>;
};

} // namespace details;

template<template<typename...> class _gOp, typename... _tParams>
using is_detected
	= typename details::detector<nonesuch, void, _gOp, _tParams...>::value_t;

template<template<typename...> class _gOp, typename... _tParams>
using detected_t = _t<details::detector<nonesuch, void, _gOp, _tParams...>>;

template<typename _tDefault, template<typename...> class _gOp,
	typename... _tParams>
using detected_or = details::detector<_tDefault, void, _gOp, _tParams...>;

template<typename _tDefault, template<typename...> class _gOp,
	typename... _tParams>
using detected_or_t = _t<detected_or<_tDefault, _gOp, _tParams...>>;

template<typename _tExpected, template<typename...> class _gOp,
	typename... _tParams>
using is_detected_exact = is_same<_tExpected, detected_t<_gOp, _tParams...>>;

template<typename _tTo, template<typename...> class _gOp, typename... _tParams>
using is_detected_convertible
	= is_convertible<detected_t<_gOp, _tParams...>, _tTo>;
//@}

/*!
\ingroup metafunctions
\brief 移除选择类类型的特定重载避免构造模板和复制/转移构造函数冲突。
\pre 第一参数为类类型。
\sa enable_if_t
\since build 538
*/
template<class _tClass, typename _tParam, typename _type = void>
using exclude_self_ctor_t
	= enable_if_t<!is_same<_tClass&, decay_t<_tParam>&>::value, _type>;

} // namespace ystdex;

#endif

