/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file type_op.hpp
\ingroup YStandardEx
\brief C++ 类型操作模板类。
\version r1563;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-04-14 08:54:25 +0800;
\par 修改时间:
	2012-08-22 01:18 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YStandardEx::TypeOperation;
*/


#ifndef YB_INC_YSTDEX_TYPEOP_HPP_
#define YB_INC_YSTDEX_TYPEOP_HPP_

#include "../ydef.h" // for <type_traits> and std::declval;

namespace ystdex
{

/*
using std::tr1::add_reference;

using std::tr1::has_nothrow_assign;
using std::tr1::has_nothrow_constructor;
using std::tr1::has_nothrow_copy;
using std::tr1::has_trivial_assign;
using std::tr1::has_trivial_constructor;
using std::tr1::has_trivial_copy;
using std::tr1::has_trivial_destructor;
*/

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
/*
using std::is_default_constructible;
using std::is_copy_constructible;
using std::is_move_constructible;

using std::is_assignable;
using std::is_copy_assignable;
using std::is_move_assignable;

using std::is_destructible;

using std::is_trivially_constructible;
using std::is_trivially_default_constructible;
using std::is_trivially_copy_constructible;
using std::is_trivially_move_constructible;

using std::is_trivially_assignable;
using std::is_trivially_copy_assignable;
using std::is_trivially_move_assignable;
using std::is_trivially_destructible;

using std::is_nothrow_constructible;
using std::is_nothrow_default_constructible;
using std::is_nothrow_copy_constructible;
using std::is_nothrow_move_constructible;

using std::is_nothrow_assignable;
using std::is_nothrow_copy_assignable;
using std::is_nothrow_move_assignable;

using std::is_nothrow_destructible;
*/
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
using std::decay;
using std::enable_if;
using std::conditional;
using std::common_type;
//	using std::underlying_type;
using std::result_of;


/*!	\defgroup template_meta_programing Template Meta Programing
\brief 模板元编程。
\since build 288 。
*/

/*!	\defgroup meta_types Meta Types
\ingroup template_meta_programing
\brief 元类型。
\since build 288 。
*/

/*!	\defgroup meta_operations Meta Operations
\ingroup template_meta_programing
\brief 元操作。
\since build 288 。
*/

/*!	\defgroup type_traits_operations Type Traits Operations
\ingroup meta_operations
\brief 类型特征操作。
\since build 306 。
*/

/*!	\defgroup unary_type_trait Unary Type Trait
\ingroup type_traits_operations
\brief 一元类型特征。
\see ISO C++2011 20.9.1[meta.rqmts] 。
\since build 306 。
*/

/*!	\defgroup binary_type_trait Binary Type Trait
\ingroup type_traits_operations
\brief 二元类型特征。
\see ISO C++2011 20.9.1[meta.rqmts] 。
\since build 306 。
*/

/*!	\defgroup metafunctions Metafunctions
\ingroup meta_operations
\brief 元函数。
\see http://www.boost.org/doc/libs/1_50_0/libs/mpl/doc/refmanual/\
metafunction.html 。
\since build 333 。
*/


/*!
\ingroup unary_type_trait
\brief 判断指定类型是否可作为返回值类型。
\note 即排除数组和函数类型的所有类型。
\since build 333 。
*/
template<typename _type>
struct is_returnable : std::integral_constant<bool, !std::is_array<_type>::value
	&& !std::is_function<_type>::value>
{};


/*!
\ingroup unary_type_trait
\brief 判断指定类型是否是指向类的指针。
\note 即排除数组和函数类型的所有类型。
\since build 333 。
*/
template<typename _type>
struct is_class_pointer : std::integral_constant<bool, !std::is_pointer<_type>
	::value && std::is_class<typename std::remove_pointer<_type>::type>::value>
{};


/*!
\ingroup unary_type_trait
\brief 判断指定类型是否是左值类类型引用。
\note 即排除数组和函数类型的所有类型。
\since build 333 。
*/
template<typename _type>
struct is_lvalue_class_reference : std::integral_constant<bool,
	!std::is_lvalue_reference<_type>::value && std::is_class<typename
	std::remove_reference<_type>::type>::value>
{};


/*!
\ingroup unary_type_trait
\brief 判断指定类型是否是右值类类型引用。
\note 即排除数组和函数类型的所有类型。
\since build 333 。
*/
template<typename _type>
struct is_rvalue_class_reference : std::integral_constant<bool,
	!std::is_lvalue_reference<_type>::value && std::is_class<typename
	std::remove_reference<_type>::type>::value>
{};


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

/*!
\since build 306 。
*/
template<typename _type1, typename _type2>
struct have_equality_operator
{
private:
	template<typename _type>
	static yconstfn bool
	test_equal(int, typename enable_if<is_convertible<decltype(std::declval<
		_type>() == std::declval<_type2>()), bool>::value, int>::type = 0)
	{
		return true;
	}
	template<typename>
	static yconstfn bool
	test_equal(...)
	{
		return false;
	}

public:
	static yconstexpr bool value = test_equal();
};

#if YB_IMPL_GNUCPP && YB_IMPL_GNUCPP >= 40600
//#	pragma GCC diagnostic warning "-Wctor-dtor-privacy"
#	pragma GCC diagnostic pop
#endif


/*!
\since build 306 。
*/
template<class _type>
struct have_nonempty_virtual_base
{
	static_assert(std::is_class<_type>::value,
		"Non-class type found @ ystdex::has_nonempty_virtual_base;");

private:
	struct A : _type
	{
		~A() ynothrow
		{}
	};
	struct B : _type
	{
		~B() ynothrow
		{}
	};
	struct C : A, B
	{
		~C() ynothrow
		{}
	};

public:
	static yconstexpr bool value = sizeof(C) < sizeof(A) + sizeof(B);
};


/*!
\since build 306 。
*/
template<class _type1, class _type2>
struct have_common_nonempty_virtual_base
{
	static_assert(std::is_class<_type1>::value
		&& std::is_class<_type2>::value,
		"Non-class type found @ ystdex::has_common_nonempty_virtual_base;");

private:
	struct A : virtual _type1
	{
		~A() ynothrow
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
		~B() ynothrow
		{}
	};
	struct C : A, B
	{
		~C() ynothrow
		{}
	};

#if YB_IMPL_GNUCPP && YB_IMPL_GNUCPP >= 40600
//#	pragma GCC diagnostic warning "-Wextra"
#	pragma GCC diagnostic pop
#endif

public:
	static yconstexpr bool value = sizeof(C) < sizeof(A) + sizeof(B);
};

} // namespace details;


