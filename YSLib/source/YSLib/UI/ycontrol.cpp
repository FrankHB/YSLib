/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycontrol.cpp
\ingroup UI
\brief 样式无关的控件。
\version 0.4184;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-02-18 13:44:34 +0800;
\par 修改时间:
	2011-06-12 00:03 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YComponent;
*/


#include "ycontrol.h"
#include "ygui.h"
#include "yuicont.h"
#include "ywindow.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Controls)

void
OnKeyHeld(IControl& c, KeyEventArgs&& e)
{
	YGUIShell& shl(FetchGUIShell());

	if(shl.RepeatHeld(shl.KeyHeldState, 240, 120))
		FetchEvent<KeyDown>(c)(c, std::move(e));
}

void
OnTouchHeld(IControl& c, TouchEventArgs&& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
	{
		YGUIShell& shl(FetchGUIShell());

		if(shl.DraggingOffset == Vec::FullScreen)
			shl.DraggingOffset = c.GetLocation() - shl.ControlLocation;
		else
			FetchEvent<TouchMove>(c)(c, std::move(e));
		shl.LastControlLocation = shl.ControlLocation;
	}
}

void
OnTouchMove(IControl& c, TouchEventArgs&& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
	{
		YGUIShell& shl(FetchGUIShell());

		if(shl.RepeatHeld(shl.TouchHeldState, 240, 60))
			CallEvent<TouchDown>(c, e);
	}
}

void
OnTouchMove_Dragging(IControl& c, TouchEventArgs&& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
	{
		YGUIShell& shl(FetchGUIShell());

	// TODO: analysis buffered coordinate delayed painting bug;
	//	if(hShl->LastControlLocation != hShl->ControlLocation)
	//	{
			c.SetLocation(shl.LastControlLocation + shl.DraggingOffset);
			c.Refresh();
	//	}
	}
}


Control::Control(const Rect& r)
	: Widget(r), AFocusRequester(),
	enabled(true), EventMap()
{
	FetchEvent<GotFocus>(EventMap) += [this](IControl&, EventArgs&&){
		this->Refresh();
	};
	FetchEvent<LostFocus>(EventMap) += [this](IControl&, EventArgs&&){
		this->Refresh();
	};
	FetchEvent<TouchDown>(EventMap) += [this](IControl&, TouchEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct)
			RequestFocus();
	};
	FetchEvent<TouchHeld>(EventMap) += OnTouchHeld;
}
Control::~Control()
{
	ReleaseFocus();
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
	Widget::SetLocation(pt);
	GetEventMap().DoEvent<EventTypeMapping<Move>::HandlerType>(Move,
		*this, EventArgs());
}
void
Control::SetSize(const Size& s)
{
	Widget::SetSize(s);
	GetEventMap().DoEvent<EventTypeMapping<Resize>::HandlerType>(Resize,
		*this, EventArgs());
}

void
Control::RequestFocus()
{
	IUIBox* p(GetContainerPtr());

	if(p && p->ResponseFocusRequest(*this))
		EventMap.GetEvent<HVisualEvent>(GotFocus)(*this, EventArgs());
}

void
Control::ReleaseFocus()
{
	IUIBox* p(GetContainerPtr());

	if(p && p->ResponseFocusRelease(*this))
		EventMap.GetEvent<HVisualEvent>(LostFocus)(*this, EventArgs());
}

void
Control::OnKeyUp_Bound_TouchUpAndLeave(KeyEventArgs&& e)
{
	IControl* pCtl(GetBoundControlPtr(e.GetKey()));

	if(pCtl)
	{
		TouchEventArgs et(TouchEventArgs::FullScreen);

		CallEvent<TouchUp>(*pCtl, et);
		CallEvent<Leave>(*pCtl, et);
		e.Handled = true;
	}
}

void
Control::OnKeyDown_Bound_EnterAndTouchDown(KeyEventArgs&& e)
{
	IControl* pCtl(GetBoundControlPtr(e.GetKey()));

	if(pCtl)
	{
		TouchEventArgs et(TouchEventArgs::FullScreen);

		CallEvent<Enter>(*pCtl, et);
		CallEvent<TouchDown>(*pCtl, et);
		e.Handled = true;
	}
}

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

