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
\brief 平台无关的图形用户界面实现。
\version 0.3554;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2011-03-28 10:26 +0800;
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
	sWnds(), KeyHeldState(Free), TouchHeldState(Free),
	DraggingOffset(Vec::FullScreen), HeldTimer(1000, false),
	ControlLocation(Point::FullScreen),
	LastControlLocation(Point::FullScreen),
	p_KeyDown(NULL), p_TouchDown(NULL), control_entered(false)
{}

void
YGUIShell::operator+=(HWND h)
{
	if(h)
		sWnds.push_back(h);
}

bool
YGUIShell::operator-=(HWND h)
{
	WNDs::iterator i(std::find(sWnds.begin(), sWnds.end(), h));

	if(i == sWnds.end())
		return false;
	sWnds.erase(i);
	return true;
}

HWND
YGUIShell::GetFirstWindowHandle() const
{
	return HWND(sWnds.empty() ? NULL : sWnds.front());
}
HWND
YGUIShell::GetTopWindowHandle() const
{
	return HWND(sWnds.empty() ? NULL : sWnds.back());
}
HWND
YGUIShell::GetTopWindowHandle(YDesktop& d, const Point& p) const
{
	for(WNDs::const_iterator i(sWnds.begin()); i != sWnds.end(); ++i)
	{
		// TODO: assert(*i);

		if(FetchDirectDesktopPtr(**i) == &d && Contains(**i, p))
			return HWND(*i);
	}
	return NULL;
}
IWidget*
YGUIShell::GetCursorWidgetPtr(YDesktop& d, const Point& pt) const
{
	HWND hWnd(GetTopWindowHandle(d, pt));

	return hWnd ? hWnd->GetTopWidgetPtr(pt) : NULL;
}

void
YGUIShell::ClearScreenWindows(YDesktop& d)
{
	for(WNDs::const_iterator i(sWnds.begin()); i != sWnds.end(); ++i)
		d.RemoveAll(*static_cast<IControl*>(GetPointer(*i)));
}

void
YGUIShell::DispatchWindows()
{
	for(WNDs::const_iterator i(sWnds.begin()); i != sWnds.end(); ++i)
	{
		// TODO: assert(*i);

		YDesktop* const pDsk(FetchDirectDesktopPtr(**i));

		if(pDsk)
			*pDsk += *static_cast<IControl*>(GetPointer(*i));
	}
}

YGUIShell::WNDs::size_type YGUIShell::RemoveAll(HWND h)
{
	return ystdex::erase_all(sWnds, h);
}

