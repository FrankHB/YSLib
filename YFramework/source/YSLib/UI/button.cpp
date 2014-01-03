/*
	© 2010-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file button.cpp
\ingroup UI
\brief 样式相关的图形用户界面按钮控件。
\version r3133
\author FrankHB <frankhb1989@gmail.com>
\since build 194
\par 创建时间:
	2010-10-04 21:23:32 +0800
\par 修改时间:
	2014-01-04 01:16 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Button
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_Button
#include YFM_YSLib_Service_YBlit
#include YFM_YSLib_UI_YGUI

namespace YSLib
{

namespace UI
{

namespace
{

using Drawing::Hue;

/*!
\brief 色调偏移。
\since build 463
*/
inline Color
RollColor(hsl_t hsl, Hue delta)
{
	delta += hsl.h;
	hsl.h = delta < 360 ? delta : delta - 360;
	return HSLToColor(hsl);
}

//! \since build 463
void
RectDrawButton(const PaintContext& pc, Size s, Hue base_hue,
	CursorState cursor_state, bool is_enabled = true)
{
	const bool inside(cursor_state != CursorState::Outside);
	const auto roll([=](const hsl_t& hsl, MonoType gr){
		return MakeGrayOrColor(RollColor(hsl, base_hue), gr, inside);
	});
	const auto& g(pc.Target);

	YAssert(bool(g), "Invalid graphics context found.");

	DrawRectRoundCorner(pc, s, is_enabled ? roll({25.640625F, 0.493671F,
		0.462891F}, 112) : FetchGUIState().Colors[Styles::Workspace]);
	if(YB_LIKELY(s.Width > 2 && s.Height > 2))
	{
		auto pt(pc.Location);
		const auto& r(pc.ClipArea);

		yunseq(pt.X += 1, pt.Y += 1, s.Width -= 2, s.Height -= 2);
		FillRect(g, r, {pt, s}, is_enabled ? roll(
			{11.304688F, 0.990431F, 0.591797F}, 243) : MakeGray(244));
		if(is_enabled)
		{
			if(s.Width > 2 && s.Height > 2)
			{
				Rect rp(pt.X + 1, pt.Y + 1, s.Width - 2, (s.Height - 2) / 2);

				FillRect(g, r, rp,
					roll({39.132872F, 0.920000F, 0.951172F}, 239));
				rp.Y += rp.Height;
				if(s.Height % 2 != 0)
					++rp.Height;
				FillRect(g, r, rp,
					roll({29.523438F, 0.969231F, 0.873047F}, 214));
			}
			if(cursor_state == CursorState::Pressed)
			{
				const Color tc(RollColor({165, 0.4F, 0.16F}, base_hue));

				TransformRect(g, r & Rect(pt, s), [=](BitmapPtr dst){
					const Color d(*dst);

					*dst = Color(d.GetR() ^ tc.GetR(), d.GetG() ^ tc.GetG(),
						d.GetB() ^ tc.GetB());
				});
			}
		}
	}
}

} // unnamed namespace;


Thumb::Thumb(const Rect& r, Hue hue)
	: Thumb(r, NoBackgroundTag())
{
	Background = std::bind(DrawThumbBackground, std::placeholders::_1,
		std::ref(*this), hue);
}
Thumb::Thumb(const Rect& r, NoBackgroundTag)
	: Control(r, NoBackgroundTag()),
	csCurrent(CursorState::Outside)
{
	yunseq(
	FetchEvent<CursorOver>(*this) += [this](CursorEventArgs&&)
	{
		if(csCurrent == CursorState::Outside)
		{
			csCurrent = CursorState::Over;
			Invalidate(*this);
		}
	},
	FetchEvent<Enter>(*this) += [this](CursorEventArgs&& e){
		if(!IsPressed() && e.Keys.any())
		{
			csCurrent = CursorState::Pressed;
			Invalidate(*this);
		}
	},
	FetchEvent<Leave>(*this) += [this](CursorEventArgs&& e){
		if(csCurrent == CursorState::Over || (IsPressed() && e.Keys.any()))
		{
			csCurrent = CursorState::Outside;
			Invalidate(*this);
		}
	}
	);
}


void
DrawThumbBackground(PaintEventArgs&& e, Thumb& tmb, Hue base_hue)
{
	const bool enabled(IsEnabled(tmb));
	const auto& pt(e.Location);
	auto& r(e.ClipArea);
	Size s(GetSizeOf(tmb));

	RectDrawButton(e, s, base_hue, tmb.GetCursorState(), enabled);
	if(enabled && IsFocused(tmb) && YB_LIKELY(s.Width > 6 && s.Height > 6))
	{
		yunseq(s.Width -= 6, s.Height -= 6);
		DrawRect(e.Target, r, pt + Vec(3, 3), s,
			tmb.GetCursorState() == CursorState::Outside ? Color(178, 178, 178)
			: HSLToColor({base_hue, 1, 0.5F}));
	}
}


void
DecorateAsCloseButton(Thumb& tmb)
{
	yunseq(
	FetchEvent<Click>(tmb) += [&](CursorEventArgs&&)
	{
		if(const auto pCon = FetchContainerPtr(tmb))
			Close(*pCon);
	},
	FetchEvent<Paint>(tmb) += [&](PaintEventArgs&& e){
		DrawCross(e.Target, e.Location, GetSizeOf(tmb), IsEnabled(tmb)
			? tmb.ForeColor : FetchGUIState().Colors[Styles::Workspace]);
	}
	);
}


Button::Button(const Rect& r, const Drawing::Font& fnt, TextAlignment a)
	: Button(r, 180, fnt, a)
{}
Button::Button(const Rect& r, Drawing::Hue h, const Drawing::Font& fnt,
	TextAlignment a)
	: Thumb(r, h),
	MLabel(fnt, a)
{}

void
Button::Refresh(PaintEventArgs&& e)
{
	// NOTE: Partial invalidation made no efficiency improved here.
	DrawText(GetSizeOf(*this), IsEnabled(*this) ? ForeColor
		: FetchGUIState().Colors[Styles::Workspace], std::move(e));
}

} // namespace UI;

} // namespace YSLib;

