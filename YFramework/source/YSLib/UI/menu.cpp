/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file menu.cpp
\ingroup UI
\brief 样式相关的菜单。
\version r1859;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-06-02 12:20:10 +0800;
\par 修改时间:
	2011-11-05 11:31 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Menu;
*/


#include "YSLib/UI/menu.h"
#include "YSLib/UI/ywindow.h"

YSL_BEGIN

using namespace Drawing;

YSL_BEGIN_NAMESPACE(Components)

Menu::Menu(const Rect& r, const shared_ptr<ListType>& h, ID id)
	: TextList(r, h, FetchGUIShell().Colors.GetPair(Styles::Highlight,
		Styles::HighlightText)),
	id(id), pParent(nullptr), mSubMenus(), vDisabled()
{
	BackColor = FetchGUIShell().Colors[Styles::Panel];
	SetAllOf(Margin, 6, 18, 4, 4);
	CyclicTraverse = true;
	if(h)
		vDisabled.resize(h->size());
	yunsequenced(
		FetchEvent<KeyDown>(*this) += [this](KeyEventArgs&& e){
			if(pHost && IsSelected())
				switch(e.GetKeyCode())
				{
				case KeySpace::Right:
					{
						auto pMnu(ShowSub(GetSelectedIndex()));

						if(pMnu)
							pMnu->SelectFirst();
					}
					break;
				case KeySpace::Left:
					{
						auto pMnu(GetParentPtr());

						if(pMnu)
							RequestFocus(*pMnu);
					}
					break;
				default:
					break;
				}
		},
		FetchEvent<LostFocus>(*this) += [this](UIEventArgs&& e){
			if(pHost)
			{
				auto pMnu(dynamic_cast<Menu*>(&e.GetSender()));

				if(pMnu)
				{
					if(pMnu->GetParentPtr() != this)
						pHost->HideUnrelated(*this, *pMnu);
				}
				else
					pHost->HideAll();
			}
		},
		GetConfirmed() += [this](IndexEventArgs&& e){
			if(this->Contains(e) && pHost && !ShowSub(e.Index))
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
		auto i(mSubMenus.find(idx));
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
Menu::IsItemEnabled(Menu::ListType::size_type idx) const
{
	YAssert(IsInInterval(idx, GetList().size()),
		"Index out of range found @ Menu::IsItemEnabled;");

	AdjustSize();
	return !vDisabled[idx];
}

void
Menu::SetItemEnabled(Menu::ListType::size_type idx, bool b)
{
	YAssert(IsInInterval(idx, GetList().size()),
		"Index out of range found @ Menu::SetItemEnabled;");

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
Menu::CheckConfirmed(Menu::ViewerType::SizeType idx) const
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
			mnu.Show();
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
Menu::PaintItem(const Graphics& g, const Rect& r, ListType::size_type i)
{
	Color t(GetTextState().Color);

	// TODO: handle highlight text color;

	if(!IsItemEnabled(i))
		GetTextState().Color = FetchGUIShell().Colors[Styles::GrayText];
	DrawText(g, GetTextState(), GetList()[i]);
	GetTextState().Color = t;
	if(r.Width > 16 && mSubMenus.find(i) != mSubMenus.end())
		WndDrawArrow(g, Rect(r.X + r.Width - 16, r.Y, 16, r.Height), 4, RDeg0,
			ForeColor);
}


void
LocateMenu(Menu& d, const Menu& s, Menu::IndexType idx)
{
	d.SetLocation(Point(s.GetLocation().X
		+ s.GetWidth(), s.GetLocation().Y + s.GetItemHeight() * idx));
}


MenuHost::MenuHost(AFrame& frm)
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
	YAssert(val.second, "Null pointer found @ Menu::operator+=;");

	yunsequenced(mMenus[val.first] = val.second,
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
	return std::find_if(mMenus.begin(), mMenus.end(), [&](const ValueType& val){
		return val.second == &mnu;
	}) != mMenus.end();
}

void
MenuHost::Clear()
{
	for(auto i(mMenus.begin()); i != mMenus.end(); ++i)
		delete(i->second);
	mMenus.clear();
}

void
MenuHost::Show(Menu::ID id, ZOrderType z)
{
	try
	{
		auto pMenu(mMenus.at(id));

		YAssert(pMenu, "Null pointer found @ MenuHost::Show;");

		ShowRaw(*pMenu, z);
	}
	catch(std::out_of_range&)
	{}
}

void
MenuHost::ShowAll(ZOrderType z)
{
	for(auto i(mMenus.cbegin()); i != mMenus.cend(); ++i)
		if(i->second)
			ShowRaw(*i->second, z);
}

void
MenuHost::ShowRaw(Menu& mnu, ZOrderType z)
{
	Frame.Add(mnu, z);
	Invalidate(mnu);
	RequestFocus(mnu);
}

void
MenuHost::Hide(Menu::ID id)
{
	try
	{
		auto pMenu(mMenus.at(id));

		YAssert(pMenu, "Null pointer found @ MenuHost::Hide;");

		HideRaw(*pMenu);
	}
	catch(std::out_of_range&)
	{}
}

void
MenuHost::HideAll()
{
	for(auto i(mMenus.cbegin()); i != mMenus.cend(); ++i)
		if(i->second)
			HideRaw(*i->second);
}

void
MenuHost::HideRaw(Menu& mnu)
{
	ReleaseFocus(mnu);
	if(mnu.IsVisible())
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

				YAssert(pMenu, "Null pointer found @ MenuHost::HideUnrelated;");

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

