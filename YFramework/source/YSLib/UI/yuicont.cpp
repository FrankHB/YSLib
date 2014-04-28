/*
	© 2011-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yuicont.cpp
\ingroup UI
\brief 样式无关的 GUI 容器。
\version r1787
\author FrankHB <frankhb1989@gmail.com>
\since build 188
\par 创建时间:
	2011-01-22 08:03:49 +0800
\par 修改时间:
	2014-04-27 10:58 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YUIContainer
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_YDesktop

using namespace ystdex;

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
LocateForWidget(const IWidget& a, const IWidget& b)
{
	list<pair<const IWidget*, Point>> lst;

	Point pt;
	const IWidget* pCon(&a);

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
	YAssertNonnull(FetchContainerPtr(wgt));

	SetLocationOf(wgt, Point(0, GetLocationOf(wgt).Y));
}

void
MoveToRight(IWidget& wgt)
{
	YAssertNonnull(FetchContainerPtr(wgt));

	SetLocationOf(wgt, Point(GetSizeOf(*FetchContainerPtr(wgt)).Width
		- GetSizeOf(wgt).Width, GetLocationOf(wgt).Y));
}

void
MoveToTop(IWidget& wgt)
{
	YAssertNonnull(FetchContainerPtr(wgt));

	SetLocationOf(wgt, Point(GetLocationOf(wgt).X, 0));
}

void
MoveToBottom(IWidget& wgt)
{
	YAssertNonnull(FetchContainerPtr(wgt));

	SetLocationOf(wgt, Point(GetLocationOf(wgt).X,
		GetSizeOf(*FetchContainerPtr(wgt)).Height - GetSizeOf(wgt).Height));
}


bool
RemoveFrom(IWidget& wgt, IWidget& con)
{
	if(FetchContainerPtr(wgt) == &con)
	{
		SetContainerPtrOf(wgt);
		if(FetchFocusingPtr(con) == &wgt)
			con.GetView().FocusingPtr = {};
		return true;
	}
	return {};
}


void
MLinearUIContainer::operator+=(IWidget& wgt)
{
	if(!Contains(wgt))
		vWidgets.push_back(&wgt);
}

bool
MLinearUIContainer::operator-=(IWidget& wgt)
{
	auto t(vWidgets.size());

	erase_all(vWidgets, &wgt);
	t -= vWidgets.size();

	YAssert(t <= 1, "Duplicate widget pointer found.");

	return t != 0;
}

bool
MLinearUIContainer::Contains(IWidget& wgt) const
{
	return
		std::find(vWidgets.cbegin(), vWidgets.cend(), &wgt) != vWidgets.end();
}

size_t
MLinearUIContainer::Find(IWidget& wgt) const
{
	return
		std::find(vWidgets.cbegin(), vWidgets.cend(), &wgt)- vWidgets.cbegin();
}

MLinearUIContainer::iterator
MLinearUIContainer::begin()
{
	return vWidgets.begin() | get_indirect;
}

MLinearUIContainer::iterator
MLinearUIContainer::end()
{
	return vWidgets.end() | get_indirect;
}


bool
MUIContainer::operator-=(IWidget& wgt)
{
	auto t(mWidgets.size());

	erase_all(mWidgets, mWidgets.begin() | get_value, mWidgets.end()
		| get_value, &wgt);
	t -= mWidgets.size();

	YAssert(t <= 1, "Duplicate widget pointer found.");

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
	return std::find(mWidgets.cbegin() | get_value, mWidgets.cend() | get_value,
		&wgt) != mWidgets.end();
}

void
MUIContainer::PaintVisibleChildren(PaintEventArgs& e)
{
	std::for_each(mWidgets.begin() | get_value, mWidgets.end() | get_value,
		[&](IWidget* const& p_wgt){
		YAssertNonnull(p_wgt);

		auto& wgt(*p_wgt);

		PaintVisibleChild(wgt, e);
	});
}

MUIContainer::iterator
MUIContainer::begin()
{
	return mWidgets.rbegin() | get_value | get_indirect;
}

MUIContainer::iterator
MUIContainer::end()
{
	return mWidgets.rend() | get_value | get_indirect;
}

} // namespace UI;

} // namespace YSLib;

