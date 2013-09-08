/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Border.cpp
\ingroup UI
\brief 图形用户界面边框。
\version r58
\author FrankHB <frankhb1989@gmail.com>
\since build 443
\par 创建时间:
	2013-09-06 23:25:42 +0800
\par 修改时间:
	2013-09-06 23:42 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Border
*/


#include "YSLib/UI/Border.h"
#include "YSLib/Service/ydraw.h"
#include "YSLib/UI/ygui.h"
#include "YSLib/UI/ywidget.h"

namespace YSLib
{

using namespace Drawing;

namespace UI
{

BorderStyle::BorderStyle()
	: ActiveColor(FetchGUIState().Colors[Styles::ActiveBorder]),
	InactiveColor(FetchGUIState().Colors[Styles::InactiveBorder])
{}


void
BorderBrush::operator()(PaintEventArgs&& e)
{
	auto& sender(e.GetSender());

	DrawRect(e.Target, Rect(e.Location, GetSizeOf(sender)), IsFocused(sender)
		? Style.ActiveColor : Style.InactiveColor);
}

} // namespace UI;

} // namespace YSLib;

