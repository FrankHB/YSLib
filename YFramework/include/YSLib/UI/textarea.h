/*
	© 2011-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file textarea.h
\ingroup UI
\brief 样式无关的用户界面文本区域部件。
\version r453
\author FrankHB <frankhb1989@gmail.com>
\since build 222
\par 创建时间:
	2011-06-30 20:09:23 +0800
\par 修改时间:
	2013-12-23 22:57 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::TextArea
*/


#ifndef YSL_INC_UI_textarea_h_
#define YSL_INC_UI_textarea_h_ 1

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
	\brief 构造：使用指定边界和默认字型缓存。
	\since build 337
	*/
	explicit
	TextArea(const Drawing::Rect& = {});
	/*!
	\brief 构造：使用指定边界和字体缓存。
	\since build 296
	*/
	TextArea(const Drawing::Rect&, Drawing::FontCache&);
	inline DefDeMoveCtor(TextArea)

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
	\brief 构造：使用指定边界和默认字型缓存。
	\since build 337
	*/
	explicit
	BufferedTextArea(const Drawing::Rect& = {});
	/*!
	\brief 构造：使用指定边界和字体缓存。
	\since build 296
	*/
	BufferedTextArea(const Drawing::Rect&, Drawing::FontCache&);
	inline DefDeMoveCtor(BufferedTextArea)

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

