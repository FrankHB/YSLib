/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywgtview.cpp
\ingroup UI
\brief 样式无关的图形用户界面部件。
\version r1115;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2011-11-12 11:29 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YWidgetView;
*/


#include "YSLib/UI/ywgtview.h"

YSL_BEGIN

using namespace Drawing;

YSL_BEGIN_NAMESPACE(Components)

Visual::Visual(const Rect& r)
	: visible(true), transparent(false),
	location(r.GetPoint()), size(r.Width, r.Height)
{}


MBackground::MBackground(const shared_ptr<Image>& hImg)
	: hBgImage(hImg)
{}

BitmapPtr
MBackground::GetBackgroundPtr() const
{
	return hBgImage ? hBgImage->GetImagePtr() : nullptr;
}

YSL_END_NAMESPACE(Components)

YSL_END

