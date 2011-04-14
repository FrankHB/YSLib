/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file checkbox.h
\ingroup Shell
\brief 样式相关的图形用户界面复选框控件实现。
\version 0.1147;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-03-22 07:17:17 +0800;
\par 修改时间:
	2011-04-13 08:06 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::CheckBox;
*/


#ifndef INCLUDED_CHECKBOX_H_
#define INCLUDED_CHECKBOX_H_

#include "ycontrol.h"
#include "button.h"
#include "ystyle.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

//! \brief 复选框。
class YCheckBox: public GMCounter<YCheckBox>, public YThumb
{
public:
	typedef YThumb ParentType;

protected:
	bool bTicked; //选中状态。

public:
	/*!
	\brief 构造：使用指定边界。
	*/
	explicit
	YCheckBox(const Rect& = Rect::Empty);

	/*!
	\brief 绘制。
	*/
	virtual void
	Draw();

private:
	/*!
	\brief 处理屏幕点击事件。
	*/
	void
	OnClick(TouchEventArgs&);
};

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

