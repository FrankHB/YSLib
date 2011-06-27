/*
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
\version 0.2489;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-02 21:42:44 +0800;
\par 修改时间:
	2011-06-24 21:10 +0800;
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
extern "C" {
#endif

#if defined __GNUC__ \
	|| (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)

#	include <stddef.h>
#	include <stdint.h>

#else

typedef int intptr_t;
typedef unsigned int uintptr_t;

#endif

// UCS(Universal Character Set) 字符类型定义。

typedef unsigned long UCSCHAR;

#define UCSCHAR_INVALID_CHARACTER (0xFFFFFFFF)
#define MIN_UCSCHAR (0)
#define MAX_UCSCHAR (0x0010FFFF)

//补充类型。
#include <cstddef>
#include <cstdint>
#include <sys/types.h>

namespace ystdex
{
	typedef std::uint8_t	u8;
	typedef std::uint16_t	u16;
	typedef std::uint32_t	u32;
	typedef std::uint64_t	u64;
	typedef std::int8_t		s8;
	typedef std::int16_t	s16;
	typedef std::int32_t	s32;
	typedef std::int64_t	s64;
	typedef volatile u8		vu8;
	typedef volatile u16	vu16;
	typedef volatile u32	vu32;
	typedef volatile u64	vu64;
	typedef volatile s8		vs8;
	typedef volatile s16	vs16;
	typedef volatile s32	vs32;
	typedef volatile s64	vs64;

	typedef u8 byte;

	typedef u16 uchar_t;
	typedef u32 fchar_t;

	typedef int errno_t;
	using std::ptrdiff_t;
	using std::size_t;
	using ::ssize_t;

	//路径类型定义。
	typedef char* path_t;
	typedef const char* const_path_t;
}

#ifdef __cplusplus
}
#endif

#endif

