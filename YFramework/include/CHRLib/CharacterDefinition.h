/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file CharacterDefinition.h
\ingroup CHRLib
\brief CHRLib 类型声明。
\version r694
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-17 17:52:35 +0800
\par 修改时间:
	2015-10-12 10:08 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	CHRLib::CharacterDefinition
*/


#ifndef YF_INC_CHRLib_CharacterDefinition_h_
#define YF_INC_CHRLib_CharacterDefinition_h_ 1

#include "YModules.h"
#include <ystdex/tstring_view.hpp> // for ystdex::byte, ystdex::octet,
//	ystdex::size_t, std::char_traits, ystdex::string_view,
//	ystdex::u16string_view, ystdex::u32string_view, ystdex::tstring_view,
//	ystdex::u16tstring_view, ystdex::u32tstring_view;
#include YFM_YCLib_Platform // for YF_API;

namespace CHRLib
{

using ystdex::byte;
//! \since build 552
using ystdex::octet;
using ystdex::size_t;

//! \brief UCS 整数类型。
using ucsint_t = std::char_traits<char32_t>::int_type;

//! \since build 645
using ystdex::basic_string_view;
//! \since build 641
//@{
using ystdex::string_view;
using ystdex::u16string_view;
using ystdex::u32string_view;
using ystdex::tstring_view;
using ystdex::u16tstring_view;
using ystdex::u32tstring_view;
//@}

} // namespace CHRLib;

#endif

