/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup Platform Platform
\brief YSLib 库平台。
*/
/*!	\defgroup DS DS
\ingroup Platform
\brief DS 平台。
*/

/*!	\file platform.h
\ingroup DS
\brief 通用平台描述文件。
\version 0.1307;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-24 00:05:08 +0800;
\par 修改时间:
	2011-05-03 12:45 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	Platform::DS;
*/


#ifndef INCLUDED_PLATFORM_H_
#define INCLUDED_PLATFORM_H_

//GNUC __attribute__ 支持。
#ifndef _ATTRIBUTE
#	ifdef __GNUC__
#		define _ATTRIBUTE(attrs) __attribute__ (attrs)
#	else
#		define _ATTRIBUTE(attrs)
#	endif
#endif


//整数类型。
#include <nds/ndstypes.h>

//字符兼容性补充。

#include <stdint.h>

namespace ystdex
{
	typedef ::uint16_t uchar_t;
	typedef ::uint32_t fchar_t;
}

//定义使用 Unicode 编码字符串。
#ifndef UNICODE
#	define UNICODE
//宽字符支持。
#	define CHRLIB_WCHAR_T_SIZE 4
#endif

#endif

