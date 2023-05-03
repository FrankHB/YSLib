/*
	© 2011-2016, 2018-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file type_op.hpp
\ingroup YStandardEx
\brief C++ 类型操作。
\version r2963
\author FrankHB <frankhb1989@gmail.com>
\since build 201
\par 创建时间:
	2011-04-14 08:54:25 +0800
\par 修改时间:
	2023-04-12 12:34 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::TypeOperation

间接扩展标准库头 \c \<type_traits> ，包括一些不保证所有类型适用的或组合的类型特征，
以及其它元编程设施。
*/


#ifndef YB_INC_ystdex_type_op_hpp_
#define YB_INC_ystdex_type_op_hpp_ 1

#include "variadic.hpp" // for "variadic.hpp", is_class, std::declval,
//	vseq::ctor_of_t, is_detected, vseq::apply, _t, bool_, is_void, equal_t,
//	is_same, detected_or_t, remove_reference_t, and_, cond_t, is_enum, vdefer,
//	underlying_type_t, empty_pack_t, vseq::front_t, common_type_t;

namespace ystdex
{

/*!
\ingroup metafunctions
\brief 算术操作元函数。
\since build 727
*/
//!@{
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
//!@}


namespace details
{

//! \since build 629
template<class _type>
struct has_nonempty_virtual_base
{
	static_assert(is_class<_type>(),
		"Non-class type found @ ystdex::has_nonempty_virtual_base;");

private:
	struct a : _type
	{};

	struct b : _type
	{};

	struct c : a, b
	{};

public:
	static yconstexpr const bool value = sizeof(c) < sizeof(a) + sizeof(b);
};


//! \since build 306
template<class _type1, class _type2>
struct have_common_nonempty_virtual_base
{
	static_assert(and_<is_class<_type1>, is_class<_type2>>(),
		"Non-class type found @ ystdex::has_common_nonempty_virtual_base;");

private:
	struct a : virtual _type1
	{};

	struct b : virtual _type2
	{};

#ifdef YB_IMPL_GNUCPP
#	pragma GCC diagnostic push
#	if YB_IMPL_GNUCPP >= 100000
// NOTE: See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90449.
#		pragma GCC diagnostic ignored "-Winaccessible-base"
#	endif
#endif

	struct c : a, b
	{};

#if YB_IMPL_GNUCPP
#	pragma GCC diagnostic pop
#endif

public:
	static yconstexpr const bool value = sizeof(c) < sizeof(a) + sizeof(b);
};

//! \since build 649
template<typename _type>
using mem_value_t = decltype(std::declval<_type>().value);

//! \since build 650
template<typename _type>
using mem_value_type_t = typename _type::value_type;

//! \since build 851
template<typename _type>
using ctor_of_or_void_t = vseq::ctor_of_t<_type>;

} // namespace details;


/*!
\brief 判断 _type 是否包含可以指定参数应用得到类型的成员 apply 模板。
\since build 683
*/
template<class _type, typename... _tParams>
struct has_mem_apply : is_detected<vseq::apply, _type, _tParams...>
{};


//! \ingroup unary_type_traits
//!@{
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
//!@}


/*!
\pre 类可被继承。
\todo 支持 final 类。
*/
//!@{
/*!
\ingroup unary_type_traits
\brief 判断指定类型是否有非空虚基类。
\since build 175
*/
template<class _type>
struct has_nonempty_virtual_base
	: bool_<details::has_nonempty_virtual_base<_type>::value>
{};


/*!
\ingroup binary_type_traits
\brief 判断指定的两个类类型是否不同且有公共非空虚基类。
\since build 660
*/
template<class _type1, class _type2>
struct have_common_nonempty_virtual_base : bool_<!is_same<_type1, _type2>::value
	&& details::have_common_nonempty_virtual_base<_type1, _type2>::value>
{};
//!@}


/*!
\ingroup binary_type_traits
\brief 判断类型是否是指定类型构造器的实例。
\note 和 vseq::is_instance 不同，只根据类型构造器判断，无视类型参数。
\sa vseq::_a
\since build 849
*/
template<typename _type, class _tCtor>
using is_instance_of = is_same<
	detected_or_t<void, details::ctor_of_or_void_t, _type>, _tCtor>;


/*!
\ingroup unary_type_traits
\brief 移除指针和引用类型。
\note 指针包括可能的 cv-qualifier 修饰。
\since build 175
*/
template<typename _type>
struct remove_rp : remove_pointer<remove_reference_t<_type>>
{};


/*!
\ingroup unary_type_traits
\brief 移除可能被 cv-qualifier 修饰的引用和指针类型。
\since build 376
*/
template<typename _type>
struct remove_rpcv : remove_cv<_t<remove_rp<_type>>>
{};


//! \ingroup metafunctions
//!@{
/*!
\brief 取底层类型。
\note 同 underlying_type_t ，但对非枚举类型结果为参数类型。
\since build 671
*/
template<typename _type>
using underlying_cond_type_t = _t<
	cond_t<is_enum<_type>, vdefer<underlying_type_t, _type>, identity<_type>>>;

/*!
\brief 条件判断，若失败使用默认类型。
\since build 723
\sa detected_or
\sa detected_or_t
*/
//!@{
template<class _tCond, typename _tDefault, template<typename...> class _gOp,
	typename... _tParams>
using cond_or = cond_t<_tCond, vdefer<_gOp, _tParams...>, identity<_tDefault>>;

template<class _tCond, typename _tDefault, template<typename...> class _gOp,
	typename... _tParams>
using cond_or_t = _t<cond_or<_tCond, _tDefault, _gOp, _tParams...>>;
//!@}

//! \since build 942
//!@{
/*!
\brief 参数包判断，若为空参数包使用默认类型。
\sa cond_or_t
\sa empty_pack_t
*/
template<typename _tDefault, template<typename...> class _gOp,
	typename... _tParams>
using nonempty_pack_or_t = cond_or_t<not_<empty_pack_t<_tParams...>>, _tDefault,
	_gOp, empty_base<_tParams...>>;

/*!
\brief 取参数包的第一个类型。
\note 若类型不存在，则视为 void 。
*/
template<typename... _tParams>
using head_of_t = nonempty_pack_or_t<void, vseq::front_t, _tParams...>;
//!@}

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
//!@}


/*!
\ingroup transformation_traits
\brief 排除选择类型的特定参数类型的重载。
\note 第一参数通常为类类型，可避免被选择的类的构造模板和复制/转移特殊成员函数冲突。
\note 类似 exclude_self_t ，但支持参数包，而不支持指定结果类型。
\since build 848
*/
template<typename _tSelected, typename... _tParams>
using exclude_self_params_t
	= enable_if_t<not_<cond_or_t<bool_<sizeof...(_tParams) == 1>,
	false_, is_same_param, _tSelected, _tParams...>>{}, yimpl(void)>;


/*!
\ingroup meta_types tags
\brief 自然数标记。
*/
//!@{
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
//!@}

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

