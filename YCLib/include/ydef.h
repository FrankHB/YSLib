﻿/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ydef.h
\ingroup YCLib
\brief 系统环境和公用类型和宏的基础定义。
\version r2678;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-02 21:42:44 +0800;
\par 修改时间:
	2011-10-09 00:02 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YCLib::YDefinition;
*/


#ifndef YCL_INC_YDEF_H_
#define YCL_INC_YDEF_H_

#ifndef NDEBUG
#	define YCL_USE_YASSERT
#endif

#ifdef __cplusplus
#	define YCL_IMPL_CPP __cplusplus
#	define YCL_IMPL_MSCPP 0
#	define YCL_IMPL_GNUCPP 0
#	ifdef _MSC_VER
#		undef YCL_IMPL_MSCPP
#		define YCL_IMPL_MSCPP _MSC_VER
#	elif defined(__GNUC__)
#		undef YCL_IMPL_GNUCPP
#		define YCL_IMPL_GNUCPP (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 \
			+ __GNUC_PATCHLEVEL__)
#	endif
#else
#	error This header is only for C++!
#endif

#include <cstddef>
#include <climits>
#include <cassert>
#include <cstdint>
#include <cwchar>
#include <type_traits>
#include <sys/types.h>

#if YCL_IMPL_CPP >= 201103L || YCL_IMPL_MSCPP >= 1600 \
	|| YCL_IMPL_GNUCPP >= 40600
//#	include <type_traits>
#	define YCL_HAS_BUILTIN_NULLPTR
#	define YCL_HAS_CONSTEXPR
#else
// TODO: complete version checking for compiler and library implementation;
//#ifdef __GNUC__
//#	include <tr1/type_traits>
#endif

//异常规范宏。

#define YCL_USE_EXCEPTION_SPECIFICATION //!< 使用 YSLib 异常规范。

// ythrow = "yielded throwing";
#ifdef YCL_USE_EXCEPTION_SPECIFICATION
#	define ythrow throw
#else
#	define ythrow(...)
#endif

#define ynothrow ythrow()


#ifdef YCL_HAS_CONSTEXPR
#define yconstexpr constexpr
#define yconstexprf constexpr
#else
#define yconstexpr const
#define yconstexprf inline
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
	/*!
	\brief 字节类型。
	\note ISO C++ 仅允许 char 和 unsigned char 类型的
		不确定值(indeterminate value) 的使用，
		而不引起未定义行为(undefined behavior) 。
	*/
	typedef unsigned char byte;

	typedef int errno_t;
	using std::ptrdiff_t;
	using std::size_t;
	using std::wint_t;
	using ::ssize_t;

#ifdef YCL_HAS_BUILTIN_NULLPTR

	using std::nullptr_t;

#else

	/*!
	\brief 空指针类。
	\note 代码参考：
	http://topic.csdn.net/u/20100924/17/ \
		BE0C26F8-5127-46CD-9136-C9A96AAFDA76.html 。
	*/
	const class nullptr_t
	{
	public:
		/*
		\brief 转换任意类型至空非成员或静态成员指针。
		*/
		template<typename T>
		inline operator T*() const
		{
			return 0;
		}

		/*
		\brief 转换任意类型至空非静态成员指针。
		*/
		template<typename C, typename T>
		inline operator T C::*() const
		{
			return 0;
		}
		/*
		\brief 支持关系运算符重载。
		*/
		template<typename T> bool
		equals(T const& rhs) const
		{
			return rhs == 0;
		}

		/*
		\brief 禁止取 nullptr 的指针。
		*/
		void operator&() const = delete;
	} nullptr = {};

	template<typename T>
	inline bool
	operator==(const nullptr_t& lhs, T const& rhs)
	{
		return lhs.equals(rhs);
	}
	template<typename T>
	inline bool
	operator==(T const& lhs, const nullptr_t& rhs)
	{
		return rhs.equals(lhs);
	}

	template<typename T>
	inline bool
	operator!=(const nullptr_t& lhs, T const& rhs)
	{
		return !lhs.equals(rhs);
	}
	template<typename T>
	inline bool
	operator!=(T const& lhs, const nullptr_t& rhs)
	{
		return !rhs.equals(lhs);
	}

#endif


	//! \brief 根据参数类型使用 std::forward 传递对应参数，保持左值性和常量性。
	#define yforward(_expr) std::forward<typename \
		std::remove_reference<decltype(_expr)>::type>(_expr)

	/*!
	\brief 无序列依赖表达式组求值实现。
	\note 无异常抛出。
	*/
	template<typename... _type>
	inline void
	unsequenced(_type&&...) ynothrow
	{}

	/*!
	\brief 无序列依赖表达式组求值。
	\note 由于实现限制，不适用于 void 类型表达式组。
	\warning 非一元形式不适用于产生副作用的表达式，包括所有可能抛出异常的表达式。
	\note 使用一元形式 %yunsequenced((_expr)) 的形式标记表达式组但不取消序列关系。
	*/
	#define yunsequenced ystdex::unsequenced
}

#endif

