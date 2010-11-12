/*
	Copyright (C) by Franksoft 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycontrol.cpp
\ingroup Shell
\brief 平台无关的控件实现。
\version 0.3250;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-02-18 13:44:34 + 08:00;
\par 修改时间:
	2010-11-12 15:09 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YControl;
*/


#include "ycontrol.h"
#include "ywindow.h"
#include "ygui.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

const ScreenPositionEventArgs
	ScreenPositionEventArgs::Empty = ScreenPositionEventArgs();


InputEventArgs InputEventArgs::Empty = InputEventArgs();


TouchEventArgs TouchEventArgs::Empty = TouchEventArgs();


KeyEventArgs KeyEventArgs::Empty = KeyEventArgs();


void
OnKeyHeld(IVisualControl& c, KeyEventArgs& e)
{
	using namespace InputStatus;

	if(RepeatHeld(KeyHeldState, 240, 120))
		c.GetKeyDown()(c, e);
}

void
OnTouchHeld(IVisualControl& c, TouchEventArgs& e)
{
	using namespace InputStatus;

	if(DraggingOffset == Vec::FullScreen)
		DraggingOffset = c.GetLocation() - VisualControlLocation;
	else
		c.GetTouchMove()(c, e);
	LastVisualControlLocation = VisualControlLocation;
}

void
OnTouchMove(IVisualControl& c, TouchEventArgs& e)
{
	using namespace InputStatus;

	if(RepeatHeld(TouchHeldState, 240, 60))
		c.GetTouchDown()(c, e);
}

void
OnDrag(IVisualControl& c, TouchEventArgs& e)
{
	using namespace InputStatus;

	if(LastVisualControlLocation != VisualControlLocation)
	{
		c.SetLocation(LastVisualControlLocation + DraggingOffset);
		c.Refresh();
	}
}


MVisualControl::MVisualControl()
	: Control(), AFocusRequester()
{}


AVisualControl::AVisualControl(HWND hWnd, const Rect& r, IUIBox* pCon)
	: Widget(hWnd, r, pCon), MVisualControl()
{
	EventMap[EControl::GotFocus] += &AVisualControl::OnGotFocus;
	EventMap[EControl::LostFocus] += &AVisualControl::OnLostFocus;
	TouchDown += &AVisualControl::OnTouchDown;
	TouchHeld += OnTouchHeld;

	IUIContainer* p(dynamic_cast<IUIContainer*>(GetContainerPtr()));

	if(p)
	{
		*p += static_cast<IWidget&>(*this);
		*p += static_cast<IVisualControl&>(*this);
	}
}
AVisualControl::~AVisualControl() ythrow()
{
	IUIContainer* p(dynamic_cast<IUIContainer*>(GetContainerPtr()));

	if(p)
	{
		*p -= static_cast<IWidget&>(*this);
		*p -= static_cast<IVisualControl&>(*this);
	}
}

void
AVisualControl::RequestFocus(EventArgs& e)
{
	IUIBox* p(GetContainerPtr());

	if(p && p->ResponseFocusRequest(*this))
		EventMap[EControl::GotFocus](*this, e);
}

void
AVisualControl::ReleaseFocus(EventArgs& e)
{
	IUIBox* p(GetContainerPtr());

	if(p && p->ResponseFocusRelease(*this))
		EventMap[EControl::LostFocus](*this, e);
}

void
AVisualControl::OnGotFocus(EventArgs&)
{
	Refresh();
}

void
AVisualControl::OnTouchDown(TouchEventArgs& e)
{
	RequestFocus(e);
}


YVisualControl::YVisualControl(HWND hWnd, const Rect& r, IUIBox* pCon)
	: YComponent(),
	AVisualControl(hWnd, r, pCon)
{}
YVisualControl::~YVisualControl() ythrow()
{}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

