/*
	Copyright by FrankHB 2010 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycontrol.cpp
\ingroup UI
\brief 样式无关的控件。
\version r3769
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-02-18 13:44:34 +0800
\par 修改时间:
	2013-07-04 02:32 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YControl
*/


#include "YSLib/UI/ycontrol.h"
#include "YSLib/UI/ygui.h"
#include "YSLib/UI/yuicont.h"
#include "YSLib/Core/ystorage.hpp"
#include <ystdex/algorithm.hpp>
#include "YSLib/UI/YBrush.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(UI)

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

	if(st.HeldTimer.RefreshHeld(st.KeyHeldState, Timers::TimeSpan(240),
		Timers::TimeSpan(60)))
		CallEvent<KeyDown>(e.GetSender(), e);
}

void
OnTouchDown_RequestToTopFocused(CursorEventArgs&& e)
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
OnTouchHeld(CursorEventArgs&& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
	{
		auto& st(FetchGUIState());

		if(st.GetTouchDownPtr() && st.HeldTimer.RefreshHeld(st.TouchHeldState,
			Timers::TimeSpan(240), Timers::TimeSpan(80)))
			CallEvent<TouchDown>(*st.GetTouchDownPtr(), e);
	}
}

void
OnTouchHeld_Dragging(CursorEventArgs&& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
	{
		auto& st(FetchGUIState());

		if(st.GetTouchDownPtr())
		{
			auto& wgt(*st.GetTouchDownPtr());

		// TODO: Analysis buffered coordinate delayed painting bug.
		//	if(st.LastControlLocation != st.ControlLocation)
		//	{
		// TODO: Merge state to make a more efficient implementation.
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
		auto pCtl(dynamic_cast<Control&>(e.GetSender()).BoundControlPtr(
			e.GetKeys()));

		return pCtl && IsEnabled(*pCtl) ? pCtl : nullptr;
	}
	catch(std::bad_function_call&)
	{}
	catch(std::bad_cast&)
	{}
	return nullptr;
}

} // unnamed namespace;

void
OnKey_Bound_TouchUpAndLeave(KeyEventArgs&& e)
{
	if(const auto pCtl = FetchEnabledBoundControlPtr(std::move(e)))
	{
		CursorEventArgs et(*pCtl, e.Keys, CursorEventArgs::Invalid);

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
		CursorEventArgs et(*pCtl, e.Keys, CursorEventArgs::Invalid);

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
		CursorEventArgs et(*pCtl, e.Keys, CursorEventArgs::Invalid);

		CallEvent<Click>(*pCtl, et);
		e.Handled = true;
	}
}


Control::ControlEventMap::ControlEventMap()
{
	FetchEvent<TouchDown>(*this) += OnTouchDown_RequestToTopFocused;
}

Control::Control(const Rect& r)
	: Control(r, NoBackgroundTag())
{
	Background = SolidBrush(ColorSpace::White);
}
Control::Control(const Rect& r, NoBackgroundTag)
	: Widget(new View(r), new Renderer(),
	new Controller(true, FetchPrototype<ControlEventMap>())),
	BoundControlPtr(std::bind(&Control::GetBoundControlPtr, this,
		std::placeholders::_1))
{
	const auto h([this](UIEventArgs&&){
		Invalidate(*this);
	});

	FetchGUIState().Wrap(*this),
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
Control::OnTouch_Close(CursorEventArgs&&)
{
	Close(*this);
}

YSL_END_NAMESPACE(UI)

YSL_END

