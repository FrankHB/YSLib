/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ystyle.cpp
\ingroup Shell
\brief 图形用户界面样式实现。
\version 0.1045;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-05-01 13:52:56 +0800;
\par 修改时间:
	2011-03-07 22:28 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YStyle;
*/


#include "ystyle.h"
#include "ygui.h"
#include "ywindow.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

void
WndDrawFocus(IWindow* pWnd, const Size&)
{
	YAssert(pWnd, "Window pointer is null.");

	DrawWindowBounds(pWnd, ColorSpace::Fuchsia);

	IWidget* pWgt(pWnd->GetFocusingPtr());

	if(pWgt)
		DrawWidgetBounds(*pWgt, ColorSpace::Aqua);
}

YSL_END_NAMESPACE(Drawing)
	
YSL_BEGIN_NAMESPACE(Components)


YSL_END_NAMESPACE(Components)

YSL_END

