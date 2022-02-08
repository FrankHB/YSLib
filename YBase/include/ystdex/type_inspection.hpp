/*
	© 2011-2013, 2016, 2018-2019, 2021-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file type_inspection.hpp
\ingroup YStandardEx
\brief 类型检查元编程设施。
\version r2028
\author FrankHB <frankhb1989@gmail.com>
\since build 832
\par 创建时间:
	2018-07-23 17:54:58 +0800
\par 修改时间:
	2022-02-08 22:26 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::TypeInspection
*/


#ifndef YB_INC_ystdex_type_inspection_hpp_
#define YB_INC_ystdex_type_inspection_hpp_ 1

#include "integral_constant.hpp" // for "integral_cnstant.hpp", <type_traits>,
//	__cpp_lib_is_null_pointer, __cpp_lib_is_final, std::remove_cv_t, nullptr_t,
//	bool_, __has_extension, false_;

/*!
\brief \c \<type_traits> 特性测试宏。
\see ISO C++20 [version.syn] 。
\see WG21 P0941R2 2.2 。
\see https://blogs.msdn.microsoft.com/vcblog/2016/10/11/c1417-features-and-stl-fixes-in-vs-15-preview-5/ 。
\see https://docs.microsoft.com/cpp/preprocessor/predefined-macros 。
\since build 832
*/
//@{
#ifndef __cpp_lib_is_final
#	if YB_IMPL_MSCPP >= 1900 || __cplusplus >= 201402L
#		define __cpp_lib_is_final 201402L
#	endif
#endif
#ifndef __cpp_lib_is_null_pointer
#	if YB_IMPL_MSCPP >= 1900 || __cplusplus >= 201309L
#		define __cpp_lib_is_null_pointer 201309L
#	endif
#endif
//@}

namespace ystdex
{

/*!	\defgroup type_traits_operations Type Traits Operations
\ingroup metafunctions traits
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
\brief 包含 ISO C++ 2011 引入的名称的命名空间。
\since build 607
*/
inline namespace cpp2011
{

//! \since build 245
//@{
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
//@}

} // inline namespace cpp2011;


/*!
\brief 包含 ISO C++ 2014 引入的名称的命名空间。
\since build 607
*/
inline namespace cpp2014
{

/*!
\see LWG 2247 。
\since build 832
*/
//@{
#if __cpp_lib_is_null_pointer >= 201309L
using std::is_null_pointer;
#else
//! \ingroup YBase_replacement_features unary_type_traits
template<typename _type>
struct is_null_pointer
	: is_same<typename std::remove_cv<_type>::type, nullptr_t>
{};
#endif
//@}

/*!
\see LWG 2112 。
\since build 938
*/
//@{
#if __cpp_lib_is_final >= 201402L
using std::is_final;
#else
/*!
\ingroup YBase_replacement_features unary_type_traits
\warning 当实现没有 __is_final 扩展时不提供支持，结果总是视为不是 final 类。
*/
template<typename _type>
struct is_final
// XXX: See https://blogs.msdn.microsoft.com/vcblog/2016/10/11/c1417-features-and-stl-fixes-in-vs-15-preview-5/,
//	https://docs.microsoft.com/cpp/preprocessor/predefined-macros,
//	https://gcc.gnu.org/bugzilla/show_bug.cgi?id=51365 and
//	https://reviews.llvm.org/rL145775.
#	if __has_extension(__is_final) || YB_IMPL_MSCPP >= 1900 \
	|| YB_IMPL_GNUCPP >= 40700 || YB_IMPL_CLANGPP >= 30200
	: bool_<__is_final(_type)>
#	else
	// XXX: This is not accurate unless 'final' is not supported by the
	//	language, which is false except in %LibDefect or %YDefinition.
	: false_
#	endif
{};
#endif
//@}

} // inline namespace cpp2014;
//@}

} // namespace ystdex;

#endif

