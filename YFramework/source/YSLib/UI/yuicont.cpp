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
\version r1877
\author FrankHB <frankhb1989@gmail.com>
\since build 188
\par 创建时间:
	2011-01-22 08:03:49 +0800
\par 修改时间:
	2014-10-14 16:56 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YUIContainer
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_YDesktop

using namespace ystdex;
using std::bind;
using namespace std::placeholders;

namespace YSLib
{

namespace UI
{

IWidget&
FetchTopLevel(IWidget& wgt)
{
	auto wgt_ref(std::ref(wgt));

	while(const auto p = FetchContainerPtr(wgt_ref))
		wgt_ref = *p;
	return wgt_ref;
}
IWidget&
FetchTopLevel(IWidget& wgt, Point& pt)
{
	auto wgt_ref(std::ref(wgt));

	while(const auto p = FetchContainerPtr(wgt_ref))
	{
		pt += GetLocationOf(wgt_ref);
		wgt_ref = *p;
	}
	return wgt_ref;
}

vector<pair<const IWidget*, Point>>
FetchTrace(const IWidget& wgt)
{
	vector<pair<const IWidget*, Point>> lst;

	Point pt;
	auto p_wgt(&wgt);

	do
	{
		lst.emplace_back(p_wgt, pt);
		pt += GetLocationOf(*p_wgt);
	}while((p_wgt = FetchContainerPtr(*p_wgt)));
	return lst;
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
LocateForTrace(const vector<pair<const IWidget*, Point>>& lst,
	const IWidget& wgt)
{
	Point pt;
	auto p_wgt(&wgt);

	do
	{
		{
			const auto i(
				std::find(lst.cbegin() | get_key, lst.cend() | get_key, p_wgt));

			if(i != lst.cend())
				return pt - i.get()->second;
		}
		pt += GetLocationOf(*p_wgt);
	}while((p_wgt = FetchContainerPtr(*p_wgt)));
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
		vWidgets.push_back(std::ref(wgt));
}

bool
MLinearUIContainer::operator-=(IWidget& wgt)
{
	auto t(vWidgets.size());

	erase_all_if(vWidgets, bind(is_equal(), _1, std::ref(wgt)));
	t -= vWidgets.size();
	YAssert(t <= 1, "Duplicate widget references found.");
	return t != 0;
}

bool
MLinearUIContainer::Contains(IWidget& wgt) const
{
	return std::count_if(vWidgets.cbegin(), vWidgets.cend(),
		bind(is_equal(), _1, std::ref(wgt))) != 0;
}

size_t
MLinearUIContainer::Find(IWidget& wgt) const
{
	return std::find_if(vWidgets.cbegin(), vWidgets.cend(),
		bind(is_equal(), _1, std::ref(wgt))) - vWidgets.cbegin();
}

MLinearUIContainer::iterator
MLinearUIContainer::begin()
{
	return vWidgets.begin() | get_get;
}

MLinearUIContainer::iterator
MLinearUIContainer::end()
{
	return vWidgets.end() | get_get;
}


bool
MUIContainer::operator-=(IWidget& wgt)
{
	auto t(mWidgets.size());

	erase_all_if(mWidgets, mWidgets.cbegin(), mWidgets.cend(),
		[&](decltype(*mWidgets.cend()) pr){
		return is_equal()(pr.second, wgt);
	});
	t -= mWidgets.size();
	YAssert(t <= 1, "Duplicate widget references found.");
	return t != 0;
}

void
MUIContainer::Add(IWidget& wgt, ZOrderType z)
{
	if(!Contains(wgt))
		mWidgets.emplace(z, std::ref(wgt));
}

bool
MUIContainer::Contains(IWidget& wgt)
{
	return std::count_if(mWidgets.cbegin(), mWidgets.cend(),
		[&](decltype(*mWidgets.cend()) pr){
		return is_equal()(pr.second, wgt);
	}) != 0;
}

void
MUIContainer::PaintVisibleChildren(PaintEventArgs& e)
{
	std::for_each(mWidgets.cbegin() | get_value, mWidgets.cend() | get_value,
		[&](const ItemType& wgt_ref){
		PaintVisibleChildAndCommit(wgt_ref, e);
	});
}

ZOrderType
MUIContainer::QueryZ(IWidget& wgt) const
{
	for(auto& pr : mWidgets)
		if(is_equal()(pr.second, wgt))
			return pr.first;
	throw std::out_of_range("Widget not found.");
}

MUIContainer::iterator
MUIContainer::begin()
{
	return mWidgets.rbegin() | get_value | get_get;
}

MUIContainer::iterator
MUIContainer::end()
{
	return mWidgets.rend() | get_value | get_get;
}

} // namespace UI;

} // namespace YSLib;

