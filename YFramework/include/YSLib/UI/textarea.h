/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file textarea.h
\ingroup UI
\brief 样式无关的用户界面文本区域部件。
\version r1418;
\author FrankHB<frankhb1989@gmail.com>
\since build 222 。
\par 创建时间:
	2011-06-30 20:09:23 +0800;
\par 修改时间:
	2012-05-25 13:51 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::UI::TextArea;
*/


#ifndef YSL_INC_UI_TEXTAREA_H_
#define YSL_INC_UI_TEXTAREA_H_

#include "ywidget.h"
//#include "../Core/yres.h"
#include "../Service/TextRenderer.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 文本区域。
*/
class TextArea : public Widget,
	public Drawing::TextState, public Drawing::TextRenderer
{
public:
	Drawing::Rotation Rotation; //!< 屏幕指向。

	/*!
	\brief 构造：使用指定边界和默认字型缓存。
	\since build 296 。
	*/
	explicit
	TextArea(const Drawing::Rect& = Drawing::Rect::Empty);
	/*!
	\brief 构造：使用指定边界和字体缓存。
	\since build 296 。
	*/
	TextArea(const Drawing::Rect&, Drawing::FontCache&);
	inline DefDeMoveCtor(TextArea)

	using Widget::GetWidth;
	using Widget::GetHeight;
};


/*!
\brief 缓冲文本区域。
*/
class BufferedTextArea : public Widget, public Drawing::TextRegion
{
public:
	Drawing::Rotation Rotation; //!< 屏幕指向。

	/*!
	\brief 构造：使用指定边界和默认字型缓存。
	\since build 296 。
	*/
	explicit
	BufferedTextArea(const Drawing::Rect& = Drawing::Rect::Empty);
	/*!
	\brief 构造：使用指定边界和字体缓存。
	\since build 296 。
	*/
	BufferedTextArea(const Drawing::Rect&, Drawing::FontCache&);
	inline DefDeMoveCtor(BufferedTextArea)

	using Widget::GetWidth;
	using Widget::GetHeight;

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

