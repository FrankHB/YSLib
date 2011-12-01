/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ypanel.cpp
\ingroup UI
\brief 样式无关的图形用户界面面板。
\version r1199;
\author FrankHB<frankhb1989@gmail.com>
\since build 201 。
\par 创建时间:
	2011-04-13 20:44:51 +0800;
\par 修改时间:
	2011-11-30 14:38 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YPanel;
*/


#include "YSLib/UI/ypanel.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

Panel::Panel(const Rect& r)
	: Control(r), MUIContainer()
{}

void
Panel::operator+=(IWidget& wgt)
{
	MUIContainer::operator+=(wgt);
	wgt.GetView().pContainer = this;
}

bool
Panel::operator-=(IWidget& wgt)
{
	if(FetchContainerPtr(wgt) == this)
	{
		wgt.GetView().pContainer = nullptr;
		if(FetchFocusingPtr(*this) == &wgt)
			GetView().pFocusing = nullptr;
		return MUIContainer::operator-=(wgt);
	}
	return false;
}

Rect
Panel::Refresh(const PaintContext& pc)
{
	const Rect& r(pc.ClipArea);
	bool result(!r.IsUnstrictlyEmpty()
		|| CheckVisibleChildren(sWidgets.begin(), sWidgets.end()));
	
	if(result)
	{
		Widget::Refresh(pc);
		return PaintChildren(pc);
	}
	return pc.ClipArea;
}

YSL_END_NAMESPACE(Components)

YSL_END

