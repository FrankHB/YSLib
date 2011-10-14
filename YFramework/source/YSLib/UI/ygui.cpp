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
\version r3972;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2011-10-08 23:48 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YGUI;
*/


#include "ygui.h"
#include "ydesktop.h"

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
				h->Refresh(PaintEventArgs(FetchContext(*h),
					Point::Zero, Rect(Point::Zero, h->GetSize())));
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
	yunsequenced(KeyHeldState = Free, TouchHeldState = Free,
		DraggingOffset = Vec::Invalid);
	HeldTimer.SetInterval(1000);
	Deactivate(HeldTimer);
	yunsequenced(ControlLocation = Point::Invalid,
		LastControlLocation = Point::Invalid,
		p_TouchDown = nullptr, p_KeyDown = nullptr);
}

void
GUIShell::TryEntering(IWidget& wgt, TouchEventArgs& e)
{
	if(!control_entered)
	{
		CallEvent<Enter>(wgt, e);
		control_entered = true;
	}
}
void
GUIShell::TryLeaving(IWidget& wgt, TouchEventArgs& e)
{
	if(control_entered)
	{
		CallEvent<Leave>(wgt, e);
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
GUIShell::ResponseKeyBase(IWidget& wgt, KeyEventArgs& e,
	Components::VisualEvent op)
{
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
GUIShell::ResponseTouchBase(IWidget& wgt, TouchEventArgs& e,
	Components::VisualEvent op)
{
	switch(op)
	{
	case TouchUp:
		ResetTouchHeldState();
		CallEvent<TouchUp>(wgt, e);
		if(p_TouchDown)
			TryLeaving(*p_TouchDown, e);
		if(p_TouchDown == &wgt)
		{
			CallEvent<Click>(wgt, e);
			p_TouchDown = nullptr;
		}
		break;
	case TouchDown:
		p_TouchDown = &wgt;
		TryEntering(wgt, e);
		CallEvent<TouchDown>(wgt, e);
		break;
	case TouchHeld:
		if(!p_TouchDown)
			return false;
		if(p_TouchDown == &wgt)
			TryEntering(wgt, e);
		else
		{
			const auto offset(LocateForWidget(wgt, *p_TouchDown));
			auto el(e);

			el -= offset;
			TryLeaving(*p_TouchDown, el);
			e += offset;
		}
		CallEvent<TouchHeld>(*p_TouchDown, e);
		break;
	default:
		YAssert(false, "Invalid operation found"
			" @ GUIShell::ResponseTouchBase;");
	}
	return true;
}

bool
GUIShell::ResponseKey(IWidget& wgt, KeyEventArgs& e,
	Components::VisualEvent op)
{
	auto p(&wgt);
	IWidget* pCon;
	bool r(false);

	e.Strategy = Components::RoutedEventArgs::Tunnel;
	while(true)
	{
		if(!(p->IsVisible() && IsEnabled(*p)))
			return false;
		if(e.Handled)
			return true;
		pCon = p;

		auto t(FetchFocusingPtr(*pCon));

		if(!t || t == pCon)
			break;
		r |= ResponseKeyBase(*p, e, op);
		p = t;
	}

	YAssert(p, "Null pointer found @ GUIShell::ResponseKey");

	e.Strategy = Components::RoutedEventArgs::Direct;
	r |= ResponseKeyBase(*p, e, op);
	e.Strategy = Components::RoutedEventArgs::Bubble;
	while(!e.Handled && (pCon = FetchContainerPtr(*p)))
	{
		p = pCon;
		r |= ResponseKeyBase(*p, e, op);
	}
	return r;
}

bool
GUIShell::ResponseTouch(IWidget& wgt, TouchEventArgs& e,
	Components::VisualEvent op)
{
	ControlLocation = e;

	auto p(&wgt);
	IWidget* pCon;
	bool r(false);

	e.Strategy = Components::RoutedEventArgs::Tunnel;
	while(true)
	{
		if(!(p->IsVisible() && IsEnabled(*p)))
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
				ClearFocusingPtrOf(*pCon);
			break;
		}
		r |= DoEvent<HTouchEvent>(p->GetController(), op, *p, e)
			!= 0;
		p = t;
		e -= p->GetLocation();
	};

	YAssert(p, "Null pointer found @ GUIShell::ResponseTouch");

	e.Strategy = Components::RoutedEventArgs::Direct;
	r |= ResponseTouchBase(*p, e, op);
	e.Strategy = Components::RoutedEventArgs::Bubble;
	while(!e.Handled && (pCon = FetchContainerPtr(*p)))
	{
		e += p->GetLocation();
		p = pCon;
		r |= DoEvent<HTouchEvent>(p->GetController(), op, *p, e)
			!= 0;
	}
	return r;
}

YSL_END_NAMESPACE(Shells)

GUIShell&
FetchGUIShell()
{
	shared_ptr<GUIShell> hShl(dynamic_pointer_cast<GUIShell>(
		FetchShellHandle()));

	YAssert(is_not_null(hShl), "Null handle found @ FetchGUIShell;");

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

