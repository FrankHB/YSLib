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
\version r5105;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2011-09-08 02:23 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Widget;
*/


#include "ydesktop.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

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
SetInvalidationOf(IWidget& wgt)
{
	wgt.GetRenderer().CommitInvalidation(Rect(Point::Zero, wgt.GetSize()));
}

void
SetInvalidationToParent(IWidget& wgt)
{
	if(const auto pCon = FetchContainerPtr(wgt))
		pCon->GetRenderer().CommitInvalidation(GetBoundsOf(wgt));
}

void
ClearFocusingPtrOf(IWidget& wgt)
{
	if(const auto p = FetchFocusingPtr(wgt))
	{
		wgt.GetFocusResponser().ClearFocusingPtr();
		CallEvent<LostFocus>(*p, wgt, EventArgs());
	}
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
	Rect rect(r);

	do
	{
		pWgt->GetRenderer().CommitInvalidation(rect);
		pWgt->GetRenderer().GetInvalidatedArea(rect);
		rect += pWgt->GetLocation();
	}while((pWgt = FetchContainerPtr(*pWgt)));
}

Rect
Render(IWidget& wgt, const Graphics& g, const Point& pt, const Rect& r)
{
	Rect rect;

	if(wgt.GetRenderer().RequiresRefresh())
	{
		const auto& g_buf(FetchContext(wgt));

		rect = g_buf.IsValid() ? wgt.Refresh(g_buf, Point::Zero,
			Rect(r.GetPoint() - wgt.GetLocation(), r)) : wgt.Refresh(g, pt, r);
		wgt.GetRenderer().ClearInvalidation();
	}
	Update(wgt, g, pt, r);
	return rect;
}

void
RenderChild(IWidget& wgt, const Graphics& g, const Point& pt, const Rect& r)
{
	const auto& rect(Intersect(Rect(pt + wgt.GetLocation(), wgt.GetSize()), r));

	if(rect != Rect::Empty)
		Render(wgt, g, pt + wgt.GetLocation(), rect);
}

void
RequestToTop(IWidget& wgt)
{
	if(auto pFrm = dynamic_cast<AFrame*>(FetchContainerPtr(wgt)))
		pFrm->MoveToTop(wgt);
}

void
Update(const IWidget& wgt, const Graphics& g, const Point& pt, const Rect& r)
{
	if(wgt.IsVisible())
		wgt.GetRenderer().UpdateTo(g, pt, r);
}

Rect
Validate(IWidget& wgt)
{
	Rect rect;

	if(wgt.GetRenderer().RequiresRefresh() && FetchContext(wgt).IsValid())
	{
		rect = wgt.Refresh(FetchContext(wgt), Point::Zero,
			Rect(Point::Zero, wgt.GetSize()));
		wgt.GetRenderer().ClearInvalidation();
	}
	return rect;
}


void
Show(IWidget& wgt)
{
	wgt.SetVisible(true);
	SetInvalidationToParent(wgt);
}

void
Hide(IWidget& wgt)
{
	wgt.SetVisible(false);
	SetInvalidationToParent(wgt);
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
	pContainer(), pRenderer(new WidgetRenderer()),
	pFocusResponser(new FocusResponser()), pController()
{}
Widget::~Widget()
{}


void
Widget::SetFocusResponser(unique_ptr<FocusResponser>&& p)
{
	pFocusResponser = p ? std::move(p)
		: unique_ptr<FocusResponser>(new FocusResponser());
	pFocusResponser->ClearFocusingPtr();
}
void
Widget::SetRenderer(unique_ptr<WidgetRenderer>&& p)
{
	pRenderer = p ? std::move(p)
		: unique_ptr<WidgetRenderer>(new WidgetRenderer());
	pRenderer->SetSize(GetSize());
}

Rect
Widget::Refresh(const Graphics& g, const Point& pt, const Rect& r)
{
	if(!IsTransparent())
		Drawing::FillRect(g, r, BackColor);
	return GetBoundsOf(*this);
}

YSL_END_NAMESPACE(Components)

YSL_END

