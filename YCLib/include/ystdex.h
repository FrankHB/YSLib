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
\version 0.2173;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-27 17:31:14 +0800; 
\par 修改时间:
	2011-04-25 12:27 +0800;
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
#include "ydef.h"
#include <cstddef>
#include <cassert>
#include <cstdlib>
#if YCL_IMPL_CPP >= 201103L || YCL_IMPL_MSCPP >= 1600
#	include <type_traits>
#	define YCL_HAS_BUILTIN_NULLPTR
#else
// TODO: complete version checking for compiler and library implementation;
//#ifdef __GNUC__
#	include <tr1/type_traits>
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
	using std::tr1::add_const;
	using std::tr1::add_cv;
	using std::tr1::add_pointer;
	using std::tr1::add_reference;
	using std::tr1::add_volatile;
	using std::tr1::aligned_storage;
	using std::tr1::alignment_of;
	using std::tr1::extent;
	using std::tr1::false_type;
	using std::tr1::has_nothrow_assign;
	using std::tr1::has_nothrow_constructor;
	using std::tr1::has_nothrow_copy;
	using std::tr1::has_trivial_assign;
	using std::tr1::has_trivial_constructor;
	using std::tr1::has_trivial_copy;
	using std::tr1::has_trivial_destructor;
	using std::tr1::has_virtual_destructor;
	using std::tr1::integral_constant;
	using std::tr1::is_abstract;
	using std::tr1::is_arithmetic;
	using std::tr1::is_array;
	using std::tr1::is_base_of;
	using std::tr1::is_class;
	using std::tr1::is_compound;
	using std::tr1::is_const;
	using std::tr1::is_convertible;
	using std::tr1::is_empty;
	using std::tr1::is_enum;
	using std::tr1::is_floating_point;
	using std::tr1::is_function;
	using std::tr1::is_fundamental;
	using std::tr1::is_integral;
	using std::tr1::is_member_function_pointer;
	using std::tr1::is_member_object_pointer;
	using std::tr1::is_member_pointer;
	using std::tr1::is_pod;
	using std::tr1::is_pointer;
	using std::tr1::is_polymorphic;
	using std::tr1::is_reference;
	using std::tr1::is_same;
	using std::tr1::is_scalar;
	using std::tr1::is_signed;
	using std::tr1::is_union;
	using std::tr1::is_unsigned;
	using std::tr1::is_void;
	using std::tr1::is_volatile;
	using std::tr1::rank;
	using std::tr1::remove_all_extents;
	using std::tr1::remove_const;
	using std::tr1::remove_cv;
	using std::tr1::remove_extent;
	using std::tr1::remove_pointer;
	using std::tr1::remove_reference;
	using std::tr1::remove_volatile;
	using std::tr1::true_type;

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

