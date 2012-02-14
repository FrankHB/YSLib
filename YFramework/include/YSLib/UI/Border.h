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
\version r1089;
\author FrankHB<frankhb1989@gmail.com>
\since build 275 。
\par 创建时间:
	2012-01-10 19:55:30 +0800;
\par 修改时间:
	2012-02-10 18:15 +0800;
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
};


/*!
\brief 边框画刷。
\warning 非虚析构。
\since build 284 。
*/
class BorderBrush
{
public:
	weak_ptr<BorderStyle> StylePtr;

	yconstexpr DefDeCtor(BorderBrush)
	BorderBrush(const shared_ptr<BorderStyle>&);
	BorderBrush(shared_ptr<BorderStyle>&&);
	BorderBrush(const weak_ptr<BorderStyle>&);
	BorderBrush(weak_ptr<BorderStyle>&&);
	inline DefDeCopyCtor(BorderBrush)
	BorderBrush(BorderBrush&&);

	inline DefDeCopyAssignment(BorderBrush)
	inline DefDeMoveAssignment(BorderBrush)

	void
	operator()(PaintEventArgs&&);
};

inline
BorderBrush::BorderBrush(const shared_ptr<BorderStyle>& p)
	: StylePtr(p)
{}
inline
BorderBrush::BorderBrush(shared_ptr<BorderStyle>&& p)
	: StylePtr(std::move(p))
{}
inline
BorderBrush::BorderBrush(const weak_ptr<BorderStyle>& p)
	: StylePtr(p)
{}
inline
BorderBrush::BorderBrush(weak_ptr<BorderStyle>&& p)
	: StylePtr(std::move(p))
{}
inline
BorderBrush::BorderBrush(BorderBrush&& bdb)
	: StylePtr(bdb.StylePtr.lock())
{}

YSL_END_NAMESPACE(Components)

YSL_END

#endif