void
YGUIShell::RemoveWindow()
{
	sWnds.pop_back();
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
	p_TouchDown = NULL;
	p_KeyDown = NULL;
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

IControl*
YGUIShell::GetFocusedEnabledVisualControlPtr(IControl* p)
{
	return p && p->IsVisible() && p->IsEnabled() ? p : NULL;
}

bool
YGUIShell::ResponseKeyUpBase(IControl& c, KeyEventArgs& e)
{
	ResetHeldState(KeyHeldState);
	if(p_KeyDown == &c && KeyHeldState == Free)
		CallEvent<KeyPress>(c, e);
	CallEvent<KeyUp>(c, e);
	CallEvent<Leave>(c, e);
	p_KeyDown = NULL;
	return true;
}

bool
YGUIShell::ResponseKeyDownBase(IControl& c, KeyEventArgs& e)
{
	p_KeyDown = &c;
	CallEvent<Enter>(c, e);
	CallEvent<KeyDown>(c, e);
	return true;
}

bool
YGUIShell::ResponseKeyHeldBase(IControl& c, KeyEventArgs& e)
{
	if(p_KeyDown != &c)
	{
		ResetHeldState(KeyHeldState);
		return false;
	}
	CallEvent<KeyHeld>(c, e);
	return true;
}

bool
YGUIShell::ResponseKey(YDesktop& d, KeyEventArgs& e,
	Components::Controls::VisualEvent op)
{
	IControl* const p(GetFocusedEnabledVisualControlPtr(
		Components::Controls::GetFocusedObjectPtr(d)));
	bool(YGUIShell::*pmf)(IControl&, KeyEventArgs&)(NULL);

	switch(op)
	{
	case KeyUp:
		pmf = &YGUIShell::ResponseKeyUpBase;
		break;
	case KeyDown:
		pmf = &YGUIShell::ResponseKeyDownBase;
		break;
	case KeyHeld:
		pmf = &YGUIShell::ResponseKeyHeldBase;
		break;
	default:
		YAssert(false, "Invalid function @@ YGUIShell::ResponseKeyBase;");
	}
	return (this->*pmf)(p ? *p : d, e);
}

bool
YGUIShell::ResponseTouch(IControl& c, TouchEventArgs& e,
	Components::Controls::VisualEvent op)
{
	ControlLocation = e;

	IControl* p(&c);
	IUIBox* pCon;

	e.Strategy = Controls::RoutedEventArgs::Tunnel;
	while(!e.IsHandled && (pCon = dynamic_cast<IUIBox*>(p)))
	{
		if(op == TouchDown)
		{
			RequestToTop(*p);
			p->RequestFocus(GetStaticRef<EventArgs>());
		}
		p->GetEventMap().DoEvent<HTouchEvent>(op, *p, e);
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

	YAssert(p, "Null pointer found @@ YGUIShell::ResponseTouch");

	e.Strategy = Controls::RoutedEventArgs::Direct;
	switch(op)
	{
	case TouchUp:
		ResetTouchHeldState();
		if(p_TouchDown == p && TouchHeldState == Free)
			CallEvent<Click>(*p, e);
		CallEvent<TouchUp>(*p, e);
		TryLeaving(*p, e);
		p_TouchDown = NULL;
		break;
	case TouchDown:
		p_TouchDown = p;
		TryEntering(*p, e);
		CallEvent<TouchDown>(*p, e);
		break;
	case TouchHeld:
		if(p_TouchDown != p)
		{
			if(p_TouchDown)
			{
				TouchEventArgs el(e);

				if(p)
					el += LocateForWidget(*p, *p_TouchDown);
				if(control_entered)
					TryLeaving(*p_TouchDown, el);
			}
		}
		else if(!control_entered)
			TryEntering(*p, e);
	/*	if(p_TouchDown != p)
		{
		ResetTouchHeldState();
		return false;
		}*/
		CallEvent<TouchHeld>(*p_TouchDown, e);
		break;
	default:
		YAssert(false, "Invalid operation found"
			" @@ YGUIShell::ResponseTouchBase;");
	}
	e.Strategy = Controls::RoutedEventArgs::Bubble;
	while(!e.IsHandled && (pCon = p->GetContainerPtr()))
	{
		e += p->GetLocation();
		p = dynamic_cast<IControl*>(pCon);
		p->GetEventMap().DoEvent<HTouchEvent>(op, *p, e);
	}
	return true;
}

bool
YGUIShell::SendWindow(IWindow& w)
{
	if(std::find(sWnds.begin(), sWnds.end(), &w) != sWnds.end())
	{
		YDesktop* const pDsk(FetchDirectDesktopPtr(w));

		if(pDsk)
		{
			*pDsk += static_cast<IControl&>(w);
			return true;
		}
	}
	return false;
}

int
YGUIShell::ShlProc(const Message& msg)
{
	using namespace Messaging;

	switch(msg.GetMessageID())
	{
	case SM_PAINT:
		{
			GHandleContext<GHHandle<YDesktop> >* const
				p(CastMessage<SM_PAINT>(msg));

			if(p && p->Handle)
				p->Handle->Draw();
		}
		return 0;
	default:
		break;
	}
	return ParentType::ShlProc(msg);
}

YSL_END_NAMESPACE(Shells)

GHHandle<YGUIShell>
FetchGUIShellHandle()
{
	return general_handle_cast<YGUIShell>(FetchShellHandle());
}

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

IControl*
GetFocusedObjectPtr(YDesktop& d)
{
	IControl* p(d.GetFocusingPtr()), *q(NULL);
	IUIBox* pCon;

	while(p && (pCon = dynamic_cast<IUIBox*>(p))
		&& (q = pCon->GetFocusingPtr()))
		p = q;
	return p;
}

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
IsFocusedByShell(const IControl& c, GHHandle<YGUIShell> hShl)
{
	if(!hShl)
		throw GeneralEvent("Null GUI handle found @@ YCheckBox"
			"::IsLockedByCurrentShell");
	return hShl->GetTouchDownPtr() == &c;
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

