/*
	© 2011-2016, 2018-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file meta.hpp
\ingroup YStandardEx
\brief 通用元编程设施。
\version r1988
\author FrankHB <frankhb1989@gmail.com>
\since build 832
\par 创建时间:
	2018-07-23 17:22:28 +0800
\par 修改时间:
	2022-02-28 21:08 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Meta
*/


#ifndef YB_INC_ystdex_meta_hpp_
#define YB_INC_ystdex_meta_hpp_ 1

#include "type_inspection.hpp" // for "type_inspection.hpp", <type_traits>,
//	__cpp_lib_transformation_trait_aliases, __cpp_lib_remove_cvref,
//	__cpp_lib_void_t, __cpp_lib_type_identity, true_, is_same, is_convertible,
//	or_, nor_, and_, is_class, is_union, is_trivial, not_, is_final, size_t_,
//	is_unsigned;

/*!
\brief \c \<type_traits> 特性测试宏。
\see ISO C++20 [version.syn] 。
\see WG21 P0941R2 2.2 。
\see https://docs.microsoft.com/cpp/visual-cpp-language-conformance 。
\see https://docs.microsoft.com/cpp/preprocessor/predefined-macros 。
\since build 934
*/
//@{
/*!
\see https://blogs.msdn.microsoft.com/vcblog/2016/10/11/c1417-features-and-stl-fixes-in-vs-15-preview-5/ 。
\since build 832
*/
//@{
#ifndef __cpp_lib_transformation_trait_aliases
#	if YB_IMPL_MSCPP >= 1800 || __cplusplus >= 201304L
#		define __cpp_lib_transformation_trait_aliases 201304L
#	endif
#endif
//@}
//! \see WG21 P1902R1 。
#ifndef __cpp_lib_remove_cvref
#	if (YB_IMPL_MSCPP >= 1920 && _MSVC_LANG >= 201711L) \
	|| __cplusplus >= 202002L
#		define __cpp_lib_remove_cvref 201711L
#	endif
#endif
/*!
\see https://blogs.msdn.microsoft.com/vcblog/2016/10/11/c1417-features-and-stl-fixes-in-vs-15-preview-5/ 。
\since build 679
*/
//@{
#ifndef __cpp_lib_void_t
#	if YB_IMPL_MSCPP >= 1900 || __cplusplus >= 201411L
#		define __cpp_lib_void_t 201411L
#	endif
#endif
//@}
//! \see WG21 P1902R1 。
#ifndef __cpp_lib_type_identity
#	if (YB_IMPL_MSCPP >= 1921 && _MSVC_LANG >= 201806L) \
	|| __cplusplus >= 202002L
#		define __cpp_lib_type_identity 201806L
#	endif
#endif
//@}

namespace ystdex
{

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

// XXX: See $2018-12 @ %Documentation::Workflow.
//! \see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61458 。
using std::aligned_storage;
//! \since build 595
using std::aligned_union;
using std::decay;
using std::enable_if;
using std::conditional;
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
#if __cpp_lib_transformation_trait_aliases >= 201304L
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

template<typename _type>
using underlying_type_t = typename underlying_type<_type>::type;
//@}
#endif
//@}

} // inline namespace cpp2014;

// NOTE: This requires the support some features of WG21 P0898R3. See
//	https://docs.microsoft.com/cpp/visual-cpp-language-conformance and
//	https://docs.microsoft.com/cpp/preprocessor/predefined-macros.
#if (YB_IMPL_MSCPP >= 1923 && _MSVC_LANG >= 202002L) || __cplusplus >= 202002L
#	define YB_Impl_TypeTraits_has_cpp20_common_type true
#else
#	define YB_Impl_TypeTraits_has_cpp20_common_type false
//! \since build 938
namespace details
{

template<typename _type1, typename _type2, typename = decay_t<_type1>,
	typename = decay_t<_type2>>
struct common_type2;

// XXX: This is to be specialized later.
template<typename, typename, typename, typename...>
struct common_type3
{};

} // namespace details;
#endif

