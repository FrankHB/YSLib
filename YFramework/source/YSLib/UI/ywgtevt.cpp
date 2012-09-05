/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywgtevt.cpp
\ingroup UI
\brief 标准部件事件定义。
\version r128
\author FrankHB<frankhb1989@gmail.com>
\since build 293
\par 创建时间:
	2010-05-01 13:52:56 +0800
\par 修改时间:
	2012-09-04 12:46 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YWidgetEvent
*/


#include "YSLib/UI/ywgtevt.h"
#include "YSLib/UI/ywidget.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

InputEventArgs::InputEventArgs(IWidget& wgt, const KeyInput& k,
	RoutingStrategy s)
	: RoutedEventArgs(wgt, s), Keys(k)
{}


KeyEventArgs::KeyEventArgs(IWidget& wgt, const InputType& k, RoutingStrategy s)
	: InputEventArgs(wgt, k, s)
{}


TouchEventArgs::TouchEventArgs(IWidget& wgt, const InputType& pt,
	RoutingStrategy s)
	: InputEventArgs(wgt, 0, s), MScreenPositionEventArgs(pt)
{}


PaintEventArgs::PaintEventArgs(IWidget& wgt)
	: UIEventArgs(wgt), PaintContext()
{}
PaintEventArgs::PaintEventArgs(IWidget& wgt, const PaintContext& pc)
	: UIEventArgs(wgt), PaintContext(pc)
{}
PaintEventArgs::PaintEventArgs(IWidget& wgt, const Drawing::Graphics& g,
	const Drawing::Point& pt, const Drawing::Rect& r)
	: UIEventArgs(wgt), PaintContext(g, pt, r)
{}


WidgetController::WidgetController(bool b)
	: AController(b),
	Paint()
{}

EventMapping::ItemType&
WidgetController::GetItem(const VisualEvent& id)
{
	if(id == Components::Paint)
		return Paint;
	throw BadEvent();
}

YSL_END_NAMESPACE(Components)

YSL_END

