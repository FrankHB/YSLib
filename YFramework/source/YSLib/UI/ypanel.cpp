/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ypanel.cpp
\ingroup UI
\brief 样式无关的图形用户界面面板。
\version r246;
\author FrankHB<frankhb1989@gmail.com>
\since build 201 。
\par 创建时间:
	2011-04-13 20:44:51 +0800;
\par 修改时间:
	2012-08-27 17:30 +0800;
\par 文本编码:
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
	SetContainerPtrOf(wgt, this);
}

bool
Panel::operator-=(IWidget& wgt)
{
	if(FetchContainerPtr(wgt) == this)
	{
		SetContainerPtrOf(wgt);
		if(FetchFocusingPtr(*this) == &wgt)
			GetView().pFocusing = nullptr;
		return MUIContainer::operator-=(wgt);
	}
	return false;
}

void
Panel::Add(IWidget& wgt, ZOrderType z)
{
	MUIContainer::Add(wgt, z);
	SetContainerPtrOf(wgt, this);
}

void
Panel::ClearContents()
{
	ClearFocusingOf(*this);
	mWidgets.clear();
	SetInvalidationOf(*this);
}

bool
Panel::MoveToTop(IWidget& wgt)
{
	using ystdex::get_value;

	auto i(std::find(mWidgets.begin() | get_value, mWidgets.end() | get_value,
		&wgt));

	if(i != mWidgets.end())
	{
		const ZOrderType z(i.get()->first);

		mWidgets.erase(i);
		mWidgets.insert(make_pair(z, static_cast<IWidget*>(&wgt)));
		Invalidate(wgt);
		return true;
	}
	return false;
}

void
Panel::Refresh(PaintEventArgs&& e)
{
	if(!e.ClipArea.IsUnstrictlyEmpty())
		PaintVisibleChildren(e);
}

YSL_END_NAMESPACE(Components)

YSL_END

