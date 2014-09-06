﻿/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ygui.cpp
\ingroup UI
\brief 平台无关的图形用户界面。
\version r4072
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2014-09-06 12:43 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YGUI
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_YGUI
#include YFM_YSLib_UI_YDesktop
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
	for(auto pr(con.GetChildren()); pr.first != pr.second; ++pr.first)
	{
		IWidget& wgt(*pr.first);

		if(Contains(wgt, pt) && IsEnabled(wgt) && IsVisible(wgt))
			return &wgt;
	}
	return {};
}

//! \since build 483
IWidget*
FetchVisibleEnabledFocusingPtr(IWidget& con)
{
	if(const auto p = FetchFocusingPtr(con))
		if(IsVisible(*p) && IsEnabled(*p))
			return p;
	return {};
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
		if(YB_UNLIKELY(CheckTimeout(*this)))
		{
			if(s == Pressed)
				yunseq(s = Held, Interval = repeated_delay);
			return true;
		}
		break;
	}
	return {};
}

size_t
InputTimer::RefreshClick(size_t s, const Duration& delay)
{
	if(s == 0 || YB_UNLIKELY(!CheckTimeout(*this)))
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


void
CallInputEvent(IWidget& wgt, const String& str, const KeyInput& k)
{
	CallEvent<TextInput>(wgt, TextInputEventArgs(wgt, str, k));
}


bool
GUIState::CheckHeldState(const KeyInput& keys, InputTimer::HeldStateType& s)
{
	if(checked_held.none())
		checked_held = keys;
	else if(checked_held != keys)
	{
		yunseq(checked_held |= keys, s = InputTimer::Free);
		return true;
	}
	return {};
}

bool
GUIState::CheckDraggingOffset(IWidget* p)
{
	if(!p)
		p = p_indp_focus;
	if(p)
	{
		if(DraggingOffset == Vec::Invalid)
			DraggingOffset = GetLocationOf(*p) - CursorLocation;
		else
			return true;
	}
	return {};
}

void
GUIState::CleanupReferences(IWidget& wgt)
{
	if(p_CursorOver == &wgt)
		p_CursorOver = {};
	if(p_indp_focus == &wgt)
		p_indp_focus = {};
	if(p_cascade_focus == &wgt)
		p_cascade_focus = {};
	if(ExteralTextInputFocusPtr == &wgt)
		ExteralTextInputFocusPtr = {};
}

void
GUIState::HandleCascade(RoutedEventArgs& e, IWidget& wgt)
{
	if(p_cascade_focus != &wgt && IsFocused(e.GetSender()))
	{
		if(p_cascade_focus)
			CallEvent<LostFocus>(*p_cascade_focus, e);
		p_cascade_focus = &wgt;
		CallEvent<GotFocus>(*p_cascade_focus, e);
	}
}

void
GUIState::Reset()
{
	yunseq(KeyHeldState = InputTimer::Free, TouchHeldState = InputTimer::Free,
		DraggingOffset = Vec::Invalid),
	HeldTimer.ResetInput(),
	yunseq(CursorLocation = Point::Invalid, p_CursorOver = {},
		p_indp_focus = {}, p_cascade_focus = {}, entered = {},
		checked_held = {}, master_key = 0, ExteralTextInputFocusPtr = {},
		CaretLocation = Point::Invalid);
}

void
GUIState::ResetHeldState(InputTimer::HeldStateType& s, const KeyInput& k)
{
	checked_held &= ~k;
	if(checked_held.none())
	{
		s = InputTimer::Free,
		HeldTimer.ResetInput();
	}
}

void
GUIState::ResponseKey(KeyEventArgs& e, UI::VisualEvent op)
{
	auto p(&e.GetSender());
	IWidget* p_con;

	e.Strategy = UI::RoutedEventArgs::Tunnel;
	while(true)
	{
		if(!(IsVisible(*p) && IsEnabled(*p)) || e.Handled)
			return;
		p_con = p;

		const auto t(FetchVisibleEnabledFocusingPtr(*p_con));

		if(!t || t == p_con)
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
	YAssertNonnull(p);
	e.Strategy = UI::RoutedEventArgs::Direct;
	e.SetSender(*p);
	ResponseKeyBase(e, op);
	if(op == KeyDown)
		HandleCascade(e, *p);
	e.Strategy = UI::RoutedEventArgs::Bubble;
	while(!e.Handled && (p_con = FetchContainerPtr(*p)))
	{
		e.SetSender(*(p = p_con));
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
	IWidget* p_con;

	e.Strategy = UI::RoutedEventArgs::Tunnel;
	while(true)
	{
		if(!(IsVisible(*p) && IsEnabled(*p)) || e.Handled)
			return;
		p_con = p;

		const auto t(FetchTopEnabledAndVisibleWidgetPtr(*p_con, e));

		if(!t || t == p_con)
		{
			if(e.Handled)
				return;
			else
				break;
		}
		e.SetSender(*p);
		ResponseCursorBase(e, op);
		p = t;
		e.Position -= GetLocationOf(*p);
	};
	YAssertNonnull(p);
	e.Strategy = UI::RoutedEventArgs::Direct;
	e.SetSender(*p);
	ResponseCursorBase(e, op);
	if(op == TouchDown)
		HandleCascade(e, *p);
	e.Strategy = UI::RoutedEventArgs::Bubble;
	while(!e.Handled && (p_con = FetchContainerPtr(*p)))
	{
		e.Position += GetLocationOf(*p);
		e.SetSender(*(p = p_con));
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
		if(e.Strategy == RoutedEventArgs::Direct && p_indp_focus)
			CallEvent<TouchHeld>(*p_indp_focus, e);
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

bool
GUIState::SendInput(const KeyInput& k, const String& str)
{
	return ExteralTextInputFocusPtr
		? (UI::CallInputEvent(*ExteralTextInputFocusPtr, str, k), true) : false;
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

char
GUIState::UpdateChar(KeyInput& keys)
{
	if(keys != checked_held)
	{
		master_key = FindFirstKeyInCategroy(keys, KeyCategory::Character);

		yunseq(keys = checked_held |= keys, KeyHeldState = InputTimer::Free);
	}
	return ExteralTextInputFocusPtr ? char()
		: MapKeyChar(checked_held, master_key);
}

void
GUIState::Wrap(IWidget& wgt)
{
	auto& controller(wgt.GetController());

	yunseq(
	FetchEvent<KeyUp>(controller).Add([this](KeyEventArgs&& e){
		auto& wgt(e.GetSender());

		master_key
			= FindFirstKeyInCategroy(checked_held, KeyCategory::Character);
		ResetHeldState(KeyHeldState, e.Keys);
		if(p_indp_focus == &wgt)
			CallEvent<KeyPress>(wgt, e);
		p_indp_focus = {};
	}, 0x00),
	FetchEvent<KeyDown>(controller).Add([this](KeyEventArgs&& e){
		p_indp_focus = &e.GetSender();
	}, 0xFF),
	FetchEvent<CursorOver>(controller).Add([this](CursorEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct)
		{
			e.Keys.reset();

			auto& wgt(e.GetSender());

			if(p_CursorOver != &wgt || WidgetIdentity != old_widget_identity)
			{
				if(p_CursorOver)
					CallEvent<Leave>(*p_CursorOver, CursorEventArgs(
						*p_CursorOver, e.Keys, e.Position - LocateForWidget(wgt,
						*p_CursorOver)));
				// FIXME: Wrong position entered for shared widget.
				CallEvent<Enter>(e.GetSender(), CursorEventArgs(e));
				yunseq(p_CursorOver = &wgt,
					old_widget_identity = WidgetIdentity);
			}
		}
	}, 0xFF),
	FetchEvent<TouchUp>(controller).Add([this](CursorEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct)
		{
			auto& wgt(e.GetSender());

			if(p_indp_focus)
			{
				e.SetSender(*p_indp_focus);
				TryLeaving(std::move(e));
				e.SetSender(e.GetSender());
			}
			ResetHeldState(TouchHeldState, e.Keys),
			DraggingOffset = Vec::Invalid;
			if(p_indp_focus == &wgt)
				CallEvent<Click>(wgt, e);
			else if(p_indp_focus)
				CallEvent<ClickAcross>(*p_indp_focus, e);
			p_indp_focus = {};
		}
	}, 0x00),
	FetchEvent<TouchDown>(controller).Add([this](CursorEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct)
		{
			p_indp_focus = &e.GetSender();
			TryEntering(std::move(e));
		}
	}, 0xFF),
	FetchEvent<TouchHeld>(controller).Add([this](CursorEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct)
		{
			auto& wgt(e.GetSender());

			if(p_indp_focus == &wgt)
				TryEntering(CursorEventArgs(e));
		//	else
		//		TryLeaving(CursorEventArgs(*p_indp_focus, e.Keys,
		//			e.Position - LocateForWidget(wgt, *p_indp_focus)));
			else if(entered)
			{
				CallEvent<Leave>(*p_indp_focus, CursorEventArgs(*p_indp_focus,
					e.Keys, e.Position - LocateForWidget(wgt, *p_indp_focus)));
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

