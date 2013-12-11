/*
	© 2011-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yuicont.cpp
\ingroup UI
\brief 样式无关的 GUI 容器。
\version r1716
\author FrankHB <frankhb1989@gmail.com>
\since build 188
\par 创建时间:
	2011-01-22 08:03:49 +0800
\par 修改时间:
	2013-12-08 23:29 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YUIContainer
*/


#include "YSLib/UI/ydesktop.h"

namespace YSLib
{

namespace UI
{

IWidget&
FetchTopLevel(IWidget& wgt)
{
	auto p_wgt(&wgt);

	while(const auto p = FetchContainerPtr(*p_wgt))
		p_wgt = p;
	return *p_wgt;
}
IWidget&
FetchTopLevel(IWidget& wgt, Point& pt)
{
	auto p_wgt(&wgt);

	while(const auto p = FetchContainerPtr(*p_wgt))
	{
		pt += GetLocationOf(*p_wgt);
		p_wgt = p;
	}
	return *p_wgt;
}


Point
LocateOffset(const IWidget* p_end, Point pt, const IWidget* p_wgt)
{
	while(p_wgt && p_wgt != p_end)
	{
		pt += GetLocationOf(*p_wgt);
		p_wgt = FetchContainerPtr(*p_wgt);
	}
	return pt;
}

Point
LocateForWidget(IWidget& a, IWidget& b)
{
	list<pair<IWidget*, Point>> lst;

	Point pt;
	IWidget* pCon(&a);

	while(pCon)
	{
		lst.push_back(make_pair(pCon, pt));
		pt += GetLocationOf(*pCon);
		pCon = FetchContainerPtr(*pCon);
	}
	pCon = &b;
	pt = {};
	while(pCon)
	{
		{
			using ystdex::get_key;

			auto i(std::find(lst.begin() | get_key, lst.end() | get_key, pCon));

			if(i != lst.cend())
				return pt - i.get()->second;
		}
		pt += GetLocationOf(*pCon);
		pCon = FetchContainerPtr(*pCon);
	}
	return Point::Invalid;
}

Point
LocateForParentContainer(const IWidget& wgt)
{
	return FetchContainerPtr(wgt)
		? LocateContainerOffset(*FetchContainerPtr(wgt), GetLocationOf(wgt))
		: Point::Invalid;
}


void
MoveToLeft(IWidget& wgt)
{
	YAssert(FetchContainerPtr(wgt), "Null pointer found.");

	SetLocationOf(wgt, Point(0, GetLocationOf(wgt).Y));
}

void
MoveToRight(IWidget& wgt)
{
	YAssert(FetchContainerPtr(wgt), "Null pointer found.");

	SetLocationOf(wgt, Point(GetSizeOf(*FetchContainerPtr(wgt)).Width
		- GetSizeOf(wgt).Width, GetLocationOf(wgt).Y));
}

void
MoveToTop(IWidget& wgt)
{
	YAssert(FetchContainerPtr(wgt), "Null pointer found.");

	SetLocationOf(wgt, Point(GetLocationOf(wgt).X, 0));
}

void
MoveToBottom(IWidget& wgt)
{
	YAssert(FetchContainerPtr(wgt), "Null pointer found.");

	SetLocationOf(wgt, Point(GetLocationOf(wgt).X,
		GetSizeOf(*FetchContainerPtr(wgt)).Height - GetSizeOf(wgt).Height));
}


bool
MUIContainer::operator-=(IWidget& wgt)
{
	using namespace ystdex;

	auto t(mWidgets.size());

	erase_all(mWidgets, mWidgets.begin() | get_value, mWidgets.end()
		| get_value, &wgt);
	t -= mWidgets.size();

	YAssert(t <= 1, "Duplicate desktop object pointer found.");

	return t != 0;
}

void
MUIContainer::Add(IWidget& wgt, ZOrderType z)
{
	if(!Contains(wgt))
		mWidgets.insert(make_pair(z, ItemType(&wgt)));
}

bool
MUIContainer::Contains(IWidget& wgt)
{
	using ystdex::get_value;

	return std::find(mWidgets.cbegin() | get_value, mWidgets.cend() | get_value,
		&wgt) != mWidgets.end();
}

void
MUIContainer::PaintVisibleChildren(PaintEventArgs& e)
{
	using ystdex::get_value;

	std::for_each(mWidgets.begin() | get_value, mWidgets.end() | get_value,
		[&](IWidget* const& p_wgt){
		YAssert(p_wgt, "Null pointer found.");

		auto& wgt(*p_wgt);

		if(UI::IsVisible(wgt))
			e.ClipArea |= PaintChild(wgt, e);
	});
}

MUIContainer::iterator
MUIContainer::begin()
{
	using namespace ystdex;

	return mWidgets.rbegin() | get_value | get_indirect;
}

MUIContainer::iterator
MUIContainer::end()
{
	using namespace ystdex;

	return mWidgets.rend() | get_value | get_indirect;
}

} // namespace UI;

} // namespace YSLib;

