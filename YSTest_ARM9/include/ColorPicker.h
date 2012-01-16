/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ColorPicker.h
\ingroup YReader
\brief 拾色器。
\version r1109;
\author FrankHB<frankhb1989@gmail.com>
\since build 275 。
\par 创建时间:
	2012-01-06 20:46:10 +0800;
\par 修改时间:
	2012-01-14 19:27 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YReader::ColorPicker;
*/


#ifndef INCLUDED_COLORPICKER_H_
#define INCLUDED_COLORPICKER_H_

#include <YSLib/UI/uicontx.h>
#include <YSLib/UI/scroll.h>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 拾色框。
\since build 275 。
*/
class ColorBox : public DialogPanel
{
protected:
	/*!
	\brief 颜色区域。
	\since build 277 。
	*/
	Control ctlColorArea;
	/*!
	\brief 颜色分量轨道。
	\since build 277 。
	*/
	HorizontalTrack trRed, trGreen, trBlue;
	/*!
	\brief 颜色分量标签。
	\since build 277 。
	*/
	Label lblRed, lblGreen, lblBlue;

public:
	ColorBox(const Point&, Color c = ColorSpace::White);

	DefGetter(const ynothrow, Color, Color, ctlColorArea.BackColor)
	void
	SetColor(Color);
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

