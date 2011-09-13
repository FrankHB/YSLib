/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file panel.h
\ingroup UI
\brief 样式无关的图形用户界面面板。
\version r1310;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-04-13 20:40:51 +0800;
\par 修改时间:
	2011-09-11 22:05 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Panel;
*/


#ifndef YSL_INC_UI_PANEL_H_
#define YSL_INC_UI_PANEL_H_

#include "ycontrol.h"
#include "yuicont.h"
#include "../Service/yres.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

//! \brief 面板。
class Panel : public Control, protected MUIContainer
{
public:
	/*!
	\brief 构造：使用指定边界。
	*/
	explicit
	Panel(const Rect& = Rect::Empty);
	inline DefDeMoveCtor(Panel)

	virtual void
	operator+=(IWidget&);

	virtual bool
	operator-=(IWidget&);

	using MUIContainer::Contains;

	virtual PDefH2(IWidget*, GetTopWidgetPtr, const Point& pt,
		bool(&f)(const IWidget&))
		ImplBodyBase2(MUIContainer, GetTopWidgetPtr, pt, f)

	virtual PDefH3(Rect, Refresh, const Graphics& g, const Point& pt,
		const Rect& r)
		ImplBodyBase3(Control, Refresh, g, pt, r)
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

