/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yuicont.cpp
\ingroup Shell
\brief 样式无关的图形用户界面容器。
\version 0.2189;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-01-22 08:03:49 +0800;
\par 修改时间:
	2011-04-28 17:22 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::UIContainer;
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


namespace
{
	void
	GetContainersListFrom(IWidget& w, map<IUIBox*, Point>& m)
	{
		Point pt;
		IUIBox* pCon(dynamic_cast<IUIBox*>(&w));

		if(!pCon)
		{
			pCon = w.GetContainerPtr();
			pt = w.GetLocation();
		}
		while(pCon != nullptr)
		{
			m.insert(make_pair(pCon, pt));
			pt += pCon->GetLocation();
			pCon = pCon->GetContainerPtr();
		}
	}
}

Point
LocateOffset(const IWidget* pCon, Point p, const IWindow* pWnd)
{
	while(pCon && dynamic_cast<const IWindow*>(pCon) != pWnd)
	{
		p += pCon->GetLocation();
		pCon = dynamic_cast<const IWidget*>(pCon->GetContainerPtr());
	}
	return p;
}

Point
LocateForWidget(IWidget& a, IWidget& b)
{
	map<IUIBox*, Point> m;

	GetContainersListFrom(b, m);

	Point pt;
	IUIBox* pCon(dynamic_cast<IUIBox*>(&a));

	if(!pCon)
	{
		pCon = a.GetContainerPtr();
		pt = a.GetLocation();
	}

	while(pCon != nullptr)
	{
		map<IUIBox*, Point>::const_iterator i(m.find(pCon));

		if(i != m.end())
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

	return pWnd ? LocateOffset(&w, Point::Zero, pWnd) : Point::FullScreen;
}

Point
LocateForDesktop(IWidget& w)
{
	if(dynamic_cast<Desktop*>(&w))
		return Point::Zero;

	Desktop* pDsk(FetchDirectDesktopPtr(w));

	return pDsk ? LocateOffset(&w, Point::Zero, pDsk) : Point::FullScreen;
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

		FillRect(g, LocateOffset(&w, Point::Zero, pWnd), w.GetSize(), c);
	}
}


MUIContainer::MUIContainer()
	: GMFocusResponser<IControl>(),
	WidgetsList(), sFOCSet()
{}

void
MUIContainer::operator+=(IWidget* p)
{
	if(CheckWidget(p))
		WidgetsList.push_back(p);
}
void
MUIContainer::operator+=(IControl* p)
{
	if(CheckWidget(p))
	{
		WidgetsList.push_back(p);
		GMFocusResponser<IControl>::operator+=(*p);
	}
}
void
MUIContainer::operator+=(GMFocusResponser<IControl>* p)
{
	if(p)
		sFOCSet.insert(p);
}

bool
MUIContainer::operator-=(IWidget* p)
{
	return RemoveWidget(p);
}
bool
MUIContainer::operator-=(IControl* p)
{
	return p ? GMFocusResponser<IControl>::operator-=(*p)
		&& RemoveWidget(p) : false;
}
bool
MUIContainer::operator-=(GMFocusResponser<IControl>* p)
{
	return sFOCSet.erase(p) != 0;
}

IControl*
MUIContainer::GetFocusingPtr() const
{
	return GMFocusResponser<IControl>::GetFocusingPtr();
}
IWidget*
MUIContainer::GetTopWidgetPtr(const Point& pt)
{
	for(WGTs::const_iterator i(WidgetsList.begin()); i != WidgetsList.end();
		++i)
		if((*i)->IsVisible() && Contains(**i, pt))
			return *i;
	return nullptr;
}
IControl*
MUIContainer::GetTopControlPtr(const Point& pt)
{
	for(FOs::const_iterator i(sFOs.begin()); i != sFOs.end(); ++i)
		if((*i)->IsVisible() && Contains(**i, pt))
			return *i;
	return nullptr;
}

bool
MUIContainer::ResponseFocusRequest(AFocusRequester& w)
{
	return w.RequestFocus<GMFocusResponser, IControl>(*this);
}

bool
MUIContainer::ResponseFocusRelease(AFocusRequester& w)
{
	return w.ReleaseFocus<GMFocusResponser, IControl>(*this);
}

bool
MUIContainer::CheckWidget(IWidget* p)
{
	return p ? std::find(WidgetsList.begin(), WidgetsList.end(), p)
		== WidgetsList.end() : false;
}

bool
MUIContainer::RemoveWidget(IWidget* p)
{
	//WidgetsList.erase(std::remove(WidgetsList.begin(),
	//	WidgetsList.end(), p), WidgetsList.end());
	const WGTs::size_type s(WidgetsList.size());

	WidgetsList.erase(std::remove(WidgetsList.begin(), WidgetsList.end(), p),
		WidgetsList.end());

	const std::ptrdiff_t t(s - WidgetsList.size());

	YAssert(t <= 1, "Duplicate desktop object pointer found"
		" @ bool MUIContainer::operator-=(IWidget*);");

	return t != 0;
}


UIContainer::UIContainer(const Rect& r)
	: Widget(r), MUIContainer()
{}

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

