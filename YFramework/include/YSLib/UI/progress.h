/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file progress.h
\ingroup UI
\brief 样式相关的图形用户界面进度部件。
\version r1246;
\author FrankHB<frankhb1989@gmail.com>
\since build 219 。
\par 创建时间:
	2011-06-20 08:57:48 +0800;
\par 修改时间:
	2011-12-20 18:59 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Progress;
*/


#ifndef YSL_INC_UI_PROGRESS_H_
#define YSL_INC_UI_PROGRESS_H_

#include "ycontrol.h"
//#include "../Core/yres.h"
//#include "ystyle.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 进度条。
\since build 219 。
*/
class ProgressBar : public Control, public GMRange<float>
{
public:
	Color BorderColor; //!< 边框颜色。

	explicit
	ProgressBar(const Rect& = Rect::Empty, ValueType = 0xFF);
	inline DefDeMoveCtor(ProgressBar)

	/*!
	\brief 设置进度关联值最大取值。
	\note 当指定值非正值时无效。
	\note 约束关联值不大于指定值。
	*/
	void
	SetMaxValue(ValueType);
	DefSetter(ValueType, Value, value)

	/*!
	\brief 刷新：在指定图形接口上下文以指定偏移起始按指定边界绘制界面。
	*/
	virtual Rect
	Refresh(const PaintContext&);
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

