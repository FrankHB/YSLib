/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file type_op.hpp
\ingroup YStandardEx
\brief C++ 类型操作。
\version r2582
\author FrankHB <frankhb1989@gmail.com>
\since build 201
\par 创建时间:
	2011-04-14 08:54:25 +0800
\par 修改时间:
	2015-11-06 14:42 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::TypeOperation
*/


#ifndef YB_INC_ystdex_type_op_hpp_
#define YB_INC_ystdex_type_op_hpp_ 1

#include "tuple.hpp" // for is_class, std::declval, or_, bool_constant,
//	is_detected, is_void, _t, remove_reference_t, vdefer, common_type_t;

namespace ystdex
{

namespace details
{

#ifdef YB_IMPL_GNUCPP
#	if YB_IMPL_GNUCPP >= 40600
#		pragma GCC diagnostic push
#		pragma GCC diagnostic ignored "-Wctor-dtor-privacy"
#	else
#		pragma GCC system_header
//临时处理：关闭所有警告。
/*
关闭编译警告：(C++ only) Ambiguous virtual bases. ，
参见 http://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html 。
*/
#	endif
#endif


//! \since build 649
//@{
template<typename _type>
using addressof_mem_t = decltype(std::declval<const _type&>().operator&());

template<typename _type>
using addressof_free_t = decltype(operator&(std::declval<const _type&>()));
//@}

//! \since build 649
template<typename _type, typename _type2>
using subscription_t
	= decltype(std::declval<_type>()[std::declval<_type2>()]);

//! \since build 649
template<typename _type, typename _type2>
using equality_operator_t
	= decltype(std::declval<_type>() == std::declval<_type2>());


#if YB_IMPL_GNUCPP && YB_IMPL_GNUCPP >= 40600
//#	pragma GCC diagnostic warning "-Wctor-dtor-privacy"
#	pragma GCC diagnostic pop
#endif


//! \ingroup binary_type_traits
//@{
//! \since build 629
template<class _type>
struct has_nonempty_virtual_base
{
	static_assert(is_class<_type>(),
		"Non-class type found @ ystdex::has_nonempty_virtual_base;");

private:
	struct A : _type
	{
		//! \since build 461
		~A()
		{}
	};
	struct B : _type
	{
		//! \since build 461
		~B()
		{}
	};
	struct C : A, B
	{
		//! \since build 461
		~C()
		{}
	};

public:
	static yconstexpr const bool value = sizeof(C) < sizeof(A) + sizeof(B);
};


/*!
\since build 306
*/
template<class _type1, class _type2>
struct have_common_nonempty_virtual_base
{
	static_assert(and_<is_class<_type1>, is_class<_type2>>(),
		"Non-class type found @ ystdex::has_common_nonempty_virtual_base;");

private:
	struct A : virtual _type1
	{
		//! \since build 461
		~A()
		{}
	};

#ifdef YB_IMPL_GNUCPP
#	if YB_IMPL_GNUCPP >= 40600
#		pragma GCC diagnostic push
#		pragma GCC diagnostic ignored "-Wextra"
#	else
#		pragma GCC system_header
#	endif
#endif

	struct B : virtual _type2
	{
		//! \since build 461
		~B()
		{}
	};
	struct C : A, B
	{
		//! \since build 461
		~C()
		{}
	};

#if YB_IMPL_GNUCPP && YB_IMPL_GNUCPP >= 40600
//#	pragma GCC diagnostic warning "-Wextra"
#	pragma GCC diagnostic pop
#endif

public:
	static yconstexpr const bool value = sizeof(C) < sizeof(A) + sizeof(B);
};
//@}

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
\ingroup unary_type_traits
\brief 判断 _type 是否包含 value 成员。
\since build 440
*/
template<class _type>
struct has_mem_value : is_detected<details::mem_value_t, _type>
{};


/*!
\ingroup unary_type_traits
\brief 判断 _type 是否包含 value_type 类型成员。
\since build 650
*/
template<class _type>
struct has_mem_value_type : is_detected<details::mem_value_type_t, _type>
{};



/*!
\brief 判断是否存在合式重载 & 操作符接受指定类型的表达式。
\pre 参数不为不完整类型。
\since build 649
*/
template<typename _type>
struct has_overloaded_addressof
	: or_<is_detected<details::addressof_mem_t, _type>,
	is_detected<details::addressof_free_t, _type>>
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
\brief 判断指定的两个类类型是否有非空虚基类。
\since build 175
*/
template<class _type1, class _type2>
struct has_common_nonempty_virtual_base : bool_constant<
	details::have_common_nonempty_virtual_base<_type1, _type2>::value>
{};


//! \ingroup transformation_traits
//@{
/*!
\brief 恒等元函数。
\note 功能可以使用 ISO C++ 11 的 std::common_type 的单一参数实例替代。
\note ISO C++ LWG 2141 建议更改 std::common 的实现，无法替代。
\note 这里的实现不依赖 std::common_type 。
\note 同 boost::mpl::identity 。
\note Microsoft Visual C++ 2013 使用 LWG 2141 建议的实现。
\see http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-defects.html#2141 。
\see http://www.boost.org/doc/libs/1_55_0/libs/mpl/doc/refmanual/identity.html 。
\see http://msdn.microsoft.com/en-us/library/vstudio/bb531344%28v=vs.120%29.aspx 。
\see http://lists.cs.uiuc.edu/pipermail/cfe-commits/Week-of-Mon-20131007/090403.html 。
\since build 334
*/
//@{
template<typename _type>
struct identity
{
	using type = _type;
};

//! \since build 595
template<typename _type>
using identity_t = _t<identity<_type>>;
//@}


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
\brief 取公共非空类型：若第一参数为非空类型则为第一参数，否则从其余参数推断。
\since build 562
*/
template<typename _type, typename... _types>
using common_nonvoid_t = _t<cond_t<is_void<_type>,
	vdefer<common_type_t, _types...>, identity<_type>>>;

/*!
\brief 取公共底层类型。
\since build 629
*/
template<typename... _types>
using common_underlying_t = common_type_t<underlying_type_t<_types>...>;


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

