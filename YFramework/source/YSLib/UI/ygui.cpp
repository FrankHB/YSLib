/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ygui.cpp
\ingroup UI
\brief 平台无关的图形用户界面。
\version r3322
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2013-03-25 23:38 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YGUI
*/


#include "YSLib/UI/ygui.h"
#include "YSLib/UI/ydesktop.h"

YSL_BEGIN

using namespace Drawing;

YSL_BEGIN_NAMESPACE(UI)

namespace
{

//! \since build 359
IWidget*
FetchTopEnabledAndVisibleWidgetPtr(IWidget& con, const Point& pt)
{
	for(auto pr = con.GetChildren(); pr.first != pr.second; ++pr.first)
	{
		IWidget& wgt(*pr.first);

		if(Contains(wgt, pt) && IsEnabled(wgt) && IsVisible(wgt))
			return &wgt;
	}
	return nullptr;
}

} // unnamed namespace;


InputTimer::InputTimer(const Duration& d)
	: timer(d)
{}

bool
InputTimer::Refresh(HeldStateType& s,
	const Duration& initial_delay, const Duration& repeated_delay)
{
	switch(s)
	{
	case Free:
		s = Pressed,
		timer.SetInterval(initial_delay);
		Activate(timer);
		break;
	case Pressed:
	case Held:
		if(YB_UNLIKELY(timer.Refresh()))
		{
			if(s == Pressed)
			{
				s = Held,
				timer.SetInterval(repeated_delay);
			}
			return true;
		}
		break;
	}
	return false;
}

void
InputTimer::Reset()
{
	Deactivate(timer);
	timer.SetInterval(Timers::TimeSpan(1000));
}


bool
RepeatHeld(InputTimer& tmr, InputTimer::HeldStateType& st,
	const Timers::Duration& initial_delay,
	const Timers::Duration& repeated_delay)
{
	const bool b(st == InputTimer::Free);

	return tmr.Refresh(st, initial_delay, repeated_delay) || b;
}


GUIState::GUIState() ynothrow
	: KeyHeldState(InputTimer::Free), TouchHeldState(InputTimer::Free),
	DraggingOffset(Vec::Invalid), HeldTimer(),
	ControlLocation(Point::Invalid),
	LastControlLocation(Point::Invalid), Colors(),
	p_KeyDown(), p_TouchDown(), control_entered(false)
{}

void
GUIState::Reset()
{
	yunseq(KeyHeldState = InputTimer::Free, TouchHeldState = InputTimer::Free,
		DraggingOffset = Vec::Invalid),
	HeldTimer.Reset();
	yunseq(ControlLocation = Point::Invalid,
		LastControlLocation = Point::Invalid,
		p_TouchDown = nullptr, p_KeyDown = nullptr);
}

void
GUIState::ResetHeldState(InputTimer::HeldStateType& s)
{
	s = InputTimer::Free,
	HeldTimer.Reset();
}

void
GUIState::TryEntering(TouchEventArgs&& e)
{
	if(!control_entered)
	{
		CallEvent<Enter>(e.GetSender(), e);
		control_entered = true;
	}
}

void
GUIState::TryLeaving(TouchEventArgs&& e)
{
	if(control_entered)
	{
		CallEvent<Leave>(e.GetSender(), e);
		control_entered = false;
	}
}

bool
GUIState::ResponseKeyBase(KeyEventArgs& e, UI::VisualEvent op)
{
	auto& wgt(e.GetSender());

	switch(op)
	{
	case KeyUp:
		ResetHeldState(KeyHeldState);
		CallEvent<KeyUp>(wgt, e);
		if(p_KeyDown == &wgt)
		{
			CallEvent<KeyPress>(wgt, e);
			p_KeyDown = nullptr;
		}
		break;
	case KeyDown:
		p_KeyDown = &wgt;
		CallEvent<KeyDown>(wgt, e);
		break;
	case KeyHeld:
	/*	if(e.Strategy == RoutedEventArgs::Direct && p_KeyDown != &c)
		{
			ResetHeldState(KeyHeldState);
			return false;
		}*/
		CallEvent<KeyHeld>(wgt, e);
		break;
	default:
		YAssert(false, "Invalid operation found.");
	}
	return true;
}

