/*
	© 2014-2016, 2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPL.h
\ingroup NPL
\brief NPL 公共接口。
\version r95
\author FrankHB <frankhb1989@gmail.com>
\since build 472
\par 创建时间:
	2014-02-02 17:44:40 +0800
\par 修改时间:
	2022-01-24 02:28 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPL
*/


#ifndef NPL_INC_NPL_h_
#define NPL_INC_NPL_h_ 1

#include "YModules.h"
#include YFM_YSLib_Adaptor_YAdaptor // for YF_API, YSLib::byte;

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


using YSLib::byte;
//@}

} // namespace NPL;

#endif

