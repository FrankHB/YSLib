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
\version 0.2314;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-01-22 08:03:49 +0800;
\par 修改时间:
	2011-06-17 00:03 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::UIContainer;
*/


#include "yuicont.h"
#include "yconsole.h"
#include "ywindow.h"
#include "ydesktop.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

#ifdef YCL_USE_YASSERT

void
yassert(bool exp, const char* msg, int line, const char* file,
	const char* comp, const char* func)
{
	if(!exp)
	{
		Console dbg;

		iprintf(
			"At line %i in file %s: \n"
			"An error occured in precedure %s of \n"
			"Components::%s:\n"
			"%s", line, file, func, comp, msg);
		dbg.Pause();
	}
}

#endif

YSL_BEGIN_NAMESPACE(Widgets)

IWindow*
FetchWindowPtr(const IWidget& w)
{
	return FetchWidgetDirectNodePtr<IWindow>(w.GetContainerPtr());
}

IUIBox*
FetchDirectContainerPtr(IWidget& w)
{
	IUIBox* const pCon(dynamic_cast<IUIBox*>(&w));

	return pCon ? pCon : w.GetContainerPtr();
}

IWindow*
FetchDirectWindowPtr(IWidget& w)
{
	return FetchWidgetDirectNodePtr<IWindow>(FetchDirectContainerPtr(w));
}

Desktop*
FetchDirectDesktopPtr(IWidget& w)
{
	return FetchWidgetDirectNodePtr<Desktop>(FetchDirectContainerPtr(w));
}

const Graphics&
FetchContext(IWidget& w)
{
	IWindow* const pWnd(FetchDirectWindowPtr(w));

	if(!pWnd)
		throw GeneralEvent("Null direct window pointer found @ FetchContext");
	return pWnd->GetContext();
}


Point
LocateOffset(const IWindow* pWnd, Point pt, const IWidget* pCon)
{
	while(pCon && dynamic_cast<const IWindow*>(pCon) != pWnd)
	{
		pt += pCon->GetLocation();
		pCon = dynamic_cast<const IWidget*>(pCon->GetContainerPtr());
	}
	return pt;
}

Point
LocateForWidget(IWidget& a, IWidget& b)
{
	list<pair<IUIBox*, Point>> lst;

	Point pt;
	IUIBox* pCon(dynamic_cast<IUIBox*>(&a));

	if(!pCon)
	{
		pCon = a.GetContainerPtr();
		pt = a.GetLocation();
	}
	while(pCon)
	{
		lst.push_back(make_pair(pCon, pt));
		pt += pCon->GetLocation();
		pCon = pCon->GetContainerPtr();
	}
	if((pCon = dynamic_cast<IUIBox*>(&b)))
		pt = Point::Zero;
	else
	{
		pCon = b.GetContainerPtr();
		pt = b.GetLocation();
	}
	while(pCon)
	{
		auto i(std::find_if(lst.begin(), lst.end(),
			[&](const pair<IUIBox*, Point>& val){
			return val.first == pCon;
		}));

		if(i != lst.cend())
			return pt - i->second;
		pt += pCon->GetLocation();
		pCon = pCon->GetContainerPtr();
	}
	return Point::FullScreen;
}

Point
LocateForWindow(IWidget& w)
{
	if(dynamic_cast<IWindow*>(&w))
		return Point::Zero;

	IWindow* const pWnd(FetchDirectWindowPtr(w));

	return pWnd ? LocateOffset(pWnd, Point::Zero, &w) : Point::FullScreen;
}

Point
LocateForDesktop(IWidget& w)
{
	if(dynamic_cast<Desktop*>(&w))
		return Point::Zero;

	Desktop* pDsk(FetchDirectDesktopPtr(w));

	return pDsk ? LocateOffset(pDsk, Point::Zero, &w) : Point::FullScreen;
}

Point
LocateForParentContainer(const IWidget& w)
{
	return w.GetContainerPtr()
		? LocateContainerOffset(*w.GetContainerPtr(),
		w.GetLocation()) : Point::FullScreen;
}

