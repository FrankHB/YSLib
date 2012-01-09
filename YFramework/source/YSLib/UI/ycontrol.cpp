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
\version r4626;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-02-18 13:44:34 +0800;
\par 修改时间:
	2012-01-04 10:38 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::UI::YComponent;
*/


#include "YSLib/UI/ygui.h"
#include "YSLib/UI/yuicont.h"
#include "YSLib/Core/ystorage.hpp"
#include <ystdex/algorithm.hpp>

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


void
Enable(IWidget& wgt, bool b)
{
	const auto need_invalidate(IsEnabled(wgt) != b);

	SetEnabledOf(wgt, b);
	if(need_invalidate)
		Invalidate(wgt);
}


void
OnKeyHeld(KeyEventArgs&& e)
{
	auto& shl(FetchGUIShell());

	if(shl.RepeatHeld(shl.KeyHeldState, 240, 120))
		CallEvent<KeyDown>(e.GetSender(), e);
}

void
OnTouchDown_RequestToTopFocused(TouchEventArgs&& e)
{
	if(e.Strategy != RoutedEventArgs::Bubble)
	{
		IWidget& wgt(e.GetSender());

		RequestToTop(wgt);
		RequestFocus(wgt);
	}
}

void
OnTouchHeld(TouchEventArgs&& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
	{
		auto& shl(FetchGUIShell());

		if(shl.GetTouchDownPtr())
		{
			auto& wgt(*shl.GetTouchDownPtr());

			if(shl.DraggingOffset == Vec::Invalid)
				shl.DraggingOffset = GetLocationOf(wgt)
					- shl.ControlLocation;
			else
				CallEvent<TouchMove>(wgt, e);
			shl.LastControlLocation = shl.ControlLocation;
		}
	}
}

void
OnTouchMove(TouchEventArgs&& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
	{
		auto& shl(FetchGUIShell());

		if(shl.GetTouchDownPtr() && shl.RepeatHeld(shl.TouchHeldState, 240, 60))
			CallEvent<TouchDown>(*shl.GetTouchDownPtr(), e);
	}
}

void
OnTouchMove_Dragging(TouchEventArgs&& e)
{
	if(e.Strategy == RoutedEventArgs::Direct)
	{
		auto& shl(FetchGUIShell());

		if(shl.GetTouchDownPtr())
		{
			auto& wgt(*shl.GetTouchDownPtr());

		// TODO: analysis buffered coordinate delayed painting bug;
		//	if(hShl->LastControlLocation != hShl->ControlLocation)
		//	{
		// TODO: merge state to make a more efficient implementation;
			Invalidate(wgt);
			SetLocationOf(wgt, shl.LastControlLocation + shl.DraggingOffset);
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
OnKey_Bound_TouchUpAndLeave(KeyEventArgs&& e)
{
	auto pCtl(FetchEnabledBoundControlPtr(std::move(e)));

	if(pCtl)
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
	auto pCtl(FetchEnabledBoundControlPtr(std::move(e)));

	if(pCtl)
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
	auto pCtl(FetchEnabledBoundControlPtr(std::move(e)));

	if(pCtl)
	{
		TouchEventArgs et(*pCtl, TouchEventArgs::Invalid);

		CallEvent<Click>(*pCtl, et);
		e.Handled = true;
	}
}


Control::ControlEventMap::ControlEventMap()
{
	FetchEvent<Paint>(*this) += Render;
	FetchEvent<TouchDown>(*this) += [](TouchEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct)
			RequestFocus(e.GetSender());
	};
	FetchEvent<TouchHeld>(*this) += OnTouchHeld;
}

Control::Control(const Rect& r)
	: Widget(new View(r), new Renderer(),
	new Controller(true, FetchPrototype<ControlEventMap>()))
{
//	const auto& h([this](EventArgs&&){
//		Invalidate(*this);
//	});
// FIXME: code above causes g++ 4.6 internal compiler error: 
//	 in gimple_expand_cfg, at cfgexpand.c:4063
	yunseq(
		FetchEvent<Move>(*this) += [this](UIEventArgs&&){
			Invalidate(*this);
		},
		FetchEvent<Resize>(*this) += [this](UIEventArgs&&){
			Invalidate(*this);
		},
		FetchEvent<TouchDown>(*this) += OnTouchDown_RequestToTopFocused,
		FetchEvent<GotFocus>(*this) += [this](UIEventArgs&&){
			Invalidate(*this);
		},
		FetchEvent<LostFocus>(*this) += [this](UIEventArgs&&){
			Invalidate(*this);
		}
	);
	BoundControlPtr = std::bind(std::mem_fn(&Control::GetBoundControlPtr), this,
		std::placeholders::_1);
}
Control::Control(const Control& ctl)
	: Widget(ctl), BoundControlPtr(ctl.BoundControlPtr)
{}

YSL_END_NAMESPACE(Components)

YSL_END

