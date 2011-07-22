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
\version 0.5000;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2011-07-22 10:32 +0800;
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
Contains(const IWidget& wgt, SPos x, SPos y)
{
	return Rect(wgt.GetLocation(), wgt.GetSize()).Contains(x, y);
}

bool
ContainsVisible(const IWidget& wgt, SPos x, SPos y)
{
	return wgt.IsVisible() && Contains(wgt, x, y);
}


void
SetBoundsOf(IWidget& wgt, const Rect& r)
{
	wgt.SetLocation(r.GetPoint());
	wgt.SetSize(r.GetSize());
}

void
Invalidate(IWidget& wgt)
{
	InvalidateCascade(wgt, Rect(Point::Zero, wgt.GetSize()));
}

void
InvalidateCascade(IWidget& wgt, const Rect& r)
{
	auto pWgt(&wgt);
	IWindow* pWnd;
	Rect rect(r);

	while((pWnd = FetchWidgetNodePtr<IWindow>(pWgt, rect)))
	{
		CommitInvalidatedAreaTo(*pWnd, rect);
		rect = FetchInvalidatedArea(*pWnd) + pWgt->GetLocation();
		pWgt = FetchContainerPtr(*pWnd);
	}
}

void
RefreshChild(IWidget& wgt, const Graphics& g, const Point& pt, const Rect& r)
{
	const auto& rect(Intersect(Rect(pt + wgt.GetLocation(), wgt.GetSize()), r));

	if(rect != Rect::Empty)
		wgt.Refresh(g, pt + wgt.GetLocation(), rect);
}

void
RequestToTop(IWidget& wgt)
{
	Desktop* pDsk(FetchDesktopPtr(wgt));

	if(pDsk && pDsk == FetchContainerPtr(wgt))
	{
		IControl* pCon(dynamic_cast<IControl*>(&wgt));

		if(pCon)
			pDsk->MoveToTop(*pCon);
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

Rect
Widget::Refresh(const Graphics& g, const Point& pt, const Rect& r)
{
	if(!IsTransparent())
		Drawing::FillRect(g, r, BackColor);
	return GetBoundsOf(*this);
}

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

