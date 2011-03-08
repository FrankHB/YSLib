/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ybutton.h
\ingroup Shell
\brief 样式相关的图形用户界面按钮控件实现。
\version 0.2970;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-10-04 21:23:32 +0800;
\par 修改时间:
	2011-03-07 21:45 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::Button;
*/


#ifndef INCLUDED_BUTTON_H_
#define INCLUDED_BUTTON_H_

#include "../Core/ysdef.h"
#include "../Core/ycounter.hpp"
#include "ycontrol.h"
#include "ylabel.h"
//#include "ystyle.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

//! \brief 基本按钮/滑块。
class YThumb : public GMCounter<YThumb>, public YControl,
	protected MButton
{
public:
	typedef YControl ParentType;

	/*!
	\brief 构造：使用指定边界和部件容器指针。
	*/
	explicit
	YThumb(const Rect& = Rect::Empty, IUIBox* = NULL);
	virtual DefEmptyDtor(YThumb)

	/*!
	\brief 绘制前景。
	*/
	virtual void
	DrawForeground();

private:
	/*!
	\brief 处理控件进入事件。
	*/
	void
	OnEnter(InputEventArgs&);

	/*!
	\brief 处理控件离开事件。
	*/
	void
	OnLeave(InputEventArgs&);
};


//! \brief 按钮。
class YButton : public GMCounter<YButton>, public YThumb,
	public Widgets::MLabel
{
public:
	typedef YThumb ParentType;

/*
	YImage BackgroundImage; //!< 背景图像。
	YImage Image; //!< 前景图像。
*/

	/*!
	\brief 构造：使用指定边界、部件容器指针和字体。
	*/
	explicit
	YButton(const Rect& = Rect::Empty, IUIBox* = NULL,
		const Drawing::Font& = Drawing::Font::GetDefault());
	virtual DefEmptyDtor(YButton)

public:
	/*!
	\brief 绘制前景。
	*/
	virtual void
	DrawForeground();
};

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

