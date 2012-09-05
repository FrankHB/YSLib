/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywgtview.cpp
\ingroup UI
\brief 样式无关的图形用户界面部件。
\version r161
\author FrankHB<frankhb1989@gmail.com>
\since build 258
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2012-09-04 12:46 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YWidgetView
*/


#include "YSLib/UI/ywidget.h"

YSL_BEGIN

using namespace Drawing;

YSL_BEGIN_NAMESPACE(Components)

Visual::Visual(const Rect& r)
	: visible(true), transparent(false),
	location(r.GetPoint()), size(r.Width, r.Height)
{}


bool
View::IsVisible() const ynothrow
{
	return pDependency ? pDependency->GetView().IsVisible()
		: visual.IsVisible();
}
bool
View::IsTransparent() const ynothrow
{
	return pDependency ? pDependency->GetView().IsTransparent()
		: visual.IsTransparent();
}

void
View::SetVisible(bool b)
{
	if(pDependency)
		pDependency->GetView().SetVisible(b);
	else
		visual.SetVisible(b);
}
void
View::SetTransparent(bool b)
{
	if(pDependency)
		pDependency->GetView().SetTransparent(b);
	else
		visual.SetTransparent(b);
}

YSL_END_NAMESPACE(Components)

YSL_END

