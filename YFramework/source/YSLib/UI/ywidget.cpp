/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywidget.cpp
\ingroup UI
\brief 样式无关的图形用户界面部件。
\version r4280
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2013-03-13 13:16 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YWidget
*/


#include "YSLib/UI/ydesktop.h"
#include "YSLib/UI/YBrush.h"
#include "YSLib/UI/ystyle.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(UI)

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


void
Close(IWidget& wgt)
{
	Hide(wgt);
	if(const auto pCon = FetchContainerPtr(wgt))
		ClearFocusingOf(*pCon);
}

void
DrawArrow(PaintEventArgs&& e, IWidget& wgt, SDst half_size, Rotation rot,
	Color c)
{
	Drawing::DrawArrow(e.Target, Rect(e.Location, GetSizeOf(wgt)), half_size,
		rot, c);
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
	Invalidate(wgt, Rect(GetSizeOf(wgt)));
}
void
Invalidate(IWidget& wgt, const Rect& bounds)
{
	auto pWgt(&wgt);
	Rect r(bounds);

	do
	{
		r = pWgt->GetRenderer().CommitInvalidation(r);
		r.GetPointRef() += GetLocationOf(*pWgt);
	}while((pWgt = FetchContainerPtr(*pWgt)));
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
	: view_ptr(new View(r)), renderer_ptr(new Renderer()),
	controller_ptr(new WidgetController(false)),
	Background(SolidBrush(b)), ForeColor(f)
{
	InitializeEvents();
}
Widget::Widget(const Widget& wgt)
	: view_ptr(ClonePolymorphic(wgt.view_ptr)),
	renderer_ptr(ClonePolymorphic(wgt.renderer_ptr)),
	controller_ptr(ClonePolymorphic(wgt.controller_ptr)),
	Background(wgt.Background), ForeColor(wgt.ForeColor)
{}
Widget::~Widget()
{
	DoReleaseFocus(*this);
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
	if(!controller_ptr)
		throw BadEvent();
	return *controller_ptr;
}

void
Widget::SetRenderer(unique_ptr<Renderer>&& p)
{
	renderer_ptr = p ? std::move(p) : unique_ptr<Renderer>(new Renderer());
	renderer_ptr->SetSize(GetSizeOf(*this));
}
void
Widget::SetView(unique_ptr<View>&& p)
{
	view_ptr = p ? std::move(p)
		: unique_ptr<View>(new View(GetBoundsOf(*this)));
}

void
Widget::Refresh(PaintEventArgs&& e)
{
	if(!e.ClipArea.IsUnstrictlyEmpty())
	{
		auto pr(GetChildren());

		while(pr.first != pr.second)
		{
			if(IsVisible(*pr.first))
				e.ClipArea |= PaintChild(*pr.first, e);
			++pr.first;
		}
	}
}

YSL_END_NAMESPACE(UI)

YSL_END

