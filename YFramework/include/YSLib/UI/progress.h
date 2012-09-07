/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file progress.h
\ingroup UI
\brief 样式相关的图形用户界面进度部件。
\version r293
\author FrankHB<frankhb1989@gmail.com>
\since build 219
\par 创建时间:
	2011-06-20 08:57:48 +0800
\par 修改时间:
	2012-09-07 10:14 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Progress
*/


#ifndef YSL_INC_UI_PROGRESS_H_
#define YSL_INC_UI_PROGRESS_H_ 1

#include "ycontrol.h"
//#include "../Core/yres.h"
//#include "ystyle.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 进度条。
\since build 219
*/
class ProgressBar : public Control, protected GMRange<float>
{
public:
	//! \since build 337
	explicit
	ProgressBar(const Rect& = {}, ValueType = 0xFF);
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
	\brief 取最大值。
	\since build 295
	*/
	using GMRange<float>::GetMaxValue;
	/*!
	\brief 取当前值。
	\since build 295
	*/
	using GMRange<float>::GetValue;

	/*!
	\brief 刷新：按指定参数绘制界面并更新状态。
	\since build 294
	*/
	void
	Refresh(PaintEventArgs&&) override;
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

