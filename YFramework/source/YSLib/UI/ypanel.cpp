/*
	© 2011-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ypanel.cpp
\ingroup UI
\brief 样式无关的 GUI 面板。
\version r268
\author FrankHB <frankhb1989@gmail.com>
\since build 201
\par 创建时间:
	2011-04-13 20:44:51 +0800
\par 修改时间:
	2013-12-23 23:41 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YPanel
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_YPanel

namespace YSLib
{

namespace UI
{

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
			GetView().FocusingPtr = {};
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

} // namespace UI;

} // namespace YSLib;

