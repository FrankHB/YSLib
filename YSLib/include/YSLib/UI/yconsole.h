/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yconsole.h
\ingroup UI
\brief 平台无关的控制台。
\version 0.1089;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-04-19 23:00:29 +0800;
\par 修改时间:
	2011-06-10 17:30 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Console
*/


#ifndef YSL_INC_SHELL_YCONSOLE_H_
#define YSL_INC_SHELL_YCONSOLE_H_

#include "ycomp.h"
#include "../Helper/yglobal.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

//! \brief 控制台。
class Console
{
public:
	YScreen& Screen;

	/*!
	\brief 构造：使用指定屏幕、有效性、前景色和背景色。
	*/
	explicit
	Console(YScreen& = FetchGlobalInstance().GetDefaultScreen(), bool = true,
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
Console::Console(YScreen& scr, bool a, Drawing::Color fc, Drawing::Color bc)
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

inline void
Console::Pause()
{
	WaitForInput();
}

YSL_END_NAMESPACE(Components)

YSL_END

#endif

