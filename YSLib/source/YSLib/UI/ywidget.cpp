/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywidget.cpp
\ingroup UI
\brief 样式无关的图形用户界面部件。
\version 0.4976;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2011-07-12 10:28 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Widget;
*/


#include "ywidget.h"
#include "ydesktop.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Widgets)

using Controls::Control;

bool
Contains(const IWidget& w, SPos x, SPos y)
{
	return Rect(w.GetLocation(), w.GetSize()).Contains(x, y);
}

bool
ContainsVisible(const IWidget& w, SPos x, SPos y)
{
	return w.IsVisible() && Contains(w, x, y);
}


void
RequestToTop(IWidget& w)
{
	Desktop* pDsk(FetchDirectDesktopPtr(w));

	if(pDsk && pDsk == FetchContainerPtr(w))
	{
		IControl* pCon(dynamic_cast<IControl*>(&w));

		if(pCon)
			pDsk->MoveToTop(*pCon);
	}
}


void
SetBoundsOf(IWidget& w, const Rect& r)
{
	w.SetLocation(r.GetPoint());
	w.SetSize(r.GetSize());
}


void
Invalidate(IWidget& wgt)
{
	Invalidate(wgt, Rect(Point::Zero, wgt.GetSize()));
}
void
Invalidate(IWidget& wgt, const Rect& r)
{
	auto pWgt(&wgt);
	IWindow* pWnd;
	Rect rect(r);

	while((pWnd = FetchWidgetDirectNodePtr<IWindow>(pWgt, rect)))
	{
		CommitInvalidatedAreaTo(*pWnd, rect);
		rect = FetchInvalidatedArea(*pWnd) + pWgt->GetLocation();
		pWgt = FetchContainerPtr(*pWnd);
	}
}


Visual::Visual(const Rect& r, Color b, Color f)
	: visible(true), transparent(false),
	location(r.GetPoint()), size(r.Width, r.Height),
	BackColor(b), ForeColor(f)
{}

void
Visual::SetSize(const Size& s)
{
	size = s;
}


Widget::Widget(const Rect& r, Color b, Color f)
	: Visual(r, b, f),
	pContainer()
{}

void
Widget::Refresh(const Graphics& g, const Point& pt, const Rect&)
{
	if(!IsTransparent())
		FillRect(g, pt, GetSize(), BackColor);
}

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

