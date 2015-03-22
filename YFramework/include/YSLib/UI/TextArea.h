/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TextArea.h
\ingroup UI
\brief 样式无关的用户界面文本区域部件。
\version r490
\author FrankHB <frankhb1989@gmail.com>
\since build 586
\par 创建时间:
	2011-06-30 20:09:23 +0800
\par 修改时间:
	2015-03-21 23:36 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::TextArea
*/


#ifndef YSL_INC_UI_TextArea_h_
#define YSL_INC_UI_TextArea_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YWidget
//#include YFM_YSLib_Service_YResource
#include YFM_YSLib_Service_TextRenderer

namespace YSLib
{

namespace UI
{

/*!
\brief 文本区域。
*/
class YF_API TextArea : public Widget,
	public Drawing::TextState, public Drawing::TextRenderer
{
public:
	Drawing::Rotation Rotation; //!< 屏幕指向。

	/*!
	\brief 构造：使用指定边界、背景画刷、文本颜色和默认字型缓存。
	\since build 484
	*/
	explicit
	TextArea(const Drawing::Rect& = {}, HBrush = MakeBlankBrush(),
		Drawing::Color = Drawing::ColorSpace::Black);
	/*!
	\brief 构造：使用指定边界、字型缓存、背景画刷和文本颜色。
	\since build 484
	*/
	TextArea(const Drawing::Rect&, Drawing::FontCache&,
		HBrush = MakeBlankBrush(), Drawing::Color = Drawing::ColorSpace::Black);
	DefDeMoveCtor(TextArea)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~TextArea() override;

	using Widget::GetWidth;
	using Widget::GetHeight;
};


/*!
\brief 缓冲文本区域。
*/
class YF_API BufferedTextArea : public Widget, public Drawing::TextRegion
{
public:
	Drawing::Rotation Rotation; //!< 屏幕指向。

	/*!
	\brief 构造：使用指定边界、背景画刷、文本颜色和默认字型缓存。
	\since build 484
	*/
	explicit
	BufferedTextArea(const Drawing::Rect& = {}, HBrush = MakeBlankBrush(),
		Drawing::Color = Drawing::ColorSpace::Black);
	/*!
	\brief 构造：使用指定边界、字型缓存、背景画刷和文本颜色。
	\since build 484
	*/
	BufferedTextArea(const Drawing::Rect&, Drawing::FontCache&,
		HBrush = MakeBlankBrush(), Drawing::Color = Drawing::ColorSpace::Black);
	DefDeMoveCtor(BufferedTextArea)

	using Widget::GetWidth;
	using Widget::GetHeight;

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

