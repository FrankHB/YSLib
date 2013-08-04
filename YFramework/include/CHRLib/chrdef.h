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
\version r659
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-17 17:52:35 +0800
\par 修改时间:
	2013-08-02 19:14 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	CHRLib::CHRDefinition
*/


#ifndef INC_CHRLib_chrdef_h_
#define INC_CHRLib_chrdef_h_ 1

#include <ydef.h>
#include <string> // for std::char_traits;
#include "YCLib/Platform.h" // for YF_API;

namespace CHRLib
{

using ystdex::byte;
using ystdex::size_t;

// 字符类型定义。
using ucs2_t = char16_t; //!< UCS-2 字符类型。
using ucs4_t = char32_t; //!< UCS-4 字符类型。
using ucsint_t = std::char_traits<ucs4_t>::int_type; //!< UCS 整数类型。

} // namespace CHRLib;

#endif

