/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yuicont.cpp
\ingroup UI
\brief 样式无关的图形用户界面容器。
\version r1594
\author FrankHB<frankhb1989@gmail.com>
\since build 188
\par 创建时间:
	2011-01-22 08:03:49 +0800
\par 修改时间:
	2012-09-04 12:46 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YUIContainer
*/


#include "YSLib/UI/ydesktop.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

IWidget&
FetchTopLevel(IWidget& wgt)
{
	auto pWgt(&wgt);

	while(const auto p = FetchContainerPtr(*pWgt))
		pWgt = p;
	return *pWgt;
}
IWidget&
FetchTopLevel(IWidget& wgt, Point& pt)
{
	auto pWgt(&wgt);

	while(const auto p = FetchContainerPtr(*pWgt))
	{
		pt += GetLocationOf(*pWgt);
		pWgt = p;
	}
	return *pWgt;
}


Point
LocateOffset(const IWidget* pEnd, Point pt, const IWidget* pWgt)
{
	while(pWgt && pWgt != pEnd)
	{
		pt += GetLocationOf(*pWgt);
		pWgt = FetchContainerPtr(*pWgt);
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
	pt = Point::Zero;
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

IWidget*
MUIContainer::GetTopWidgetPtr(const Point& pt, bool(&f)(const IWidget&))
{
	using ystdex::get_value;

	const auto i(std::find_if(mWidgets.rbegin() | get_value,
		mWidgets.rend() | get_value, [&](const ItemType& pWgt){
		YAssert(pWgt, "Null pointer found.");

		return Components::Contains(*pWgt, pt) && f(*pWgt);
	}));
	return i == mWidgets.rend() ? nullptr : *i;
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
		[&](IWidget* const& pWgt){
		YAssert(pWgt, "Null pointer found.");

		auto& wgt(*pWgt);

		if(Components::IsVisible(wgt))
			e.ClipArea |= PaintChild(wgt, e);
	});
}

YSL_END_NAMESPACE(Components)

YSL_END

