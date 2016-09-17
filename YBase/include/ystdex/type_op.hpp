/*
	© 2011-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file type_op.hpp
\ingroup YStandardEx
\brief C++ 类型操作。
\version r2766
\author FrankHB <frankhb1989@gmail.com>
\since build 201
\par 创建时间:
	2011-04-14 08:54:25 +0800
\par 修改时间:
	2016-09-17 11:50 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::TypeOperation

间接扩展标准库头 <type_traits> ，包括一些不适用于所有类型或组合的类型特征，
以及其它元编程设施。
*/


#ifndef YB_INC_ystdex_type_op_hpp_
#define YB_INC_ystdex_type_op_hpp_ 1

#include "tuple.hpp" // for is_class, std::declval, is_detected, vseq::apply,
//	_t, bool_constant, is_void, is_same, remove_reference_t, and_, cond_t,
//	is_enum, vdefer, underlying_type_t, common_type_t;

namespace ystdex
{

/*!
\ingroup metafunctions
\brief 算术操作元函数。
\since build 727
*/
//@{
template<class...>
struct min_;

template<class _v1>
struct min_<_v1> : _v1
{};

template<class _v1, class _v2, class... _vn>
struct min_<_v1, _v2, _vn...>
	: min_<ystdex::conditional_t<(_v1::value < _v2::value), _v1, _v2>, _vn...>
{};


template<class...>
struct max_;

template<class _v1>
struct max_<_v1> : _v1
{};

template<class _v1, class _v2, class... _vn>
struct max_<_v1, _v2, _vn...>
	: max_<ystdex::conditional_t<(_v1::value > _v2::value), _v1, _v2>, _vn...>
{};
//@}


namespace details
{

//! \since build 649
template<typename _type, typename _type2>
using subscription_t
	= decltype(std::declval<_type>()[std::declval<_type2>()]);

//! \since build 649
template<typename _type, typename _type2>
using equality_operator_t
	= decltype(std::declval<_type>() == std::declval<_type2>());


//! \since build 629
template<class _type>
struct has_nonempty_virtual_base
{
	static_assert(is_class<_type>(),
		"Non-class type found @ ystdex::has_nonempty_virtual_base;");

private:
	struct A : _type
	{};

	struct B : _type
	{};

	struct C : A, B
	{};

public:
	static yconstexpr const bool value = sizeof(C) < sizeof(A) + sizeof(B);
};


//! \since build 306
template<class _type1, class _type2>
struct have_common_nonempty_virtual_base
{
	static_assert(and_<is_class<_type1>, is_class<_type2>>(),
		"Non-class type found @ ystdex::has_common_nonempty_virtual_base;");

private:
	struct A : virtual _type1
	{};

	struct B : virtual _type2
	{};

#ifdef YB_IMPL_GNUCPP
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wextra"
#endif

