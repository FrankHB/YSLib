/*
	© 2010-2016, 2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YWidgetEvent.cpp
\ingroup UI
\brief 标准部件事件定义。
\version r204
\author FrankHB <frankhb1989@gmail.com>
\since build 293
\par 创建时间:
	2010-05-01 13:52:56 +0800
\par 修改时间:
	2019-01-10 01:28 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YWidgetEvent
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_YWidgetEvent // for ystdex::search_map_by,
//	ystdex::emplace_hint_in_place;
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

EventRef
WidgetController::GetEvent(VisualEvent id) const
{
	if(id == UI::Paint)
		return Paint;
	throw BadEvent();
}


EventRef
GetMappedEvent(VisualEventMap& m, VisualEvent id, EventItem(&f)())
{
	return ystdex::search_map_by([&](VisualEventMap::const_iterator i){
		return ystdex::emplace_hint_in_place(m, i, id, f());
	}, m, id).first->second;
}

} // namespace UI;

} // namespace YSLib;

