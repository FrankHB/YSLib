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
\version 0.4008;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-02-18 13:44:34 + 08:00;
\par 修改时间:
	2011-02-20 20:55 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YControl;
*/


#include "ycontrol.h"
#include "ygui.h"
#include "yuicont.h"
#include "ywindow.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

const ScreenPositionEventArgs
	ScreenPositionEventArgs::Empty = ScreenPositionEventArgs();


const InputEventArgs InputEventArgs::Empty = InputEventArgs();


const KeyEventArgs KeyEventArgs::Empty = KeyEventArgs();


const TouchEventArgs TouchEventArgs::Empty = TouchEventArgs();


void
OnKeyHeld(IVisualControl& c, KeyEventArgs& e)
{
	using namespace InputStatus;

	if(RepeatHeld(KeyHeldState, 240, 120))
		FetchEvent<KeyDown>(c)(c, e);
}

void
OnTouchHeld(IVisualControl& c, TouchEventArgs& e)
{
	using namespace InputStatus;

	if(DraggingOffset == Vec::FullScreen)
		DraggingOffset = c.GetLocation() - VisualControlLocation;
	else
		FetchEvent<TouchMove>(c)(c, e);
	LastVisualControlLocation = VisualControlLocation;
}

void
OnTouchMove(IVisualControl& c, TouchEventArgs& e)
{
	using namespace InputStatus;

	if(RepeatHeld(TouchHeldState, 240, 60))
		FetchEvent<TouchDown>(c)(c, e);
}

void
OnDrag(IVisualControl& c, TouchEventArgs&)
{
	using namespace InputStatus;

	if(LastVisualControlLocation != VisualControlLocation)
	{
		c.SetLocation(LastVisualControlLocation + DraggingOffset);
		c.Refresh();
	}
}


VisualControl::VisualControl(const Rect& r, IUIBox* pCon)
	: Widget(r, pCon), Control(), AFocusRequester()
{
	FetchEvent<GotFocus>(*this) += &VisualControl::OnGotFocus;
	FetchEvent<LostFocus>(*this) += &VisualControl::OnLostFocus;
	FetchEvent<TouchDown>(*this) += &VisualControl::OnTouchDown;
	FetchEvent<TouchHeld>(*this) += OnTouchHeld;

	IUIContainer* p(dynamic_cast<IUIContainer*>(GetContainerPtr()));

	if(p)
	{
		*p += static_cast<IWidget&>(*this);
		*p += static_cast<IVisualControl&>(*this);
	}
}
VisualControl::~VisualControl() ythrow()
{
	IUIContainer* p(dynamic_cast<IUIContainer*>(GetContainerPtr()));

	if(p)
	{
		*p -= static_cast<IWidget&>(*this);
		*p -= static_cast<IVisualControl&>(*this);
	}
}

bool
VisualControl::IsFocused() const
{
	IUIBox* p(GetContainerPtr());

	return p ? p->GetFocusingPtr() == this : false;
}

void
VisualControl::SetLocation(const Point& pt)
{
	Visual::SetLocation(pt);

	EventArgs e;

	GetEventMap().DoEvent<EventTypeMapping<Move>::HandlerType>(Move,
		*this, e);
}
void
VisualControl::SetSize(const Size& s)
{
	Visual::SetSize(s);

	EventArgs e;

	GetEventMap().DoEvent<EventTypeMapping<Resize>::HandlerType>(Resize,
		*this, e);
}

void
VisualControl::RequestFocus(EventArgs& e)
{
	IUIBox* p(GetContainerPtr());

	if(p && p->ResponseFocusRequest(*this))
		EventMap.GetEvent<HVisualEvent>(GotFocus)(*this, e);
}

void
VisualControl::ReleaseFocus(EventArgs& e)
{
	IUIBox* p(GetContainerPtr());

	if(p && p->ResponseFocusRelease(*this))
		EventMap.GetEvent<HVisualEvent>(LostFocus)(*this, e);
}

void
VisualControl::OnGotFocus(EventArgs&)
{
	Refresh();
}

void
VisualControl::OnLostFocus(EventArgs&)
{
	Refresh();
}

void
VisualControl::OnTouchDown(TouchEventArgs& e)
{
	RequestFocus(e);
}


YVisualControl::YVisualControl(const Rect& r, IUIBox* pCon)
	: YComponent(),
	VisualControl(r, pCon)
{}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

