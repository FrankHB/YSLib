/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Border.h
\ingroup UI
\brief 样式无关的图形用户界面附加容器。
\version r1046;
\author FrankHB<frankhb1989@gmail.com>
\since build 275 。
\par 创建时间:
	2012-01-10 19:55:30 +0800;
\par 修改时间:
	2012-01-13 00:24 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::UI::Border;
*/


#ifndef YSL_INC_UI_BORDER_H_
#define YSL_INC_UI_BORDER_H_

#include "ywgtevt.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 边框样式。
\warning 非虚析构。
\since build 276 。
*/
class BorderStyle
{
public:
	Color ActiveColor, InactiveColor;

	BorderStyle();

	void
	OnPaint(PaintEventArgs&&);
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

