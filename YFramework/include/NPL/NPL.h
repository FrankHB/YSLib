/*
	© 2014-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPL.h
\ingroup NPL
\brief NPL 公共接口。
\version r73
\author FrankHB <frankhb1989@gmail.com>
\since build 472
\par 创建时间:
	2014-02-02 17:44:40 +0800
\par 修改时间:
	2015-05-11 11:41 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPL
*/


#ifndef NPL_INC_NPL_h_
#define NPL_INC_NPL_h_ 1

#include "YModules.h"
#include "YCLib/YModules.h"
#include YFM_YCLib_Platform // for YF_API;
#include <ydef.h>

namespace NPL
{

//! \since build 472
//@{
/*!
\brief NPL 元标签。
\since build 597
*/
struct YF_API NPLTag
{};


using ystdex::byte;
//@}


/*!
\brief SXML 表示。
\see http://okmij.org/ftp/Scheme/SXML.html 。
\since build 597
*/
namespace SXML
{

//! \brief 限定解析选项。
enum class ParseOption
{
	Normal,
	Strict,
	String,
	Attribute
};

} // namespace SXML;

} // namespace NPL;

#endif

