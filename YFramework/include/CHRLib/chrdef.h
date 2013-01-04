/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup CHRLib CHRLib
\ingroup YFramework
\brief 字符编码处理库。
*/

/*!	\file chrdef.h
\ingroup CHRLib
\brief CHRLib 类型定义。
\version r643
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-17 17:52:35 +0800
\par 修改时间:
	2013-01-04 16:55 +0800
\par 字符集:
	UTF-8
\par 模块名称:
	CHRLib::CHRDefinition
*/


#ifndef CHRLIB_INC_CHRDEF_H_
#define CHRLIB_INC_CHRDEF_H_ 1

#include <ydef.h>
#include <string> // for std::char_traits;
#include <YCLib/Platform.h> // for YF_API;

#define CHRLIB_BEGIN	namespace CHRLib {
#define CHRLIB_END		}
#define CHRLIB_			::CHRLib::
#define CHRLIB			::CHRLib

#define CHRLIB_BEGIN_NAMESPACE(s)	namespace s {
#define CHRLIB_END_NAMESPACE(s)	}

CHRLIB_BEGIN

using ystdex::byte;
using ystdex::size_t;

// 字符类型定义。
typedef char16_t ucs2_t; //!< UCS-2 字符类型。
typedef char32_t ucs4_t; //!< UCS-4 字符类型。
typedef std::char_traits<ucs4_t>::int_type ucsint_t; //!< UCS 整数类型。

CHRLIB_END

#endif

