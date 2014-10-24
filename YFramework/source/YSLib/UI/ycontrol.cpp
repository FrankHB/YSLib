/*
	© 2010-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycontrol.cpp
\ingroup UI
\brief 样式无关的控件。
\version r3962
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-02-18 13:44:34 +0800
\par 修改时间:
	2014-10-21 12:49 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YControl
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_YControl
#include YFM_YSLib_UI_YGUI
#include YFM_YSLib_UI_YUIContainer
#include YFM_YSLib_Core_YStorage
#include <ystdex/algorithm.hpp>
#include YFM_YSLib_UI_YBrush

namespace YSLib
{

namespace UI
{

namespace
{

//! \since build 536
void
OnKey_Bound_Events(KeyEventArgs& e, VisualEvent id)
{
	if(const auto p_control = dynamic_cast<Control*>(&e.GetSender()))
		if(p_control->BoundControlPtr)
		{
			auto p_ctl(p_control->BoundControlPtr(e.GetKeys()));

			if(p_ctl && IsEnabled(*p_ctl))
			{
				TryExpr(DoEvent<HCursorEvent>(p_ctl->GetController(), id,
					CursorEventArgs(*p_ctl, e.Keys, Point::Invalid)))
				CatchIgnore(BadEvent&)
				e.Handled = true;
			}
		}
}

//! \since build 434
void
TouchHeld_DragWidget(IWidget* p = {})
{
	auto& st(FetchGUIState());

	if(!p)
		p = st.GetIndependentFocusPtr();
	if(st.CheckDraggingOffset(p))
	{
	// TODO: Compare subsequent states to make a more efficient implementation.
	// TODO: Merge state to make a more efficient implementation.
		InvalidateParent(*p);
		SetLocationOf(*p, st.CursorLocation + st.DraggingOffset);
	}
}

} // unnamed namespace;


EventMapping::ItemType&
Controller::GetItemRef(const VisualEvent& id, EventMapping::MappedType(&f)())
{
	return GetEvent(EventMap, id, f);
}


bool
Enable(IWidget& wgt, bool b)
{
	const auto enability_changed(IsEnabled(wgt) != b);

	SetEnabledOf(wgt, b);
	if(enability_changed)
		Invalidate(wgt);
	return enability_changed;
}


void
OnUIEvent_Invalidate(UIEventArgs&& e)
{
	Invalidate(e.GetSender());
}

void
OnKeyHeld(KeyEventArgs&& e)
{
	auto& st(FetchGUIState());

	if(st.HeldTimer.RefreshHeld(st.KeyHeldState, Timers::TimeSpan(240),
		Timers::TimeSpan(60)))
		CallEvent<KeyDown>(e.GetSender(), e);
}

void
OnTouchDown_RequestToFrontFocused(CursorEventArgs&& e)
{
	IWidget& wgt(e.GetSender());

	if(e.Strategy != RoutedEventArgs::Bubble)
		RequestToFront(wgt);
	if(e.Strategy == RoutedEventArgs::Direct)
		ClearFocusingOf(wgt);
	if(e.Strategy != RoutedEventArgs::Tunnel)
		RequestFocus(wgt);
}

void
OnTouchHeld(CursorEventArgs&& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
	{
		auto& st(FetchGUIState());

		if(st.GetIndependentFocusPtr() && st.HeldTimer.RefreshHeld(
			st.TouchHeldState, Timers::TimeSpan(240), Timers::TimeSpan(80)))
			CallEvent<TouchDown>(*st.GetIndependentFocusPtr(), e);
	}
}

void
OnTouchHeld_Dragging(CursorEventArgs&& e)
{
	if(e.Strategy == RoutedEventArgs::Direct && !e.Handled)
		TouchHeld_DragWidget();
}

void
OnTouchHeld_DraggingRaw(CursorEventArgs&& e, IWidget& wgt)
{
	if(e.Strategy == RoutedEventArgs::Direct && !e.Handled)
		TouchHeld_DragWidget(&wgt);
}


void
OnKey_Bound_TouchUp(KeyEventArgs&& e)
{
	OnKey_Bound_Events(e, TouchUp);
}

void
OnKey_Bound_TouchDown(KeyEventArgs&& e)
{
	OnKey_Bound_Events(e, TouchDown);
}

void
OnKey_Bound_Click(KeyEventArgs&& e)
{
	OnKey_Bound_Events(e, Click);
}


Control::ControlEventMap::ControlEventMap()
{
	FetchEvent<TouchDown>(*this) += OnTouchDown_RequestToFrontFocused;
}

Control::Control(const Rect& r)
	: Widget(new View(r), new Renderer(),
	new Controller(true, FetchPrototype<ControlEventMap>())),
	BoundControlPtr(std::bind(&Control::GetBoundControlPtr, this,
		std::placeholders::_1))
{
	FetchGUIState().Wrap(*this),
	yunseq(
	FetchEvent<Move>(*this) += [](UIEventArgs&& e){
		InvalidateParent(e.GetSender());
	},
	FetchEvent<Resize>(*this) += OnUIEvent_Invalidate
	);
}
Control::Control(const Rect& r, HBrush b)
	: Control(r)
{
	Background = b;
}
Control::Control(const Control& ctl)
	: Widget(ctl), BoundControlPtr(ctl.BoundControlPtr)
{}

void
Control::OnTouch_Close(CursorEventArgs&&)
{
	Close(*this);
}

} // namespace UI;

} // namespace YSLib;

