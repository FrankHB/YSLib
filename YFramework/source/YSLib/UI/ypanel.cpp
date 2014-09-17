/*
	© 2011-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ypanel.cpp
\ingroup UI
\brief 样式无关的 GUI 面板。
\version r290
\author FrankHB <frankhb1989@gmail.com>
\since build 201
\par 创建时间:
	2011-04-13 20:44:51 +0800
\par 修改时间:
	2014-09-17 03:24 +0800
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
	: Control(r, MakeBlankBrush()), MUIContainer()
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
	return RemoveFrom(wgt, *this) ? MUIContainer::operator-=(wgt) : false;
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
Panel::MoveToFront(IWidget& wgt)
{
	const auto i(std::find_if(mWidgets.cbegin(), mWidgets.cend(),
		[&](decltype(*mWidgets.cend()) pr){
		return ystdex::is_equal()(pr.second, wgt);
	}));

	if(i != mWidgets.cend())
	{
		const ZOrderType z(i->first);

		mWidgets.erase(i);
		mWidgets.emplace(z, std::ref(wgt));
		Invalidate(wgt);
		return true;
	}
	return {};
}

void
Panel::Refresh(PaintEventArgs&& e)
{
	if(!e.ClipArea.IsUnstrictlyEmpty())
		PaintVisibleChildren(e);
}

} // namespace UI;

} // namespace YSLib;

