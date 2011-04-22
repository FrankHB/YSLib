/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yconsole.h
\ingroup Shell
\brief 平台无关的控制台。
\version 0.1079;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-04-19 23:00:29 +0800;
\par 修改时间:
	2011-04-21 07:18 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YConsole
*/


#ifndef YSL_INC_SHELL_YCONSOLE_H_
#define YSL_INC_SHELL_YCONSOLE_H_

#include "ycomp.h"
#include "../Helper/yglobal.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

//! \brief 控制台。
class YConsole : public YComponent
{
public:
	typedef YComponent ParentType;

	YScreen& Screen;

	/*!
	\brief 构造：使用指定屏幕、有效性、前景色和背景色。
	*/
	explicit
	YConsole(YScreen& = GetGlobal().GetDefaultScreen(), bool = true,
		Drawing::Color = Drawing::ColorSpace::White,
		Drawing::Color = Drawing::ColorSpace::Black);
	/*!
	\brief 析构。
	\note 自动停用。
	*/
	virtual
	~YConsole();

	/*!
	\brief 激活：使用指定屏幕、有效性、前景色和背景色。
	*/
	friend void
	Activate(YConsole&, Drawing::Color = Drawing::ColorSpace::White,
		Drawing::Color = Drawing::ColorSpace::Black);

	/*!
	\brief 停用。
	*/
	friend void
	Deactivate(YConsole&);

	void
	Pause();
};

inline
YConsole::YConsole(YScreen& scr, bool a, Drawing::Color fc, Drawing::Color bc)
	: YComponent(),
	Screen(scr)
{
	if(a)
		Activate(*this, fc, bc);
}
inline
YConsole::~YConsole()
{
	Deactivate(*this);
}

inline void
YConsole::Pause()
{
	WaitForInput();
}

YSL_END_NAMESPACE(Components)

YSL_END

#endif

