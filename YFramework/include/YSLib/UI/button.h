/*
	© 2010-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file button.h
\ingroup UI
\brief 样式相关的 GUI 按钮控件。
\version r2269
\author FrankHB <frankhb1989@gmail.com>
\since build 194
\par 创建时间:
	2010-10-04 21:23:32 +0800
\par 修改时间:
	2014-01-11 11:28 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Button
*/


#ifndef YSL_INC_UI_button_h_
#define YSL_INC_UI_button_h_ 1

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
protected:
	/*!
	\brief 指针设备光标状态。
	\since build 463
	*/
	CursorState csCurrent;

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
	\since build 311
	*/
	explicit
	Thumb(const Rect&, NoBackgroundTag);

public:
	DefDeMoveCtor(Thumb)

	/*!
	\brief 判断按钮当前是否处于按下状态。
	\since build 302
	*/
	DefPred(const ynothrow, Pressed, csCurrent == CursorState::Pressed)

	//! \since build 463
	DefGetter(const ynothrow, CursorState, CursorState, csCurrent)
};

/*!
\brief 绘制指定色调的基本按钮背景。
\relates Thumb
\since build 302
*/
YF_API void
DrawThumbBackground(PaintEventArgs&& e, Thumb&, Hue);

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
	\brief 构造：使用指定边界、字体和文本对齐样式。
	\since build 392
	*/
	explicit
	Button(const Rect& = {}, const Drawing::Font& = {},
		TextAlignment = TextAlignment::Center);
	/*!
	\brief 构造：使用指定边界、色调、字体和文本对齐样式。
	\since build 392
	*/
	Button(const Rect&, Drawing::Hue, const Drawing::Font& = {},
		TextAlignment = TextAlignment::Center);
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

