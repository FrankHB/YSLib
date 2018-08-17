﻿/*
	© 2011-2013, 2016, 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file type_inspection.hpp
\ingroup YStandardEx
\brief 类型检查元编程设施。
\version r1936
\author FrankHB <frankhb1989@gmail.com>
\since build 832
\par 创建时间:
	2018-07-23 17:54:58 +0800
\par 修改时间:
	2018-08-17 03:57 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::TypeInspection
*/


#ifndef YB_INC_ystdex_type_inspection_hpp_
#define YB_INC_ystdex_type_inspection_hpp_ 1

#include "../ydef.h" // for <type_traits>;

namespace ystdex
{

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
using std::is_pod;
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

//! \since build 832
#if __cpp_lib_is_null_pointer >= 201309L
using std::is_null_pointer;
#else
template<typename _type>
struct is_null_pointer : is_same<remove_cv_t<_type>, std::nullptr_t>
{};
#endif

} // inline namespace cpp2014;
//@}

} // namespace ystdex;

#endif

