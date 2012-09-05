/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file form.h
\ingroup UI
\brief 样式无关的图形用户界面窗体。
\version r569
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-04-30 00:51:36 +0800
\par 修改时间:
	2012-09-04 11:00 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Form
*/


#ifndef YSL_INC_UI_FORM_H_
#define YSL_INC_UI_FORM_H_ 1

#include "ywindow.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

typedef Window Form;

/*//窗体。
class Form : public Window
{
//	UIContainer Client;

public:
	///!
	\brief 构造：使用指定边界、背景图像和容器指针。
	//
	explicit
	Form(const Rect& = Rect::Empty,
		const shared_ptr<Drawing::Image>& = make_shared<Drawing::Image>(),
		IWidget* = nullptr);
	inline DefDeMoveCtor(Form)
};*/

YSL_END_NAMESPACE(Components)

YSL_END

#endif

