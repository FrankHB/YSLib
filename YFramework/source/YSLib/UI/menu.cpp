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
\version r1228
\author FrankHB <frankhb1989@gmail.com>
\since build 203
\par 创建时间:
	2011-06-02 12:20:10 +0800
\par 修改时间:
	2014-08-14 21:57 +0800
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

Menu::Menu(const Rect& r, const shared_ptr<ListType>& h, ID id)
	: TextList(r, h, FetchGUIState().Colors.GetPair(Styles::Highlight,
		Styles::HighlightText)),
	id(id), pParent(nullptr), mSubMenus(), vDisabled(h ? h->size() : 0)
{
	Background = SolidBrush(FetchGUIState().Colors[Styles::Panel]),
	Unit.Margin = Padding(6, 18, 4, 4);
	CyclicTraverse = true;
	yunseq(
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
					if(i->second == p_mnu->id)
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
	FetchEvent<Paint>(Unit).Add([this]{
		// TODO: Handle different highlight text colors.
		if(!IsItemEnabled(idxShared))
			Unit.ForeColor = FetchGUIState().Colors[Styles::GrayText];
	}, BackgroundPriority),
	FetchEvent<Paint>(Unit) += [this](PaintEventArgs&& e){
		const auto& unit(GetBoundsOf(Unit) + e.Location);

		if(YB_LIKELY(unit.Width > 16) && ystdex::exists(mSubMenus, idxShared))
			DrawArrow(e.Target, Rect(unit.X + unit.Width - 16, unit.Y, 16,
				unit.Height) & e.ClipArea, 4, RDeg0, Unit.ForeColor);
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
Menu::CheckConfirmed(Menu::ListType::size_type idx) const
{
	return TextList::CheckConfirmed(idx) && IsItemEnabled(idx);
}

bool
Menu::Show(ZOrderType z)
{
	if(pHost)
	{
		pHost->Show(id, z);
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
		pHost->Hide(id);
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
	: Frame(frm), mMenus(), Roots()
{}
MenuHost::~MenuHost()
{
	HideAll();
	Clear();
}

void
MenuHost::operator+=(const ValueType& val)
{
	YAssertNonnull(val.second);
	yunseq(mMenus[val.first] = val.second,
		val.second->id = val.first, val.second->pHost = this);
}

void
MenuHost::operator+=(Menu& mnu)
{
	mMenus[mnu.id] = &mnu;
	mnu.pHost = this;
}

bool
MenuHost::operator-=(Menu::ID id)
{
	const auto i(mMenus.find(id));

	if(i != mMenus.end())
	{
		auto& mnu(*i->second);

		mnu.pHost = {};
		mMenus.erase(i);
		return true;
	}
	return {};
}

bool
MenuHost::IsShowing(Menu::ID id)
{
	const auto i(mMenus.find(id));

	return i == mMenus.end() ? false : Frame.Contains(*i->second);
}

bool
MenuHost::Contains(Menu& mnu)
{
	using ystdex::get_value;

	return std::find(mMenus.begin() | get_value, mMenus.end() | get_value, &mnu)
		!= mMenus.end();
}

void
MenuHost::Clear()
{
	std::for_each(mMenus.begin(), mMenus.end(), delete_second_mem());
	mMenus.clear();
}

void
MenuHost::Show(Menu::ID id, ZOrderType z)
{
	const auto i(mMenus.find(id));

	if(i != mMenus.end())
		ShowRaw(*i->second, z);
}

void
MenuHost::ShowAll(ZOrderType z)
{
	using ystdex::get_value;

	std::for_each(mMenus.cbegin() | get_value, mMenus.cend() | get_value,
		[this, z](const ItemType& p_mnu){
		if(p_mnu)
			ShowRaw(*p_mnu, z);
	});
}

void
MenuHost::ShowRaw(Menu& mnu, ZOrderType z)
{
	Frame.Add(mnu, z);
//依赖 mnu 的 GotFocus 事件默认会调用自身的 Invalidate 函数。
//	Invalidate(mnu);
	RequestFocus(mnu);
}

void
MenuHost::Hide(Menu::ID id)
{
	const auto i(mMenus.find(id));

	if(i != mMenus.end())
		HideRaw(*i->second);
}

void
MenuHost::HideAll()
{
	using ystdex::get_value;

	std::for_each(mMenus.cbegin() | get_value, mMenus.cend() | get_value,
		[this](const ItemType& p_mnu){
		if(p_mnu)
			HideRaw(*p_mnu);
	});
}

void
MenuHost::HideRaw(Menu& mnu)
{
	ReleaseFocus(mnu);
	if(IsVisible(mnu))
		Invalidate(mnu);
	Frame -= mnu;
}

void
MenuHost::HideUnrelated(Menu& mnu, Menu& mnuParent)
{
	if(Contains(mnuParent))
	{
		auto p_mnu(&mnu);

		while(p_mnu && p_mnu != &mnuParent)
		{
			const auto i(mMenus.find(p_mnu->GetID()));

			if(i != mMenus.end())
				HideRaw(*i->second);
			p_mnu = p_mnu->GetParentPtr();
		}
		if(!p_mnu)
			HideAll();
	}
	else
		HideAll();
}

} // namespace UI;

} // namespace YSLib;

