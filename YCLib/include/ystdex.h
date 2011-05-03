/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ystdex.h
\ingroup YCLib
\brief YCLib C++ 标准库扩展。
\version 0.2312;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-27 17:31:14 +0800; 
\par 修改时间:
	2011-05-03 14:31 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YCLib::YStandardExtend;
*/


#ifndef YCL_INC_YSTDEX_H_
#define YCL_INC_YSTDEX_H_

#ifdef __cplusplus
#	define YCL_IMPL_CPP __cplusplus
#	define YCL_IMPL_MSCPP 0
#	define YCL_IMPL_GNUCPP 0
#	ifdef _MSC_VER
#		undef YCL_IMPL_MSCPP
#		define YCL_IMPL_MSCPP _MSC_VER
#	elif defined(__GNUC__)
#		undef YCL_IMPL_GNUCPP
#		define YCL_IMPL_GNUCPP 1
#	endif
#else
#	error This header is only for C++!
#endif

#include "ydef.h"
#include <cstddef>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <type_traits>

#if YCL_IMPL_CPP >= 201103L || YCL_IMPL_MSCPP >= 1600
//#	include <type_traits>
#	define YCL_HAS_BUILTIN_NULLPTR
#else
// TODO: complete version checking for compiler and library implementation;
//#ifdef __GNUC__
//#	include <tr1/type_traits>
#endif

#ifndef YCL_CHAR_BIT
#define YCL_CHAR_BIT CHAR_BIT
#endif

#ifndef YCL_UNUSED
#	define YCL_UNUSED(arg) ((arg) = (arg))
#endif

#ifndef YCL_VOID
#	define YCL_VOID(arg) ((void)(arg))
#endif

#ifndef YCL_VOIDX
#	define YCL_VOIDX(arg) (static_cast<void>(arg))
#endif

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
//	using std::is_literal_type;
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
//	using std::result_of;


	//非 ISO C/C++ 扩展库函数。
	using ::memcmp;

	/*!
	\brief 带空指针检查的字符串长度计算。
	\return 当字符指针非空时为 std::strlen 计算的串长，否则为 0 。
	*/
	std::size_t
	strlen_n(const char*);

	/*!
	\brief 带空指针检查的字符串复制。
	\return 成功时为复制的字符串，失败时为空指针。

	当目标字符串和源字符串都非空时用 std::strcpy 复制字符串。
	*/
	char*
	strcpy_n(char*, const char*);

	/*!
	\brief 带空指针检查的字符串复制。
	\return 成功时为复制的字符串的结尾指针，失败时为空指针。

	当目标字符串和源字符串都非空时用 stpcpy 复制字符串。
	*/
	char*
	stpcpy_n(char*, const char*);

	/*!
	\brief 带空指针检查的字符串忽略大小写比较。
	\return 成功时为比较结果，否则为 EOF 。

	当两个字符串都非空时 stricmp 比较的字符串结果。
	*/
	int
	stricmp_n(const char*, const char*);

	/*!
	\brief 带空指针检查的字符串复制。
	\return 成功时为复制的字符串的结尾指针，失败时为空指针。

	当字符指针非空时用 strdup 复制字符串。
	*/
	char*
	strdup_n(const char*);

	/*!
	\brief 带空指针检查的字符串连接。
	\return 返回目标参数。

	对传入参数进行非空检查后串接指定的两个字符串，结果复制至指定位置。
	*/
	char*
	strcpycat(char*, const char*, const char*);

	/*!
	\brief 带空指针检查的字符串连接复制。
	\return 目标参数。

	对传入参数进行非空检查后串接指定的两个字符串，
	结果复制至用指定分配函数（默认为 std::malloc）新分配的空间。
	*/
	char*
	strcatdup(const char*, const char*, void*(*)(std::size_t) = std::malloc);

	/*!
	\brief 判断指定路径的文件是否存在。
	\note 使用 C 标准函数库实现。
	*/
	bool
	fexists(CPATH);
}

#endif