/*!
\ingroup binary_type_trait
\brief 判断是否存在合式的结果可转换为 bool 类型的 == 操作符接受指定类型的表达式。
\since build 306 。
*/
template<typename _type1, typename _type2>
struct has_equality_operator : integral_constant<bool,
	details::have_equality_operator<_type1, _type2>::value>
{};


/*!
\ingroup binary_type_trait
\brief 判断指定类型是否有非空虚基类。
\since build 175 。
*/
template<class _type>
struct has_nonempty_virtual_base : integral_constant<bool,
	details::have_nonempty_virtual_base<_type>::value>
{};


/*!
\ingroup unary_type_trait
\brief 判断指定的两个类类型是否有非空虚基类。
\since build 175 。
*/
template<class _type1, class _type2>
struct has_common_nonempty_virtual_base : integral_constant<bool,
	details::have_common_nonempty_virtual_base<_type1, _type2>::value>
{};


/*!
\ingroup meta_operations
\brief 移除指针和引用类型。
\since build 175 。
*/
template<typename _type>
struct remove_rp
{
	typedef typename remove_pointer<typename remove_reference<_type>
		::type>::type type;
};


/*!
\ingroup meta_operations
\brief 数组类型退化。
\since build 290 。

参数为数组类型时同 std::decay ，否则结果类型为参数。
*/
template<typename _type>
struct array_decay
{
	typedef typename conditional<is_array<_type>::value,
		typename decay<_type>::type, _type>::type type;
};


