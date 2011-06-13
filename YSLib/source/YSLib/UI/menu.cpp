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
\version 0.1658;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-06-02 12:20:10 +0800;
\par 修改时间:
	2011-06-10 17:03 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Menu;
*/


#include "menu.h"
#include "ywindow.h"

YSL_BEGIN

using namespace Drawing;
using namespace Runtime;

YSL_BEGIN_NAMESPACE(Components)

using namespace Forms;
using namespace Widgets;

YSL_BEGIN_NAMESPACE(Controls)

Menu::Menu(const Rect& r, const shared_ptr<ListType>& h, ID id)
	: TextList(r, h, FetchGUIShell().Colors.GetPair(Styles::Highlight,
		Styles::HighlightText)),
	id(id), pParent(nullptr), mSubMenus()
{
	BackColor = FetchGUIShell().Colors[Styles::Panel];
	SetAllTo(Margin, 6, 18, 4, 4);
	FetchEvent<LostFocus>(*this) += [this](IControl&, EventArgs&&){
		if(pHost)
		{
			if(this->IsVisible())
				this->Refresh();
			if(!pHost->SubMenuPointer || pHost->SubMenuPointer->pParent != this)
			{
				pHost->HideAll();
				pHost->SubMenuPointer = nullptr;
			}
		}
	};
	GetConfirmed() += [this](IControl&, IndexEventArgs&& e){
		if(this->Contains(e) && pHost)
		{
			try
			{
				Menu& mnu((*this)[e.Index]);

				pHost->SubMenuPointer = &mnu;
				mnu.Show();
			}
			catch(std::out_of_range&)
			{
				pHost->Hide(this->id);
			}
		}
	};
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
Menu::Show(ZOrderType z)
{
	if(pHost)
	{
		pHost->Show(id, z);
		return true;
	}
	return false;
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
	DrawText(g, GetTextState(), GetList()[i]);

	if(r.Width > 16 && mSubMenus.find(i) != mSubMenus.end())
	{
		const Rect arrow_bounds(r.X + r.Width - 16, r.Y, 16, r.Height);

		WndDrawArrow(g, arrow_bounds, 4, RDeg0, ForeColor);
	}
}


MenuHost::MenuHost(AFrame& frm)
	: Frame(frm), SubMenuPointer(), mMenus()
{}
MenuHost::~MenuHost()
{
	HideAll();
	for(auto i(mMenus.cbegin()); i != mMenus.cend(); ++i)
		ydelete(i->second);
}

void
MenuHost::operator+=(const ValueType& val)
{
	YAssert(val.second, "Null pointer found @ Menu::operator+=;");

	mMenus[val.first] = val.second;
	val.second->id = val.first;
	val.second->pHost = this;
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
		Menu& mnu((*this)[id]);

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
		Menu& mnu((*this)[id]);

		return Frame.Contains(mnu);
	}
	catch(std::out_of_range&)
	{}
	return false;
}

bool
MenuHost::Contains(Menu& mnu)
{
	return std::find_if(mMenus.cbegin(), mMenus.cend(),
		[&](const ValueType& val){
		return val.second == &mnu;
	}) != mMenus.end();
}

void
MenuHost::Clear()
{
	for(auto i(mMenus.begin()); i != mMenus.end(); ++i)
		i->second->pHost = nullptr;
	mMenus.clear();
}

void
MenuHost::Show(Menu::ID id, ZOrderType z)
{
	try
	{
		Menu* pMenu(mMenus.at(id));

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
	mnu.RequestFocus();
}

void
MenuHost::Hide(Menu::ID id)
{
	try
	{
		Menu* pMenu(mMenus.at(id));

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
	mnu.ReleaseFocus();
	Frame -= mnu;
}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

