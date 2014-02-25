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
\version r3262
\author FrankHB <frankhb1989@gmail.com>
\since build 194
\par 创建时间:
	2010-10-04 21:23:32 +0800
\par 修改时间:
	2014-02-23 20:16 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Button
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_Button
#include YFM_YSLib_UI_YGUI
#include <ystdex/cast.hpp>

namespace YSLib
{

//! \since build 472
using namespace Drawing;

namespace UI
{

namespace
{

//! \since build 472
void
RectDrawButton(const PaintContext& pc, const Size& s, Hue base_hue,
	CursorState cursor_state, bool is_enabled, bool is_focused)
{
	const bool inside(cursor_state != CursorState::Outside);
	const bool is_pressed(cursor_state == CursorState::Pressed);
	const auto& g(pc.Target);
	const Rect r(pc.Location, s);

	FillRect(g, pc.ClipArea, r, is_enabled ? (inside ? RollColor(is_pressed
		? hsl_t{29.2F, .86F, .916F} : hsl_t{30.F, .786F, .945F}, base_hue)
		: MakeGray(234)) : MakeGray(239));
	DrawRect(g, pc.ClipArea, r, is_enabled ? (inside ? RollColor(is_pressed
		? hsl_t{30.2F, .733F, .618F} : hsl_t{30.F, .72F, .706F}, base_hue)
		: (is_focused ? RollColor({30.F, 1.F, .6F}, base_hue)
		: FetchGUIState().Colors[Styles::Workspace])) : MakeGray(217));
}

} // unnamed namespace;


Thumb::Thumb(const Rect& r, Hue h)
	: Thumb(r, NoBackgroundTag())
{
	using namespace Styles;
	static struct Init
	{
		Init()
		{
			AddHandlers<Thumb>(FetchDefault(), {{ThumbBackground,
				[](PaintEventArgs&& e){
					const auto& tmb(
						ystdex::polymorphic_downcast<Thumb&>(e.GetSender()));

					RectDrawButton(e, GetSizeOf(tmb), tmb.GetHue(),
						tmb.GetCursorState(), IsEnabled(tmb), IsFocused(tmb));
				}
			}});
		}
	} init;

	yunseq(hue = h, Background = Painter(typeid(Thumb), ThumbBackground));
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
DecorateAsCloseButton(Thumb& tmb)
{
	yunseq(
	FetchEvent<Click>(tmb) += [&](CursorEventArgs&&)
	{
		if(const auto pCon = FetchContainerPtr(tmb))
			Close(*pCon);
	},
	FetchEvent<Paint>(tmb) += [&](PaintEventArgs&& e){
		DrawCross(e.Target, e.ClipArea, {e.Location, GetSizeOf(tmb)},
			IsEnabled(tmb) ? tmb.ForeColor
			: FetchGUIState().Colors[Styles::Workspace]);
	}
	);
}


Button::Button(const Rect& r, const Drawing::Font& fnt, TextAlignment a)
	: Button(r, 180, fnt, a)
{}
Button::Button(const Rect& r, Hue h, const Drawing::Font& fnt, TextAlignment a)
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