//! \since build 934
inline namespace cpp2020
{

//! \ingroup transformation_traits
//@{
//! \since build 833
//@{
#if __cpp_lib_remove_cvref >= 201711L
using std::remove_cvref;
using std::remove_cvref_t;
#else
/*!
\ingroup YBase_replacement_features
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


#if YB_Impl_TypeTraits_has_cpp20_common_type
//! \since build 245
using std::common_type;
//! \since build 607
using std::common_type_t;
#else
//! \since build 938
//@{
/*!
\ingroup YBase_replacement_features
\brief 取公共类型。
\note 兼容 ISO C++20 的要求。
\see LWG 2141 。
\see LWG 2408 。
\see LWG 2465 。
\see LWG 2763 。
\see LWG 3205 。
\see WG21 P0435R1 。
\see WG21 P0548R1 。
\see WG21 P0898R3 。

同 ISO C++20 的 std::common_type ，但不支持程序定义的特化，
	而直接使用 std::common_type 的程序定义的特化（若存在）。
从 ISO C++11 起提供 std::common_type 的要求在不同标准版本中略有不同，如：
自 ISO C++14 起 LWG 2141 的解决要求成员 type 类型（若存在）为退化类型；
WG21 P0435R1 解决 LWG 2465（同时涵盖 LWG 2460 的第一部分），
	不再允许程序提供具有 cv 限定符的特化的模板参数。
符合先前版本（如 ISO C++11 ）要求的程序定义的特化，因为这些新增的限制，
	可能不再符合之后版本的 std::common_type 的要求。
本特征在确定二元形式的 type 时，依次使用：
	std::common_type 特化的 type 类型（若存在），
		或其退化类型（在使用 ISO C++14 前的标准版本时）；
	若不存在上述类型，依照 ISO C++20 起自 WG21 P0898R3 的新增规则，
		尝试确定并提供 type 类型。
上述策略兼容 ISO C++20 的 std::common_type 的行为，
	同时支持为之前版本标准的 std::common_type 提供的特化。
其它变化包括：
LWG 2408 的解决要求实现支持 SFINAE ；
LWG 2763 的解决修复了一处特化问题；
P0548R1 调整了 P0435R1 依赖的特化；
在 ISO C++20 之后，LWG 3205 简化了 ISO C++20 新增规则的实现，但没有改变实质性要求。
以上特性和修正都在本特征的实现中被支持。
关于 ISO C++11 以来的 SFINAE 支持的其它提案，另见 N3843 和 P0015 。
*/
//@{
template<typename...>
struct common_type
{};

template<typename _type>
struct common_type<_type> : common_type<_type, _type>
{};

template<typename _type1, typename _type2>
struct common_type<_type1, _type2> : details::common_type2<_type1, _type2>
{};

template<typename _type1, typename _type2, typename... _types>
struct common_type<_type1, _type2, _types...>
	: details::common_type3<void, _type1, _type2, _types...>
{};


//! \since build 340
template<typename... _types>
using common_type_t = typename common_type<_types...>::type;
//@}
//@}
#endif
//@}

} // inline namespace cpp2020;


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
\see CWG 1558 。
\see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=59204 。
\see https://stackoverflow.com/questions/31907885/void-t-fails-on-visual-studio-2015 。
\since build 688
*/
#if YB_IMPL_MSCPP >= 1900 || YB_IMPL_GNUCPP > 50000 || YB_IMPL_CLANGPP \
	|| __cplusplus >= 201703L
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
#if __cpp_lib_void_t >= 201411L
using std::void_t;
#else
//! \ingroup YBase_replacement_features
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
template<bool>
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


#if !YB_Impl_TypeTraits_has_cpp20_common_type
//! \since build 938
namespace details
{

template<typename _type1, typename _type2>
using cond_test_t
	= decltype(true ? std::declval<_type1>() : std::declval<_type2>());

#if false
// NOTE: Exposition only. This is the implementation conforming to
//	%std::common_type by default (without the program-defined specialization).
template<typename _type1, typename _type2>
using common_type2_specialized_t = cond_test_t<_type1, _type2>;
#else
// NOTE: This supports %std::common_type specializations for %common_type.
#	if __cplusplus >= 201411L
template<typename _type1, typename _type2>
using common_type2_specialized_t = std::common_type_t<_type1, _type2>;
#	else
// NOTE: The resolution of LWG 2141 is mandated since ISO C++14. For a valid
//	specialization of %std::common_type in ISO C++11, it might has a nested
//	member %type of a reference, which is not conforming since ISO C++14. So,
//	just decay it here.
template<typename _type1, typename _type2>
using common_type2_specialized_t
	= decay_t<typename std::common_type<_type1, _type2>::type>;
#	endif
#endif

template<typename _type1, typename _type2, typename = void>
struct common_type_cond_ref
{};

// NOTE: See LWG 3205. Current libstdc++, libc++ and Microsoft VC++'s
//	implementations all applies the resolution in ISO C++20 mode.
template<typename _type1, typename _type2>
struct common_type_cond_ref<_type1, _type2,
	void_t<cond_test_t<const _type1&, const _type2&>>>
	: remove_cvref<cond_test_t<_type1, _type2>>
{};


template<typename _type1, typename _type2, typename = void>
struct common_type2_decayed : common_type_cond_ref<_type1, _type2>
{};

// NOTE: See WG21 P0898R3.
template<typename _type1, typename _type2>
struct common_type2_decayed<_type1, _type2,
	void_t<common_type2_specialized_t<_type1, _type2>>>
	: decay<common_type2_specialized_t<_type1, _type2>>
{};


template<typename, typename, typename _tDecayed1, typename _tDecayed2>
struct common_type2 : common_type<_tDecayed1, _tDecayed2>
{};

template<typename _type1, typename _type2>
struct common_type2<_type1, _type2, _type1, _type2>
	: common_type2_decayed<_type1, _type2>
{};


template<typename _type1, typename _type2, typename... _types>
struct common_type3<
	void_t<common_type_t<_type1, _type2>>, _type1, _type2, _types...>
	: common_type<common_type_t<_type1, _type2>, _types...>
{};

} // namespace details;
#endif


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


