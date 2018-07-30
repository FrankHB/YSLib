/*
	© 2011-2016, 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file meta.hpp
\ingroup YStandardEx
\brief 通用元编程设施。
\version r1575
\author FrankHB <frankhb1989@gmail.com>
\since build 832
\par 创建时间:
	2018-07-23 17:22:28 +0800
\par 修改时间:
	2018-07-25 20:09 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Meta
*/


#ifndef YB_INC_ystdex_meta_hpp_
#define YB_INC_ystdex_meta_hpp_ 1

#include "type_inspection.hpp" // for "type_inspection.hpp",
//	__cpp_lib_transformation_trait_aliases, __cpp_lib_void_t, is_same;
#include "integral_constant.hpp" // for true_;

namespace ystdex
{

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
//! \since build 595
using std::aligned_union;
using std::decay;
using std::enable_if;
using std::conditional;
using std::common_type;
//! \since build 439
using std::underlying_type;
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
#if __cpp_lib_transformation_trait_aliases >= 201304
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
using std::aligned_union_t;
using std::decay_t;
using std::enable_if_t;
using std::conditional_t;
using std::common_type_t;
using std::underlying_type_t;
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
template<size_t _vLen, typename... _types>
using aligned_union_t = typename aligned_union<_vLen, _types...>::type;

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
//@}
#endif
//@}

} // inline namespace cpp2014;

//! \ingroup transformation_traits
//@{
//! \since build 833
//@{
#if __cplusplus > 201703L
using std::remove_cvref;
using std::remove_cvref_t;
#else
/*!
\brief 移除可能被 cv-qualifier 修饰的引用类型。
\note remove_pointer 包含 cv-qualifier 的移除，不需要对应版本。
\see WG21 P0550R2 。
*/
//@{
template<typename _type>
struct remove_cvref : remove_cv<remove_reference_t<_type>>
{};

//! \since build 833
template<typename _type>
using remove_cvref_t = _t<remove_cvref<_type>>;
#endif
//@}
//@}


//! \ingroup metafunctions
//@{
//! \brief 表达式 SFINAE 别名模板。
//@{
//! \since build 653
template<typename _type>
struct always
{
private:
	template<typename...>
	struct impl
	{
		using type = _type;
	};

public:
	template<typename... _types>
	using apply = impl<_types...>;
};

/*!
\brief void_t 的一般化，可指定别名类型。
\see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=59204 。
\see CWG 1558 。
\since build 688
*/
#if YB_IMPL_GNUCPP
template<typename _type, typename...>
using well_formed_t = _type;
#else
template<typename _type, typename... _types>
using well_formed_t = typename always<_type>::template apply<_types...>::type;
#endif

//! \since build 831
inline namespace cpp2017
{

/*!
\see WG21 N3911 。
\see WG21 N4296 20.10.2[meta.type.synop] 。
\since build 591
*/
#if __cpp_lib_void_t >= 201411
using std::void_t;
#else
template<typename... _types>
using void_t = well_formed_t<void, _types...>;
#endif

} // inline namespace cpp2017;


/*!
\brief 类似 void_t 的元函数，具有较低匹配优先级，避免偏特化歧义；但使用条件表达式。
\sa void_t
\note 无定义。
\since build 684
*/
template<bool _bCond>
struct when;

/*!
\brief 类似 void_t 的元函数，但具有较低匹配优先级，避免偏特化歧义。
\sa void_t
\since build 688
*/
template<typename... _types>
using when_valid = well_formed_t<when<true>, _types...>;

