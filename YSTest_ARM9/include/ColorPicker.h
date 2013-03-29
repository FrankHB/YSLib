/*
	Copyright by FrankHB 2012 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ColorPicker.h
\ingroup YReader
\brief 拾色器。
\version r140
\author FrankHB <frankhb1989@gmail.com>
\since build 275
\par 创建时间:
	2012-01-06 20:46:10 +0800
\par 修改时间:
	2013-03-29 12:59 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::ColorPicker
*/


#ifndef INC_YReader_ColorPicker_h_
#define INC_YReader_ColorPicker_h_ 1

#include <YSLib/UI/uicontx.h>
#include <YSLib/UI/scroll.h>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(UI)

/*!
\brief 拾色框。
\since build 275
*/
class ColorBox : public DialogPanel
{
public:
	//! \since build 394
	using DialogPanel::btnOK;

protected:
	/*!
	\brief 颜色区域。
	\since build 277
	*/
	Control ctlColorArea;
	/*!
	\brief 颜色分量轨道。
	\since build 277
	*/
	HorizontalTrack trRed, trGreen, trBlue;
	/*!
	\brief 颜色分量标签。
	\since build 277
	*/
	Label lblRed, lblGreen, lblBlue;

public:
	ColorBox(const Point&, Color c = ColorSpace::White);

	/*!
	\brief 取当前选定的颜色。
	\since build 294
	*/
	DefGetter(const, Color, Color, GetColorRef())

private:
	/*!
	\brief 取当前选定的颜色引用。
	\since build 294
	*/
	Color&
	GetColorRef() const;

public:
	void
	SetColor(Color);
};

YSL_END_NAMESPACE(UI)

YSL_END

#endif

