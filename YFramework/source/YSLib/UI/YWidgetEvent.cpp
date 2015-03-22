/*
	© 2010-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YWidgetEvent.cpp
\ingroup UI
\brief 标准部件事件定义。
\version r186
\author FrankHB <frankhb1989@gmail.com>
\since build 293
\par 创建时间:
	2010-05-01 13:52:56 +0800
\par 修改时间:
	2015-03-21 16:15 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YWidgetEvent
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_YWidgetEvent
#include YFM_YSLib_UI_YWidget

namespace YSLib
{

namespace UI
{

ImplDeDtor(UIEventArgs)


ImplDeDtor(RoutedEventArgs)


InputEventArgs::InputEventArgs(IWidget& wgt, const KeyInput& k,
	RoutingStrategy s)
	: RoutedEventArgs(wgt, s), Keys(k)
{}
ImplDeDtor(InputEventArgs)


KeyEventArgs::KeyEventArgs(IWidget& wgt, const InputType& k, RoutingStrategy s)
	: InputEventArgs(wgt, k, s)
{}
ImplDeDtor(KeyEventArgs)


CursorEventArgs::CursorEventArgs(IWidget& wgt, const KeyInput& k,
	const InputType& pt, RoutingStrategy s)
	: InputEventArgs(wgt, k, s), Position(pt)
{}
ImplDeDtor(CursorEventArgs)


CursorWheelEventArgs::CursorWheelEventArgs(IWidget& wgt, WheelDelta d,
	const KeyInput& k, const InputType& pt, RoutingStrategy s)
	: CursorEventArgs(wgt, k, pt, s), delta(d)
{}
ImplDeDtor(CursorWheelEventArgs)


TextInputEventArgs::TextInputEventArgs(IWidget& wgt, String str,
	const KeyInput& k, RoutingStrategy s)
	: InputEventArgs(wgt, k, s), Text(std::move(str))
{}
TextInputEventArgs::TextInputEventArgs(InputEventArgs e, String str)
	: InputEventArgs(std::move(e)), Text(std::move(str))
{}
ImplDeDtor(TextInputEventArgs)


PaintEventArgs::PaintEventArgs(IWidget& wgt)
	: UIEventArgs(wgt), PaintContext()
{}
PaintEventArgs::PaintEventArgs(IWidget& wgt, const PaintContext& pc)
	: UIEventArgs(wgt), PaintContext(pc)
{}
PaintEventArgs::PaintEventArgs(IWidget& wgt, const Drawing::Graphics& g,
	const Drawing::Point& pt, const Drawing::Rect& r)
	: UIEventArgs(wgt), PaintContext{g, pt, r}
{}
ImplDeDtor(PaintEventArgs)


ImplDeDtor(BadEvent)


ImplDeDtor(UIEventSignal)


ImplDeDtor(AController)


WidgetController::WidgetController(bool b)
	: AController(b),
	Paint()
{}

EventMapping::ItemType&
WidgetController::GetItem(VisualEvent id) const
{
	if(id == UI::Paint)
		return Paint;
	throw BadEvent();
}


EventMapping::ItemType&
GetEvent(EventMapping::MapType& m, VisualEvent id,
	EventMapping::MappedType(&f)())
{
	auto pr(ystdex::search_map(m, id));

	if(pr.second)
		pr.first = m.emplace_hint(pr.first, EventMapping::PairType(id, f()));
	return pr.first->second;
}

} // namespace UI;

} // namespace YSLib;