	struct C : A, B
	{};

#if YB_IMPL_GNUCPP
#	pragma GCC diagnostic pop
#endif

public:
	static yconstexpr const bool value = sizeof(C) < sizeof(A) + sizeof(B);
};

//! \since build 649
template<typename _type>
using mem_value_t = decltype(std::declval<_type>().value);

//! \since build 650
template<typename _type>
using mem_value_type_t = typename _type::value_type;


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


//! \ingroup unary_type_traits
//@{
/*!
\brief 判断 _type 是否包含可以指定参数应用得到类型的成员 apply 模板。
\since build 683
*/
template<class _type, typename... _tParams>
struct has_mem_apply : is_detected<vseq::apply, _type, _tParams...>
{};


/*!
\brief 判断 _type 是否包含 type 类型成员。
\since build 683
*/
template<class _type>
struct has_mem_type : is_detected<_t, _type>
{};


/*!
\brief 判断 _type 是否包含 value 数据成员。
\since build 440
*/
template<typename _type>
struct has_mem_value : is_detected<details::mem_value_t, _type>
{};


/*!
\brief 判断 _type 是否包含 value_type 类型成员。
\since build 650
*/
template<typename _type>
struct has_mem_value_type : is_detected<details::mem_value_type_t, _type>
{};
//@}


//! \ingroup binary_type_traits
//@{
/*!
\brief 判断是否存在合式的结果为非 void 类型的 [] 操作符接受指定类型的表达式。
\since build 399
*/
template<typename _type1, typename _type2>
struct has_subscription
	: bool_constant<is_detected<details::subscription_t, _type1, _type2>::value
	&& !is_void<detected_t<details::subscription_t, _type1, _type2>>::value>
{};


/*!
\brief 判断是否存在合式的结果可转换为 bool 类型的 == 操作符接受指定类型的表达式。
\since build 649
*/
template<typename _type, typename _type2 = _type>
struct has_equality_operator : is_detected_convertible<bool,
	details::equality_operator_t, _type, _type2>
{};


/*!
\brief 判断指定类型是否有非空虚基类。
\since build 175
*/
template<class _type>
struct has_nonempty_virtual_base
	: bool_constant<details::has_nonempty_virtual_base<_type>::value>
{};
//@}


/*!
\ingroup unary_type_traits
\brief 判断指定的两个类类型是否不同且有公共非空虚基类。
\pre 类可被继承。
\since build 660
\todo 支持 final 类。
*/
template<class _type1, class _type2>
struct have_common_nonempty_virtual_base
	: bool_constant<!is_same<_type1, _type2>::value
	&& details::have_common_nonempty_virtual_base<_type1, _type2>::value>
{};


//! \ingroup transformation_traits
//@{
/*!
\brief 移除可能被 cv-qualifier 修饰的引用类型。
\note remove_pointer 包含 cv-qualifier 的移除，不需要对应版本。
\since build 376
*/
//@{
template<typename _type>
struct remove_rcv : remove_cv<remove_reference_t<_type>>
{};

//! \since build 448
template<typename _type>
using remove_rcv_t = _t<remove_rcv<_type>>;
//@}


/*!
\brief 移除指针和引用类型。
\note 指针包括可能的 cv-qualifier 修饰。
\since build 175
*/
template<typename _type>
struct remove_rp : remove_pointer<remove_reference_t<_type>>
{};


/*!
\brief 移除可能被 cv-qualifier 修饰的引用和指针类型。
\since build 376
*/
template<typename _type>
struct remove_rpcv : remove_cv<_t<remove_rp<_type>>>
{};


/*!
\brief 取成员指针类型指向的类类型。
\since build 612
*/
template<typename _type>
using member_target_type_t = _t<details::member_target_type_impl<_type>>;
//@}


//! \ingroup metafunctions
//@{
/*!
\brief 取底层类型。
\note 同 underlying_type_t ，但对非枚举类型结果为参数类型。
\since build 671
*/
template<typename _type>
using underlying_cond_type_t
	= cond_t<is_enum<_type>, vdefer<underlying_type_t, _type>, _type>;

/*!
\brief 条件判断，若失败使用默认类型。
\since build 723
\sa detected_or
\sa detected_or_t
*/
//@{
template<class _tCond, typename _tDefault, template<typename...> class _gOp,
	typename... _tParams>
using cond_or = cond_t<_tCond, vdefer<_gOp, _tParams...>, _tDefault>;

template<class _tCond, typename _tDefault, template<typename...> class _gOp,
	typename... _tParams>
using cond_or_t = _t<cond_or<_tCond, identity<_tDefault>, _gOp, _tParams...>>;
//@}

/*!
\brief 取公共非空类型：若第一参数为非空类型则为第一参数，否则从其余参数推断。
\since build 562
*/
template<typename _type, typename... _types>
using common_nonvoid_t
	= cond_or_t<is_void<_type>, _type, common_type_t, _types...>;

/*!
\brief 取公共底层类型。
\since build 629
*/
template<typename... _types>
using common_underlying_t = common_type_t<underlying_cond_type_t<_types>...>;


/*!	\defgroup tags Tags
\ingroup meta
\brief 标记。
\note 可能是类型或元类型。
\since build 447
*/

/*!
\ingroup meta_types
\brief 自然数标记。
*/
//@{
template<size_t _vN>
struct n_tag
{
	using type = n_tag<_vN - 1>;
};

template<>
struct n_tag<0>
{
	using type = void;
};
//@}

/*!
\ingroup tags
\brief 第一分量标记。
*/
using first_tag = n_tag<0>;

/*!
\ingroup tags
\brief 第二分量标记。
*/
using second_tag = n_tag<1>;

} // namespace ystdex;

#endif