//! \since build 867
//@{
template<typename _type1, typename _type2,
	bool _vSame = is_same<_type1, _type2>::value>
struct is_same_or_convertible : is_convertible<_type1, _type2>
{};

template<typename _type1, typename _type2>
struct is_same_or_convertible<_type1, _type2, true> : true_
{};
//@}

} // namespace details;

//! \ingroup YBase_replacement_features
//@{
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

template<typename _tRes, typename... _tParams ynoexcept_param(ne)>
struct is_referenceable_function<_tRes(_tParams...) ynoexcept_qual(ne)> : true_
{};

template<typename _tRes, typename... _tParams ynoexcept_param(ne)>
struct is_referenceable_function<_tRes(_tParams..., ...) ynoexcept_qual(ne)>
	: true_
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
	: nor_<is_array<_type>, is_abstract<_type>, is_function<_type>>
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
//@}


/*!
\pre remove_all_extents\<_type> 是完整类型或（可能 cv 修饰的） \c void 。
\see ISO C++17 [class.prop]/2 。
\since build 853
*/
//@{
/*!
\brief 判断指定类型是否是平凡的非联合类类型。
\note 注意和 ISO C++ 的 \c std::is_class 类似，排除联合。
*/
template<typename _type>
struct is_trivial_class : and_<is_trivial<_type>, is_class<_type>>
{};

template<typename _type>
struct is_trivial_class_type : and_<is_trivial<_type>, is_class_type<_type>>
{};

//! \brief 判断指定类型是否是平凡的联合类型。
template<typename _type>
struct is_trivial_union : and_<is_trivial<_type>, is_union<_type>>
{};
//@}


/*!
\brief 判断指定类型是否是可被继承的类类型。
\since build 940
*/
template<typename _type>
struct is_inheritable_class : and_<is_class<_type>, not_<is_final<_type>>>
{};
//@}


/*!
\brief 判断指定类型是否为 const 或 volatile 类型。
\since build 590
*/
template<typename _type>
struct is_cv : or_<is_const<_type>, is_volatile<_type>>
{};
//@}


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
\pre remove_all_extents\<_type> 是完整类型、（可能 cv 修饰的） \c void ，
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


/*!
\brief 判断指定类型是否为可复制构造但不可无抛出异常地转移构造的类型。
\note 和 std::move_if_noexcept 需要避免复制的条件相同。
\since build 865
*/
template<typename _type>
struct is_throwing_move_copyable : and_<is_copy_constructible<_type>,
	not_<is_nothrow_move_constructible<_type>>>
{};
//@}
//@}


//! \note 参数可以是不完整类型。
//@{
//! \ingroup type_traits_operations
//@{
/*!
\brief 判断指定类型是否已退化为指定类型。
\since build 841
*/
template<typename _type, typename _tTo = _type>
struct is_decayed : is_same<_type, decay_t<_tTo>>
{};


/*!
\brief 判断指定类型是否为无限定的指定类型。
\since build 865
*/
template<typename _type, typename _tTo = _type>
struct is_unqualified : is_same<_type, remove_cv_t<_tTo>>
{};
//@}


