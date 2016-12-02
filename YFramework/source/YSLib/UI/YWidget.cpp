/*
	© 2009-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YWidget.cpp
\ingroup UI
\brief 样式无关的 GUI 部件。
\version r4490
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2016-12-01 22:05 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YWidget
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_YDesktop
#include YFM_YSLib_Service_YBrush
#include YFM_YSLib_UI_YGUI

namespace YSLib
{

namespace UI
{

ImplDeDtor(IWidget)


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
	wgt.GetRenderer().CommitInvalidation(Rect(GetSizeOf(wgt)));
}

void
SetInvalidationToParent(IWidget& wgt)
{
	if(const auto p_con = FetchContainerPtr(wgt))
		p_con->GetRenderer().CommitInvalidation(GetBoundsOf(wgt));
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


void
Close(IWidget& wgt)
{
	Hide(wgt);
	if(const auto p_con = FetchContainerPtr(wgt))
		ClearFocusingOf(*p_con);
}

void
Hide(IWidget& wgt)
{
	SetVisibleOf(wgt, false);
	ReleaseFocus(wgt);
	Invalidate(wgt);
}

void
Invalidate(IWidget& wgt, const Rect& bounds)
{
	Rect r(bounds);

	for(auto p_wgt(make_observer(&wgt)); p_wgt;
		p_wgt = FetchContainerPtr(*p_wgt))
	{
		r = p_wgt->GetRenderer().CommitInvalidation(r);
		r.GetPointRef() += GetLocationOf(*p_wgt);
	}
}

void
InvalidateAll(IWidget& wgt, const Rect& bounds)
{
	InvalidateChildren(wgt, bounds);
	Invalidate(wgt, bounds);
}

void
InvalidateChildren(IWidget& wgt, const Rect& bounds)
{
	Rect r(wgt.GetRenderer().CommitInvalidation(bounds));

	for(auto pr(wgt.GetChildren()); pr.first != pr.second; ++pr.first)
	{
		auto& child(*pr.first);

		InvalidateChildren(child, Rect(r - GetLocationOf(child)));
	}
}

void
InvalidateParent(IWidget& wgt)
{
	if(const auto p_con = FetchContainerPtr(wgt))
		Invalidate(*p_con, GetBoundsOf(wgt));
}

void
InvalidateVisible(IWidget& wgt, const Rect& bounds)
{
	auto p_wgt(make_observer(&wgt));
	Rect r(bounds);

	while(IsVisible(*p_wgt))
	{
		r = p_wgt->GetRenderer().CommitInvalidation(r);
		r.GetPointRef() += GetLocationOf(*p_wgt);
		if(!(p_wgt = FetchContainerPtr(*p_wgt)))
			break;
	}
}

void
InvalidateVisibleParent(IWidget& wgt)
{
	if(IsVisible(wgt))
		if(const auto p_con = FetchContainerPtr(wgt))
			InvalidateVisible(*p_con, GetBoundsOf(wgt));
}

void
PaintChild(IWidget& wgt, PaintEventArgs&& e)
{
	auto& sender(e.GetSender());

	if(Clip(e.ClipArea, Rect(e.Location += GetLocationOf(sender),
		GetSizeOf(sender))))
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
PaintChildAndCommit(IWidget& wgt, PaintEventArgs& e)
{
	e.ClipArea |= PaintChild(wgt, e);
}

void
RequestToFront(IWidget& wgt)
{
	if(const auto p_pnl = dynamic_cast<Panel*>(FetchContainerPtr(wgt).get()))
		p_pnl->MoveToFront(wgt);
}

void
Show(IWidget& wgt)
{
	SetVisibleOf(wgt, true);
	RequestFocus(wgt);
	Invalidate(wgt);
}


Widget::Widget(const Rect& r)
	: view_ptr(new View(r)), renderer_ptr(new Renderer()),
	controller_ptr(new WidgetController({})), Background()
{
	InitializeEvents();
}
Widget::Widget(const Rect& r, HBrush b)
	// XXX: $2015-03 @ %Documentation::Workflow::Annual2015.
	: IWidget(), view_ptr(new View(r)), renderer_ptr(new Renderer()),
	controller_ptr(new WidgetController({})), Background(b)
{
	InitializeEvents();
}
Widget::Widget(const Widget& wgt)
	// XXX: $2015-03 @ %Documentation::Workflow::Annual2015.
	: IWidget(), view_ptr(ystdex::clone_polymorphic(Deref(wgt.view_ptr))),
	renderer_ptr(ystdex::clone_polymorphic(Deref(wgt.renderer_ptr))),
	controller_ptr(ystdex::clone_polymorphic(Deref(wgt.controller_ptr))),
	Background(wgt.Background)
{}
Widget::~Widget()
{
	FetchGUIState().CleanupReferences(*this);
}

void
Widget::InitializeEvents()
{
	(FetchEvent<Paint>(*this).Add(std::ref(Background), BackgroundPriority))
		+= std::bind(&Widget::Refresh, this, std::placeholders::_1);
}

HBrush
Widget::MakeBlankBrush()
{
	return SolidBrush(ColorSpace::White);
}

void
Widget::SetRenderer(unique_ptr<Renderer> p)
{
	renderer_ptr = p ? std::move(p) : make_unique<Renderer>();
	renderer_ptr->SetSize(GetSizeOf(*this));
}
void
Widget::SetView(unique_ptr<AView> p)
{
	if(!p)
		p.reset(new View(GetBoundsOf(*this)));
	swap(*p, *view_ptr);
	view_ptr = std::move(p);
}

void
Widget::Refresh(PaintEventArgs&& e)
{
	if(!e.ClipArea.IsUnstrictlyEmpty())
		for(auto pr(GetChildren()); pr.first != pr.second; ++pr.first)
			PaintVisibleChildAndCommit(*pr.first, e);
}

} // namespace UI;

} // namespace YSLib;