bool
GUIState::ResponseTouchBase(TouchEventArgs& e, UI::VisualEvent op)
{
	auto& wgt(e.GetSender());

	switch(op)
	{
	case TouchUp:
		ResetHeldState(TouchHeldState);
		DraggingOffset = Vec::Invalid;
		CallEvent<TouchUp>(wgt, e);
		if(p_TouchDown)
		{
			e.SetSender(*p_TouchDown);
			TryLeaving(std::move(e));
			e.SetSender(wgt);
		}
		if(p_TouchDown == &wgt)
		{
			CallEvent<Click>(wgt, e);
			p_TouchDown = nullptr;
		}
		break;
	case TouchDown:
		p_TouchDown = &wgt;
		TryEntering(std::move(e));
		CallEvent<TouchDown>(wgt, e);
		break;
	case TouchHeld:
		if(!p_TouchDown)
			return false;
		if(p_TouchDown == &wgt)
			TryEntering(TouchEventArgs(e));
		else
			TryLeaving(TouchEventArgs(*p_TouchDown, e.Keys,
				e - LocateForWidget(wgt, *p_TouchDown)));
		CallEvent<TouchHeld>(*p_TouchDown, e);
		break;
	default:
		YAssert(false, "Invalid operation found.");
	}
	return true;
}

bool
GUIState::ResponseKey(KeyEventArgs& e, UI::VisualEvent op)
{
	auto p(&e.GetSender());
	IWidget* pCon;
	bool r(false);

	e.Strategy = UI::RoutedEventArgs::Tunnel;
	while(true)
	{
		if(!(IsVisible(*p) && IsEnabled(*p)))
			return false;
		if(e.Handled)
			return true;
		pCon = p;

		auto t(FetchFocusingPtr(*pCon));

		if(!t || t == pCon)
		{
			if(e.Handled)
				return true;
			else
				break;
		}
		e.SetSender(*p);
		r |= ResponseKeyBase(e, op);
		p = t;
	}

	YAssert(p, "Null pointer found.");

	e.Strategy = UI::RoutedEventArgs::Direct;
	e.SetSender(*p);
	r |= ResponseKeyBase(e, op);
	e.Strategy = UI::RoutedEventArgs::Bubble;
	while(!e.Handled && (pCon = FetchContainerPtr(*p)))
	{
		e.SetSender(*(p = pCon));
		r |= ResponseKeyBase(e, op);
	}
	return r/* || e.Handled*/;
}

bool
GUIState::ResponseTouch(TouchEventArgs& e, UI::VisualEvent op)
{
	ControlLocation = e;

	auto p(&e.GetSender());
	IWidget* pCon;
	bool r(false);

	e.Strategy = UI::RoutedEventArgs::Tunnel;
	while(true)
	{
		if(!(IsVisible(*p) && IsEnabled(*p)))
			return false;
		if(e.Handled)
			return true;
		pCon = p;

		const auto t(FetchTopEnabledAndVisibleWidgetPtr(*pCon, e));

		if(!t || t == pCon)
		{
			if(e.Handled)
				return true;
			else
				break;
		}
		e.SetSender(*p);
		r |= DoEvent<HTouchEvent>(p->GetController(), op, e) != 0;
		p = t;
		e -= GetLocationOf(*p);
	};

	YAssert(p, "Null pointer found.");

	e.Strategy = UI::RoutedEventArgs::Direct;
	e.SetSender(*p);
	r |= ResponseTouchBase(e, op);
	e.Strategy = UI::RoutedEventArgs::Bubble;
	while(!e.Handled && (pCon = FetchContainerPtr(*p)))
	{
		e += GetLocationOf(*p);
		e.SetSender(*(p = pCon));
		r |= DoEvent<HTouchEvent>(p->GetController(), op, e) != 0;
	}
	return r/* || e.Handled*/;
}


GUIState&
FetchGUIState()
{
	static GUIState* pState(new GUIState());

	return *pState;
}

YSL_END_NAMESPACE(UI)

YSL_END

