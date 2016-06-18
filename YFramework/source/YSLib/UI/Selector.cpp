/*
	© 2011-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Selector.cpp
\ingroup UI
\brief 样式相关的图形用户界面选择控件。
\version r1089
\author FrankHB <frankhb1989@gmail.com>
\since build 282
\par 创建时间:
	2011-03-22 07:20:06 +0800
\par 修改时间:
	2016-06-15 12:17 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Selector
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_Selector
#include YFM_YSLib_Service_YBlit
#include YFM_YSLib_UI_YGUI
#include <ystdex/cast.hpp> // for ystdex::polymorphic_downcast;
#include <ystdex/scope_guard.hpp> // for ystdex::swap_guard;

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
	const auto& g(pc.Target);
	const Rect& bounds(pc.ClipArea);
	const Rect r(pc.Location, s);

	FillRect(g, bounds, r, is_enabled ? (inside ? Color((is_pressed
		? HSL(30.F, 1.F, .925F) : HSL(30.F, 1.F, .976F)).Roll(base_hue))
		: ColorSpace::White) : MakeGray(230));
	DrawRect(g, bounds, r, is_enabled ? (inside ? Color((is_pressed
		? HSL(26.5F, 1.F, .435F) : HSL(30.F, 1.F, .6F)).Roll(base_hue))
		: MakeGray(112)) : MakeGray(188));

	if(is_ticked)
		DrawTick(g, bounds, r, is_enabled ? ColorSpace::Black : MakeGray(112));
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
	const Rect& bounds(pc.ClipArea);
	const Rect r(pc.Location, s);

	FillCircle(g, bounds, pt, rad, is_enabled ? (inside ? Color((is_pressed
		? HSL(30.F, 1.F, .925F) : HSL(30.F, 1.F, .976F)).Roll(base_hue))
		: ColorSpace::White) : MakeGray(230));
	DrawCircle(g, bounds, pt, rad, is_enabled ? (inside ? Color((is_pressed
		? HSL(26.5F, 1.F, .435F) : HSL(30.F, 1.F, .6F)).Roll(base_hue))
		: MakeGray(112)) : MakeGray(188));
	// XXX: Minimal size.
	if(is_ticked && YB_LIKELY(r.Width > 4 && r.Height > 4))
		FillCircle(g, bounds, pt, rad - 2,
			is_enabled ? ColorSpace::Black : MakeGray(112));
}

//! \since build 569
void
BoxRefresh(AView& v, MLabel& lbl, PaintEventArgs& e, std::function<void()> f)
{
	{
		using namespace std::placeholders;
		ystdex::state_guard<Size, void> gd([&](bool, Size& s) ynothrow{
			// XXX: Possilble inconsistent state.
			YSLib::FilterExceptions([&]{
				SwapSizeOf(v, s);
			}, yfsig);
		}, 13, 13);

		f();
	}
	{
		ystdex::swap_guard<SPos, void>
			gd(lbl.Margin.Left, SPos(lbl.Margin.Left + 13));

		lbl(std::move(e));
	}
}

} // unnamed namespace;


CheckBox::CheckBox(const Rect& r)
	: Thumb(r, ystdex::raw_tag()), MCheckBox()
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

	FetchEvent<Click>(*this) += [this]{
		Tick(GetState() == Checked ? Unchecked : Checked);
	};
}

void
CheckBox::SetTicked(StateType st)
{
	if(mSelector.State != st)
		Ticked(TickedArgs(*this, st));
}

void
CheckBox::Refresh(PaintEventArgs&& e)
{
	FetchGUIConfiguration().Styles.PaintAsStyle({typeid(CheckBox),
		CheckBoxBackground}, std::move(e));
}

void
CheckBox::Tick(StateType st)
{
	Ticked(TickedArgs(*this, mSelector.State = st));
}


CheckButton::CheckButton(const Rect& r)
	: CheckBox(r)
{
	Margin.Top = 0;
}

void
CheckButton::Refresh(PaintEventArgs&& e)
{
	BoxRefresh(GetView(), *this, e, [&, this]{
		CheckBox::Refresh(std::move(e));
	});
}


void
MSharedSelection::ShareTo(MSharedSelection& rb) const
{
	rb.p_selector = p_selector;
}


RadioBox::RadioBox(const Rect& r)
	: Thumb(r, ystdex::raw_tag()), MSharedSelection()
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
						rb.GetCursorState(), rb.IsSelected(), IsEnabled(rb));
				}
			}});
		}
	} init;

	FetchEvent<Click>(*this) += [this]{
		SetSelected();
	};
}

void
RadioBox::SetSelected()
{
	const auto p_wgt(GetState());

	if(p_wgt.get() != this)
	{
		if(p_wgt)
			Invalidate(*p_wgt);
		Select();
	}
}

void
RadioBox::Refresh(PaintEventArgs&& e)
{
	FetchGUIConfiguration().Styles.PaintAsStyle({typeid(RadioBox),
		RadioBoxBackground}, std::move(e));
}

void
RadioBox::Select()
{
	UpdateState(make_observer(this));
	Selected(SelectedArgs(*this, this));
}

void
RadioBox::ShareTo(RadioBox& rb) const
{
	MSharedSelection::ShareTo(rb);
}


RadioButton::RadioButton(const Rect& r)
	: RadioBox(r)
{
	Margin.Top = 0;
}

void
RadioButton::Refresh(PaintEventArgs&& e)
{
	BoxRefresh(GetView(), *this, e, [&, this]{
		RadioBox::Refresh(std::move(e));
	});
}

} // namespace UI;

} // namespace YSLib;

