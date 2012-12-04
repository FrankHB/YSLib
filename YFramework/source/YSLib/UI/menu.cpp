/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file menu.cpp
\ingroup UI
\brief 样式相关的菜单。
\version r1002
\author FrankHB<frankhb1989@gmail.com>
\since build 203
\par 创建时间:
	2011-06-02 12:20:10 +0800
\par 修改时间:
	2012-12-04 14:50 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Menu
*/


#include "YSLib/UI/menu.h"
#include "YSLib/UI/ywindow.h"
#include "YSLib/UI/YBrush.h"
#include "YSLib/Service/TextRenderer.h"

YSL_BEGIN

using namespace Drawing;

YSL_BEGIN_NAMESPACE(Components)

Menu::Menu(const Rect& r, const shared_ptr<ListType>& h, ID id)
	: TextList(r, h, FetchGUIState().Colors.GetPair(Styles::Highlight,
		Styles::HighlightText)),
	id(id), pParent(nullptr), mSubMenus(), vDisabled()
{
	Background = SolidBrush(FetchGUIState().Colors[Styles::Panel]),
	SetAllOf(Margin, 6, 18, 4, 4);
	CyclicTraverse = true;
	if(h)
		vDisabled.resize(h->size());
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
							if(const auto pMnu = ShowSub(GetSelectedIndex()))
								pMnu->SelectFirst();
					}
					else if(k[KeyCodes::Left] || k[KeyCodes::Esc])
					{
						if(const auto pMnu = GetParentPtr())
							RequestFocus(*pMnu);
						else if(k[KeyCodes::Esc])
							Hide();
					}
				}
			}
		},
		FetchEvent<LostFocus>(*this) += [this](UIEventArgs&& e){
			if(pHost)
			{
				if(const auto pMnu = dynamic_cast<Menu*>(&e.GetSender()))
				{
					if(pMnu->GetParentPtr() != this)
						pHost->HideUnrelated(*this, *pMnu);
				}
				else
					pHost->HideAll();
			}
		},
		GetConfirmed() += [this](IndexEventArgs&& e){
			if(Contains(e) && pHost && !ShowSub(e.Value))
				pHost->HideAll();
		}
	);
	//刷新文本状态，防止第一次绘制前不确定文本间距，无法正确根据内容重设大小。
	RefreshTextState();
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
			i->second->pParent = nullptr;
			mSubMenus.erase(i);
			return true;
		}
		return false;
	}
	return false;
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
	return false;
}

Menu*
Menu::ShowSub(IndexType idx, ZOrderType z)
{
	if(pHost)
	{
		try
		{
			auto& mnu((*this)[idx]);

			LocateMenu(mnu, *this, idx);
			mnu.Show(z);
			return &mnu;
		}
		catch(std::out_of_range&)
		{}
	}
	return nullptr;
}

bool
Menu::Hide()
{
	if(pHost)
	{
		pHost->Hide(id);
		return true;
	}
	return false;
}

void
Menu::DrawItem(const Graphics& g, const Rect& mask, const Rect& r,
	ListType::size_type i)
{
	Color t(tsList.Color);

	// TODO: Handle different highlight text colors.

	if(!IsItemEnabled(i))
		tsList.Color = FetchGUIState().Colors[Styles::GrayText];
	DrawClippedText(g, mask, tsList, GetList()[i], false);
	tsList.Color = t;
	if(YB_LIKELY(r.Width > 16))
		if(mSubMenus.find(i) != mSubMenus.end())
			DrawArrow(g, Rect(r.X + r.Width - 16, r.Y, 16, r.Height), 4,
				RDeg0, ForeColor);
}


void
LocateMenu(Menu& dst, const Menu& src, Menu::IndexType idx)
{
	SetLocationOf(dst, Point(src.GetX() + src.GetWidth(),
		src.GetY() + src.GetItemHeight() * idx));
}


MenuHost::MenuHost(Window& frm)
	: Frame(frm), mMenus()
{}
MenuHost::~MenuHost()
{
	HideAll();
	Clear();
}

void
MenuHost::operator+=(const ValueType& val)
{
	YAssert(val.second, "Null pointer found.");

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
	try
	{
		auto& mnu((*this)[id]);

		mnu.pHost = nullptr;
		return mMenus.erase(id) != 0;
	}
	catch(std::out_of_range&)
	{}
	return false;
}

bool
MenuHost::IsShowing(Menu::ID id)
{
	try
	{
		auto& mnu((*this)[id]);

		return Frame.Contains(mnu);
	}
	catch(std::out_of_range&)
	{}
	return false;
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
	try
	{
		auto pMenu(mMenus.at(id));

		YAssert(pMenu, "Null pointer found.");

		ShowRaw(*pMenu, z);
	}
	catch(std::out_of_range&)
	{}
}

void
MenuHost::ShowAll(ZOrderType z)
{
	using ystdex::get_value;

	std::for_each(mMenus.cbegin() | get_value, mMenus.cend() | get_value,
		[this, z](const ItemType& pMnu){
		if(pMnu)
			ShowRaw(*pMnu, z);
	});
}

void
MenuHost::ShowRaw(Menu& mnu, ZOrderType z)
{
	Frame.Add(mnu, z);
//依赖于 mnu 的 GotFocus 事件默认会调用自身的 Invalidate 函数。
//	Invalidate(mnu);
	RequestFocus(mnu);
}

void
MenuHost::Hide(Menu::ID id)
{
	try
	{
		auto pMenu(mMenus.at(id));

		YAssert(pMenu, "Null pointer found.");

		HideRaw(*pMenu);
	}
	catch(std::out_of_range&)
	{}
}

void
MenuHost::HideAll()
{
	using ystdex::get_value;

	std::for_each(mMenus.cbegin() | get_value, mMenus.cend() | get_value,
		[this](const ItemType& pMnu){
		if(pMnu)
			HideRaw(*pMnu);
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
		auto pMnu(&mnu);

		while(pMnu && pMnu != &mnuParent)
		{
			try
			{
				auto pMenu(mMenus.at(pMnu->GetID()));

				YAssert(pMenu, "Null pointer found.");

				HideRaw(*pMenu);
			}
			catch(std::out_of_range&)
			{}
			pMnu = pMnu->GetParentPtr();
		}
		if(!pMnu)
			HideAll();
	}
	else
	{
		HideAll();
	}
}


YSL_END_NAMESPACE(Components)

YSL_END