/*!
\ingroup meta_operations
\brief 保持修饰符的类型退化。
\since build 290 。

参数移除引用后为数组或函数类型时同 std::decay ，否则结果为参数。
*/
template<typename _type>
struct qualified_decay
{
private:
	typedef typename remove_reference<_type>::type value_type;

public:
	typedef typename conditional<is_function<value_type>::value
		|| is_array<value_type>::value, typename decay<_type>::type,
		_type>::type type;
};


/*!
\ingroup meta_operations
\brief 数组及数组引用类型退化。
\since build 290 。

参数为非引用类型时同 array_decay ，
否则结果为被引用的类型的 array_decay 结果的引用。
*/
//@{
template<typename _type>
struct array_ref_decay
{
	typedef typename array_decay<_type>::type type;
};

template<typename _type>
struct array_ref_decay<_type&>
{
	typedef typename array_decay<_type>::type type;
	typedef typename array_decay<_type>::type& reference;
};

template<typename _type>
struct array_ref_decay<_type&&>
{
	typedef typename array_decay<_type>::type type;
	typedef typename array_decay<_type>::type&& reference;
};
//@}


/*!
\ingroup meta_operations
\brief 取指定整数类型的位宽度。
\since build 260 。
*/
template<typename _tInt>
struct integer_width
	: public integral_constant<size_t, sizeof(_tInt) * CHAR_BIT>
{};


/*!
\ingroup meta_operations
\brief 取指定整数类型和条件表达式对应的有符号或无符号整数类型。
\since build 260 。
*/
//@{
template<typename _type, bool>
struct make_signed_c
{
	typedef typename std::make_signed<_type>::type type;
};

template<typename _type>
struct make_signed_c<_type, false>
{
	typedef typename std::make_unsigned<_type>::type type;
};
//@}


/*!
\ingroup meta_operations
\brief 取按指定宽度的整数类型。
\since build 260 。
*/
//@{
template<size_t>
struct make_fixed_width_int
{
	typedef int type;
	typedef unsigned unsigned_type;
};

template<>
struct make_fixed_width_int<8U>
{
	typedef std::int8_t type;
	typedef std::uint8_t unsigned_type;
};

template<>
struct make_fixed_width_int<16U>
{
	typedef std::int16_t type;
	typedef std::uint16_t unsigned_type;
};

template<>
struct make_fixed_width_int<32U>
{
	typedef std::int32_t type;
	typedef std::uint32_t unsigned_type;
};

template<>
struct make_fixed_width_int<64U>
{
	typedef std::int64_t type;
	typedef std::uint64_t unsigned_type;
};
//@}


/*!
\ingroup meta
\brief 标记。
*/
//@{
/*!
\ingroup meta_types
\brief 自然数标记。
*/
//@{
template<size_t _vN>
struct n_tag
{
	typedef n_tag<_vN - 1> type;
};

template<>
struct n_tag<0>
{
	typedef void type;
};
//@}

/*!
\ingroup meta_types
\brief 第一分量标记。
*/
typedef n_tag<0> first_tag;

/*!
\ingroup meta_types
\brief 第二分量标记。
*/
typedef n_tag<1> second_tag;

/*!
\ingroup meta_types
\brief 变量参数标记。
\since build 303 。
*/
template<size_t... _vSeq>
struct variadic_sequence
{};
//@}


/*!
\ingroup meta_operations
\brief 取自然数变量标记后继。
\since build 303 。
*/
//@{
template<class>
struct make_successor;

template<size_t... _vSeq>
struct make_successor<variadic_sequence<_vSeq...>>
{
	typedef variadic_sequence<_vSeq..., sizeof...(_vSeq)> type;
};
//@}


/*!
\ingroup meta_operations
\brief 取自然数变量标记序列。
\since build 303 。
*/
//@{
template<size_t _vN>
struct make_natural_sequence
{
	typedef typename make_successor<typename
		make_natural_sequence<_vN - 1>::type>::type type;
};

template<>
struct make_natural_sequence<0>
{
	typedef variadic_sequence<> type;
};
//@}

} // namespace ystdex;

#endif

