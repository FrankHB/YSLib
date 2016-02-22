/*
	© 2009-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YGUI.cpp
\ingroup UI
\brief 平台无关的图形用户界面。
\version r4411
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2016-02-13 15:26 +0800
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

//! \since build 673
IWidget&
FetchTopEnabledAndVisibleWidget(IWidget& con, const Point& pt,
	VisualEvent id)
{
	for(auto pr(con.GetChildren()); pr.first != pr.second; ++pr.first)
	{
		auto& wgt(*pr.first);

		if(Contains(wgt, pt) && IsVisible(wgt)
			&& wgt.GetController().IsEventEnabled(id))
			return wgt;
	}
	return con;
}

//! \since build 673
inline PDefH(bool, IsVisibleEnabled, IWidget& wgt)
	ImplRet(IsVisible(wgt) && IsEnabled(wgt))

} // unnamed namespace;


yconstexpr const InputTimer::Duration InputTimer::DefaultDuration;

InputTimer::InputTimer(Duration d) ynothrow
	: Timer(d)
{}

bool
InputTimer::RefreshHeld(HeldStateType& s, Duration initial_delay,
	Duration repeated_delay)
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
ImplDeDtor(InputTimer)

size_t
InputTimer::RefreshTap(size_t s, Duration delay)
{
	const auto res(s == 0 || YB_UNLIKELY(!CheckTimeout(*this)) ? s + 1 : 0);

	Interval = delay;
	Activate(*this);
	return res;
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
GUIState::CheckDraggingOffset(observer_ptr<IWidget> p)
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
	// TODO: Simplify using %unseq_apply?
	if(p_CursorOver.get() == &wgt)
		p_CursorOver = {};
	if(p_indp_focus.get() == &wgt)
		p_indp_focus = {};
	if(p_cascade_focus.get() == &wgt)
		p_cascade_focus = {};
	if(p_entered_toplevel.get() == &wgt)
		p_entered_toplevel = {};
	if(ExternalTextInputFocusPtr.get() == &wgt)
		ExternalTextInputFocusPtr = {};
}

void
GUIState::HandleCascade(RoutedEventArgs& e, IWidget& wgt)
{
	if(p_cascade_focus.get() != &wgt && IsFocused(e.GetSender()))
	{
		if(p_cascade_focus)
			CallEvent<LostFocus>(*p_cascade_focus, e);
		p_cascade_focus = make_observer(&wgt);
		CallEvent<GotFocus>(*p_cascade_focus, e);
	}
}

size_t
GUIState::RefreshTap(const Point& pt, size_t n, const Timers::Duration& delay)
{
	const auto taps(TapTimer.RefreshTap(tap_count, delay));

	tap_count = taps < n ? max<size_t>(taps, 1) : 0;
	if(pt != Point::Invalid)
	{
		if(tap_count == 0 || taps == 0 || tap_location == Point::Invalid)
			tap_location = pt;
		else if(!Rect(-SPos(TapArea.Width) / 2, -SPos(TapArea.Height) / 2,
			TapArea).Contains(pt - tap_location))
		{
			tap_location = Point::Invalid;
			return 0;
		}
	}
	return taps;
}

void
GUIState::Reset()
{
	yunseq(KeyHeldState = InputTimer::Free, TouchHeldState = InputTimer::Free,
		DraggingOffset = Vec::Invalid),
	HeldTimer.ResetInput(), TapTimer.ResetInput();
	yunseq(CursorLocation = Point::Invalid, CursorOverLocation = Point::Invalid,
		p_CursorOver = {}, p_cascade_focus = {}, p_entered_toplevel = {},
		p_indp_focus = {}, entered = {}, checked_held = {}, master_key = 0,
		ExternalTextInputFocusPtr = {}, CaretLocation = Point::Invalid,
		tap_location = Point::Invalid, tap_count = 0);
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
GUIState::ResponseCursor(CursorEventArgs& e, UI::VisualEvent op)
{
	if(op == CursorOver)
		CursorOverLocation = CursorLocation;
	CursorLocation = e;

	auto wgt_ref(ystdex::ref(e.GetSender()));
	IWidget* p_con;

	e.Strategy = UI::RoutedEventArgs::Tunnel;
	while(true)
	{
		if(!IsVisibleEnabled(wgt_ref) || e.Handled)
			return;
		if(wgt_ref.get().GetView().HitChildren(e))
		{
			// NOTE: Following code simulates a %Control at (0, 0) is
			//	responsible directly without user-defined event handlers.
			e.Strategy = RoutedEventArgs::Direct;
			switch(op)
			{
			case TouchUp:
				// NOTE: See %Wrap.
				if(p_indp_focus)
				{
					e.SetSender(*p_indp_focus);
					TryLeaving(std::move(e));
				}
				ResetHeldState(TouchHeldState, e.Keys),
				DraggingOffset = Vec::Invalid;
				if(p_indp_focus)
					CallEvent<ClickAcross>(*p_indp_focus, e);
				p_indp_focus = {};
				break;
			case TouchDown:
				// NOTE: See %Wrap.
				p_indp_focus = {};
				// NOTE: See %OnTouchDown_RequestToFrontFocused.
				ClearFocusingOf(wgt_ref);
				break;
			// TODO: %CursorOver.
			case TouchHeld:
				// NOTE: See %ResponseCursorBase.
				if(p_indp_focus)
					CallEvent<TouchHeld>(*p_indp_focus, e);
				// TODO: Send %Leave event.
			default:
				break;
			}
			if(e.Handled)
				return;
			e.Strategy = RoutedEventArgs::Bubble;
			goto handle_bubbled;
		}

		auto& t(FetchTopEnabledAndVisibleWidget(wgt_ref, e, op));

		if(&t == &wgt_ref.get())
			break;
		ResponseCursorBase(e, op);
		e.SetSender(wgt_ref = t);
		e.Position -= GetLocationOf(wgt_ref);
	}
	e.Strategy = RoutedEventArgs::Direct;
	e.SetSender(wgt_ref);
	ResponseCursorBase(e, op);
	if(op == TouchDown)
		HandleCascade(e, wgt_ref);
	e.Strategy = RoutedEventArgs::Bubble;
	while(!e.Handled && (p_con = FetchContainerPtr(wgt_ref).get()))
	{
		e.Position += GetLocationOf(wgt_ref);
		e.SetSender(wgt_ref = *p_con);
handle_bubbled:
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

void
GUIState::ResponseKey(KeyEventArgs& e, UI::VisualEvent op)
{
	auto wgt_ref(ystdex::ref(e.GetSender()));

	e.Strategy = RoutedEventArgs::Tunnel;
	while(true)
	{
		if(!IsVisibleEnabled(wgt_ref) || e.Handled)
			return;
		if(const auto p = FetchFocusingPtr(wgt_ref))
			if(IsVisibleEnabled(*p))
			{
				ResponseKeyBase(e, op);
				e.SetSender(wgt_ref = *p);
				continue;
			}
		break;
	}
	e.Strategy = RoutedEventArgs::Direct;
	e.SetSender(wgt_ref);
	ResponseKeyBase(e, op);
	if(op == KeyDown)
		HandleCascade(e, wgt_ref);
	e.Strategy = RoutedEventArgs::Bubble;
	while(!e.Handled)
		if(const auto p_con = FetchContainerPtr(wgt_ref))
		{
			e.SetSender(wgt_ref = *p_con);
			ResponseKeyBase(e, op);
		}
		else
			break;
}

void
GUIState::ResponseKeyBase(KeyEventArgs& e, UI::VisualEvent op)
{
	YAssert(op == KeyUp || op == KeyDown || op == KeyHeld,
		"Invalid operation found.");
	DoEvent<HKeyEvent>(e.GetSender().GetController(), op, std::move(e));
}

bool
GUIState::SendInput(const KeyInput& k, const String& str)
{
	return ExternalTextInputFocusPtr ? (UI::CallInputEvent(
		*ExternalTextInputFocusPtr, str, k), true) : false;
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
GUIState::UpdateChar(const KeyInput& keys)
{
	if(keys != checked_held)
	{
		master_key = FindFirstKeyInCategroy(keys, KeyCategory::Character);

		yunseq(checked_held |= keys, KeyHeldState = InputTimer::Free);
	}
	return ExternalTextInputFocusPtr ? char()
		: MapKeyChar(checked_held, master_key);
}

void
GUIState::Wrap(IWidget& wgt)
{
	auto& controller(wgt.GetController());

	yunseq(
	FetchEvent<KeyUp>(controller).Add([this](KeyEventArgs&& e){
		auto& sender(e.GetSender());

		master_key
			= FindFirstKeyInCategroy(checked_held, KeyCategory::Character);
		ResetHeldState(KeyHeldState, e.Keys);
		if(p_indp_focus.get() == &sender)
			CallEvent<KeyPress>(sender, e);
		p_indp_focus = {};
	}, 0x00),
	FetchEvent<KeyDown>(controller).Add([this](KeyEventArgs&& e){
		p_indp_focus = make_observer(&e.GetSender());
	}, 0xFF),
	FetchEvent<CursorOver>(controller).Add([this](CursorEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct)
		{
			e.Keys.reset();

			auto& sender(e.GetSender());

			if(p_CursorOver.get() != &sender || WidgetIdentity != shared_wgt_id)
			{
				if(p_CursorOver)
				{
					Point pt;

					if(p_entered_toplevel.get() == &FetchTopLevel(sender, pt))
						CallEvent<Leave>(*p_CursorOver, CursorEventArgs(sender,
							e.Keys, e.Position - entered_top_location + pt));
				}
				CallEvent<Enter>(sender, CursorEventArgs(e));
				entered_top_location = {};
				p_entered_toplevel = make_observer(&FetchTopLevel(sender,
					entered_top_location));
				yunseq(p_CursorOver = make_observer(&sender),
					shared_wgt_id = WidgetIdentity);
			}
		}
	}, 0xFF),
	FetchEvent<TouchUp>(controller).Add([this](CursorEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct)
		{
			auto& sender(e.GetSender());

			if(p_indp_focus)
			{
				e.SetSender(*p_indp_focus);
				TryLeaving(std::move(e));
				e.SetSender(sender);
			}
			ResetHeldState(TouchHeldState, e.Keys),
			DraggingOffset = Vec::Invalid;
			if(p_indp_focus.get() == &sender)
				CallEvent<Click>(sender, e);
			else if(p_indp_focus)
				CallEvent<ClickAcross>(*p_indp_focus, e);
			p_indp_focus = {};
		}
	}, 0x00),
	FetchEvent<TouchDown>(controller).Add([this](CursorEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct)
		{
			p_indp_focus = make_observer(&e.GetSender());
			TryEntering(std::move(e));
		}
	}, 0xFF),
	FetchEvent<TouchHeld>(controller).Add([this](CursorEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct)
		{
			auto& sender(e.GetSender());

			if(p_indp_focus.get() == &sender)
				TryEntering(CursorEventArgs(e));
		//	else
		//		TryLeaving(CursorEventArgs(*p_indp_focus, e.Keys,
		//			e.Position - LocateForWidget(wgt, *p_indp_focus)));
			else if(entered)
			{
				CallEvent<Leave>(*p_indp_focus,
					CursorEventArgs(*p_indp_focus, e.Keys,
					e.Position - LocateForWidget(sender, *p_indp_focus)));
				entered = {};
			}
		}
	}, 0xFF)
	);
}

GUIState&
FetchGUIState() ynothrow
{
	static_assert(std::is_nothrow_default_constructible<GUIState>(),
		"State class is invalid");
	static GUIState state;

	return state;
}


GUIConfiguration&
FetchGUIConfiguration()
{
	static GUIConfiguration conf;

	return conf;
}

} // namespace UI;

} // namespace YSLib;

