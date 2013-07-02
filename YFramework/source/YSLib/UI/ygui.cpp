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
\version r3396
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2013-07-03 06:49 +0800
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
	: Timer(d)
{}

bool
InputTimer::RefreshHeld(HeldStateType& s, const Duration& initial_delay,
	const Duration& repeated_delay)
{
	switch(s)
	{
	case Free:
		yunseq(s = Pressed, Interval = initial_delay);
		Activate(*this);
		break;
	case Pressed:
	case Held:
		if(YB_UNLIKELY(Timer::Refresh()))
		{
			if(s == Pressed)
				yunseq(s = Held, Interval = repeated_delay);
			return true;
		}
		break;
	}
	return false;
}

size_t
InputTimer::RefreshClick(size_t s, const Duration& delay)
{
	if(s == 0 || YB_UNLIKELY(!Timer::Refresh()))
		Interval = delay;
	else
		return 0;
	Activate(*this);
	return s + 1;
}

void
InputTimer::ResetInput()
{
	Interval = Timers::TimeSpan(1000);
}


bool
RepeatHeld(InputTimer& tmr, InputTimer::HeldStateType& st,
	const Timers::Duration& initial_delay,
	const Timers::Duration& repeated_delay)
{
	const bool b(st == InputTimer::Free);

	return tmr.RefreshHeld(st, initial_delay, repeated_delay) || b;
}


GUIState::GUIState() ynothrow
	: KeyHeldState(InputTimer::Free), TouchHeldState(InputTimer::Free),
	DraggingOffset(Vec::Invalid), HeldTimer(), ControlLocation(Point::Invalid),
	LastControlLocation(Point::Invalid), Colors(), p_KeyDown(), p_TouchDown(),
	control_entered(false)
{}

void
GUIState::Reset()
{
	yunseq(KeyHeldState = InputTimer::Free, TouchHeldState = InputTimer::Free,
		DraggingOffset = Vec::Invalid),
	HeldTimer.ResetInput();
	yunseq(ControlLocation = Point::Invalid,
		LastControlLocation = Point::Invalid, p_TouchDown = {}, p_KeyDown = {});
}

void
GUIState::ResetHeldState(InputTimer::HeldStateType& s)
{
	s = InputTimer::Free,
	HeldTimer.ResetInput();
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
		CallEvent<KeyUp>(wgt, e);
		ResetHeldState(KeyHeldState);
		if(p_KeyDown == &wgt)
			CallEvent<KeyPress>(wgt, e);
		p_KeyDown = {};
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
		CallEvent<TouchUp>(wgt, e);
		ResetHeldState(TouchHeldState),
		DraggingOffset = Vec::Invalid;
		if(p_TouchDown)
		{
			e.SetSender(*p_TouchDown);
			TryLeaving(std::move(e));
			e.SetSender(wgt);
		}
		if(p_TouchDown == &wgt)
			CallEvent<Click>(wgt, e);
		p_TouchDown = {};
		break;
	case TouchDown:
		p_TouchDown = &wgt;
		TryEntering(std::move(e));
		CallEvent<TouchDown>(wgt, e);
		break;
	case TouchHeld:
		if(!p_TouchDown)
			return false;
		{
			auto& wgt_d(*p_TouchDown);

			if(p_TouchDown == &wgt)
				TryEntering(TouchEventArgs(e));
			else
				TryLeaving(TouchEventArgs(wgt_d, e.Keys,
					e - LocateForWidget(wgt, wgt_d)));
			if(e.Strategy == RoutedEventArgs::Direct)
			{
				if(DraggingOffset == Vec::Invalid)
					DraggingOffset = GetLocationOf(wgt_d) - ControlLocation;
				else
					CallEvent<TouchHeld>(wgt_d, e);
				LastControlLocation = ControlLocation;
			}
		}
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
		r |= DoEvent<HKeyEvent>(p->GetController(), op, std::move(e)) != 0;
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
		r |= DoEvent<HKeyEvent>(p->GetController(), op, std::move(e)) != 0;
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
		r |= DoEvent<HTouchEvent>(p->GetController(), op, std::move(e)) != 0;
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
		r |= DoEvent<HTouchEvent>(p->GetController(), op, std::move(e)) != 0;
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

