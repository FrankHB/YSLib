/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ygui.cpp
\ingroup UI
\brief 平台无关的图形用户界面。
\version r4020;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2011-12-05 07:15 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YGUI;
*/


#include "YSLib/UI/ygui.h"
#include "YSLib/UI/ydesktop.h"

YSL_BEGIN

using namespace Drawing;
using namespace Components;

YSL_BEGIN_NAMESPACE(Shells)

GUIShell::GUIShell()
	: Shell(),
	KeyHeldState(Free), TouchHeldState(Free),
	DraggingOffset(Vec::Invalid), HeldTimer(1000, false),
	ControlLocation(Point::Invalid),
	LastControlLocation(Point::Invalid), Colors(),
	p_KeyDown(), p_TouchDown(), control_entered(false)
{}

int
GUIShell::OnGotMessage(const Message& msg)
{
	using namespace Messaging;

	switch(msg.GetMessageID())
	{
	case SM_PAINT:
		{
			auto h(FetchTarget<SM_PAINT>(msg));
			
			if(h)
				h->Refresh(PaintContext(h->GetContext(), Point::Zero,
					Rect(Point::Zero, GetSizeOf(*h))));
		}
		return 0;
	default:
		break;
	}
	return Shell::OnGotMessage(msg);
}

bool
GUIShell::RepeatHeld(HeldStateType& s,
	Timers::TimeSpan InitialDelay, Timers::TimeSpan RepeatedDelay)
{
	//三状态自动机。
	switch(s)
	{
	case Free:
		/*
		必须立即转移状态，否则 Activate(HeldTimer) 会使 HeldTimer.Refresh()
		始终为 false ，导致状态无法转移。
		*/
		s = Pressed;
		HeldTimer.SetInterval(InitialDelay); //初始键按下延迟。
		Activate(HeldTimer);
		break;

	case Pressed:
	case Held:
		if(HeldTimer.Refresh())
		{
			if(s == Pressed)
			{
				s = Held;
				HeldTimer.SetInterval(RepeatedDelay); //重复键按下延迟。
			}
			return true;
		}
		break;
	}
	return false;
}

void
GUIShell::ResetHeldState(HeldStateType& s)
{
	Deactivate(HeldTimer);
	s = Free;
}

void
GUIShell::ResetGUIStates()
{
	yunseq(KeyHeldState = Free, TouchHeldState = Free,
		DraggingOffset = Vec::Invalid);
	HeldTimer.SetInterval(1000);
	Deactivate(HeldTimer);
	yunseq(ControlLocation = Point::Invalid,
		LastControlLocation = Point::Invalid,
		p_TouchDown = nullptr, p_KeyDown = nullptr);
}

void
GUIShell::TryEntering(TouchEventArgs&& e)
{
	if(!control_entered)
	{
		CallEvent<Enter>(e.GetSender(), e);
		control_entered = true;
	}
}
void
GUIShell::TryLeaving(TouchEventArgs&& e)
{
	if(control_entered)
	{
		CallEvent<Leave>(e.GetSender(), e);
		control_entered = false;
	}
}

void
GUIShell::ResetTouchHeldState()
{
	ResetHeldState(TouchHeldState);
	DraggingOffset = Vec::Invalid;
}

bool
GUIShell::ResponseKeyBase(KeyEventArgs& e, Components::VisualEvent op)
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
		YAssert(false, "Invalid operation found @ GUIShell::ResponseKeyBase;");
	}
	return true;
}

bool
GUIShell::ResponseTouchBase(TouchEventArgs& e, Components::VisualEvent op)
{
	auto& wgt(e.GetSender());

	switch(op)
	{
	case TouchUp:
		ResetTouchHeldState();
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
			TryLeaving(TouchEventArgs(*p_TouchDown,
				e - LocateForWidget(wgt, *p_TouchDown)));
		CallEvent<TouchHeld>(*p_TouchDown, e);
		break;
	default:
		YAssert(false, "Invalid operation found"
			" @ GUIShell::ResponseTouchBase;");
	}
	return true;
}

bool
GUIShell::ResponseKey(KeyEventArgs& e, Components::VisualEvent op)
{
	auto p(&e.GetSender());
	IWidget* pCon;
	bool r(false);

	e.Strategy = Components::RoutedEventArgs::Tunnel;
	while(true)
	{
		if(!(IsVisible(*p) && IsEnabled(*p)))
			return false;
		if(e.Handled)
			return true;
		pCon = p;

		auto t(FetchFocusingPtr(*pCon));

		if(!t || t == pCon)
			break;
		e.SetSender(*p);
		r |= ResponseKeyBase(e, op);
		p = t;
	}

	YAssert(p, "Null pointer found @ GUIShell::ResponseKey");

	e.Strategy = Components::RoutedEventArgs::Direct;
	e.SetSender(*p);
	r |= ResponseKeyBase(e, op);
	e.Strategy = Components::RoutedEventArgs::Bubble;
	while(!e.Handled && (pCon = FetchContainerPtr(*p)))
	{
		p = pCon;
		e.SetSender(*p);
		r |= ResponseKeyBase(e, op);
	}
	return r;
}

bool
GUIShell::ResponseTouch(TouchEventArgs& e, Components::VisualEvent op)
{
	ControlLocation = e;

	auto p(&e.GetSender());
	IWidget* pCon;
	bool r(false);

	e.Strategy = Components::RoutedEventArgs::Tunnel;
	while(true)
	{
		if(!(IsVisible(*p) && IsEnabled(*p)))
			return false;
		if(e.Handled)
			return true;
		pCon = p;
		if(op == TouchDown)
		{
			RequestToTop(*p);
			RequestFocus(*p);
		}

		auto t(pCon->GetTopWidgetPtr(e, IsEnabledAndVisible));

		if(!t || t == pCon)
		{
			if(op == TouchDown)
				ClearFocusingOf(*pCon);
			break;
		}
		e.SetSender(*p);
		r |= DoEvent<HTouchEvent>(p->GetController(), op, e) != 0;
		p = t;
		e -= GetLocationOf(*p);
	};

	YAssert(p, "Null pointer found @ GUIShell::ResponseTouch");

	e.Strategy = Components::RoutedEventArgs::Direct;
	e.SetSender(*p);
	r |= ResponseTouchBase(e, op);
	e.Strategy = Components::RoutedEventArgs::Bubble;
	while(!e.Handled && (pCon = FetchContainerPtr(*p)))
	{
		e += GetLocationOf(*p);
		p = pCon;
		e.SetSender(*p);
		r |= DoEvent<HTouchEvent>(p->GetController(), op, e) != 0;
	}
	return r;
}

YSL_END_NAMESPACE(Shells)

GUIShell&
FetchGUIShell()
{
	shared_ptr<GUIShell> hShl(dynamic_pointer_cast<GUIShell>(
		FetchShellHandle()));

	YAssert(bool(hShl), "Null handle found @ FetchGUIShell;");

	return *hShl;
}

YSL_BEGIN_NAMESPACE(Components)

void
RequestFocusCascade(IWidget& wgt)
{
	auto p(&wgt);

	do
	{
		RequestFocus(*p);
	}while((p = FetchContainerPtr(*p)));
}

void
ReleaseFocusCascade(IWidget& wgt)
{
	auto p(&wgt);

	do
	{
		ReleaseFocus(*p);
	}while((p = FetchContainerPtr(*p)));
}


bool
IsFocusedByShell(const IWidget& wgt, const GUIShell& shl)
{
	return shl.GetTouchDownPtr() == &wgt;
}

YSL_END_NAMESPACE(Components)

YSL_END

