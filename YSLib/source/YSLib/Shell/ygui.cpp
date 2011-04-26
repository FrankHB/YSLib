/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ygui.cpp
\ingroup Shell
\brief 平台无关的图形用户界面。
\version 0.3755;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2011-04-25 12:49 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YGUI;
*/


#include "ygui.h"
#include "ywindow.h"
#include "ydesktop.h"

YSL_BEGIN

using namespace Drawing;
using namespace Components;
using namespace Components::Controls;
using namespace Components::Widgets;

YSL_BEGIN_NAMESPACE(Shells)

YGUIShell::YGUIShell()
	: YShell(),
	KeyHeldState(Free), TouchHeldState(Free),
	DraggingOffset(Vec::FullScreen), HeldTimer(1000, false),
	ControlLocation(Point::FullScreen),
	LastControlLocation(Point::FullScreen), Colors(),
	p_KeyDown(nullptr), p_TouchDown(nullptr), control_entered(false)
{}

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
YGUIShell::TryEntering(IControl& c, TouchEventArgs& e)
{
	if(!control_entered && p_TouchDown == &c)
	{
		CallEvent<Enter>(c, e);
		control_entered = true;
	}
}
void
YGUIShell::TryLeaving(IControl& c, TouchEventArgs& e)
{
	if(control_entered && p_TouchDown == &c)
	{
		CallEvent<Leave>(c, e);
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
YGUIShell::ResponseKeyBase(IControl& c, KeyEventArgs& e,
	Components::Controls::VisualEvent op)
{
	switch(op)
	{
	case KeyUp:
		ResetHeldState(KeyHeldState);
		CallEvent<KeyUp>(c, e);
		if(p_KeyDown == &c)
		{
			CallEvent<KeyPress>(c, e);
			p_KeyDown = nullptr;
		}
		break;
	case KeyDown:
		p_KeyDown = &c;
		CallEvent<KeyDown>(c, e);
		break;
	case KeyHeld:
	/*	if(e.Strategy == RoutedEventArgs::Direct && p_KeyDown != &c)
		{
			ResetHeldState(KeyHeldState);
			return false;
		}*/
		CallEvent<KeyHeld>(c, e);
		break;
	default:
		YAssert(false, "Invalid function @ YGUIShell::ResponseKeyBase;");
	}
	return true;
}

bool
YGUIShell::ResponseTouchBase(IControl& c, TouchEventArgs& e,
	Components::Controls::VisualEvent op)
{
	switch(op)
	{
	case TouchUp:
		ResetTouchHeldState();
		CallEvent<TouchUp>(c, e);
		TryLeaving(c, e);
		if(p_TouchDown == &c)
		{
			CallEvent<Click>(c, e);
			p_TouchDown = nullptr;
		}
		break;
	case TouchDown:
		p_TouchDown = &c;
		TryEntering(c, e);
		CallEvent<TouchDown>(c, e);
		break;
	case TouchHeld:
		if(p_TouchDown != &c)
		{
			if(p_TouchDown)
			{
				TouchEventArgs el(e);

				el += LocateForWidget(c, *p_TouchDown);
				if(control_entered)
					TryLeaving(*p_TouchDown, el);
			}
		}
		else if(!control_entered)
			TryEntering(c, e);
	/*	if(p_TouchDown != &c)
		{
		ResetTouchHeldState();
		return false;
		}*/
		CallEvent<TouchHeld>(*p_TouchDown, e);
		break;
	default:
		YAssert(false, "Invalid operation found"
			" @ YGUIShell::ResponseTouchBase;");
	}
	return true;
}

bool
YGUIShell::ResponseKey(IControl& c, KeyEventArgs& e,
	Components::Controls::VisualEvent op)
{
	IControl* p(&c);
	IUIBox* pCon;
	bool r(false);

	e.Strategy = Controls::RoutedEventArgs::Tunnel;
	while((pCon = dynamic_cast<IUIBox*>(p)))
	{
		if(!(p->IsVisible() && p->IsEnabled()))
			return false;
		if(e.Handled)
			return true;
		r |= ResponseKeyBase(*p, e, op);

		IControl* t(pCon->GetFocusingPtr());

		if(!t)
			break;
		p = t;
	}

	YAssert(p, "Null pointer found @ YGUIShell::ResponseKey");

	e.Strategy = Controls::RoutedEventArgs::Direct;
	r |= ResponseKeyBase(*p, e, op);
	e.Strategy = Controls::RoutedEventArgs::Bubble;
	while(!e.Handled && (pCon = p->GetContainerPtr()))
	{
		p = dynamic_cast<IControl*>(pCon);
		if(!p)
			break;
		r |= ResponseKeyBase(*p, e, op);
	}
	return r;
}

bool
YGUIShell::ResponseTouch(IControl& c, TouchEventArgs& e,
	Components::Controls::VisualEvent op)
{
	ControlLocation = e;

	IControl* p(&c);
	IUIBox* pCon;
	bool r(false);

	e.Strategy = Controls::RoutedEventArgs::Tunnel;
	while((pCon = dynamic_cast<IUIBox*>(p)))
	{
		if(!(p->IsVisible() && p->IsEnabled()))
			return false;
		if(e.Handled)
			return true;
		if(op == TouchDown)
		{
			RequestToTop(*p);
			p->RequestFocus(GetStaticRef<EventArgs>());
		}
		r |= p->GetEventMap().DoEvent<HTouchEvent>(op, *p, e) != 0;
		e -= p->GetLocation();

		IControl* t;

		if(!(t = pCon->GetTopControlPtr(e)))
		{
			if(pCon && op == TouchDown)
				pCon->ClearFocusingPtr();
			break;
		}
		p = t;
	}

	YAssert(p, "Null pointer found @ YGUIShell::ResponseTouch");

	e.Strategy = Controls::RoutedEventArgs::Direct;
	r |= ResponseTouchBase(*p, e, op);
	e.Strategy = Controls::RoutedEventArgs::Bubble;
	while(!e.Handled && (pCon = p->GetContainerPtr()))
	{
		e += p->GetLocation();
		p = dynamic_cast<IControl*>(pCon);
		if(!p)
			break;
		r |= p->GetEventMap().DoEvent<HTouchEvent>(op, *p, e) != 0;
	}
	return r;
}

int
YGUIShell::ShlProc(const Message& msg)
{
	using namespace Messaging;

	switch(msg.GetMessageID())
	{
	case SM_PAINT:
		{
			GHandleContext<GHandle<YDesktop> >* const
				p(CastMessage<SM_PAINT>(msg));

			if(p && p->Handle)
				p->Handle->Paint();
		}
		return 0;
	default:
		break;
	}
	return ParentType::ShlProc(msg);
}

YSL_END_NAMESPACE(Shells)

YGUIShell&
FetchGUIShell()
{
	GHandle<YGUIShell> hShl(dynamic_pointer_cast<YGUIShell>(
		FetchShellHandle()));

	YAssert(hShl, "Invalid handle found @ FetchGUIShell;");

	return *hShl;
}

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

void
RequestFocusCascade(IControl& c)
{
	IControl* p(&c);

	do
	{
		p->RequestFocus(GetStaticRef<EventArgs>());
	}while((p = dynamic_cast<IControl*>(p->GetContainerPtr())));
}

void
ReleaseFocusCascade(IControl& c)
{
	IControl* p(&c);

	do
	{
		p->ReleaseFocus(GetStaticRef<EventArgs>());
	}while((p = dynamic_cast<IControl*>(p->GetContainerPtr())));
}


bool
IsFocusedByShell(const IControl& c, const YGUIShell& shl)
{
	return shl.GetTouchDownPtr() == &c;
}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Drawing)

namespace
{
	void
	DrawWidgetBounds(IWindow& w, const Point& p, const Size& s, Color c)
	{
		DrawRect(w.GetContext(), p, s, c);
	}
}

void
DrawWindowBounds(IWindow* pWnd, Color c)
{
	YAssert(pWnd, "Window pointer is null.");

	DrawWidgetBounds(*pWnd, Point::Zero, pWnd->GetSize(), c);
}

void
DrawWidgetBounds(IWidget& w, Color c)
{
	IWindow* pWnd(FetchDirectWindowPtr(w));

	if(pWnd)
		DrawWidgetBounds(*pWnd, LocateOffset(&w, Point::Zero, pWnd),
			w.GetSize(), c);
}

void
DrawWidgetOutline(IWidget& w, Color c)
{
	IWindow* pWnd(FetchWindowPtr(w));

	if(pWnd)
		DrawWidgetBounds(*pWnd, LocateOffset(&w, Point::Zero, pWnd),
			w.GetSize(), c);
}

YSL_END_NAMESPACE(Drawing)

YSL_END

