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
\version r1229;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-06-20 08:57:48 +0800;
\par 修改时间:
	2011-09-01 02:07 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Progress;
*/


#ifndef YSL_INC_UI_PROGRESS_H_
#define YSL_INC_UI_PROGRESS_H_

#include "ywidget.h"
//#include "../Core/yres.h"
//#include "ystyle.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

//! \brief 进度条。
class ProgressBar : public Widget, public GMRange<u16>
{
public:
	Color BorderColor; //!< 边框颜色。

	explicit
	ProgressBar(const Rect& = Rect::Empty, u16 = 0xFF);

	DefSetter(ValueType, MaxValue, max_value)
	DefSetter(ValueType, Value, value)

	/*!
	\brief 刷新：在指定图形接口上下文以指定偏移起始按指定边界绘制界面。
	*/
	virtual Rect
	Refresh(const Graphics&, const Point&, const Rect&);
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

