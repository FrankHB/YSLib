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
\version r5078;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2011-08-13 06:27 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Widget;
*/


#include "ydesktop.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

const Graphics WidgetRenderer::InvalidGraphics;


bool
BufferedWidgetRenderer::RequiresRefresh() const
{
	return !rInvalidated.IsEmpty();
}

void
BufferedWidgetRenderer::GetInvalidatedArea(Rect& r) const
{
	r = rInvalidated;
}

void
BufferedWidgetRenderer::SetSize(const Size& s)
{
	Buffer.SetSize(s.Width, s.Height);
	static_cast<Size&>(rInvalidated) = s;
}

void
BufferedWidgetRenderer::ClearInvalidation()
{
	//只设置一个分量为零可能会使 CommitInvalidation 结果错误。
	static_cast<Size&>(rInvalidated) = Size::Zero;
}

void
BufferedWidgetRenderer::CommitInvalidation(const Rect& r)
{
	rInvalidated = Unite(rInvalidated, r);
}

void
BufferedWidgetRenderer::FillInvalidation(Color c)
{
	FillRect(Buffer, rInvalidated, c);
}

void
BufferedWidgetRenderer::UpdateTo(const Graphics& g, const Point& pt,
	const Rect& r) const
{
	CopyTo(g.GetBufferPtr(), GetContext(), g.GetSize(),
		r, static_cast<const Point&>(r) - pt, r);
}

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
SetInvalidationOf(IWidget& wgt)
{
	wgt.GetRenderer().CommitInvalidation(Rect(Point::Zero, wgt.GetSize()));
}

void
SetInvalidationToParent(IWidget& wgt)
{
	auto pCon(FetchContainerPtr(wgt));

	if(pCon)
		pCon->GetRenderer().CommitInvalidation(Rect(wgt.GetLocation(),
			wgt.GetSize()));
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
	Desktop* pDsk(FetchDesktopPtr(wgt));

	if(pDsk && pDsk == FetchContainerPtr(wgt))
	{
		IControl* pCon(dynamic_cast<IControl*>(&wgt));

		if(pCon)
			pDsk->MoveToTop(*pCon);
	}
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
	pContainer(), pRenderer(new WidgetRenderer())
{}

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

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

