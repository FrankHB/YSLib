/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywgtview.cpp
\ingroup UI
\brief 样式无关的 GUI 部件。
\version r190
\author FrankHB <frankhb1989@gmail.com>
\since build 258
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2013-08-08 05:37 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YWidgetView
*/


#include "YSLib/UI/ywidget.h"

namespace YSLib
{

using namespace Drawing;

namespace UI
{

Visual::Visual(const Rect& r)
	: visible(true),
	location(r.GetPoint()), size(r.Width, r.Height)
{}


bool
View::IsVisible() const ynothrow
{
	return DependencyPtr ? DependencyPtr->GetView().IsVisible()
		: visual.IsVisible();
}

void
View::SetVisible(bool b)
{
	if(DependencyPtr)
		DependencyPtr->GetView().SetVisible(b);
	else
		visual.SetVisible(b);
}

} // namespace UI;

} // namespace YSLib;

