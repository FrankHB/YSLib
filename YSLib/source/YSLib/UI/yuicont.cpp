/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yuicont.cpp
\ingroup UI
\brief 样式无关的图形用户界面容器。
\version r2461;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-01-22 08:03:49 +0800;
\par 修改时间:
	2011-09-17 23:24 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YUIContainer;
*/


#include "ydesktop.h"
#include "yconsole.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

Desktop*
FetchDesktopPtr(IWidget& wgt)
{
	return FetchWidgetNodePtr<Desktop>(&wgt);
}


Point
LocateOffset(const IWidget* pEnd, Point pt, const IWidget* pWgt)
{
	while(pWgt && pWgt != pEnd)
	{
		pt += pWgt->GetLocation();
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
		pt += pCon->GetLocation();
		pCon = FetchContainerPtr(*pCon);
	}
	pCon = &b;
	pt = Point::Zero;
	while(pCon)
	{
		auto i(std::find_if(lst.begin(), lst.end(),
			[&](const pair<IWidget*, Point>& val){
			return val.first == pCon;
		}));

		if(i != lst.cend())
			return pt - i->second;
		pt += pCon->GetLocation();
		pCon = FetchContainerPtr(*pCon);
	}
	return Point::Invalid;
}

Point
LocateForParentContainer(const IWidget& wgt)
{
	return FetchContainerPtr(wgt)
		? LocateContainerOffset(*FetchContainerPtr(wgt),
		wgt.GetLocation()) : Point::Invalid;
}


void
MoveToLeft(IWidget& wgt)
{
	YAssert(FetchContainerPtr(wgt),
		"In function \"void\n"
		"ATrack::MoveToLeft(IWidget&)\": \n"
		"Container pointer of the widget is null.");

	wgt.SetLocation(Point(0, wgt.GetLocation().Y));
}

void
MoveToRight(IWidget& wgt)
{
	YAssert(FetchContainerPtr(wgt),
		"In function \"void\n"
		"ATrack::MoveToLeft(IWidget&)\": \n"
		"Container pointer of the widget is null.");

	wgt.SetLocation(Point(FetchContainerPtr(wgt)->GetSize().Width
		- wgt.GetSize().Width, wgt.GetLocation().Y));
}

void
MoveToTop(IWidget& wgt)
{
	YAssert(FetchContainerPtr(wgt),
		"In function \"void\n"
		"ATrack::MoveToLeft(IWidget&)\": \n"
		"Container pointer of the widget is null.");

	wgt.SetLocation(Point(wgt.GetLocation().X, 0));
}

void
MoveToBottom(IWidget& wgt)
{
	YAssert(FetchContainerPtr(wgt),
		"In function \"void\n"
		"ATrack::MoveToLeft(IWidget&)\": \n"
		"Container pointer of the widget is null.");

	wgt.SetLocation(Point(wgt.GetLocation().X,
		FetchContainerPtr(wgt)->GetSize().Height - wgt.GetSize().Height));
}


bool
MUIContainer::operator-=(IWidget& wgt)
{
	auto t(sWidgets.size());

	for(auto i(sWidgets.begin()); i != sWidgets.end();)
		if(i->second == &wgt)
			sWidgets.erase(i++);
		else
			++i;
	t -= sWidgets.size();

	YAssert(t <= 1, "Duplicate desktop object pointer found"
		" @ bool MUIContainer::operator-=(IWidget&);");

	return t != 0;
}

IWidget*
MUIContainer::GetTopWidgetPtr(const Point& pt, bool(&f)(const IWidget&))
{
	const auto i(std::find_if(sWidgets.rbegin(), sWidgets.rend(),
		[&](const PairType& val){
		YAssert(val.second, "Null widget pointer found @"
			" MUIContainer::GetTopWidgetPtr;");

		return Components::Contains(*val.second, pt) && f(*val.second);
	}));
	return i == sWidgets.rend() ? nullptr : i->second;
}

void
MUIContainer::Add(IWidget& wgt, ZOrderType z)
{
	if(!Contains(wgt))
		sWidgets.insert(make_pair(z, static_cast<ItemType>(&wgt)));
}

bool
MUIContainer::Contains(IWidget& wgt)
{
	return std::find_if(sWidgets.cbegin(), sWidgets.cend(),
		[&](const WidgetMap::value_type& val){
		return val.second == &wgt;
	}) != sWidgets.end();
}

YSL_END_NAMESPACE(Components)

YSL_END

