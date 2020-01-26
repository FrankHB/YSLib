/*
	© 2010-2014, 2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Form.h
\ingroup UI
\brief 样式无关的 GUI 窗体。
\version r604
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-04-30 00:51:36 +0800
\par 修改时间:
	2020-01-25 16:28 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Form
*/


#ifndef YSL_INC_UI_Form_h_
#define YSL_INC_UI_Form_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YWindow

namespace YSLib
{

namespace UI
{

using Form = Window;

#if false
//窗体。
class Form : public Window
{
	Panel Client;

public:
	/*!
	\brief 构造：使用指定边界、背景图像和容器指针。
	\since build 327
	*/
	explicit
	Form(const Rect& = {}, const shared_ptr<Drawing::Image>& = {},
		IWidget* = {});
};
#endif

} // namespace UI;

} // namespace YSLib;

#endif

