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
\version 0.2445;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-02 21:42:44 +0800;
\par 修改时间:
	2011-03-06 08:44 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YCLib::YDefinition;
*/


#ifndef INCLUDED_YDEF_H_
#define INCLUDED_YDEF_H_

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

namespace ystdex
{
	typedef int errno_t;
}

// UCS(Universal Character Set) 字符类型定义。

typedef unsigned long UCSCHAR;

#define UCSCHAR_INVALID_CHARACTER (0xFFFFFFFF)
#define MIN_UCSCHAR (0)
#define MAX_UCSCHAR (0x0010FFFF)


//路径类型定义。
typedef char* PATH;
typedef const char* CPATH;

#ifdef __cplusplus
}
#endif

#endif

