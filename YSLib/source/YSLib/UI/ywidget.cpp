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
\version r5159;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2011-10-01 18:52 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YWidget;
*/


#include "ydesktop.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

bool
Contains(const IWidget& wgt, SPos x, SPos y)
{
	return GetBoundsOf(wgt).Contains(x, y);
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
		wgt.GetFocusResponder().ClearFocusingPtr();
		CallEvent<LostFocus>(*p, wgt, EventArgs());
	}
}

IWidget*
CheckWidget(IWidget& wgt, const Point& pt, bool(&f)(const IWidget&))
{
	return Contains(wgt, pt) && f(wgt) ? &wgt : nullptr;
}

void
Invalidate(IWidget& wgt)
{
	InvalidateCascade(wgt, Rect(Point::Zero, wgt.GetSize()));
}

void
InvalidateCascade(IWidget& wgt, const Rect& bounds)
{
	auto pWgt(&wgt);
	Rect r(bounds);

	do
	{
		pWgt->GetRenderer().CommitInvalidation(r);
		pWgt->GetRenderer().GetInvalidatedArea(r);
		r += pWgt->GetLocation();
	}while((pWgt = FetchContainerPtr(*pWgt)));
}

void
Render(IWidget& wgt, PaintEventArgs&& e)
{
	Rect r;

	if(wgt.GetRenderer().RequiresRefresh())
	{
		const auto& g_buf(FetchContext(wgt));

		r = g_buf.IsValid() ? wgt.Refresh(PaintEventArgs(g_buf, Point::Zero,
			Rect(e.ClipArea.GetPoint() - wgt.GetLocation(), e.ClipArea)))
			: wgt.Refresh(e);
		wgt.GetRenderer().ClearInvalidation();
	}
	Update(wgt, e);
	e.ClipArea = r;
}

void
RenderChild(IWidget& wgt, PaintEventArgs&& e)
{
	const auto& r(Intersect(Rect(e.Location + wgt.GetLocation(),
		wgt.GetSize()), e.ClipArea));

	if(!r.IsEmptyStrict())
		Render(wgt, PaintEventArgs(e.Target, e.Location + wgt.GetLocation(),
			r));
}

void
RequestToTop(IWidget& wgt)
{
	if(auto pFrm = dynamic_cast<AFrame*>(FetchContainerPtr(wgt)))
		pFrm->MoveToTop(wgt);
}

void
Update(const IWidget& wgt, const PaintEventArgs& e)
{
	if(wgt.IsVisible())
		wgt.GetRenderer().UpdateTo(e);
}

Rect
Validate(IWidget& wgt)
{
	Rect r;

	if(wgt.GetRenderer().RequiresRefresh() && FetchContext(wgt).IsValid())
	{
		r = wgt.Refresh(PaintEventArgs(FetchContext(wgt), Point::Zero,
			Rect(Point::Zero, wgt.GetSize())));
		wgt.GetRenderer().ClearInvalidation();
	}
	return r;
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


inline
WidgetController::WidgetController(bool b)
	: AController(b),
	Paint()
{
	Paint += Render;
}

EventMapping::ItemType&
WidgetController::GetItemRef(const VisualEvent& id)
{
	if(id == Components::Paint)
		return Paint;
	throw BadEvent();
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
	pContainer(), pRenderer(new Renderer()),
	pFocusResponser(new FocusResponder()),
	pController(new WidgetController(false))
{}
Widget::Widget(const Widget& wgt)
	: Visual(wgt),
	pContainer(), pRenderer(wgt.pRenderer->Clone()),
	pFocusResponser(ClonePolymorphic(wgt.pFocusResponser)),
	pController(ClonePolymorphic(wgt.pController))
{}
Widget::~Widget()
{
	ReleaseFocus(*this);
}


void
Widget::SetFocusResponser(unique_ptr<FocusResponder>&& p)
{
	pFocusResponser = p ? std::move(p)
		: unique_ptr<FocusResponder>(new FocusResponder());
	pFocusResponser->ClearFocusingPtr();
}
void
Widget::SetRenderer(unique_ptr<Renderer>&& p)
{
	pRenderer = p ? std::move(p)
		: unique_ptr<Renderer>(new Renderer());
	pRenderer->SetSize(GetSize());
}

Rect
Widget::Refresh(const PaintEventArgs& e)
{
	if(!IsTransparent())
		Drawing::FillRect(e.Target, e.ClipArea, BackColor);
	return Rect(e.Location, GetSize());
}

YSL_END_NAMESPACE(Components)

YSL_END

