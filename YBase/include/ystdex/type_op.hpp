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
\version r2308
\author FrankHB <frankhb1989@gmail.com>
\since build 201
\par 创建时间:
	2011-04-14 08:54:25 +0800
\par 修改时间:
	2015-11-04 11:19 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::TypeOperation
*/


#ifndef YB_INC_ystdex_type_op_hpp_
#define YB_INC_ystdex_type_op_hpp_ 1

#include "type_traits.hpp" // for or_, is_const, is_volatile, is_same, decay_t,
//	is_object, is_void, not_, is_abstract, is_function, is_class, is_union,
//	is_pointer, remove_pointer_t, is_lvalue_reference, is_rvalue_reference,
//	is_pod, is_copy_constructible, is_copy_assignable, is_move_constructible,
//	is_move_assignable, is_nothrow_copy_constructible,
//	is_nothrow_copy_assignable, is_nothrow_move_constructible,
//	is_nothrow_move_assignable, is_trivially_copy_constructible,
//	is_trivially_copy_assignable, is_trivially_move_constructible,
//	is_trivially_move_assignable, is_convertible, std::declval, bool_constant,
//	remove_pointer_t, remove_reference_t, is_array,  enable_if_t, is_same;

namespace ystdex
{

/*!
\ingroup unary_type_traits
\tparam _type 需要判断特征的类型参数。
*/
//@{
/*!
\brief 判断指定类型是否为 const 或 volatile 类型。
\since build 590
*/
template<typename _type>
struct is_cv : or_<is_const<_type>, is_volatile<_type>>
{};


/*!
\brief 判断指定类型是否已退化。
\since build 529
*/
template<typename _type>
struct is_decayed : or_<is_same<decay_t<_type>, _type>>
{};


//! \note 以下参数可能是 cv 修饰的类型，结果和去除 cv 修饰符的类型一致。
//@{
/*!
\brief 判断指定类型是否为对象或 void 类型。
\since build 630
*/
template<typename _type>
struct is_object_or_void : or_<is_object<_type>, is_void<_type>>
{};


/*!
\brief 判断指定类型是否可作为返回值类型。
\note 即排除数组类型、抽象类类型和函数类型的所有类型。
\see ISO C++11 8.3.5/8 和 ISO C++11 10.4/3 。
\since build 333
*/
template<typename _type>
struct is_returnable
	: not_<or_<is_array<_type>, is_abstract<_type>, is_function<_type>>>
{};


/*!
\brief 判断指定类型是否是类类型。
\since build 588
*/
template<typename _type>
struct is_class_type
	: or_<is_class<_type>, is_union<_type>>
{};


//! \since build 630
//@{
//! \brief 判断指定类型是否是指向对象类型的指针。
template<typename _type>
struct is_pointer_to_object
	: and_<is_pointer<_type>, is_object<remove_pointer_t<_type>>>
{};


//! \brief 判断指定类型是否是指向对象类型的指针。
template<typename _type>
struct is_object_pointer
	: and_<is_pointer<_type>, is_object_or_void<remove_pointer_t<_type>>>
{};


//! \brief 判断指定类型是否是指向函数类型的指针。
template<typename _type>
struct is_function_pointer
	: and_<is_pointer<_type>, is_function<remove_pointer_t<_type>>>
{};
//@}


//! \since build 333
//@{
//! \brief 判断指定类型是否是指向类类型的指针。
template<typename _type>
struct is_class_pointer
	: and_<is_pointer<_type>, is_class<remove_pointer_t<_type>>>
{};


//! \brief 判断指定类型是否是类类型左值引用。
template<typename _type>
struct is_lvalue_class_reference
	: and_<is_lvalue_reference<_type>, is_class<remove_reference_t<_type>>>
{};


//! \brief 判断指定类型是否是类类型右值引用。
template<typename _type>
struct is_rvalue_class_reference
	: and_<is_rvalue_reference<_type>, is_class<remove_reference_t<_type>>>
{};


/*!
\pre remove_all_extents<_type> 是完整类型或（可能 cv 修饰的） \c void 。
\see ISO C++11 9/10 。
*/
//@{
//! \brief 判断指定类型是否是 POD struct 。
template<typename _type>
struct is_pod_struct : and_<is_pod<_type>, is_class<_type>>
{};


//! \brief 判断指定类型是否是 POD union 。
template<typename _type>
struct is_pod_union : and_<is_pod<_type>, is_union<_type>>
{};
//@}
//@}
//@}


/*!
\pre remove_all_extents<_type> 是完整类型、（可能 cv 修饰的） \c void ，
	或未知大小的数组。
\since build 630
*/
//@{
template<typename _type>
struct is_copyable
	: and_<is_copy_constructible<_type>, is_copy_assignable<_type>>
{};


template<typename _type>
struct is_moveable
	: and_<is_move_constructible<_type>, is_move_assignable<_type>>
{};


template<typename _type>
struct is_nothrow_copyable : and_<is_nothrow_copy_constructible<_type>,
	is_nothrow_copy_assignable<_type>>
{};


template<typename _type>
struct is_nothrow_moveable : and_<is_nothrow_move_constructible<_type>,
	is_nothrow_move_assignable<_type>>
{};


#	if !YB_IMPL_GNUC || YB_IMPL_GNUCPP >= 50000
template<typename _type>
struct is_trivially_copyable : and_<is_trivially_copy_constructible<_type>,
	is_trivially_copy_assignable<_type>>
{};


template<typename _type>
struct is_trivially_moveable : and_<is_trivially_move_constructible<_type>,
	is_trivially_move_assignable<_type>>
{};
#	endif
//@}
//@}


//! \ingroup binary_type_traits
//@{
/*!
\brief 判断指定类型之间是否可转换。
\since build 575
*/
template<typename _type1, typename _type2>
struct is_interoperable
	: or_<is_convertible<_type1, _type2>, is_convertible<_type2, _type1>>
{};


/*!
\brief 判断指定类型之间是否协变。
\warning 对内建函数类型需要包含 \c \<ystdex/functional.hpp\> 。
\since build 447
*/
template<typename _tFrom, typename _tTo>
struct is_covariant : is_convertible<_tFrom, _tTo>
{};


/*!
\brief 判断指定类型之间是否逆变。
\warning 对内建函数类型需要包含 \c \<ystdex/functional.hpp\> 。
\since build 447
*/
template<typename _tFrom, typename _tTo>
struct is_contravariant : is_convertible<_tTo, _tFrom>
{};
//@}


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

//! \since build 562
//@{
template<bool, typename, typename... _types>
struct common_nonvoid_impl
{
	using type = common_type_t<_types...>;
};

template<typename _type, typename... _types>
struct common_nonvoid_impl<false, _type, _types...>
{
	using type = _type;
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
using identity_t = typename identity<_type>::type;
//@}


/*!
\brief 移除可能被 cv-qualifier 修饰的引用类型。
\note remove_pointer 包含 cv-qualifier 的移除，不需要对应版本。
\since build 376
*/
//@{
template<typename _type>
struct remove_rcv
{
	using type = remove_cv_t<remove_reference_t<_type>>;
};

//! \since build 448
template<typename _type>
using remove_rcv_t = typename remove_rcv<_type>::type;
//@}


/*!
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
\brief 移除可能被 cv-qualifier 修饰的引用和指针类型。
\since build 376
*/
template<typename _type>
struct remove_rpcv
{
	using type = remove_cv_t<typename remove_rp<_type>::type>;
};


/*!
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
\brief 取成员指针类型指向的类类型。
\since build 612
*/
template<typename _type>
using member_target_type_t
	= typename details::member_target_type_impl<_type>::type;
//@}


//! \ingroup metafunctions
//@{
/*!
\brief 取公共非空类型：若第一参数为非空类型则为第一参数，否则从其余参数推断。
\since build 562
*/
template<typename _type, typename... _types>
using common_nonvoid_t = typename
	details::common_nonvoid_impl<is_void<_type>::value, _type, _types...>::type;

/*!
\brief 取公共底层类型。
\since build 629
*/
template<typename... _types>
using common_underlying_t = common_type_t<underlying_type_t<_types>...>;


/*!
\sa enable_if_t
\since build 575
*/
//@{
template<typename _tFrom, typename _tTo, typename _type = void>
using enable_if_convertible_t
	= enable_if_t<is_convertible<_tFrom, _tTo>::value, _type>;

template<typename _type1, typename _type2, typename _type = void>
using enable_if_interoperable_t
	= enable_if_t<is_interoperable<_type1, _type2>::value, _type>;

//! \since build 614
template<typename _type1, typename _type2, typename _type = void>
using enable_if_same_t
	= enable_if_t<is_same<_type1, _type2>::value, _type>;
//@}
//@}


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