/*!
\ingroup unary_type_trait
\brief 判断类型是否为非 const 对象类型。
\since build 867
*/
template<typename _type>
struct is_unqualified_object
	: and_<is_object<_type>, is_unqualified<_type>>
{};
//@}


//! \ingroup binary_type_traits
//@{
/*!
\brief 判断指定类型之间是否相同或可转换。
\note 类似 is_convertible ，但不要求完整类型。
\since build 850
*/
template<typename _type1, typename _type2>
struct is_same_or_convertible : details::is_same_or_convertible<_type1, _type2>
{};


/*!
\brief 判断指定类型之间是否相同或可互相转换。
\since build 575
*/
template<typename _type1, typename _type2>
struct is_interoperable : or_<is_same_or_convertible<_type1, _type2>,
	is_convertible<_type2, _type1>>
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


/*!
\brief 判断第一参数和第二参数指定的参数类型相同。
\since build 865
*/
template<typename _type, typename _tParam>
struct is_same_param : is_same<_type&, decay_t<_tParam>&>
{};
//@}


/*!
\ingroup metafunctions
\since build 671
*/
//@{
//! \brief 判断第一参数和之后的参数指定的类型相同。
template<typename _type, typename... _types>
struct are_same : and_<is_same<_type, _types>...>
{};

//! \brief 判断第一参数在之后参数指定的类型中出现。
template<typename _type, typename... _types>
struct is_in_types : or_<is_same<_type, _types>...>
{};
//@}


/*!
\ingroup metafunctions
\brief 条件判断。
*/
//@{
/*!
\sa conditional
\since build 846
*/
template<typename _tCond, typename _tThen = true_, typename _tElse = false_>
using cond = conditional<_tCond::value, _tThen, _tElse>;

/*!
\sa conditional_t
\since build 730
*/
template<typename _tCond, typename _tThen = true_, typename _tElse = false_>
using cond_t = _t<cond<_tCond, _tThen, _tElse>>;
//@}

//! \ingroup transformation_traits
//@{
/*!
\brief 恒等元函数。
\note 功能可以使用 ISO C++11 的 std::common_type 的单一参数实例替代。
\note LWG 2141 建议更改 std::common_type 的实现，无法替代。
\note 这里的实现不依赖 std::common_type 。
\note 同 boost::mpl::identity 。
\note Microsoft VC++ 2013 使用 LWG 2141 建议的实现。
\sa id
\see LWG 2141 。
\see http://www.boost.org/doc/libs/1_55_0/libs/mpl/doc/refmanual/identity.html 。
\see http://msdn.microsoft.com/library/vstudio/bb531344%28v=vs.120%29.aspx 。
\see http://lists.cs.uiuc.edu/pipermail/cfe-commits/Week-of-Mon-20131007/090403.html 。
\since build 334

结果总是和参数类型相同的元函数。
注意和 ISO C++ 从 Range TS 引入的 std::identity 不同，
	后者是 <functional> 中提供的关于值的仿函数。
在 ystdex 中，identity 的名称保留为元函数。
可使用 id 代替 std::identity 的使用：id<> 实现和 std::identity 的相同的功能。
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


/*!
\see WG21 P0887R1 。
\since build 934
*/
inline namespace cpp2020
{

#if __cpp_lib_type_identity >= 201806L
using std::type_identity;
using std::type_identity_t;
#else
template<typename _type>
struct type_identity : yimpl(identity<_type>)
{};

template<typename _type>
using type_identity_t = _t<type_identity<_type>>;
#endif

} //inline namespace cpp2020;
//@}


//! \since build 843
template<typename _type>
using sizeof_t = size_t_<sizeof(_type)>;


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

//! \since build 865
template<typename _type1, typename _type2, typename _type = void>
using enable_if_same_param_t
	= enable_if_t<is_same_param<_type1, _type2>::value, _type>;

/*!
\brief 启用无符号类型参数的重载。
\since build 933
*/
template<typename _tParam, typename _type = remove_cvref_t<_tParam>>
using enable_if_unsigned_t
	= enable_if_t<is_unsigned<remove_cvref_t<_tParam>>::value, _type>;

/*!
\brief 排除选择类型的特定参数的重载。
\note 第一参数通常为类类型，可避免被选择的类的构造模板和复制/转移特殊成员函数冲突。
\since build 687
*/
template<typename _tSelected, typename _tParam, typename _type = void>
using exclude_self_t
	= enable_if_t<!is_same_param<_tSelected, _tParam>::value, _type>;
//@}
//@}

} // namespace ystdex;

#endif

