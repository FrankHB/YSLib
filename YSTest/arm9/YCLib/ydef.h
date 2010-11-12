/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup YCLib YCommonLib
\brief YSLib 基础库。
*/

/*!	\file ydef.h
\ingroup YCLib
\brief 平台相关的公共组件无关函数与宏定义集合。
\version 0.2282;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-02 21:42:44 + 08:00;
\par 修改时间:
	2010-11-11 21:34 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YCLib::YDefinition;
*/


#ifndef INCLUDED_YDEF_H_
#define INCLUDED_YDEF_H_

// YDefinition ：系统环境和公用变量类型的基础定义。

#ifdef __cplusplus
extern "C" {
#endif

#if defined __GNUC__ \
	|| (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)

#	include <stddef.h>
#	include <stdint.h>

typedef intptr_t INT_PTR;
typedef uintptr_t UINT_PTR;

#else

typedef int INT_PTR;
typedef unsigned int UINT_PTR;

#endif

typedef long LONG_PTR;
typedef unsigned long ULONG_PTR;


#define _nullterminated

#undef FALSE
#define FALSE 0

#undef TRUE
#define TRUE 1

#ifndef BASETYPES
#define BASETYPES
typedef unsigned char UCHAR, *PUCHAR;
typedef unsigned short USHORT, *PUSHORT;
typedef unsigned long ULONG, *PULONG;
typedef char* PSZ;
#endif

typedef int             BOOL;
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;


typedef void FVOID(void);
typedef FVOID *PFVOID;


// UCS(Universal Character Set) 字符类型定义。

typedef unsigned long UCSCHAR;

#define UCSCHAR_INVALID_CHARACTER (0xFFFFFFFF)
#define MIN_UCSCHAR (0)
#define MAX_UCSCHAR (0x0010FFFF)


typedef INT_PTR			IRES;
typedef UINT_PTR		URES;
typedef LONG_PTR		LRES;

typedef UINT_PTR        WPARAM;
typedef LONG_PTR        LPARAM;

typedef IRES			ERRNO;

#define LOBYTE(w)       ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define HIBYTE(w)       ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))
#define LOWORD(l)       ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIWORD(l)       ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#define MAKEWORD(a, b)  ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) \
	| ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#define MAKELONG(a, b)  ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) \
	| ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))


typedef char* STR;
typedef const char* CSTR;

//路径类型定义。
typedef STR PATH;
typedef CSTR CPATH;

#ifdef __cplusplus
}
#endif

#endif

