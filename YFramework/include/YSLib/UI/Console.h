/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Console.h
\ingroup UI
\brief 基于平台中立接口的控制台实现。
\version r1115;
\author FrankHB<frankhb1989@gmail.com>
\since build 296 。
\par 创建时间:
	2011-04-19 23:00:29 +0800;
\par 修改时间:
	2012-04-07 14:18 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::UI::Console
*/


#ifndef YSL_INC_UI_YCONSOLE_H_
#define YSL_INC_UI_YCONSOLE_H_

#include "ycomp.h"
#include "../Core/ydevice.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

//! \brief 控制台。
class Console
{
public:
	Devices::Screen& Screen;

	/*!
	\brief 构造：使用指定屏幕、有效性、前景色和背景色。
	\since build 296 。
	*/
	explicit
	Console(Devices::Screen&, bool = true,
		Drawing::Color = Drawing::ColorSpace::White,
		Drawing::Color = Drawing::ColorSpace::Black);
	/*!
	\brief 析构。
	\note 自动停用。
	*/
	virtual
	~Console();

	/*!
	\brief 激活：使用指定屏幕、有效性、前景色和背景色。
	*/
	friend void
	Activate(Console&, Drawing::Color = Drawing::ColorSpace::White,
		Drawing::Color = Drawing::ColorSpace::Black);

	/*!
	\brief 停用。
	*/
	friend void
	Deactivate(Console&);

	void
	Pause();
};

inline
Console::Console(Devices::Screen& scr, bool a, Drawing::Color fc, Drawing::Color bc)
	: Screen(scr)
{
	if(a)
		Activate(*this, fc, bc);
}
inline
Console::~Console()
{
	Deactivate(*this);
}

YSL_END_NAMESPACE(Components)

YSL_END

#endif

