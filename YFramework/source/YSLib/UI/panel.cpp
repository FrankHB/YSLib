/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file panel.cpp
\ingroup UI
\brief 样式无关的图形用户界面面板。
\version r1185;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-04-13 20:44:51 +0800;
\par 修改时间:
	2011-11-09 14:27 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Panel;
*/


#include "YSLib/UI/panel.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

Panel::Panel(const Rect& r)
	: Control(r), MUIContainer()
{}

void
Panel::operator+=(IWidget& wgt)
{
	MUIContainer::operator+=(wgt);
	wgt.GetContainerPtrRef() = this;
}

bool
Panel::operator-=(IWidget& wgt)
{
	if(FetchContainerPtr(wgt) == this)
	{
		wgt.GetContainerPtrRef() = nullptr;
		if(FetchFocusingPtr(*this) == &wgt)
			GetContainerPtrRef() = nullptr;
		return MUIContainer::operator-=(wgt);
	}
	return false;
}

YSL_END_NAMESPACE(Components)

YSL_END

