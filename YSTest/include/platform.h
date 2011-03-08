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
\version 0.1276;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-24 00:05:08 +0800;
\par 修改时间:
	2011-03-05 17:05 +0800;
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


//字符兼容性补充。

#include <stdint.h>

namespace std
{
	typedef ::uint16_t char16_t;
	typedef ::uint32_t char32_t;
}


#include <nds.h>
#include <fat.h>
#include <unistd.h>
#include <sys/dir.h>

#define YCL_YCOMMON_H <ycommon.h>
#define YSL_YSBUILD_H <YSLib/Helper/yshelper.h>

//包含 PALib 。
//#include <PA9.h>

#ifdef USE_EFS
//包含 EFSLib 。
#include "efs_lib.h"
#endif

//定义使用 Unicode 编码字符串。
#ifndef UNICODE
#	define UNICODE
//宽字符支持。
#	define CHRLIB_WCHAR_T_SIZE 4
#endif

#endif

