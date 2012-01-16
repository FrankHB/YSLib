/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file button.h
\ingroup UI
\brief 样式相关的图形用户界面按钮控件。
\version r3083;
\author FrankHB<frankhb1989@gmail.com>
\since build 194 。
\par 创建时间:
	2010-10-04 21:23:32 +0800;
\par 修改时间:
	2012-01-15 04:21 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::UI::Button;
*/


#ifndef YSL_INC_UI_BUTTON_H_
#define YSL_INC_UI_BUTTON_H_

#include "ycontrol.h"
#include "label.h"
//#include "ystyle.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 按钮模块。
\since build 160 。
*/
class MButton
{
protected:
	bool bPressed; //!< 键按下状态：是否处于按下状态。

	/*!
	\brief 构造：使用键按下状态。
	*/
	explicit
	MButton(bool = false);
	inline DefDeMoveCtor(MButton)

public:
	DefPred(const ynothrow, Pressed, bPressed)
};

inline
MButton::MButton(bool b)
	: bPressed(b)
{}


/*!
\brief 基本按钮/滑块。
\since build 205 。
*/
class Thumb : public Control, protected MButton
{
public:
	/*!
	\brief 构造：使用指定边界。
	*/
	explicit
	Thumb(const Rect& = Rect::Empty);
	inline DefDeMoveCtor(Thumb)

	/*!
	\brief 刷新：在指定图形接口上下文以指定偏移起始按指定边界绘制界面。
	*/
	virtual Rect
	Refresh(const PaintContext&);
};


/*!
\brief 按钮。
\since build 205 。
*/
class Button : public Thumb, protected Components::MLabel
{
public:
	using MLabel::Font;
	using MLabel::Margin;
	using MLabel::HorizontalAlignment;
	using MLabel::VerticalAlignment;
	using MLabel::Text;
/*
	YImage BackgroundImage; //!< 背景图像。
	YImage Image; //!< 前景图像。
*/

	/*!
	\brief 构造：使用指定边界和字体。
	*/
	explicit
	Button(const Rect& = Rect::Empty,
		const Drawing::Font& = FetchPrototype<Drawing::Font>());
	inline DefDeMoveCtor(Button)

	/*!
	\brief 刷新：在指定图形接口上下文以指定偏移起始按指定边界绘制界面。
	*/
	virtual Rect
	Refresh(const PaintContext&);
};


/*!
\brief 关闭按钮。
\since build 276 。
*/
class CloseButton : public Thumb
{
public:
	/*!
	\brief 构造：使用指定边界和字体。
	*/
	explicit
	CloseButton(const Rect& = Rect::Empty);
	inline DefDeMoveCtor(CloseButton)

	/*!
	\brief 刷新：在指定图形接口上下文以指定偏移起始按指定边界绘制界面。
	*/
	virtual Rect
	Refresh(const PaintContext&);
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

