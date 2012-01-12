/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Border.cpp
\ingroup UI
\brief 样式无关的图形用户界面附加容器。
\version r1064;
\author FrankHB<frankhb1989@gmail.com>
\since build 276 。
\par 创建时间:
	2012-01-10 19:56:59 +0800;
\par 修改时间:
	2012-01-13 00:25 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::UI::Border;
*/


#include "YSLib/UI/Border.h"
#include "YSLib/Service/ydraw.h"
#include "YSLib/UI/ygui.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

BorderStyle::BorderStyle()
	: ActiveColor(FetchGUIShell().Colors[Styles::ActiveBorder]),
	InactiveColor(FetchGUIShell().Colors[Styles::InactiveBorder])
{}

void
BorderStyle::OnPaint(PaintEventArgs&& e)
{
	auto& wgt(e.GetSender());

	DrawRect(e.Target, e.Location, GetSizeOf(wgt),
		IsFocused(wgt) ? ActiveColor : InactiveColor);
}

YSL_END_NAMESPACE(Components)

YSL_END

