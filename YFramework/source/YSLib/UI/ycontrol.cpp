/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycontrol.cpp
\ingroup UI
\brief 样式无关的控件。
\version r4697;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-02-18 13:44:34 +0800;
\par 修改时间:
	2012-08-22 10:09 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::UI::YComponent;
*/


#include "YSLib/UI/ycontrol.h"
#include "YSLib/UI/ygui.h"
#include "YSLib/UI/yuicont.h"
#include "YSLib/Core/ystorage.hpp"
#include <ystdex/algorithm.hpp>
#include "YSLib/UI/yrender.h"
#include "YSLib/UI/YBrush.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

EventMapping::ItemType&
GetEvent(EventMapping::MapType& m, const VisualEvent& id,
	EventMapping::MappedType(&f)())
{
	auto pr(ystdex::search_map(m, id));

	if(pr.second)
		pr.first = m.insert(pr.first, EventMapping::PairType(id, f()));
	return pr.first->second;
}


EventMapping::ItemType&
Controller::GetItemRef(const VisualEvent& id, EventMapping::MappedType(&f)())
{
	return GetEvent(EventMap, id, f);
}


bool
Enable(IWidget& wgt, bool b)
{
	const auto enability_changed(IsEnabled(wgt) != b);

	SetEnabledOf(wgt, b);
	if(enability_changed)
		Invalidate(wgt);
	return enability_changed;
}


void
OnKeyHeld(KeyEventArgs&& e)
{
	auto& st(FetchGUIState());

	if(st.HeldTimer.Refresh(st.KeyHeldState, Timers::TimeSpan(240),
		Timers::TimeSpan(60)))
		CallEvent<KeyDown>(e.GetSender(), e);
}

void
OnTouchDown_RequestToTopFocused(TouchEventArgs&& e)
{
	IWidget& wgt(e.GetSender());

	if(e.Strategy != RoutedEventArgs::Bubble)
		RequestToTop(wgt);
	if(e.Strategy == RoutedEventArgs::Direct)
		ClearFocusingOf(wgt);
	if(e.Strategy != RoutedEventArgs::Tunnel)
		RequestFocus(wgt);
}

void
OnTouchHeld(TouchEventArgs&& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
	{
		auto& st(FetchGUIState());

		if(st.GetTouchDownPtr())
		{
			auto& wgt(*st.GetTouchDownPtr());

			if(st.DraggingOffset == Vec::Invalid)
				st.DraggingOffset = GetLocationOf(wgt)
					- st.ControlLocation;
			else
				CallEvent<TouchMove>(wgt, e);
			st.LastControlLocation = st.ControlLocation;
		}
	}
}

void
OnTouchMove(TouchEventArgs&& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
	{
		auto& st(FetchGUIState());

		if(st.GetTouchDownPtr() && st.HeldTimer.Refresh(st.TouchHeldState,
			Timers::TimeSpan(240), Timers::TimeSpan(80)))
			CallEvent<TouchDown>(*st.GetTouchDownPtr(), e);
	}
}

void
OnTouchMove_Dragging(TouchEventArgs&& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
	{
		auto& st(FetchGUIState());

		if(st.GetTouchDownPtr())
		{
			auto& wgt(*st.GetTouchDownPtr());

		// TODO: analysis buffered coordinate delayed painting bug;
		//	if(st.LastControlLocation != st.ControlLocation)
		//	{
		// TODO: merge state to make a more efficient implementation;
			Invalidate(wgt);
			SetLocationOf(wgt, st.LastControlLocation + st.DraggingOffset);
		//	}
		}
	}
}

namespace
{
	IWidget*
	FetchEnabledBoundControlPtr(KeyEventArgs&& e)
	{
		try
		{
			auto pCtl(dynamic_cast<Control&>(e.GetSender())
				.BoundControlPtr(e.GetKeys()));

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
OnKey_Bound_TouchUpAndLeave(KeyEventArgs&& e)
{
	if(const auto pCtl = FetchEnabledBoundControlPtr(std::move(e)))
	{
		TouchEventArgs et(*pCtl, TouchEventArgs::Invalid);

		CallEvent<TouchUp>(*pCtl, et);
		CallEvent<Leave>(*pCtl, et);
		e.Handled = true;
	}
}

void
OnKey_Bound_EnterAndTouchDown(KeyEventArgs&& e)
{
	if(const auto pCtl = FetchEnabledBoundControlPtr(std::move(e)))
	{
		TouchEventArgs et(*pCtl, TouchEventArgs::Invalid);

		CallEvent<Enter>(*pCtl, et);
		CallEvent<TouchDown>(*pCtl, et);
		e.Handled = true;
	}
}

void
OnKey_Bound_Click(KeyEventArgs&& e)
{
	if(const auto pCtl = FetchEnabledBoundControlPtr(std::move(e)))
	{
		TouchEventArgs et(*pCtl, TouchEventArgs::Invalid);

		CallEvent<Click>(*pCtl, et);
		e.Handled = true;
	}
}


Control::ControlEventMap::ControlEventMap()
{
	yunseq(
		FetchEvent<TouchDown>(*this) += OnTouchDown_RequestToTopFocused,
		FetchEvent<TouchHeld>(*this) += OnTouchHeld
	);
}

Control::Control(const Rect& r)
	: Widget(new View(r), new Renderer(),
	new Controller(true, FetchPrototype<ControlEventMap>())),
	BoundControlPtr(std::bind(&Control::GetBoundControlPtr, this,
		std::placeholders::_1))
{
	const auto h([this](UIEventArgs&&){
		Invalidate(*this);
	});
	yunseq(
		FetchEvent<Move>(*this) += h,
		FetchEvent<Resize>(*this) += h,
		FetchEvent<GotFocus>(*this) += h,
		FetchEvent<LostFocus>(*this) += h
	);
}
Control::Control(const Control& ctl)
	: Widget(ctl), BoundControlPtr(ctl.BoundControlPtr)
{}

void
Control::OnTouch_Close(TouchEventArgs&&)
{
	Close(*this);
}

YSL_END_NAMESPACE(Components)

YSL_END

