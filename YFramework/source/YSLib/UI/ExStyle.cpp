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
\version r126
\author FrankHB <frankhb1989@gmail.com>
\since build 472
\par 创建时间:
	2014-02-03 03:57:08 +0800
\par 修改时间:
	2014-02-18 01:00 +0800
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

	YAssert(bool(g), "Invalid graphics context found.");

	DrawRectRoundCorner(pc, s, is_enabled ? roll(is_focused, 112, {25.640625F,
		0.493671F, 0.462891F}) : FetchGUIState().Colors[Styles::Workspace]);
	if(YB_LIKELY(s.Width > 2 && s.Height > 2))
	{
		auto pt(pc.Location);
		const auto& r(pc.ClipArea);

		yunseq(pt.X += 1, pt.Y += 1, s.Width -= 2, s.Height -= 2);
		FillRect(g, r, {pt, s}, is_enabled ? roll(is_focused, 243,
			{11.304688F, 0.990431F, 0.591797F}) : MakeGray(244));
		if(is_enabled)
		{
			if(s.Width > 2 && s.Height > 2)
			{
				Rect rp(pt.X + 1, pt.Y + 1, s.Width - 2, (s.Height - 2) / 2);

				FillRect(g, r, rp,
					roll({}, 239, {39.132872F, 0.920000F, 0.951172F}));
				rp.Y += rp.Height;
				if(s.Height % 2 != 0)
					++rp.Height;
				FillRect(g, r, rp,
					roll({}, 214, {29.523438F, 0.969231F, 0.873047F}));
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


void
InitExStyles()
{
	HandlerTable ht;
	AddHandlers<Thumb>(ht, {{Thumb::ThumbBackground,
		[](PaintEventArgs&& e){
			const auto&
				tmb(ystdex::polymorphic_downcast<Thumb&>(e.GetSender()));

			RectDrawButton_Aoi(e, GetSizeOf(tmb), tmb.GetHue(),
				tmb.GetCursorState(), IsEnabled(tmb), IsFocused(tmb));
		}
	}});
	FetchGUIState().Styles.Add("Aoi", std::move(ht));
}

} // namespace Styles;

} // namespace UI;

} // namespace YSLib;

