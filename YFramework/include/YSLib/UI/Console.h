/*
	© 2011-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Console.h
\ingroup UI
\brief 基于平台中立接口的控制台实现。
\version r153
\author FrankHB <frankhb1989@gmail.com>
\since build 296
\par 创建时间:
	2011-04-19 23:00:29 +0800
\par 修改时间:
	2013-12-24 09:25 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Console
*/


#ifndef YSL_INC_UI_Console_h_
#define YSL_INC_UI_Console_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YComponent
#include YFM_YSLib_Core_YDevice

namespace YSLib
{

namespace UI
{

//! \brief 控制台。
class YF_API Console
{
public:
	Devices::Screen& Screen;

	/*!
	\brief 构造：使用指定屏幕、有效性、前景色和背景色。
	\since build 296
	*/
	explicit
	Console(Devices::Screen& scr, bool a = true,
		Drawing::Color fc = Drawing::ColorSpace::White,
		Drawing::Color bc = Drawing::ColorSpace::Black)
		: Screen(scr)
	{
		if(a)
			Activate(*this, fc, bc);
	}
	/*!
	\brief 析构。
	\note 自动停用。
	*/
	virtual
	~Console()
	{
		Deactivate(*this);
	}

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

} // namespace UI;

} // namespace YSLib;

#endif

