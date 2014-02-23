/*
	© 2011-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Selector.cpp
\ingroup UI
\brief 样式相关的图形用户界面选择控件。
\version r950
\author FrankHB <frankhb1989@gmail.com>
\since build 282
\par 创建时间:
	2011-03-22 07:20:06 +0800
\par 修改时间:
	2014-02-23 18:10 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Selector
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_Selector
#include YFM_YSLib_Service_YBlit
#include YFM_YSLib_UI_YGUI
#include <ystdex/cast.hpp>

namespace YSLib
{

using namespace Drawing;

namespace UI
{

namespace
{

//! \since build 479
void
RectDrawCheckBox(const PaintContext& pc, const Size& s, Hue base_hue,
	CursorState cursor_state, bool is_ticked, bool is_enabled)
{
	const bool inside(cursor_state != CursorState::Outside);
	const bool is_pressed(cursor_state == CursorState::Pressed);
	const SDst rad(min(s.Width, s.Height));
	const auto& g(pc.Target);
	const auto pt(pc.Location + Size(rad, rad));
	const auto& bounds(pc.ClipArea);
	const Rect r(pc.Location, s);

	FillRect(g, bounds, r, is_enabled ? (inside ? RollColor(is_pressed
		? hsl_t{30.F, 1.F, .925F} : hsl_t{30.F, 1.F, .976F}, base_hue)
		: ColorSpace::White) : MakeGray(230));
	DrawRect(g, bounds, r, is_enabled ? (inside ? RollColor(is_pressed
		? hsl_t{26.5F, 1.F, .435F} : hsl_t{30.F, 1.F, .6F}, base_hue)
		: MakeGray(112)) : MakeGray(188));

	if(is_ticked)
	{
		const auto c(is_enabled ? ColorSpace::Black : MakeGray(112));
		const Point p1(r.X + 2, r.Y + r.Height / 2),
			p3(r.X + r.Width - 2, r.Y + 1);
		Point p2(r.X + r.Width / 2 - 1, r.Y + r.Height - 3);

		p2 += Vec(0, -1);
		DrawLineSeg(g, bounds, p1 + Vec(1, 0), p2, c);
		DrawLineSeg(g, bounds, p2, p3 + Vec(-1, 0), c);
		p2 += Vec(0, 2);
		DrawLineSeg(g, bounds, p1 + Vec(0, 1), p2, c);
		DrawLineSeg(g, bounds, p2, p3 + Vec(0, 1), c);
		p2 += Vec(0, -1);
		DrawLineSeg(g, bounds, p1, p2, c);
		DrawLineSeg(g, bounds, p2, p3, c);
	}
}

//! \since build 479
void
RectDrawRadioBox(const PaintContext& pc, const Size& s, Hue base_hue,
	CursorState cursor_state, bool is_ticked, bool is_enabled)
{
	const bool inside(cursor_state != CursorState::Outside);
	const bool is_pressed(cursor_state == CursorState::Pressed);
	const SDst rad(min(s.Width / 2, s.Height / 2));
	const auto& g(pc.Target);
	const auto pt(pc.Location + Size(rad, rad));
	const auto& bounds(pc.ClipArea);
	const Rect r(pc.Location, s);

	FillCircle(g, bounds, pt, rad, is_enabled ? (inside ? RollColor(is_pressed
		? hsl_t{30.F, 1.F, .925F} : hsl_t{30.F, 1.F, .976F}, base_hue)
		: MakeGray(112)) : MakeGray(188));
	DrawCircle(g, bounds, pt, rad, is_enabled ? (inside ? RollColor(is_pressed
		? hsl_t{26.5F, 1.F, .435F} : hsl_t{30.F, 1.F, .6F}, base_hue)
		: ColorSpace::White) : MakeGray(230));
	// XXX: Minimal size.
	if(is_ticked && YB_LIKELY(r.Width > 4 && r.Height > 4))
		FillCircle(g, bounds, pt, rad - 2, ColorSpace::Black);
}

} // unnamed namespace;


CheckBox::CheckBox(const Rect& r)
	: Thumb(r, NoBackgroundTag()), MCheckBox()
{
	using namespace Styles;
	static struct Init
	{
		Init()
		{
			AddHandlers<CheckBox>(FetchDefault(), {{CheckBoxBackground,
				[](PaintEventArgs&& e){
					auto& cb(
						ystdex::polymorphic_downcast<CheckBox&>(e.GetSender()));

					RectDrawCheckBox(e, GetSizeOf(cb), cb.GetHue(),
						cb.GetCursorState(), cb.IsTicked(), IsEnabled(cb));
				}
			}});
		}
	} init;

	FetchEvent<Click>(*this) += [this](CursorEventArgs&&){
		Tick(GetState() == Checked ? Unchecked : Checked);
	};
}

void
CheckBox::SetTicked(StateType st)
{
	if(mSelector.UpdateState(st))
		Ticked(TickedArgs(*this, st));
}

void
CheckBox::Tick(StateType st)
{
	Ticked(TickedArgs(*this, mSelector.State = st));
}

void
CheckBox::Refresh(PaintEventArgs&& e)
{
	FetchGUIState().Styles.PaintAsStyle({typeid(CheckBox), CheckBoxBackground},
		std::move(e));
}


CheckButton::CheckButton(const Rect& r)
	: CheckBox(r)
{
	Margin.Top = 0;
}

void
CheckButton::Refresh(PaintEventArgs&& e)
{
	auto& view(GetView());
	const Size s(GetSizeOf(*this));

	view.GetSizeRef() = {13, 13};
	CheckBox::Refresh(std::move(e));
	view.GetSizeRef() = s;
	Margin.Left += 13;
	DrawText(s, ForeColor, e);
	Margin.Left -= 13;
}


RadioBox::RadioBox(const Rect& r)
	: Thumb(r, NoBackgroundTag()), MRadioBox()
{
	using namespace Styles;
	static struct Init
	{
		Init()
		{
			AddHandlers<RadioBox>(FetchDefault(), {{RadioBoxBackground,
				[](PaintEventArgs&& e){
					auto& rb(
						ystdex::polymorphic_downcast<RadioBox&>(e.GetSender()));

					RectDrawRadioBox(e, GetSizeOf(rb), rb.GetHue(),
						rb.GetCursorState(), rb.IsTicked(), IsEnabled(rb));
				}
			}});
		}
	} init;

	FetchEvent<Click>(*this) += [this](CursorEventArgs&&){
		Tick(GetState() == Checked ? Unchecked : Checked);
	};
}

void
RadioBox::SetTicked(StateType st)
{
	if(UpdateState(st))
		Tick(st);
}

void
RadioBox::Tick(StateType st)
{
	SetState(st),
	SetWidgetPtr(this),
	Ticked(TickedArgs(*this, st));
}

void
RadioBox::Refresh(PaintEventArgs&& e)
{
	FetchGUIState().Styles.PaintAsStyle({typeid(RadioBox), RadioBoxBackground},
		std::move(e));
}


RadioButton::RadioButton(const Rect& r)
	: RadioBox(r)
{
	Margin.Top = 0;
}

void
RadioButton::Refresh(PaintEventArgs&& e)
{
	auto& view(GetView());
	const Size s(GetSizeOf(*this));

	view.GetSizeRef() = {13, 13};
	RadioBox::Refresh(std::move(e));
	view.GetSizeRef() = s;
	Margin.Left += 13;
	DrawText(s, ForeColor, e);
	Margin.Left -= 13;
}

} // namespace UI;

} // namespace YSLib;

