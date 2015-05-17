/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YTextBase.h
\ingroup Adaptor
\brief 外部库关联：文本接口。
\version r87
\author FrankHB <frankhb1989@gmail.com>
\since build 400
\par 创建时间:
	2013-04-24 00:02:44 +0800
\par 修改时间:
	2015-05-13 11:53 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Adaptor::YTextBase
*/


#ifndef YSL_INC_Adaptor_YTextBase_H_
#define YSL_INC_Adaptor_YTextBase_H_ 1

#include "YModules.h"
#include YFM_YSLib_Adaptor_YAdaptor

//包含 CHRLib 。
#include "../../CHRLib/YModules.h"
#include YFM_CHRLib_CharacterProcessing

namespace YSLib
{

//! \since build 597
using platform::basic_string;

using platform::string;
using platform::wstring;
using platform::sfmt;
using platform::vsfmt;

using platform::to_string;
using platform::to_wstring;

/*!
\brief YSLib 基本字符串（使用 UCS-2 ）。
\since build 245
*/
using ucs2string = basic_string<CHRLib::ucs2_t>;
/*!
\brief YSLib 基本字符串（使用 UCS-4 ）。
\since build 253
*/
using ucs4string = basic_string<CHRLib::ucs4_t>;

namespace Text
{

using namespace CHRLib;
//! \since build 512
using platform::IsPrint;

} // namespace Text;

using Text::ucs4_t;
using Text::ucs2_t;
using Text::ucsint_t;

} // namespace YSLib;

#endif