/*!
\brief 类似 enable_if_t 的元函数，但具有较低匹配优先级，避免偏特化歧义。
\sa enable_if_t
\sa when
\since build 649
*/
template<bool _bCond>
using enable_when = enable_if_t<_bCond, when<true>>;
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
\see WG21 N4502 6.6 。
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
\see WG21 N4502 。
*/
//@{
namespace details
{

template<typename _tDefault, typename, template<typename...> class, typename...>
struct detector
{
	using value_t = false_;
	using type = _tDefault;
};

template<typename _tDefault, template<typename...> class _gOp,
	typename... _tParams>
struct detector<_tDefault, void_t<_gOp<_tParams...>>, _gOp, _tParams...>
{
	using value_t = true_;
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
\ingroup unary_type_traits
\tparam _type 需要判断特征的类型参数。
*/
//@{
//! \note 以下参数可能是 cv 修饰的类型，结果和去除 cv 修饰符的类型一致。
//@{
/*!
\brief 判断是否为未知大小的数组类型。
\since build 651
*/
template<typename _type>
struct is_array_of_unknown_bound
	: bool_<is_array<_type>::value && extent<_type>::value == 0>
{};


/*!
\brief 判断指定类型是否为对象或 void 类型。
\since build 630
*/
template<typename _type>
struct is_object_or_void : or_<is_object<_type>, is_void<_type>>
{};


//! \since build 694
namespace details
{

template<typename _type>
struct is_referenceable_function : false_
{};

template<typename _tRes, typename... _tParams>
struct is_referenceable_function<_tRes(_tParams...)> : true_
{};

template<typename _tRes, typename... _tParams>
struct is_referenceable_function<_tRes(_tParams..., ...)> : true_
{};

} // namespace details;

/*!
\brief 判断指定类型是否为可引用类型。
\see ISO C++11 17.3.20 [defns.referenceable] 。
\since build 694
*/
template<typename _type>
struct is_referenceable : or_<is_object<_type>, is_reference<_type>,
	details::is_referenceable_function<_type>>
{};



/*!
\brief 判断指定类型是否可作为返回类型。
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


/*!
\brief 判断类型是否为非 const 对象类型。
\since build 650
*/
template<typename _type>
struct is_nonconst_object
	: and_<is_object<_type>, is_same<add_const_t<_type>, _type>>
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
\brief 判断指定类型是否为 const 或 volatile 类型。
\since build 590
*/
template<typename _type>
struct is_cv : or_<is_const<_type>, is_volatile<_type>>
{};


/*!
\brief 判断指定类型是否可分解为一个参数为类型的模板和类型参数。
\since build 683
*/
//@{
template<typename>
struct is_decomposable : false_
{};

template<template<typename...> class _gOp, typename... _types>
struct is_decomposable<_gOp<_types...>> : true_
{};
//@}


/*!
\brief 判断指定类型是否已退化。
\since build 529
*/
template<typename _type>
struct is_decayed : is_same<decay_t<_type>, _type>
{};


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


template<typename _type>
struct is_trivially_copyable : and_<is_trivially_copy_constructible<_type>,
	is_trivially_copy_assignable<_type>>
{};


template<typename _type>
struct is_trivially_moveable : and_<is_trivially_move_constructible<_type>,
	is_trivially_move_assignable<_type>>
{};
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


//! \since build 832
//@{
//! \brief 判断指定类型是否可被指定参数类型构造且要求显式构造。
template <typename _type, typename _tFrom>
struct is_explicitly_constructible : and_<is_constructible<_type, _tFrom>,
	not_<is_convertible<_tFrom, _type>>>
{};


//! \brief 判断指定类型是否可被指定参数类型无异常抛出构造且要求显式构造。
template <typename _type, typename _tFrom>
struct is_explicitly_nothrow_constructible : and_<is_nothrow_constructible<
	_type, _tFrom>, not_<is_convertible<_tFrom, _type>>>
{};


//! \brief 判断指定类型是否可被指定参数类型构造且可隐式构造。
template <typename _type, typename _tFrom>
struct is_implicitly_constructible : and_<is_constructible<_type, _tFrom>,
	is_convertible<_tFrom, _type>>
{};


//! \brief 判断指定类型是否可被指定参数类型无异常抛出构造且可隐式构造。
template <typename _type, typename _tFrom>
struct is_implicitly_nothrow_constructible : and_<is_nothrow_constructible<
	_type, _tFrom>, is_convertible<_tFrom, _type>>
{};
//@}
//@}


/*!
\ingroup metafunctions
\since build 671
*/
//@{
//! \brief 判断第一个参数和之后的参数指定的类型相同。
template<typename _type, typename... _types>
struct are_same : and_<is_same<_type, _types>...>
{};

//! \brief 判断第一个参数在之后参数指定的类型中出现。
template<typename _type, typename... _types>
struct is_in_types : or_<is_same<_type, _types...>>
{};
//@}


/*!
\ingroup metafunctions
\brief 条件判断。
\sa conditional_t
\since build 730
*/
template<typename _tCond, typename _tThen = true_, typename _tElse = false_>
using cond_t = conditional_t<_tCond::value, _tThen, _tElse>;

//! \ingroup transformation_traits
//@{
/*!
\brief 恒等元函数。
\note 功能可以使用 ISO C++ 11 的 std::common_type 的单一参数实例替代。
\note LWG 2141 建议更改 std::common_type 的实现，无法替代。
\note 这里的实现不依赖 std::common_type 。
\note 同 boost::mpl::identity 。
\note Microsoft VC++ 2013 使用 LWG 2141 建议的实现。
\see LWG 2141 。
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
\sa enable_if_t
\since build 575
*/
//@{
template<typename _tFrom, typename _tTo, typename _type = void>
using enable_if_convertible_t
	= enable_if_t<is_convertible<_tFrom, _tTo>::value, _type>;

//! \since build 679
template<typename _tFrom, typename _tTo, typename _type = void>
using enable_if_inconvertible_t
	= enable_if_t<!is_convertible<_tFrom, _tTo>::value, _type>;

template<typename _type1, typename _type2, typename _type = void>
using enable_if_interoperable_t
	= enable_if_t<is_interoperable<_type1, _type2>::value, _type>;

//! \since build 614
template<typename _type1, typename _type2, typename _type = void>
using enable_if_same_t
	= enable_if_t<is_same<_type1, _type2>::value, _type>;

/*!
\brief 移除选择类类型的特定重载避免构造模板和复制/转移特殊成员函数冲突。
\pre 第一参数为类类型。
\since build 687
*/
template<class _tClass, typename _tParam, typename _type = void>
using exclude_self_t
	= enable_if_t<!is_same<_tClass&, decay_t<_tParam>&>::value, _type>;
//@}
//@}

} // namespace ystdex;

#endif

