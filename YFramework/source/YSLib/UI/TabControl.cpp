/*
	© 2014-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TabControl.cpp
\ingroup UI
\brief 样式相关的图形用户界面标签页控件。
\version r298
\author FrankHB <frankhb1989@gmail.com>
\since build 494
\par 创建时间:
	2014-04-19 11:21:05 +0800
\par 修改时间:
	2015-03-24 21:59 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::TabControl
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_TabControl
#include YFM_YSLib_UI_YGUI
#include YFM_YSLib_Service_YGDI // for Drawing::Clip;
#include <ystdex/iterator.hpp>

namespace YSLib
{

using namespace Drawing;

namespace UI
{

TabBar::TabBar(const Rect& r)
	: Control(r), MLinearUIContainer()
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
		 FetchGUIConfiguration().Colors[IsFocused(sender)
		 ? Styles::ActiveBorder : Styles::InactiveBorder]);
}

SPos
TabBar::PerformLayout()
{
	const SDst h(GetHeight());
	SPos x(Offset);

	for(const auto& wgt_ref : vWidgets)
	{
		Size s(GetSizeOf(wgt_ref));

		if(s.Width < MinWidth)
			s.Width = MinWidth;
		if(h < s.Height)
			s.Height = h;
		SetBoundsOf(wgt_ref, Rect(x, 0, s));
		// XXX: Conversion to 'SPos' might be implementation-defined.
		x += SPos(s.Width);
	}
	return x;
}

void
TabBar::Refresh(PaintEventArgs&& e)
{
	if(!e.ClipArea.IsUnstrictlyEmpty())
		for(auto pr(GetChildren()); pr.first != pr.second; ++pr.first)
		{
			auto& wgt(*pr.first);

			if(IsVisible(wgt))
			{
				PaintContext& pc(e);
				PaintEventArgs e_new(wgt, pc);

				if(Clip(e_new.ClipArea,
					Rect(e_new.Location += GetLocationOf(wgt), GetSizeOf(wgt))))
				{
					if(PaintTabBackground)
						PaintTabBackground(std::move(e_new));
					wgt.GetRenderer().Paint(wgt, std::move(e_new));
					if(PaintTabForeground)
						PaintTabForeground(std::move(e_new));
				}
				pc.ClipArea |= e_new.ClipArea;
			}
		}
}

bool
TabBar::SwitchPage(Control& ctl)
{
	return IsEnabled(ctl);
}


TabControl::TabControl(const Rect& r)
	: Control(r),
	p_bar(new TabBar(Size(r.Width, BarHeight)))
{}
ImplDeDtor(TabControl)

void
TabControl::operator+=(IWidget& wgt)
{
	tab_pages.push_back(&dynamic_cast<Panel&>(wgt));
	SetContainerPtrOf(wgt, this);
	SetVisibleOf(wgt, {});
}

void
TabControl::Attach(Control& ctl)
{
	AddWidgetHandlerAdaptor<TouchDown>(ctl, *this, [&]{
		if(SwitchTab(ctl))
			Invalidate(*this);
	});
}

Rect
TabControl::GetPageBounds() const ynothrow
{
	const SDst h(GetHeight());
	const SDst h_tb(max(GetTabBarRef().GetHeight(), BarHeight));

	// XXX: Conversion to 'SPos' might be implementation-defined.
	return Rect(0, SPos(h_tb), GetWidth(), h < h_tb ? 0 : h - h_tb);
}

size_t
TabControl::Find(IWidget& wgt) const
{
	return size_t(std::find(tab_pages.cbegin(), tab_pages.cend(), &wgt)
		- tab_pages.cbegin());
}

bool
TabControl::SwitchTab(Control& ctl)
{
	const auto idx(GetTabBarRef().Find(ctl));

	return idx < GetTabCount() ? SwitchPage(idx) : false;
}

bool
TabControl::SwitchPage(size_t idx)
{
	if(idx < tab_pages.size())
	{
		if(const auto p = tab_pages[idx])
			return SwitchPage(*p);
	}
	return {};
}
bool
TabControl::SwitchPage(Control& ctl)
{
	YAssert(Find(ctl) != tab_pages.size(), "Invalid page found.");
	if(Deref(p_bar).SwitchPage(ctl))
	{
		if(p_page)
			SetVisibleOf(*p_page, {});

		const auto i(Find(ctl));

		p_page = tab_pages[i];
		SetBoundsOf(*p_page, GetPageBounds()),
		SetVisibleOf(*p_page, true);
		return true;
	}
	return {};
}

void
TabControl::UpdateTabPages()
{
	auto& tb(GetTabBarRef());

	tb.PerformLayout();

	const auto tp_count(tab_pages.size());
	size_t i(0);

	for(auto pr(tb.GetChildren()); !p_page && i < tp_count
		&& pr.first != pr.second; yunseq(++pr.first, ++i))
		if(tab_pages[i])
		{
			p_page = tab_pages[i];
			SetVisibleOf(*p_page, true),
			SetBoundsOf(*p_page, GetPageBounds());
		}
	Invalidate(*this);
}

} // namespace UI;

} // namespace YSLib;

