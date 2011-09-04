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
\version r4431;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-02-18 13:44:34 +0800;
\par 修改时间:
	2011-09-03 15:47 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YComponent;
*/


#include "ygui.h"
#include "yuicont.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

void
Enable(IWidget& wgt, bool b)
{
	const auto need_invalidate(IsEnabled(wgt) != b);

	SetEnabledOf(wgt, b);
	if(need_invalidate)
		Invalidate(wgt);
}


bool
IsFocused(const IWidget& wgt)
{
	const auto p(FetchContainerPtr(wgt));

	return p ? p->GetFocusingPtr() == &wgt : false;
}

void
RequestFocusFrom(IWidget& dst, IWidget& src)
{
	if(auto p = FetchContainerPtr(dst))
		if(p->ResponseFocusRequest(dst))
			CallEvent<GotFocus>(dst, src, EventArgs());
}

void
ReleaseFocusFrom(IWidget& dst, IWidget& src)
{
	if(auto p = FetchContainerPtr(dst))
		if(p->ResponseFocusRelease(dst))
			CallEvent<LostFocus>(dst, src, EventArgs());
}


void
OnKeyHeld(IWidget& wgt, KeyEventArgs&& e)
{
	auto& shl(FetchGUIShell());

	if(shl.RepeatHeld(shl.KeyHeldState, 240, 120))
		FetchEvent<KeyDown>(wgt)(wgt, std::move(e));
}

void
OnTouchHeld(IWidget& wgt, TouchEventArgs&& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
	{
		auto& shl(FetchGUIShell());

		if(shl.DraggingOffset == Vec::FullScreen)
			shl.DraggingOffset = wgt.GetLocation() - shl.ControlLocation;
		else
			FetchEvent<TouchMove>(wgt)(wgt, std::move(e));
		shl.LastControlLocation = shl.ControlLocation;
	}
}

void
OnTouchMove(IWidget& wgt, TouchEventArgs&& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
	{
		auto& shl(FetchGUIShell());

		if(shl.RepeatHeld(shl.TouchHeldState, 240, 60))
			CallEvent<TouchDown>(wgt, e);
	}
}

void
OnTouchMove_Dragging(IWidget& wgt, TouchEventArgs&& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
	{
		auto& shl(FetchGUIShell());

	// TODO: analysis buffered coordinate delayed painting bug;
	//	if(hShl->LastControlLocation != hShl->ControlLocation)
	//	{
	// TODO: merge state to make a more efficient implementation;
		Invalidate(wgt);
		wgt.SetLocation(shl.LastControlLocation + shl.DraggingOffset);
		Invalidate(wgt);
	//	}
	}
}

namespace
{
	IWidget*
	FetchEnabledBoundControlPtr(IWidget& wgt, KeyEventArgs&& e)
	{
		try
		{
			auto pCtl(dynamic_cast<Control&>(wgt)
				.BoundControlPtr(e.GetKeyCode()));

			return pCtl && IsEnabled(*pCtl) ? pCtl : nullptr;
		}
		catch(std::bad_function_call&)
		{}
		catch(std::bad_cast&)
		{}
		return nullptr;
	}
}

void
OnKey_Bound_TouchUpAndLeave(IWidget& wgt, KeyEventArgs&& e)
{
	auto pCtl(FetchEnabledBoundControlPtr(wgt, std::move(e)));

	if(pCtl)
	{
		TouchEventArgs et(TouchEventArgs::FullScreen);

		CallEvent<TouchUp>(*pCtl, et);
		CallEvent<Leave>(*pCtl, et);
		e.Handled = true;
	}
}

void
OnKey_Bound_EnterAndTouchDown(IWidget& wgt, KeyEventArgs&& e)
{
	auto pCtl(FetchEnabledBoundControlPtr(wgt, std::move(e)));

	if(pCtl)
	{
		TouchEventArgs et(TouchEventArgs::FullScreen);

		CallEvent<Enter>(*pCtl, et);
		CallEvent<TouchDown>(*pCtl, et);
		e.Handled = true;
	}
}

void
OnKey_Bound_Click(IWidget& wgt, KeyEventArgs&& e)
{
	auto pCtl(FetchEnabledBoundControlPtr(wgt, std::move(e)));

	if(pCtl)
	{
		TouchEventArgs et(TouchEventArgs::FullScreen);

		CallEvent<Click>(*pCtl, et);
		e.Handled = true;
	}
}


Control::Control(const Rect& r)
	: Widget(r),
	controller(true)
{
	FetchEvent<TouchDown>(*this) += [this](IWidget&, TouchEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct)
			RequestFocus(*this);
	};
	FetchEvent<TouchHeld>(*this) += OnTouchHeld;
	FetchEvent<GotFocus>(*this) += [this](IWidget&, EventArgs&&){
		Invalidate(*this);
	};
	FetchEvent<LostFocus>(*this) += [this](IWidget&, EventArgs&&){
		Invalidate(*this);
	};
	BoundControlPtr = std::bind(std::mem_fn(&Control::GetBoundControlPtr), this,
		std::placeholders::_1);
}
Control::~Control()
{
	ReleaseFocus(*this);
}

void
Control::SetLocation(const Point& pt)
{
	Widget::SetLocation(pt);
	CallEvent<Move>(*this, EventArgs());
}
void
Control::SetSize(const Size& s)
{
	Widget::SetSize(s);
	CallEvent<Resize>(*this, EventArgs());
}

Rect
Control::Refresh(const Graphics& g, const Point& pt, const Rect& r)
{
	Widget::Refresh(g, pt, r);
	return GetBoundsOf(*this);
}

YSL_END_NAMESPACE(Components)

YSL_END

