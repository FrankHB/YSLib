/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ExStyle.cpp
\ingroup UI
\brief 样式相关的图形用户界面按钮控件。
\version r344
\author FrankHB <frankhb1989@gmail.com>
\since build 472
\par 创建时间:
	2014-02-03 03:57:08 +0800
\par 修改时间:
	2014-06-26 15:22 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::ExStyle
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_ExStyle
#include YFM_YSLib_UI_YGUI
#include YFM_YSLib_Service_YBlit
#include YFM_YSLib_UI_Button
#include YFM_YSLib_UI_Selector
#include <ystdex/cast.hpp>

namespace YSLib
{

using namespace Drawing;

namespace UI
{

namespace Styles
{

namespace
{

//! \since build 472
void
RectDrawButton_Aoi(const PaintContext& pc, Size s, Hue base_hue,
	CursorState cursor_state, bool is_enabled, bool is_focused)
{
	const bool inside(cursor_state != CursorState::Outside);
	const auto roll([=](bool b, MonoType gr, const hsl_t& hsl){
		return MakeGrayOrColor(RollColor(hsl, base_hue), gr, inside || b);
	});
	const auto& g(pc.Target);

	DrawRectRoundCorner(pc, s, is_enabled ? roll(is_focused, 112, {25.640625F,
		0.493671F, 0.462891F}) : FetchGUIState().Colors[Styles::Workspace]);
	if(YB_LIKELY(s.Width > 2 && s.Height > 2))
	{
		auto pt(pc.Location);
		const auto& bounds(pc.ClipArea);

		yunseq(pt.X += 1, pt.Y += 1, s.Width -= 2, s.Height -= 2);
		FillRect(g, bounds, {pt, s}, is_enabled ? roll(is_focused, 243,
			{11.304688F, 0.990431F, 0.591797F}) : MakeGray(244));
		if(is_enabled)
		{
			if(s.Width > 2 && s.Height > 2)
			{
				Rect rp(pt.X + 1, pt.Y + 1, s.Width - 2, (s.Height - 2) / 2);

				FillRect(g, bounds, rp,
					roll({}, 239, {39.132872F, 0.920000F, 0.951172F}));
				rp.Y += rp.Height;
				if(s.Height % 2 != 0)
					++rp.Height;
				FillRect(g, bounds, rp,
					roll({}, 214, {29.523438F, 0.969231F, 0.873047F}));
			}
			if(cursor_state == CursorState::Pressed)
			{
				const Color tc(RollColor({165, 0.4F, 0.16F}, base_hue));

				TransformRect(g, bounds & Rect(pt, s), [=](BitmapPtr dst){
					const Color d(*dst);

					*dst = Color(d.GetR() ^ tc.GetR(), d.GetG() ^ tc.GetG(),
						d.GetB() ^ tc.GetB());
				});
			}
		}
	}
}


//! \since build 481
//@{
enum SelectorIndex_Aoi
{
	SelectorIndex_Aoi_C1 = 4,
	SelectorIndex_Aoi_C2 = 8,
	SelectorIndex_Aoi_C3 = 11,
	SelectorIndex_Aoi_End = 14
};

enum ControlState : yimpl(size_t)
{
	ControlState_Focused = 0,
	ControlState_Enabled = 1,
	ControlState_Locked = 2,
	ControlState_Ticked = 3
};


void
RectDrawSelector_Aoi(void(*f)(const PaintContext&, const Size&, const bool[],
	const Color[], size_t), const PaintContext& pc, Thumb& tmb, bool is_ticked)
{
	const Hue base_hue(tmb.GetHue());
	const auto roll([=](float h, float s, float l){
		return RollColor({h, s, l}, base_hue);
	});
	const Color colors[]{MakeGray(177), MakeGray(143), roll(2.3F, .315F, .486F),
		roll(25.9F, .519F, .359F), MakeGray(246), MakeGray(244),
		roll(2.1F, .737F, .925F), roll(20.8F, .743F, .863F),
		roll(32.7F, .073F, .704F), roll(23.9F, .914F, .725F),
		roll(25.5F, .905F, .669F), MakeGray(232), roll(20.F, .934F, .88F),
		roll(20.8F, .923F, .847F)};
	const bool ctl_states[]{IsFocused(tmb), IsEnabled(tmb),
		IsFocusedByShell(tmb), is_ticked};

	f(pc, GetSizeOf(tmb), ctl_states, colors, [](CursorState s)->size_t{
		switch(s)
		{
		case CursorState::Outside:
			return 1;
		case CursorState::Over:
			return 2;
		case CursorState::Pressed:
			return 3;
		default:
			YAssert(false, "Invalid state found.");
		};
		return 0;
	}(tmb.GetCursorState()));
}


void
RectDrawCheckBox_Aoi(const PaintContext& pc, CheckBox& cb)
{
	RectDrawSelector_Aoi([](const PaintContext& pc, const Size& s,
		const bool cst[], const Color ccs[], size_t c_idx){
		const auto& g(pc.Target);
		const auto& pt(pc.Location);
		const auto& bounds(pc.ClipArea);
		const Rect r(pt, s);
		const bool is_enabled(cst[ControlState_Enabled]);

		DrawRect(g, bounds, r, ccs[is_enabled ? c_idx : 0]);
		// XXX: Minimal size.
		if(YB_LIKELY(r.Width > 10 && r.Height > 10))
		{
			Rect rt(r);

			Diminish(rt);
			FillRect(g, bounds, rt, ccs[SelectorIndex_Aoi_C1
				+ (cst[ControlState_Locked] || cst[ControlState_Focused]
				? 2 : is_enabled ? c_idx : 0)]);
			if(is_enabled)
			{
				--c_idx;
				Diminish(rt);
				DrawRect(g, bounds, rt, ccs[SelectorIndex_Aoi_C2 + c_idx]);
				Diminish(rt);
				FillRect(g, bounds, rt, ccs[SelectorIndex_Aoi_C3 + c_idx]);
			}
		}
		if(cst[ControlState_Ticked])
			DrawTick(g, bounds, r, is_enabled ? Color(4, 34, 113)
				: MakeGray(190), is_enabled ? Color(108, 166, 208)
				: MakeGray(199));
	}, pc, cb, cb.IsTicked());
}

void
RectDrawRadioBox_Aoi(const PaintContext& pc, RadioBox& rb)
{
	RectDrawSelector_Aoi([](const PaintContext& pc, const Size& s,
		const bool cst[], const Color ccs[], size_t c_idx){
		const auto& g(pc.Target);
		const SDst rad(min(s.Width / 2, s.Height / 2));
		const auto pt(pc.Location + Size(rad, rad));
		const auto& bounds(pc.ClipArea);
		const Rect r(pc.Location, s);
		const bool is_enabled(cst[ControlState_Enabled]);

		DrawCircle(g, bounds, pt, rad, ccs[is_enabled ? c_idx : 0]);
		// XXX: Minimal size.
		if(YB_LIKELY(r.Width > 10 && r.Height > 10))
		{
			SDst radt(rad);

			FillCircle(g, bounds, pt, --radt, ccs[SelectorIndex_Aoi_C1
				+ (cst[ControlState_Locked] || cst[ControlState_Focused]
				? 2 : is_enabled ? c_idx : 0)]);
			if(is_enabled)
			{
				--c_idx;
				DrawCircle(g, bounds, pt, --radt,
					ccs[SelectorIndex_Aoi_C2 + c_idx]);
				DrawCircle(g, bounds, pt, --radt,
					ccs[SelectorIndex_Aoi_C3 + c_idx]);
			}
		}
		if(cst[ControlState_Ticked])
			FillCircle(g, bounds, pt, rad - 2, is_enabled ? Color(11, 130, 199)
				: MakeGray(190));
	}, pc, rb, rb.IsSelected());
}
//@}

} // unnamed namespace;


void
InitExStyles()
{
	YTraceDe(Notice, "Initializing extra styles ...");

	HandlerTable ht;

	YTraceDe(Informative, "Adding Thumb style 'Aoi'...");
	AddHandlers<Thumb>(ht, {{Thumb::ThumbBackground,
		[](PaintEventArgs&& e){
			const auto&
				tmb(ystdex::polymorphic_downcast<Thumb&>(e.GetSender()));

			RectDrawButton_Aoi(e, GetSizeOf(tmb), tmb.GetHue(),
				tmb.GetCursorState(), IsEnabled(tmb), IsFocused(tmb));
		}
	}}),
	YTraceDe(Informative, "Adding CheckBox style 'Aoi'...");
	AddHandlers<CheckBox>(ht, {{CheckBox::CheckBoxBackground,
		[](PaintEventArgs&& e){
			RectDrawCheckBox_Aoi(e,
				ystdex::polymorphic_downcast<CheckBox&>(e.GetSender()));
		}
	}}),
	YTraceDe(Informative, "Adding RadioBox style 'Aoi'...");
	AddHandlers<RadioBox>(ht, {{CheckBox::CheckBoxBackground,
		[](PaintEventArgs&& e){
			RectDrawRadioBox_Aoi(e,
				ystdex::polymorphic_downcast<RadioBox&>(e.GetSender()));
		}
	}});
	YTraceDe(Notice, "Adding style 'Aoi'...");
	FetchGUIState().Styles.Add("Aoi", std::move(ht));
	YTraceDe(Notice, "Extra style(s) initialized successfully.");
}

} // namespace Styles;

} // namespace UI;

} // namespace YSLib;

