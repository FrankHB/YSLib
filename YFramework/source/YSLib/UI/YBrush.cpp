﻿/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YBrush.cpp
\ingroup UI
\brief 图形用户界面画刷。
\version r1118;
\author FrankHB<frankhb1989@gmail.com>
\since build 293 。
\par 创建时间:
	2012-01-10 19:56:59 +0800;
\par 修改时间:
	2012-03-14 21:14 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::UI::Border;
*/


#include "YSLib/UI/YBrush.h"
#include "YSLib/Service/ydraw.h"
#include "YSLib/UI/ygui.h"
#include "YSLib/UI/ywidget.h"

YSL_BEGIN

using namespace Drawing;

YSL_BEGIN_NAMESPACE(Components)

void
SolidBrush::operator()(PaintEventArgs&& e)
{
	auto& sender(e.GetSender());

	if(!sender.GetView().IsTransparent())
		Drawing::FillRect(e.Target, e.ClipArea, Color);
}


MBackground::MBackground(const shared_ptr<Image>& hImg)
	: hBgImage(hImg)
{}

BitmapPtr
MBackground::GetBackgroundPtr() const
{
	return hBgImage ? hBgImage->GetImagePtr() : nullptr;
}


BorderStyle::BorderStyle()
	: ActiveColor(FetchGUIState().Colors[Styles::ActiveBorder]),
	InactiveColor(FetchGUIState().Colors[Styles::InactiveBorder])
{}


void
BorderBrush::operator()(PaintEventArgs&& e)
{
	if(auto pStyle = StylePtr.lock())
	{
		auto& wgt(e.GetSender());

		if(!wgt.GetView().IsTransparent())
			DrawRect(e.Target, e.Location, GetSizeOf(wgt), IsFocused(wgt)
				? pStyle->ActiveColor : pStyle->InactiveColor);
	}
}

YSL_END_NAMESPACE(Components)

YSL_END
