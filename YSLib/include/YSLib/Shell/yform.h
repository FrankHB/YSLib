/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yform.h
\ingroup Shell
\brief 平台无关的 GUI 窗体实现。
\version 0.1472;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-04-30 00:51:36 +0800;
\par 修改时间:
	2011-03-18 17:26 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YForm;
*/


#ifndef INCLUDED_YFORM_H_
#define INCLUDED_YFORM_H_

#include "../Core/ysdef.h"
#include "../Core/ycounter.hpp"
#include "../Core/yres.h"
#include "ywindow.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Forms)

//窗体。
class YForm : public GMCounter<YForm>, public YWindow
{
public:
	typedef YWindow ParentType;

//	Widgets::YUIContainer Client;

protected:

public:
	/*!
	\brief 构造：使用指定边界、背景图像、窗口句柄和 Shell 句柄。
	*/
	explicit
	YForm(const Rect& = Rect::Empty,
		const GHStrong<Drawing::YImage> = ynew Drawing::YImage(), HWND = NULL);
	/*!
	\brief 析构。
	\note 无异常抛出。
	*/
	virtual
	~YForm() ythrow();

/*	virtual void
	SetSize(SDST, SDST);
	virtual void
	SetBounds(const Rect&);

protected:
	virtual void
	DrawBackground();

	virtual void
	DrawForeground();

public:
	virtual void
	Draw();
*/
};

YSL_END_NAMESPACE(Forms)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

