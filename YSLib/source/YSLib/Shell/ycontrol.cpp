/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycontrol.cpp
\ingroup Shell
\brief 平台无关的控件实现。
\version 0.4117;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-02-18 13:44:34 +0800;
\par 修改时间:
	2011-04-17 23:00 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YComponent;
*/


#include "ycontrol.h"
#include "ygui.h"
#include "yuicont.h"
#include "ywindow.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

void
OnKeyHeld(IControl& c, KeyEventArgs& e)
{
	GHandle<YGUIShell> hShl(FetchGUIShellHandle());

	if(hShl->RepeatHeld(hShl->KeyHeldState, 240, 120))
		FetchEvent<KeyDown>(c)(c, e);
}

void
OnTouchHeld(IControl& c, TouchEventArgs& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
	{
		GHandle<YGUIShell> hShl(FetchGUIShellHandle());

		if(hShl->DraggingOffset == Vec::FullScreen)
			hShl->DraggingOffset = c.GetLocation() - hShl->ControlLocation;
		else
			FetchEvent<TouchMove>(c)(c, e);
		hShl->LastControlLocation = hShl->ControlLocation;
	}
}

void
OnTouchMove(IControl& c, TouchEventArgs& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
	{
		GHandle<YGUIShell> hShl(FetchGUIShellHandle());

		if(hShl->RepeatHeld(hShl->TouchHeldState, 240, 60))
			CallEvent<TouchDown>(c, e);
	}
}

void
OnTouchMove_Dragging(IControl& c, TouchEventArgs& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
	{
		GHandle<YGUIShell> hShl(FetchGUIShellHandle());

	// TODO: analysis buffered coordinate delayed painting bug;
	//	if(hShl->LastControlLocation != hShl->ControlLocation)
	//	{
			c.SetLocation(hShl->LastControlLocation + hShl->DraggingOffset);
			c.Refresh();
	//	}
	}
}


Control::Control(const Rect& r)
	: Widget(r), AFocusRequester(), enabled(true),
	EventMap()
{
	FetchEvent<GotFocus>(EventMap) += &Control::OnGotFocus;
	FetchEvent<LostFocus>(EventMap) += &Control::OnLostFocus;
	FetchEvent<TouchDown>(EventMap) += &Control::OnTouchDown;
	FetchEvent<TouchHeld>(EventMap) += OnTouchHeld;
}
Control::~Control()
{
	ReleaseFocus(GetStaticRef<EventArgs>());
}

bool
Control::IsFocused() const
{
	IUIBox* p(GetContainerPtr());

	return p ? p->GetFocusingPtr() == this : false;
}

void
Control::SetLocation(const Point& pt)
{
	Visual::SetLocation(pt);
	GetEventMap().DoEvent<EventTypeMapping<Move>::HandlerType>(Move,
		*this, GetStaticRef<EventArgs>());
}
void
Control::SetSize(const Size& s)
{
	Visual::SetSize(s);
	GetEventMap().DoEvent<EventTypeMapping<Resize>::HandlerType>(Resize,
		*this, GetStaticRef<EventArgs>());
}

void
Control::RequestFocus(EventArgs& e)
{
	IUIBox* p(GetContainerPtr());

	if(p && p->ResponseFocusRequest(*this))
		EventMap.GetEvent<HVisualEvent>(GotFocus)(*this, e);
}

void
Control::ReleaseFocus(EventArgs& e)
{
	IUIBox* p(GetContainerPtr());

	if(p && p->ResponseFocusRelease(*this))
		EventMap.GetEvent<HVisualEvent>(LostFocus)(*this, e);
}

void
Control::OnGotFocus(EventArgs&)
{
	Refresh();
}

void
Control::OnLostFocus(EventArgs&)
{
	Refresh();
}

void
Control::OnTouchDown(TouchEventArgs& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
		RequestFocus(e);
}


YControl::YControl(const Rect& r)
	: YComponent(),
	Control(r)
{}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

