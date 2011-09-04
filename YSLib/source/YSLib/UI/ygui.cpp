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
\version r3930;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2011-09-04 23:41 +0800;
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

YGUIShell::YGUIShell()
	: YShell(),
	KeyHeldState(Free), TouchHeldState(Free),
	DraggingOffset(Vec::FullScreen), HeldTimer(1000, false),
	ControlLocation(Point::FullScreen),
	LastControlLocation(Point::FullScreen), Colors(),
	p_KeyDown(), p_TouchDown(), control_entered(false)
{}

int
YGUIShell::OnGotMessage(const Message& msg)
{
	using namespace Messaging;

	switch(msg.GetMessageID())
	{
	case SM_PAINT:
		{
			auto h(FetchTarget<SM_PAINT>(msg));
			
			if(h)
				h->Refresh(FetchContext(*h),
					Point::Zero, Rect(Point::Zero, h->GetSize()));
		}
		return 0;
	default:
		break;
	}
	return YShell::OnGotMessage(msg);
}

bool
YGUIShell::RepeatHeld(HeldStateType& s,
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
YGUIShell::ResetHeldState(HeldStateType& s)
{
	Deactivate(HeldTimer);
	s = Free;
}

void
YGUIShell::ResetGUIStates()
{
	KeyHeldState = Free;
	TouchHeldState = Free;
	DraggingOffset = Vec::FullScreen;
	HeldTimer.SetInterval(1000);
	Deactivate(HeldTimer);
	ControlLocation = Point::FullScreen;
	LastControlLocation = Point::FullScreen;
	p_TouchDown = nullptr;
	p_KeyDown = nullptr;
}

void
YGUIShell::TryEntering(IWidget& wgt, TouchEventArgs& e)
{
	if(!control_entered)
	{
		CallEvent<Enter>(wgt, e);
		control_entered = true;
	}
}
void
YGUIShell::TryLeaving(IWidget& wgt, TouchEventArgs& e)
{
	if(control_entered)
	{
		CallEvent<Leave>(wgt, e);
		control_entered = false;
	}
}

void
YGUIShell::ResetTouchHeldState()
{
	ResetHeldState(TouchHeldState);
	DraggingOffset = Vec::FullScreen;
}

bool
YGUIShell::ResponseKeyBase(IWidget& wgt, KeyEventArgs& e,
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
		YAssert(false, "Invalid operation found @ YGUIShell::ResponseKeyBase;");
	}
	return true;
}

bool
YGUIShell::ResponseTouchBase(IWidget& wgt, TouchEventArgs& e,
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
			" @ YGUIShell::ResponseTouchBase;");
	}
	return true;
}

bool
YGUIShell::ResponseKey(IWidget& wgt, KeyEventArgs& e,
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

		auto t(pCon->GetFocusingPtr());

		if(!t || t == pCon)
			break;
		r |= ResponseKeyBase(*p, e, op);
		p = t;
	}

	YAssert(p, "Null pointer found @ YGUIShell::ResponseKey");

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
YGUIShell::ResponseTouch(IWidget& wgt, TouchEventArgs& e,
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
				pCon->ClearFocusingPtr();
			break;
		}
		r |= p->GetController().GetEventMap().DoEvent<HTouchEvent>(op, *p,
			std::move(e)) != 0;
		p = t;
		e -= p->GetLocation();
	};

	YAssert(p, "Null pointer found @ YGUIShell::ResponseTouch");

	e.Strategy = Components::RoutedEventArgs::Direct;
	r |= ResponseTouchBase(*p, e, op);
	e.Strategy = Components::RoutedEventArgs::Bubble;
	while(!e.Handled && (pCon = FetchContainerPtr(*p)))
	{
		e += p->GetLocation();
		p = pCon;
		r |= p->GetController().GetEventMap().DoEvent<HTouchEvent>(op, *p,
			std::move(e)) != 0;
	}
	return r;
}

YSL_END_NAMESPACE(Shells)

YGUIShell&
FetchGUIShell()
{
	shared_ptr<YGUIShell> hShl(dynamic_pointer_cast<YGUIShell>(
		FetchShellHandle()));

	YAssert(is_null(hShl), "Null handle found @ FetchGUIShell;");

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
IsFocusedByShell(const IWidget& wgt, const YGUIShell& shl)
{
	return shl.GetTouchDownPtr() == &wgt;
}

YSL_END_NAMESPACE(Components)

YSL_END

