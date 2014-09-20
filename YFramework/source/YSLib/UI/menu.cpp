/*
	© 2011-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file menu.cpp
\ingroup UI
\brief 样式相关的菜单。
\version r1324
\author FrankHB <frankhb1989@gmail.com>
\since build 203
\par 创建时间:
	2011-06-02 12:20:10 +0800
\par 修改时间:
	2014-09-20 18:30 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Menu
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_Menu
#include YFM_YSLib_UI_YWindow
#include YFM_YSLib_UI_YBrush
#include YFM_YSLib_Service_TextRenderer

namespace YSLib
{

using namespace Drawing;

namespace UI
{

Menu::Menu(const Rect& r, const shared_ptr<ListType>& h)
	: TextList(r, h, FetchGUIState().Colors.GetPair(Styles::Highlight,
		Styles::HighlightText)),
	pParent(), mSubMenus(), vDisabled(h ? h->size() : 0)
{
	auto& unit(GetUnitRef());

	yunseq(
	Background = SolidBrush(FetchGUIState().Colors[Styles::Panel]),
	LabelBrush.Margin = Padding(6, 18, 4, 4),
	CyclicTraverse = true,
	FetchEvent<KeyDown>(*this) += [this](KeyEventArgs&& e){
		if(pHost)
		{
			const auto& k(e.GetKeys());

			if(k.count() == 1)
			{
				if(k[KeyCodes::Right])
				{
					if(IsSelected())
						if(const auto p_mnu = ShowSub(GetSelectedIndex()))
							p_mnu->SelectFirst();
				}
				else if(k[KeyCodes::Left] || k[KeyCodes::Esc])
				{
					if(const auto p_mnu = GetParentPtr())
						RequestFocus(*p_mnu);
					else if(k[KeyCodes::Esc])
						Hide();
				}
			}
		}
	},
	FetchEvent<LostFocus>(*this) += [this](UIEventArgs&& e){
		if(pHost)
		{
			{
				const auto i(pHost->Roots.find(&e.GetSender()));

				if(i != pHost->Roots.end())
				{
					auto p_mnu(this);

					while(const auto pParent = p_mnu->GetParentPtr())
						p_mnu = pParent;
					if(i->second == p_mnu)
						return;
				}
			}
			if(const auto p_mnu = dynamic_cast<Menu*>(&e.GetSender()))
			{
				if(p_mnu->GetParentPtr() != this)
					pHost->HideUnrelated(*this, *p_mnu);
			}
			else
				pHost->HideAll();
		}
	},
	Confirmed += [this](IndexEventArgs&& e){
		if(Contains(e) && pHost && !ShowSub(e.Value))
			pHost->HideAll();
	},
	Iterated += [this](size_t idx){
		SetEnabledOf(GetUnitRef(), IsItemEnabled(idx));
	},
	FetchEvent<Paint>(unit).Add([this]{
		// TODO: Handle different highlight text colors.
		if(!IsEnabled(GetUnitRef()))
			LabelBrush.ForeColor = FetchGUIState().Colors[Styles::GrayText];
	}, BackgroundPriority),
	FetchEvent<Paint>(unit) += [this](PaintEventArgs&& e){
		const auto& ubound(GetBoundsOf(GetUnitRef()) + e.Location);

		if(YB_LIKELY(ubound.Width > 16) && ystdex::exists(mSubMenus, idxShared))
			DrawArrow(e.Target, e.ClipArea, Rect(ubound.X + ubound.Width - 16,
				ubound.Y, 16, ubound.Height), 4, RDeg0, LabelBrush.ForeColor);
	}
	);
}

void
Menu::operator+=(const ValueType& val)
{
	if(val.second && IsInInterval(val.first, GetList().size()))
	{
		val.second->pParent = this;
		mSubMenus.insert(val);
	}
}

bool
Menu::operator-=(IndexType idx)
{
	if(IsInInterval(idx, GetList().size()))
	{
		const auto i(mSubMenus.find(idx));

		if(i != mSubMenus.end() && i->second)
		{
			i->second->pParent = {};
			mSubMenus.erase(i);
			return true;
		}
		return {};
	}
	return {};
}

bool
Menu::IsItemEnabled(ListType::size_type idx) const
{
	YAssert(IsInInterval(idx, GetList().size()), "Index is out of range.");
	AdjustSize();
	return !vDisabled[idx];
}

void
Menu::SetItemEnabled(Menu::ListType::size_type idx, bool b)
{
	YAssert(IsInInterval(idx, GetList().size()), "Index is out of range.");
	AdjustSize();
	vDisabled[idx] = !b;
}

void
Menu::AdjustSize() const
{
	const auto list_size(GetList().size());

	if(vDisabled.size() != list_size)
		vDisabled.resize(list_size);
}

bool
Menu::Show(ZOrderType z)
{
	if(pHost)
	{
		pHost->Show(*this, z);
		return true;
	}
	return {};
}

Menu*
Menu::ShowSub(IndexType idx, ZOrderType z)
{
	if(pHost)
	{
		const auto i(mSubMenus.find(idx));

		if(i != mSubMenus.end())
		{
			auto& mnu(*i->second);

			LocateMenu(mnu, *this, idx);
			mnu.Show(z);
			return &mnu;
		}
	}
	return {};
}

bool
Menu::Hide()
{
	if(pHost)
	{
		pHost->Hide(*this);
		return true;
	}
	return {};
}

void
LocateMenu(Menu& dst, const Menu& src, Menu::IndexType idx)
{
	SetLocationOf(dst, Point(src.GetX() + src.GetWidth(),
		src.GetY() + src.GetItemHeight() * idx));
}


MenuHost::MenuHost(Window& frm)
	: Frame(frm)
{}
MenuHost::~MenuHost()
{
	// TODO: Explicit exception specification.
	HideAll();
	Clear();
}

void
MenuHost::operator+=(Menu& mnu)
{
	menus.insert(mnu);
	mnu.pHost = this;
}

bool
MenuHost::operator-=(Menu& mnu)
{
	if(menus.erase(mnu) != 0)
	{
		mnu.pHost = {};
		return true;
	}
	return {};
}

bool
MenuHost::IsShowing(Menu& mnu) const
{
	return Contains(mnu) ? Frame.Contains(mnu) : false;
}

void
MenuHost::Clear()
{
	menus.clear();
}

void
MenuHost::Show(Menu& mnu, ZOrderType z)
{
	if(Contains(mnu))
		ShowRaw(mnu, z);
}

void
MenuHost::ShowAll(ZOrderType z)
{
	std::for_each(menus.begin(), menus.end(), [this, z](IWidget& wgt){
		ShowRaw(ystdex::polymorphic_downcast<Menu&>(wgt), z);
	});
}

void
MenuHost::ShowRaw(Menu& mnu, ZOrderType z)
{
	YAssert(Contains(mnu), "Invalid menu found.");

	Frame.Add(mnu, z);
//依赖 mnu 的 GotFocus 事件默认会调用自身的 Invalidate 函数。
//	Invalidate(mnu);
	RequestFocus(mnu);
}

void
MenuHost::Hide(Menu& mnu)
{
	if(Contains(mnu))
		HideRaw(mnu);
}

void
MenuHost::HideAll()
{
	std::for_each(menus.begin(), menus.end(), [this](Menu& mnu){
		HideRaw(mnu);
	});
}

void
MenuHost::HideRaw(Menu& mnu)
{
	YAssert(Contains(mnu), "Invalid menu found.");

	ReleaseFocus(mnu);
	if(IsVisible(mnu))
		Invalidate(mnu);
	Frame -= mnu;
}

void
MenuHost::HideUnrelated(Menu& mnu, Menu& mnu_parent)
{
	if(Contains(mnu_parent))
	{
		auto p_mnu(&mnu);

		for(; p_mnu && p_mnu != &mnu_parent; p_mnu = p_mnu->GetParentPtr())
			// XXX: %Hide or %HideRaw?
			Hide(*p_mnu);
		if(!p_mnu)
			HideAll();
	}
	else
		HideAll();
}

} // namespace UI;

} // namespace YSLib;

