/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ypanel.cpp
\ingroup Shell
\brief 样式无关的图形用户界面面板。
\version 0.1133;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-04-13 20:44:51 +0800;
\par 修改时间:
	2011-05-26 09:34 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YPanel;
*/


#include "ypanel.h"
#include "ywindow.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

Panel::Panel(const Rect& r)
	: Control(r), MUIContainer()
{}
Panel::~Panel()
{}

void
Panel::operator+=(IWidget& wgt)
{
	MUIContainer::operator+=(wgt);
	wgt.GetContainerPtr() = this;
}
void
Panel::operator+=(IControl& ctl)
{
	MUIContainer::operator+=(ctl);
	ctl.GetContainerPtr() = this;
}

bool
Panel::operator-=(IWidget& wgt)
{
	if(wgt.GetContainerPtr() == this)
	{
		wgt.GetContainerPtr() = nullptr;
		return MUIContainer::operator-=(wgt);
	}
	return false;
}
bool
Panel::operator-=(IControl& ctl)
{
	if(ctl.GetContainerPtr() == this)
	{
		ctl.GetContainerPtr() = nullptr;
		return MUIContainer::operator-=(ctl);
	}
	return false;
}

void
Panel::ClearFocusingPtr()
{
	IControl* const p(GetFocusingPtr());

	if(p)
	{
		MUIContainer::ClearFocusingPtr();
		EventMap.GetEvent<HVisualEvent>(LostFocus)(*p, EventArgs());
	}
}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

