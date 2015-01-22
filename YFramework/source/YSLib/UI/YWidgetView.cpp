/*
	© 2009-2013, 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YWidgetView.cpp
\ingroup UI
\brief 样式无关的 GUI 部件。
\version r202
\author FrankHB <frankhb1989@gmail.com>
\since build 258
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2015-01-22 18:42 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YWidgetView
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_YWidget

namespace YSLib
{

using namespace Drawing;

namespace UI
{

bool
View::IsVisible() const ynothrow
{
	return
		DependencyPtr ? DependencyPtr->GetView().IsVisible() : visual.Visible;
}

void
View::SetVisible(bool b)
{
	if(DependencyPtr)
		DependencyPtr->GetView().SetVisible(b);
	else
		visual.Visible = b;
}

} // namespace UI;

} // namespace YSLib;