Point
LocateForParentWindow(const IWidget& w)
{
	const IWindow* const pWnd(FetchWindowPtr(w));

	return pWnd ? LocateWindowOffset(*pWnd, w.GetLocation())
		: Point::FullScreen;
}


void
MoveToLeft(IWidget& w)
{
	YAssert(w.GetContainerPtr(),
		"In function \"void\n"
		"ATrack::MoveToLeft(IWidget& w)\": \n"
		"Container pointer of the widget is null.");

	w.SetLocation(Point(0, w.GetLocation().Y));
}

void
MoveToRight(IWidget& w)
{
	YAssert(w.GetContainerPtr(),
		"In function \"void\n"
		"ATrack::MoveToLeft(IWidget& w)\": \n"
		"Container pointer of the widget is null.");

	w.SetLocation(Point(w.GetContainerPtr()->GetSize().Width
		- w.GetSize().Width, w.GetLocation().Y));
}

void
MoveToTop(IWidget& w)
{
	YAssert(w.GetContainerPtr(),
		"In function \"void\n"
		"ATrack::MoveToLeft(IWidget& w)\": \n"
		"Container pointer of the widget is null.");

	w.SetLocation(Point(w.GetLocation().X, 0));
}

void
MoveToBottom(IWidget& w)
{
	YAssert(w.GetContainerPtr(),
		"In function \"void\n"
		"ATrack::MoveToLeft(IWidget& w)\": \n"
		"Container pointer of the widget is null.");

	w.SetLocation(Point(w.GetLocation().X,
		w.GetContainerPtr()->GetSize().Height - w.GetSize().Height));
}


void
Fill(IWidget& w, Color c)
{
	IWindow* pWnd(FetchDirectWindowPtr(w));

	if(pWnd)
	{
		const Graphics& g(pWnd->GetContext());

		FillRect(g, LocateOffset(pWnd, Point::Zero, &w), w.GetSize(), c);
	}
}


void
MUIContainer::operator+=(IWidget& wgt)
{
	if(!Contains(wgt))
		sWidgets.insert(make_pair(DefaultZOrder, &wgt));
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
bool
MUIContainer::operator-=(IControl& ctl)
{
	return GMFocusResponser<IControl>::operator-=(ctl)
		&& (*this -= static_cast<IWidget&>(ctl));
}
bool
MUIContainer::operator-=(GMFocusResponser<IControl>& rsp)
{
	return sFocusContainers.erase(&rsp) != 0;
}

IControl*
MUIContainer::GetFocusingPtr() const
{
	return GMFocusResponser<IControl>::GetFocusingPtr();
}
IWidget*
MUIContainer::GetTopWidgetPtr(const Point& pt)
{
	for(auto i(sWidgets.cbegin()); i != sWidgets.cend(); ++i)
		if(i->second->IsVisible() && Widgets::Contains(*i->second, pt))
			return i->second;
	return nullptr;
}
IControl*
MUIContainer::GetTopControlPtr(const Point& pt)
{
	for(auto i(sFocusObjects.cbegin()); i != sFocusObjects.cend(); ++i)
		if((*i)->IsVisible() && Widgets::Contains(**i, pt))
			return *i;
	return nullptr;
}

void
MUIContainer::Add(IControl& ctl, ZOrderType z)
{
	if(!Contains(ctl))
	{
		sWidgets.insert(make_pair(z, static_cast<ItemType>(&ctl)));
		GMFocusResponser<IControl>::operator+=(ctl);
	}
}

bool
MUIContainer::ResponseFocusRequest(AFocusRequester& req)
{
	return req.RequestFocus<GMFocusResponser, IControl>(*this);
}

bool
MUIContainer::ResponseFocusRelease(AFocusRequester& req)
{
	return req.ReleaseFocus<GMFocusResponser, IControl>(*this);
}

bool
MUIContainer::Contains(IWidget& wgt)
{
	return std::find_if(sWidgets.cbegin(), sWidgets.cend(),
		[&](const WidgetMap::value_type& val){
		return val.second == &wgt;
	}) != sWidgets.end();
}


UIContainer::UIContainer(const Rect& r)
	: Widget(r), MUIContainer()
{}

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

