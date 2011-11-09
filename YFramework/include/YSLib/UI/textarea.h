/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file textarea.h
\ingroup UI
\brief 样式无关的用户界面文本区域部件。
\version r1374;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-06-30 20:09:23 +0800;
\par 修改时间:
	2011-11-05 10:48 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::TextArea;
*/


#ifndef YSL_INC_UI_TEXTAREA_H_
#define YSL_INC_UI_TEXTAREA_H_

#include "ywidget.h"
//#include "../Adaptor/yfont.h"
//#include "../Core/yres.h"
#include "../Service/ytext.h"
#include "../Helper/yglobal.h"

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

	explicit
	TextArea(const Drawing::Rect& = Drawing::Rect::Empty,
		FontCache& = FetchGlobalInstance().GetFontCache());
	inline DefDeMoveCtor(TextArea)

	using Widget::GetSize;
	using Widget::GetWidth;
	using Widget::GetHeight;

	using Widget::SetSize;
};


/*!
\brief 缓冲文本区域。
*/
class BufferedTextArea : public Widget, public Drawing::TextRegion
{
public:
	Drawing::Rotation Rotation; //!< 屏幕指向。

	explicit
	BufferedTextArea(const Drawing::Rect& = Drawing::Rect::Empty,
		FontCache& = FetchGlobalInstance().GetFontCache());
	inline DefDeMoveCtor(BufferedTextArea)

	using Widget::GetSize;
	using Widget::GetWidth;
	using Widget::GetHeight;

	using Widget::SetSize;

	virtual Rect
	Refresh(const PaintContext&);
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

