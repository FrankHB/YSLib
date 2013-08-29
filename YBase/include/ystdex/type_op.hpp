/*
	Copyright by FrankHB 2011 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file type_op.hpp
\ingroup YStandardEx
\brief C++ 类型操作。
\version r1128
\author FrankHB <frankhb1989@gmail.com>
\since build 201
\par 创建时间:
	2011-04-14 08:54:25 +0800
\par 修改时间:
	2013-08-29 10:37 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::TypeOperation
*/


#ifndef YB_INC_ystdex_type_op_hpp_
#define YB_INC_ystdex_type_op_hpp_ 1

#include "../ydef.h" // for <type_traits> and std::declval;

namespace ystdex
{

#if 0
using std::tr1::add_reference;

using std::tr1::has_nothrow_assign;
using std::tr1::has_nothrow_constructor;
using std::tr1::has_nothrow_copy;
using std::tr1::has_trivial_assign;
using std::tr1::has_trivial_constructor;
using std::tr1::has_trivial_copy;
using std::tr1::has_trivial_destructor;
#endif

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
#if 0
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
#endif
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
//using std::aligned_union;
using std::decay;
using std::enable_if;
using std::conditional;
using std::common_type;
//! \since build 439
using std::underlying_type;
using std::result_of;


/*!	\defgroup template_meta_programing Template Meta Programing
\brief 模板元编程。
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

/*!	\defgroup type_traits_operations Type Traits Operations
\ingroup meta_operations
\brief 类型特征操作。
\since build 306
*/

/*!	\defgroup unary_type_trait Unary Type Trait
\ingroup type_traits_operations
\brief 一元类型特征。
\see ISO C++11 20.9.1[meta.rqmts] 。
\since build 306
*/

/*!	\defgroup binary_type_trait Binary Type Trait
\ingroup type_traits_operations
\brief 二元类型特征。
\see ISO C++11 20.9.1[meta.rqmts] 。
\since build 306
*/

/*!	\defgroup metafunctions Metafunctions
\ingroup meta_operations
\brief 元函数。
\see http://www.boost.org/doc/libs/1_50_0/libs/mpl/doc/refmanual/metafunction.html 。
\since build 333
*/


/*!
\ingroup type_traits_operations
\brief ISO C++ 1y 兼容类型操作别名。
\todo 条件编译：尽可能使用语言实现。
*/
//@{
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
	size_t _vAlign = yalignof(typename std::aligned_storage<_vLen>::type)>
using aligned_storage_t = typename aligned_storage<_vLen, _vAlign>::type;
//@}

//! \since build 339
//@{
//template<size_t _vLen, typename... _types>
//using aligned_union_t = typename aligned_union<_vLen, _types...>::type;

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
//@}


/*!
\ingroup unary_type_trait
\brief 判断指定类型是否可作为返回值类型。
\note 即排除数组类型、抽象类类型和函数类型的所有类型。
\see ISO C++11 8.3.5/8 和 ISO C++11 10.4/3 。
\since build 333
*/
template<typename _type>
struct is_returnable : integral_constant<bool, !is_array<_type>::value
	&& !is_abstract<_type>::value && !is_function<_type>::value>
{};


/*!
\ingroup unary_type_trait
\brief 判断是否可被退化。
\since build 339
*/
template<typename _type>
struct is_decayable
	: integral_constant<bool, !is_same<decay_t<_type>, _type>::value>
{};


/*!
\ingroup unary_type_trait
\brief 判断指定类型是否是指向类类型对象的指针。
\since build 333
*/
template<typename _type>
struct is_class_pointer : integral_constant<bool, is_pointer<_type>::value
	&& is_class<remove_pointer_t<_type>>::value>
{};


/*!
\ingroup unary_type_trait
\brief 判断指定类型是否是左值类类型引用。
\since build 333
*/
template<typename _type>
struct is_lvalue_class_reference : integral_constant<bool, !is_lvalue_reference<
	_type>::value && is_class<remove_reference_t<_type>>::value>
{};


/*!
\ingroup unary_type_trait
\brief 判断指定类型是否是右值类类型引用。
\note 即排除数组和函数类型的所有类型。
\since build 333
*/
template<typename _type>
struct is_rvalue_class_reference : integral_constant<bool, !is_lvalue_reference<
	_type>::value && is_class<typename remove_reference<_type>::type>::value>
{};


/*!
\ingroup unary_type_trait
\brief 判断指定类型是否是 POD struct 。
\see ISO C++11 9/10 。
\since build 333
*/
template<typename _type>
struct is_pod_struct : integral_constant<bool,
	is_pod<_type>::value && is_class<_type>::value>
{};


/*!
\ingroup unary_type_trait
\brief 判断指定类型是否是 POD union 。
\see ISO C++11 9/10 。
\since build 333
*/
template<typename _type>
struct is_pod_union : integral_constant<bool,
	is_pod<_type>::value && is_union<_type>::value>
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
\brief 测试包含指定名称的嵌套成员。
\warning 当参数包含前置下划线时可能和保留名称冲突。
\since build 440
*/
#define YB_HAS_MEMBER(_n) \
	template<class _type> \
	struct has_mem_##_n \
	{ \
	private: \
		template<typename _type2> \
		static std::true_type \
		test(ystdex::empty_base<typename _type2::_n>*); \
		template<typename _type2> \
		static std::false_type \
		test(...); \
	\
	public: \
		static yconstexpr bool value = decltype(test<_type>(nullptr))::value; \
	};


/*!
\since build 440

测试包含 value 成员。
*/
YB_HAS_MEMBER(value)


/*!
\brief 测试包含指定的 2 个类型的表达式是否合式。
\since build 399
*/
#define YB_TYPE_OP_TEST_2(_n, _expr) \
	template<typename _type1, typename _type2> \
	struct _n \
	{ \
	private: \
		template<typename _type> \
		static std::true_type \
		test(ystdex::enable_if_t<(_expr), int>); \
		template<typename> \
		static std::false_type \
		test(...); \
	\
	public: \
		static yconstexpr bool value = decltype(test<_type1>(0))::value; \
	};


/*!
\since build 306

测试可比较相等性。
*/
YB_TYPE_OP_TEST_2(have_equality_operator, (is_convertible<decltype(std::declval<
	_type>() == std::declval<_type2>()), bool>::value))


//! \since build 399
YB_TYPE_OP_TEST_2(has_subscription, !is_void<decltype(std::declval<_type>()[
	std::declval<_type2>()])>::value)


#if YB_IMPL_GNUCPP && YB_IMPL_GNUCPP >= 40600
//#	pragma GCC diagnostic warning "-Wctor-dtor-privacy"
#	pragma GCC diagnostic pop
#endif


/*!
\since build 306
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
\since build 306
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
\ingroup unary_type_trait
\brief 判断 _type 是否包含 value 成员。
\since build 440
*/
template<class _type>
struct has_mem_value : std::integral_constant<bool,
	details::has_mem_value<remove_cv_t<_type>>::value>
{};


/*!
\ingroup binary_type_trait
\brief 判断是否存在合式的结果为非 void 类型的 [] 操作符接受指定类型的表达式。
\since build 399
*/
template<typename _type1, typename _type2>
struct has_subscription : details::has_subscription<_type1, _type2>
{};


/*!
\ingroup binary_type_trait
\brief 判断是否存在合式的结果可转换为 bool 类型的 == 操作符接受指定类型的表达式。
\since build 306
*/
template<typename _type1, typename _type2>
struct has_equality_operator : integral_constant<bool,
	details::have_equality_operator<_type1, _type2>::value>
{};


/*!
\ingroup binary_type_trait
\brief 判断指定类型是否有非空虚基类。
\since build 175
*/
template<class _type>
struct has_nonempty_virtual_base : integral_constant<bool,
	details::have_nonempty_virtual_base<_type>::value>
{};


/*!
\ingroup unary_type_trait
\brief 判断指定的两个类类型是否有非空虚基类。
\since build 175
*/
template<class _type1, class _type2>
struct has_common_nonempty_virtual_base : integral_constant<bool,
	details::have_common_nonempty_virtual_base<_type1, _type2>::value>
{};


/*!
\ingroup metafunctions
\brief 恒等元函数。
\note 功能可以使用 common_type 的单一参数实例替代。
\note 这里的实现保证不是 common_type 的实例。
\since build 334
*/
template<typename _type>
struct identity
{
	using type = _type;
};


/*!
\ingroup metafunctions
\brief 移除可能被 cv-qualifier 修饰的引用类型。
\note remove_pointer 包含 cv-qualifier 的移除，不需要对应版本。
\since build 376
*/
template<typename _type>
struct remove_rcv
{
	using type = remove_cv_t<remove_reference_t<_type>>;
};


/*!
\ingroup metafunctions
\brief 移除指针和引用类型。
\note 指针包括可能的 cv-qualifier 修饰。
\since build 175
*/
template<typename _type>
struct remove_rp
{
	using type = remove_pointer_t<remove_reference_t<_type>>;
};


/*!
\ingroup metafunctions
\brief 移除可能被 cv-qualifier 修饰的引用和指针类型。
\since build 376
*/
template<typename _type>
struct remove_rpcv
{
	using type = remove_cv_t<typename remove_rp<_type>::type>;
};


/*!
\ingroup metafunctions
\brief 数组类型退化。
\since build 290

参数为数组类型时同 decay ，否则结果类型为参数。
*/
template<typename _type>
struct array_decay
{
	using type = conditional_t<is_array<_type>::value, decay_t<_type>, _type>;
};


/*!
\ingroup metafunctions
\brief 保持修饰符的类型退化。
\note 结果不移除非数组或函数类型的引用，可用于参数转发。
\since build 290

参数移除引用后为数组或函数类型时同 decay ，否则结果为参数。
*/
template<typename _type>
struct qualified_decay
{
private:
	using value_type = remove_reference_t<_type>;

public:
	using type = conditional_t<is_function<value_type>::value
		|| is_array<value_type>::value, decay_t<_type>, _type>;
};


/*!
\ingroup metafunctions
\brief 数组及数组引用类型退化。
\since build 290

参数为非引用类型时同 array_decay ，
否则成员 type 为去除引用后 array_dacay 的类型，reference 为 type 的对应引用。
*/
//@{
template<typename _type>
struct array_ref_decay
{
	using type = typename array_decay<_type>::type;
};

template<typename _type>
struct array_ref_decay<_type&>
{
	using type = typename array_decay<_type>::type;
	using reference = type&;
};

template<typename _type>
struct array_ref_decay<_type&&>
{
	using type = typename array_decay<_type>::type;
	using reference = type&&;
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
	using type = n_tag<_vN - 1>;
};

template<>
struct n_tag<0>
{
	using type = void;
};
//@}

/*!
\ingroup meta_types
\brief 第一分量标记。
*/
using first_tag = n_tag<0>;

/*!
\ingroup meta_types
\brief 第二分量标记。
*/
using second_tag = n_tag<1>;
//@}

} // namespace ystdex;

#endif

