/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Selector.h
\ingroup UI
\brief 样式相关的图形用户界面选择控件。
\version r1198;
\author FrankHB<frankhb1989@gmail.com>
\since build 282 。
\par 创建时间:
	2011-03-22 07:17:17 +0800;
\par 修改时间:
	2012-02-01 13:12 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Selector;
*/


#ifndef YSL_INC_UI_SELECTOR_H_
#define YSL_INC_UI_SELECTOR_H_

#include "button.h"
#include "ystyle.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 复选框。
\since build 205 。
*/
class CheckBox : public Thumb
{
protected:
	bool bTicked; //选中状态。

public:
	/*!
	\brief 构造：使用指定边界。
	*/
	explicit
	CheckBox(const Rect& = Rect::Empty);
	inline DefDeMoveCtor(CheckBox)

	DefPred(const ynothrow, Ticked, bTicked)

	/*!
	\brief 刷新：在指定图形接口上下文以指定偏移起始按指定边界绘制界面。
	*/
	virtual Rect
	Refresh(const PaintContext&);
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

