/*
	© 2011-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Menu.cpp
\ingroup UI
\brief 样式相关的菜单。
\version r1458
\author FrankHB <frankhb1989@gmail.com>
\since build 203
\par 创建时间:
	2011-06-02 12:20:10 +0800
\par 修改时间:
	2016-02-12 00:42 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Menu
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_Menu
#include YFM_YSLib_UI_YWindow
#include YFM_YSLib_Service_YBrush
#include YFM_YSLib_Service_TextRenderer

namespace YSLib
{

using namespace Drawing;

namespace UI
{

Menu::Menu(const Rect& r, const shared_ptr<ListType>& h)
	: TextList(r, h, FetchGUIConfiguration().Colors.GetPair(Styles::Highlight,
		Styles::HighlightText)),
	vDisabled(h ? h->size() : 0)
{
	auto& unit(GetUnitRef());

	yunseq(
	Background = SolidBrush(FetchGUIConfiguration().Colors[Styles::Panel]),
	LabelBrush.Margin = Padding(6, 18, 4, 4),
	CyclicTraverse = true,
	SelectionOptions[ClearSelectionOnConfirm] = true,
	SelectionOptions[ClearSelectionOnLeave] = true,
	SelectionOptions[SelectOnHover] = true,
	IsRelated = [this](IWidget& wgt){
		if(pHost)
		{
			const auto idx(GetSelectedIndex());

			if(idx != size_t(-1))
			{
				const auto i(mSubMenus.find(idx));

				if(i != mSubMenus.end() && i->second
					&& pHost->IsShowing(*i->second))
					return true;
			}
		}

		auto p_mnu = dynamic_cast<Menu*>(&wgt);

		if(!p_mnu)
			p_mnu = dynamic_cast<Menu*>(FetchContainerPtr(wgt).get());
		return p_mnu ? p_mnu->GetParentPtr().get() == this : false;
	},
	FetchEvent<KeyDown>(*this) += [this](KeyEventArgs&& e){
		if(pHost)
		{
			const auto& k(e.GetKeys());

			if(k.count() == 1)
			{
				if(k[KeyCodes::Right])
				{
					if(IsSelected())
						TryShowingSub(GetSelectedIndex());
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
	FetchEvent<CursorOver>(*this) += [this](CursorEventArgs&& e){
		if(SelectionOptions[SelectOnHover]
			&& e.Strategy != RoutedEventArgs::Bubble
			&& FetchGUIState().IsCursorMoved())
		{
			const auto idx(CheckPoint(GetSizeOf(*this), e));

			if(idx != size_t(-1))
			{
				RequestFocus(*this);
				TryShowingSub(idx);
			}
		}
	},
	FetchEvent<LostFocus>(*this) += [this](UIEventArgs&& e){
		if(pHost)
		{
			{
				const auto i(pHost->Roots.find(e.GetSender()));

				if(i != pHost->Roots.end())
				{
					auto p_mnu(make_observer(this));

					while(const auto p = p_mnu->GetParentPtr())
						p_mnu = p;
					if(i->second == p_mnu)
						return;
				}
			}
			if(const auto p_mnu = dynamic_cast<Menu*>(&e.GetSender()))
			{
				if(p_mnu->GetParentPtr().get() != this)
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
			LabelBrush.ForeColor
				= FetchGUIConfiguration().Colors[Styles::GrayText];
	}, BackgroundPriority),
	FetchEvent<Paint>(unit) += [this](PaintEventArgs&& e){
		const auto& ubound(GetBoundsOf(GetUnitRef()) + e.Location);

		if(YB_LIKELY(ubound.Width > 16) && ystdex::exists(mSubMenus, idxShared))
			DrawArrow(e.Target, e.ClipArea, Rect(ubound.GetRight() - 16,
				ubound.Y, 16, ubound.Height), 4, RDeg0, LabelBrush.ForeColor);
	}
	);
}
ImplDeDtor(Menu)

void
Menu::operator+=(const ValueType& val)
{
	if(val.second && IsInInterval(val.first, GetList().size()))
	{
		val.second->pParent = make_observer(this);
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
Menu::Hide()
{
	if(pHost)
	{
		pHost->Hide(*this);
		return true;
	}
	return {};
}

bool
Menu::Show()
{
	if(pHost)
	{
		pHost->Show(*this);
		return true;
	}
	return {};
}

observer_ptr<Menu>
Menu::ShowSub(IndexType idx)
{
	if(pHost)
	{
		const auto i(mSubMenus.find(idx));

		if(i != mSubMenus.end())
		{
			auto& mnu(*i->second);

			LocateMenu(mnu, *this, idx);
			mnu.Show();
			return make_observer(&mnu);
		}
	}
	return {};
}

void
Menu::TryShowingSub(IndexType idx)
{
	if(const auto p_mnu = ShowSub(idx))
		p_mnu->SelectFirst();
}

void
LocateMenu(Menu& dst, const Menu& src, Menu::IndexType idx)
{
	const Rect& r(GetBoundsOf(src));

	// XXX: Conversion to 'SPos' might be implementation-defined.
	SetLocationOf(dst, {r.GetRight(), r.Y + SPos(src.GetItemHeight() * idx)});
}


void
MenuHost::operator+=(Menu& mnu)
{
	menus.insert(mnu);
	mnu.pHost = make_observer(this);
	UI::Hide(mnu);
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
	return Contains(mnu) ? IsVisible(mnu) : false;
}

void
MenuHost::Clear()
{
	menus.clear();
}

void
MenuHost::Show(Menu& mnu)
{
	if(Contains(mnu))
		ShowRaw(mnu);
}

void
MenuHost::ShowAll()
{
	std::for_each(menus.begin(), menus.end(), [this](IWidget& wgt){
		ShowRaw(ystdex::polymorphic_downcast<Menu&>(wgt));
	});
}

void
MenuHost::ShowRaw(Menu& mnu)
{
	YAssert(Contains(mnu), "Invalid menu found.");

	UI::Show(mnu);
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
	{
		Invalidate(mnu);
		UI::Hide(mnu);
	}
}

void
MenuHost::HideUnrelated(Menu& mnu, Menu& mnu_parent)
{
	if(Contains(mnu_parent))
	{
		auto p_mnu(make_observer(&mnu));

		for(; p_mnu && p_mnu != make_observer(&mnu_parent);
			p_mnu = p_mnu->GetParentPtr())
			// XXX: %Hide or %HideRaw?
			Hide(*p_mnu);
		if(!p_mnu)
			HideAll();
	}
	else
		HideAll();
}


void
BindTopLevelPopupMenu(MenuHost& mh, Menu& mnu, IWidget& wgt, KeyIndex k)
{
	FetchEvent<Click>(wgt) += [&, k](CursorEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct && e.Keys.count() == 1 && e[k])
		{
			mnu.GetView().SetLocation(LocateForTopOffset(e.Position, wgt));
			mh.Show(mnu);
		}
	};
}

} // namespace UI;

} // namespace YSLib;

