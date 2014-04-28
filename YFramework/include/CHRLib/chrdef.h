/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file chrdef.h
\ingroup CHRLib
\brief CHRLib 类型定义。
\version r672
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-17 17:52:35 +0800
\par 修改时间:
	2014-04-25 08:56 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	CHRLib::CHRDefinition
*/


#ifndef YF_INC_CHRLib_chrdef_h_
#define YF_INC_CHRLib_chrdef_h_ 1

#include "YModules.h"
#include <ydef.h>
#include <string> // for std::char_traits;
#include YFM_YCLib_Platform // for YF_API;

namespace CHRLib
{

using ystdex::byte;
using ystdex::size_t;

//! \brief 字符类型声明。
//@{
using ucs2_t = char16_t; //!< UCS-2 字符类型。
using ucs4_t = char32_t; //!< UCS-4 字符类型。
using ucsint_t = std::char_traits<ucs4_t>::int_type; //!< UCS 整数类型。
//@}

} // namespace CHRLib;

#endif

