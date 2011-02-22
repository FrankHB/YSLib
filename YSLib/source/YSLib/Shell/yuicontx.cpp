/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yuicontx.cpp
\ingroup Shell
\brief 平台无关的图形用户界面控件实现。
\version 0.1028;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-02-21 09:01:13 + 08:00;
\par 修改时间:
	2011-02-21 09:23 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YUIContainerEx;
*/


#include "yuicontx.h"
#include "ywindow.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

AUIBoxControl::AUIBoxControl(const Rect& r, IUIBox* pCon)
	: VisualControl(r, pCon), MSimpleFocusResponser()
{}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

