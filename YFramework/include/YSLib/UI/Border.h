/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Border.h
\ingroup UI
\brief 图形用户界面边框。
\version r91
\author FrankHB <frankhb1989@gmail.com>
\since build 443
\par 创建时间:
	2013-09-06 23:23:56 +0800
\par 修改时间:
	2013-09-06 23:39 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Border
*/


#ifndef YSL_INC_UI_Border_h_
#define YSL_INC_UI_Border_h_ 1

#include "YBrush.h"

namespace YSLib
{

namespace UI
{

/*!
\brief 边框样式。
\warning 非虚析构。
\since build 276
*/
class YF_API BorderStyle
{
public:
	Color ActiveColor, InactiveColor;

	BorderStyle();
};


/*!
\brief 边框画刷。
\warning 非虚析构。
\since build 284
*/
class YF_API BorderBrush
{
public:
	/*!
	\brief 边框样式。
	\since build 295
	*/
	BorderStyle Style;

	DefDeCtor(BorderBrush)
	/*!
	\brief 复制构造：默认实现。
	\since build 297
	*/
	DefDeCopyCtor(BorderBrush)
	/*!
	\brief 初始化；使用指定边框样式。
	\since build 295
	*/
	BorderBrush(const BorderStyle& style)
		: Style(style)
	{}

	DefDeCopyAssignment(BorderBrush)

	void
	operator()(PaintEventArgs&&);
};

} // namespace UI;

} // namespace YSLib;

#endif

