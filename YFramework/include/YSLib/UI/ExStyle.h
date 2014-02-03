/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ExStyle.h
\ingroup UI
\brief 扩展样式。
\version r57
\author FrankHB <frankhb1989@gmail.com>
\since build 472
\par 创建时间:
	2014-02-03 03:53:51 +0800
\par 修改时间:
	2014-02-03 04:24 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::ExStyle
*/


#ifndef YSL_INC_UI_ExStyle_h_
#define YSL_INC_UI_ExStyle_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YStyle
#include YFM_YSLib_UI_Button

namespace YSLib
{

namespace UI
{

namespace Styles
{

/*!
\brief 初始化扩展样式。
\since build 472
*/
YF_API void
InitExStyles();

} // namespace Styles;

} // namespace UI;

} // namespace YSLib;

#endif

