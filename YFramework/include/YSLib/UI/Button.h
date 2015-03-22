﻿/*
	© 2010-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Button.h
\ingroup UI
\brief 样式相关的 GUI 按钮控件。
\version r2322
\author FrankHB <frankhb1989@gmail.com>
\since build 586
\par 创建时间:
	2010-10-04 21:23:32 +0800
\par 修改时间:
	2015-03-21 18:53 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Button
*/


#ifndef YSL_INC_UI_Button_h_
#define YSL_INC_UI_Button_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YControl
#include YFM_YSLib_UI_Label
#include YFM_YSLib_UI_YStyle

namespace YSLib
{

namespace UI
{

/*!
\brief 基本按钮。
\since build 205
*/
class YF_API Thumb : public Control
{
public:
	/*!
	\brief 视觉样式项目。
	\since build 468
	*/
	enum StyleItem : Styles::StyleItem
	{
		ThumbBackground,
		EndStyle
	};

	/*!
	\brief 默认前景色。
	\since build 525
	*/
	Color ForeColor{Drawing::ColorSpace::Black};

protected:
	/*!
	\brief 指针设备光标状态。
	\since build 463
	*/
	CursorState csCurrent;

private:
	//! \since build 468
	Drawing::Hue hue = 180;

public:
	/*!
	\brief 构造：使用指定边界和色调。
	\since build 337
	*/
	explicit
	Thumb(const Rect& = {}, Drawing::Hue = 180);

protected:
	/*!
	\brief 无背景构造：使用指定边界。
	\since build 485
	*/
	explicit
	Thumb(const Rect&, ystdex::raw_tag);

public:
	DefDeMoveCtor(Thumb)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~Thumb() override;

	/*!
	\brief 判断按钮当前是否处于按下状态。
	\since build 302
	*/
	DefPred(const ynothrow, Pressed, csCurrent == CursorState::Pressed)

	//! \since build 463
	DefGetter(const ynothrow, CursorState, CursorState, csCurrent)
	//! \since build 468
	DefGetter(const ynothrow, Hue, Hue, hue)
};

/*!
\brief 装饰 Thumb 为关闭按钮。
\relates Thumb
\since build 302

在指定 Thumb 上增加 Click 事件响应：关闭父容器；增加 Paint 事件响应：绘制“×”。
*/
YF_API void
DecorateAsCloseButton(Thumb&);


/*!
\brief 按钮。
\since build 205
*/
class YF_API Button : public Thumb, protected MLabel
{
public:
	//! \since build 525
	using MLabel::ForeColor;
	using MLabel::Font;
	using MLabel::Margin;
	using MLabel::HorizontalAlignment;
	using MLabel::VerticalAlignment;
	using MLabel::Text;

	/*!
	\brief 构造：使用指定边界、字体、文本颜色和文本对齐样式。
	\since build 525
	*/
	explicit
	Button(const Rect& = {}, const Drawing::Font& = {}, Color
		= Drawing::ColorSpace::Black, TextAlignment = TextAlignment::Center);
	/*!
	\brief 构造：使用指定边界、色调、字体、文本颜色和文本对齐样式。
	\since build 525
	*/
	Button(const Rect&, Drawing::Hue, const Drawing::Font& = {}, Color
		= Drawing::ColorSpace::Black, TextAlignment = TextAlignment::Center);
	DefDeMoveCtor(Button)

	/*!
	\brief 刷新：按指定参数绘制界面并更新状态。
	\since build 294
	*/
	void
	Refresh(PaintEventArgs&&) override;
};

} // namespace UI;

} // namespace YSLib;

#endif

