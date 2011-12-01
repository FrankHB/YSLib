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
\version r5259;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2011-11-29 17:14 +0800;
\par 字符集:
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
ClearFocusingOf(IWidget& wgt)
{
	if(const auto p = FetchFocusingPtr(wgt))
	{
		wgt.GetView().pFocusing = nullptr;
		CallEvent<LostFocus>(*p, UIEventArgs(wgt));
	}
}

IWidget*
CheckWidget(IWidget& wgt, const Point& pt, bool(&f)(const IWidget&))
{
	return Contains(wgt, pt) && f(wgt) ? &wgt : nullptr;
}

void
RequestToTop(IWidget& wgt)
{
	if(auto pFrm = dynamic_cast<Window*>(FetchContainerPtr(wgt)))
		pFrm->MoveToTop(wgt);
}


void
Show(IWidget& wgt)
{
	SetVisibleOf(wgt, true);
	SetInvalidationToParent(wgt);
}

void
Hide(IWidget& wgt)
{
	SetVisibleOf(wgt, false);
	SetInvalidationToParent(wgt);
}


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


Widget::Widget(const Rect& r, Color b, Color f)
	: pView(new View(r)), pRenderer(new Renderer()),
	pController(new WidgetController(false)),
	BackColor(b), ForeColor(f)
{}
Widget::Widget(const Widget& wgt)
	: pView(ClonePolymorphic(wgt.pView)),
	pRenderer(ClonePolymorphic(wgt.pRenderer)),
	pController(ClonePolymorphic(wgt.pController)),
	BackColor(wgt.BackColor), ForeColor(wgt.ForeColor)
{}
Widget::~Widget()
{
	ReleaseFocus(*this);
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

Rect
Widget::Refresh(const PaintContext& pc)
{
	if(!IsTransparent())
		Drawing::FillRect(pc.Target, pc.ClipArea, BackColor);
	return Rect(pc.Location, GetSizeOf(*this));
}

YSL_END_NAMESPACE(Components)

YSL_END

