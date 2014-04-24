/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TabControl.cpp
\ingroup UI
\brief 样式相关的图形用户界面标签页控件。
\version r181
\author FrankHB <frankhb1989@gmail.com>
\since build 494
\par 创建时间:
	2014-04-19 11:21:05 +0800
\par 修改时间:
	2014-04-24 12:55 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::TabControl
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_TabControl
#include YFM_YSLib_UI_YGUI
#include <ystdex/iterator.hpp>

namespace YSLib
{

using namespace Drawing;

namespace UI
{

TabBar::TabBar(const Rect& r)
	: Control(r), MLinearUIContainer(), MSharedSelection()
{
	FetchEvent<Resize>(*this) += [this]{
		PerformLayout();
	};
}

void
TabBar::operator+=(IWidget& tab)
{
	MLinearUIContainer::operator+=(tab);
	SetContainerPtrOf(tab, this);
}

bool
TabBar::operator-=(IWidget& tab)
{
	return RemoveFrom(tab, *this) ? MLinearUIContainer::operator-=(tab) : false;
}

void
TabBar::DefaultPaintTabBorder(PaintEventArgs&& e)
{
	auto& sender(e.GetSender());

	DrawRect(e.Target, e.ClipArea, Rect(e.Location, GetSizeOf(sender)),
		 FetchGUIState().Colors[IsFocused(sender)
		 ? Styles::ActiveBorder : Styles::InactiveBorder]);
}

SPos
TabBar::PerformLayout()
{
	const Size s(MinWidth, GetHeight());
	SPos x(Offset);

	for(const auto& wgt : vWidgets)
	{
		SetBoundsOf(*wgt, Rect(x, 0, s));
		x += MinWidth;
	}
	return x;
}

void
TabBar::Refresh(PaintEventArgs&& e)
{
	if(!e.ClipArea.IsUnstrictlyEmpty())
		for(auto pr = GetChildren(); pr.first != pr.second; ++pr.first)
		{
			auto& wgt(*pr.first);

			if(PaintTabBackground)
				PaintTabBackground(PaintEventArgs(wgt, e));
			PaintVisibleChild(wgt, e);
			if(PaintTabForeground)
				PaintTabForeground(PaintEventArgs(wgt, e));
		}
}

bool
TabBar::SwitchPage(Control& ctl)
{
	if(IsEnabled(ctl))
	{
		UpdateState(&ctl);
		Selected(SelectedArgs(*this, &ctl));
		return true;
	}
	return {};
}


TabControl::TabControl(const Rect& r)
	: Control(r),
	p_bar(new TabBar(Size(r.Width, BarHeight)))
{}

void
TabControl::operator+=(IWidget& wgt)
{
	tab_pages.push_back(&dynamic_cast<Panel&>(wgt));
}

void
TabControl::Attach(Control& ctl)
{
	AddWidgetHandlerAdaptor<TouchDown>(ctl, *this, [&]{
		SwitchPage(ctl);
	});
}

Rect
TabControl::GetPageBounds() const ynothrow
{
	const SDst h(GetHeight());

	return Rect(0, max<SDst>(BarHeight, h), GetWidth(),
		h < BarHeight ? 0 : h - BarHeight);
}

bool
TabControl::SwitchPage(size_t idx)
{
	if(idx < tab_pages.size())
	{
		if(const auto p_tab = tab_pages[idx])
			return SwitchPage(*p_tab);
	}
	return {};
}
bool
TabControl::SwitchPage(Control& ctl)
{
	YAssert(([this, &ctl]{
		return std::find_if(tab_pages.begin(), tab_pages.end(),
			[&](Panel* p){
			return p == &ctl;
		}) != tab_pages.end();
	}()), "Invalid tab found."),
	YAssert(bool(p_bar), "Null pointer found.");

	p_page = {};
	if(p_bar->SwitchPage(ctl))
	{
		for(const auto& p : tab_pages)
			if(p)
			{
				SetVisibleOf(*p, p == &ctl);
				SetBoundsOf(*p, GetPageBounds());
				p_page = p;
			}
		return true;
	}
	return {};
}

} // namespace UI;

} // namespace YSLib;

