/*
	Copyright by FrankHB 2009-2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ygui.cpp
\ingroup UI
\brief 平台无关的图形用户界面。
\version r3871
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2013-09-14 09:47 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YGUI
*/


#include "YSLib/UI/ygui.h"
#include "YSLib/UI/ydesktop.h"
#include <ystdex/cast.hpp> // for ystdex::polymorphic_downcast;

namespace YSLib
{

using namespace Drawing;

namespace UI
{

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
	DraggingOffset(Vec::Invalid), HeldTimer(), CursorLocation(Point::Invalid),
	Colors(), p_KeyDown(), p_CursorOver(), p_TouchDown(), entered()
{}

bool
GUIState::CheckDraggingOffset(IWidget* p)
{
	if(!p)
		p = p_TouchDown;
	if(p)
	{
		if(DraggingOffset == Vec::Invalid)
			DraggingOffset = GetLocationOf(*p) - CursorLocation;
		else
			return true;
	}
	return false;
}

void
GUIState::CleanupReferences(IWidget& wgt)
{
	if(p_KeyDown == &wgt)
		p_KeyDown = {};
	if(p_CursorOver == &wgt)
		p_CursorOver = {};
	if(p_TouchDown == &wgt)
		p_TouchDown = {};
}

void
GUIState::Reset()
{
	yunseq(KeyHeldState = InputTimer::Free, TouchHeldState = InputTimer::Free,
		DraggingOffset = Vec::Invalid),
	HeldTimer.ResetInput();
	yunseq(CursorLocation = Point::Invalid, p_TouchDown = {}, p_KeyDown = {},
		p_CursorOver = {}, entered = {});
}

void
GUIState::ResetHeldState(InputTimer::HeldStateType& s)
{
	s = InputTimer::Free,
	HeldTimer.ResetInput();
}

void
GUIState::ResponseKey(KeyEventArgs& e, UI::VisualEvent op)
{
	auto p(&e.GetSender());
	IWidget* pCon;

	e.Strategy = UI::RoutedEventArgs::Tunnel;
	while(true)
	{
		if(!(IsVisible(*p) && IsEnabled(*p)) || e.Handled)
			return;
		pCon = p;

		const auto t(FetchFocusingPtr(*pCon));

		if(!t || t == pCon)
		{
			if(e.Handled)
				return;
			else
				break;
		}
		e.SetSender(*p);
		ResponseKeyBase(e, op);
		p = t;
	}

	YAssert(p, "Null pointer found.");

	e.Strategy = UI::RoutedEventArgs::Direct;
	e.SetSender(*p);
	ResponseKeyBase(e, op);
	e.Strategy = UI::RoutedEventArgs::Bubble;
	while(!e.Handled && (pCon = FetchContainerPtr(*p)))
	{
		e.SetSender(*(p = pCon));
		ResponseKeyBase(e, op);
	}
}

void
GUIState::ResponseKeyBase(KeyEventArgs& e, UI::VisualEvent op)
{
	YAssert(op == KeyUp || op == KeyDown || op == KeyHeld,
		"Invalid operation found.");

	DoEvent<HKeyEvent>(e.GetSender().GetController(), op, std::move(e));
}

void
GUIState::ResponseCursor(CursorEventArgs& e, UI::VisualEvent op)
{
	CursorLocation = e;

	auto p(&e.GetSender());
	IWidget* pCon;

	e.Strategy = UI::RoutedEventArgs::Tunnel;
	while(true)
	{
		if(!(IsVisible(*p) && IsEnabled(*p)) || e.Handled)
			return;
		pCon = p;

		const auto t(FetchTopEnabledAndVisibleWidgetPtr(*pCon, e));

		if(!t || t == pCon)
		{
			if(e.Handled)
				return;
			else
				break;
		}
		e.SetSender(*p);
		ResponseCursorBase(e, op);
		p = t;
		e -= GetLocationOf(*p);
	};

	YAssert(p, "Null pointer found.");

	e.Strategy = UI::RoutedEventArgs::Direct;
	e.SetSender(*p);
	ResponseCursorBase(e, op);
	e.Strategy = UI::RoutedEventArgs::Bubble;
	while(!e.Handled && (pCon = FetchContainerPtr(*p)))
	{
		e += GetLocationOf(*p);
		e.SetSender(*(p = pCon));
		ResponseCursorBase(e, op);
	}
}

void
GUIState::ResponseCursorBase(CursorEventArgs& e, UI::VisualEvent op)
{
	auto& controller(e.GetSender().GetController());

	switch(op)
	{
	case TouchHeld:
		if(e.Strategy == RoutedEventArgs::Direct && p_TouchDown)
			CallEvent<TouchHeld>(*p_TouchDown, e);
		break;
	case TouchUp:
	case TouchDown:
	case CursorOver:
		DoEvent<HCursorEvent>(controller, op, std::move(e));
		break;
	case CursorWheel:
		DoEvent<HCursorWheelEvent>(controller, CursorWheel,
			std::move(ystdex::polymorphic_downcast<CursorWheelEventArgs&>(e)));
		break;
	default:
		YAssert(false, "Invalid operation found.");
	}
}

void
GUIState::TryEntering(CursorEventArgs&& e)
{
	if(!entered)
	{
		CallEvent<Enter>(e.GetSender(), e);
		entered = true;
	}
}

void
GUIState::TryLeaving(CursorEventArgs&& e)
{
	if(entered)
	{
		CallEvent<Leave>(e.GetSender(), e);
		entered = {};
	}
}

void
GUIState::Wrap(IWidget& wgt)
{
	auto& controller(wgt.GetController());

	yunseq(
	FetchEvent<KeyUp>(controller).Add([this](KeyEventArgs&& e){
		auto& wgt(e.GetSender());

		ResetHeldState(KeyHeldState);
		if(p_KeyDown == &wgt)
			CallEvent<KeyPress>(wgt, e);
		p_KeyDown = {};
	}, 0x00),
	FetchEvent<KeyDown>(controller).Add([this](KeyEventArgs&& e){
		p_KeyDown = &e.GetSender();
	}, 0xFF),
	FetchEvent<CursorOver>(controller).Add([this](CursorEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct)
		{
			e.Keys.reset();

			auto& wgt(e.GetSender());

			if(p_CursorOver != &wgt)
			{
				if(p_CursorOver)
					CallEvent<Leave>(*p_CursorOver, CursorEventArgs(
						*p_CursorOver, e.Keys, e - LocateForWidget(wgt,
						*p_CursorOver)));
				CallEvent<Enter>(e.GetSender(), CursorEventArgs(e));
				p_CursorOver = &wgt;
			}
		}
	}, 0xFF),
	FetchEvent<TouchUp>(controller).Add([this](CursorEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct)
		{
			auto& wgt(e.GetSender());

			if(p_TouchDown)
			{
				e.SetSender(*p_TouchDown);
				TryLeaving(std::move(e));
				e.SetSender(e.GetSender());
			}
			ResetHeldState(TouchHeldState),
			DraggingOffset = Vec::Invalid;
			if(p_TouchDown == &wgt)
				CallEvent<Click>(wgt, e);
			else if(p_TouchDown)
				CallEvent<ClickAcross>(*p_TouchDown, e);
			p_TouchDown = {};
		}
	}, 0x00),
	FetchEvent<TouchDown>(controller).Add([this](CursorEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct)
		{
			p_TouchDown = &e.GetSender();
			TryEntering(std::move(e));
		}
	}, 0xFF),
	FetchEvent<TouchHeld>(controller).Add([this](CursorEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct)
		{
			auto& wgt(e.GetSender());

			if(p_TouchDown == &wgt)
				TryEntering(CursorEventArgs(e));
		//	else
		//		TryLeaving(CursorEventArgs(*p_TouchDown, e.Keys,
		//			e - LocateForWidget(wgt, *p_TouchDown)));
			else if(entered)
			{
				CallEvent<Leave>(*p_TouchDown, CursorEventArgs(*p_TouchDown,
					e.Keys, e - LocateForWidget(wgt, *p_TouchDown)));
				entered = {};
			}
		}
	}, 0xFF)
	);
}


GUIState&
FetchGUIState()
{
	static GUIState* pState(new GUIState());

	return *pState;
}

} // namespace UI;

} // namespace YSLib;

