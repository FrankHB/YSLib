/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywidget.cpp
\ingroup UI
\brief 样式无关的图形用户界面部件。
\version r5407;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2012-03-18 16:35 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::UI::YWidget;
*/


#include "YSLib/UI/ydesktop.h"
#include "YSLib/Service/ydraw.h"

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
	return IsVisible(wgt) && Contains(wgt, x, y);
}


void
SetBoundsOf(IWidget& wgt, const Rect& r)
{
	SetLocationOf(wgt, r.GetPoint()),
	SetSizeOf(wgt, r.GetSize());
}

void
SetInvalidationOf(IWidget& wgt)
{
	wgt.GetRenderer().CommitInvalidation(Rect(Point::Zero, GetSizeOf(wgt)));
}

void
SetInvalidationToParent(IWidget& wgt)
{
	if(const auto pCon = FetchContainerPtr(wgt))
		pCon->GetRenderer().CommitInvalidation(GetBoundsOf(wgt));
}

void
SetLocationOf(IWidget& wgt, const Point& pt)
{
	wgt.GetView().SetLocation(pt);
	CallEvent<Move>(wgt, UIEventArgs(wgt));
}

void
SetSizeOf(IWidget& wgt, const Size& s)
{
	wgt.GetRenderer().SetSize(s);
	wgt.GetView().SetSize(s);
	CallEvent<Resize>(wgt, UIEventArgs(wgt));
}


IWidget*
CheckWidget(IWidget& wgt, const Point& pt, bool(&f)(const IWidget&))
{
	return Contains(wgt, pt) && f(wgt) ? &wgt : nullptr;
}

void
Close(IWidget& wgt)
{
	Hide(wgt);
	if(const auto pCon = FetchContainerPtr(wgt))
		ClearFocusingOf(*pCon);
}

void
Hide(IWidget& wgt)
{
	SetVisibleOf(wgt, false);
	ReleaseFocus(wgt);
	Invalidate(wgt);
}

void
Invalidate(IWidget& wgt)
{
	Invalidate(wgt, Rect(Point::Zero, GetSizeOf(wgt)));
}
void
Invalidate(IWidget& wgt, const Rect& bounds)
{
	auto pWgt(&wgt);
	Rect r(bounds);

	do
	{
		r = pWgt->GetRenderer().CommitInvalidation(r);
		r += GetLocationOf(*pWgt);
	}while((pWgt = FetchContainerPtr(*pWgt)));
}

void
PaintChild(IWidget& wgt, PaintEventArgs&& e)
{
	auto& sender(e.GetSender());

	e.Location += GetLocationOf(sender);
	e.ClipArea = Intersect(e.ClipArea, Rect(e.Location, GetSizeOf(sender)));
	if(!e.ClipArea.IsUnstrictlyEmpty())
		wgt.GetRenderer().Paint(sender, std::move(e));
}
Rect
PaintChild(IWidget& wgt, const PaintContext& pc)
{
	PaintEventArgs e(wgt, pc);

	PaintChild(wgt, std::move(e));
	return e.ClipArea;
}

void
RequestToTop(IWidget& wgt)
{
	if(auto pFrm = dynamic_cast<Panel*>(FetchContainerPtr(wgt)))
		pFrm->MoveToTop(wgt);
}

void
Show(IWidget& wgt)
{
	SetVisibleOf(wgt, true);
	RequestFocus(wgt);
	Invalidate(wgt);
}


Widget::Widget(const Rect& r, Color b, Color f)
	: pView(new View(r)), pRenderer(new Renderer()),
	pController(new WidgetController(false)),
	Background(SolidBrush(b)), ForeColor(f)
{
	InitializeEvents();
}
Widget::Widget(const Widget& wgt)
	: pView(ClonePolymorphic(wgt.pView)),
	pRenderer(ClonePolymorphic(wgt.pRenderer)),
	pController(ClonePolymorphic(wgt.pController)),
	Background(wgt.Background), ForeColor(wgt.ForeColor)
{}
Widget::~Widget()
{
	// TODO: use ReleaseFocus without call event;
	if(auto p = FetchContainerPtr(*this))
	{
		auto& pFocusing(p->GetView().pFocusing);

		if(pFocusing == this)
			pFocusing = nullptr;
	}
//	ReleaseFocus(*this);
}

void
Widget::InitializeEvents()
{
	(FetchEvent<Paint>(*this).Add(std::ref(Background), BackgroundPriority))
		+= std::bind(&Widget::Refresh, this, std::placeholders::_1);
}

AController&
Widget::GetController() const
{
	if(!pController)
		throw BadEvent();
	return *pController;
}

void
Widget::SetRenderer(unique_ptr<Renderer>&& p)
{
	pRenderer = p ? std::move(p)
		: unique_ptr<Renderer>(new Renderer());
	pRenderer->SetSize(GetSizeOf(*this));
}
void
Widget::SetView(unique_ptr<View>&& p)
{
	pView = p ? std::move(p)
		: unique_ptr<View>(new View(GetBoundsOf(*this)));
}

void
Widget::Refresh(PaintEventArgs&&)
{}

YSL_END_NAMESPACE(Components)

YSL_END

