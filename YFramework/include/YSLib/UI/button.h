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
\version r3201;
\author FrankHB<frankhb1989@gmail.com>
\since build 194 。
\par 创建时间:
	2010-10-04 21:23:32 +0800;
\par 修改时间:
	2012-05-25 13:44 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::UI::Button;
*/


#ifndef YSL_INC_UI_BUTTON_H_
#define YSL_INC_UI_BUTTON_H_

#include "ycontrol.h"
#include "label.h"
#include "ystyle.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 基本按钮/滑块。
\since build 205 。
*/
class Thumb : public Control
{
protected:
	/*!
	\brief 按下状态：表示按钮当前是否处于按下状态。
	\since build 288 。
	*/
	bool bPressed;

public:
	/*!
	\brief 构造：使用指定边界和色调。
	\since build 302 。
	*/
	explicit
	Thumb(const Rect& = Rect::Empty, Drawing::Hue = 180);

protected:
	/*!
	\brief 无背景构造：使用指定边界和色调。
	\since build 311 。
	*/
	explicit
	Thumb(const Rect&, NoBackgroundTag);

public:
	inline DefDeMoveCtor(Thumb)

	/*!
	\brief 判断按钮当前是否处于按下状态。
	\since build 302 。
	*/
	DefPred(const ynothrow, Pressed, bPressed)
};


/*!
\brief 绘制指定色调的基本按钮背景。
\since build 302 。
*/
void
DrawThumbBackground(PaintEventArgs&& e, Thumb&, Hue);


/*!
\brief 装饰 Thumb 为关闭按钮。
\since build 302 。

在指定 Thumb 上增加 Click 事件响应：关闭父容器；增加 Paint 事件响应：绘制“×”。
*/
void
DecorateAsCloseButton(Thumb&);


/*!
\brief 按钮。
\since build 205 。
*/
class Button : public Thumb, protected MLabel
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
	\brief 刷新：按指定参数绘制界面并更新状态。
	\since build 294 。
	*/
	void
	Refresh(PaintEventArgs&&) override;
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

