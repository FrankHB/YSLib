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
\version r758
\author FrankHB <frankhb1989@gmail.com>
\since build 282
\par 创建时间:
	2011-03-22 07:20:06 +0800
\par 修改时间:
	2014-01-20 19:46 +0800
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

void
Diminish(Rect& r, SDst off1 = 1, SDst off2 = 2)
{
	YAssert(r.Width > 2 && r.Height > 2, "Boundary is too small.");

	yunseq(r.X += off1, r.Y += off1, r.Width -= off2, r.Height -= off2);
}


//! \since build 465
//@{

void
RectDrawCheckBox(const PaintContext& pc, const Size& s,
	CursorState cursor_state, bool is_locked = {}, bool is_ticked = {},
	bool is_focused = {}, bool is_enabled = {})
{
	const auto& g(pc.Target);

	YAssert(bool(g), "Invalid graphics context found.");

	const auto& pt(pc.Location);
	const auto& bounds(pc.ClipArea);
	static yconstexpr Color
		cc0[]{MakeGray(177), MakeGray(143), {85, 134, 163}, {44, 98, 139}};
	static yconstexpr Color
		cc1[]{MakeGray(246), MakeGray(244), {222, 249, 250}, {194, 228, 246}};
	static yconstexpr Color
		cc2[]{{174, 179, 185}, {121, 198, 249}, {94, 182, 247}};
	static yconstexpr Color
		cc3[]{MakeGray(232), {196, 234, 253}, {180, 227, 252}};
	auto c_idx([](CursorState s)->size_t{
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
	}(cursor_state));
	Rect r(pt, s);

	DrawRect(g, bounds, r, cc0[is_enabled ? c_idx : 0]);
	// XXX: Minimal size.
	if(YB_LIKELY(r.Width > 10 && r.Height > 10))
	{
		Rect rt(r);

		Diminish(rt);
		FillRect(g, bounds, rt,
			cc1[is_locked || is_focused ? 2 : is_enabled ? c_idx : 0]);
		if(is_enabled)
		{
			--c_idx;
			Diminish(rt);
			DrawRect(g, bounds, rt, cc2[c_idx]);
			Diminish(rt);
			FillRect(g, bounds, rt, cc3[c_idx]);
		}
	}
	if(is_ticked)
	{
		const auto c1(is_enabled ? Color(4, 34, 113) : MakeGray(190)),
			c2(is_enabled ? Color(108, 166, 208) : MakeGray(199));
		Point p1(r.X + 2, r.Y + r.Height / 2), p2(r.X + r.Width / 2 - 1,
			r.Y + r.Height - 3), p3(r.X + r.Width - 2, r.Y + 1);

		p2 += Vec(0, -1);
		DrawLineSeg(g, bounds, p1 + Vec(1, 0), p2, c2);
		DrawLineSeg(g, bounds, p2, p3 + Vec(-1, 0), c2);
		p2 += Vec(0, 2);
		DrawLineSeg(g, bounds, p1 + Vec(0, 1), p2, c2);
		DrawLineSeg(g, bounds, p2, p3 + Vec(0, 1), c2);
		p2 += Vec(0, -1);
		DrawLineSeg(g, bounds, p1, p2, c1);
		DrawLineSeg(g, bounds, p2, p3, c1);
	}
}
//@}

} // unnamed namespace;


CheckBox::CheckBox(const Rect& r)
	: Thumb(r, NoBackgroundTag()),
	bTicked(false)
{
	using namespace Styles;
	static struct Init
	{
		Init()
		{
			FetchDefault().insert({{typeid(CheckBox), CheckBoxBackground},
				[](PaintEventArgs&& e){
					auto& cb(
						ystdex::polymorphic_downcast<CheckBox&>(e.GetSender()));

					cb.PaintBox(e, GetSizeOf(cb));
				}
			});
		}
	} init;

	FetchEvent<Click>(*this) += [this](CursorEventArgs&&){
		bTicked = !bTicked;
		Ticked(TickedArgs(*this, bTicked));
	};
}

void
CheckBox::SetTicked(bool b)
{
	const bool old_tick(bTicked);

	bTicked = b;
	if(bTicked != old_tick)
		Ticked(TickedArgs(*this, b));
}

void
CheckBox::Tick(bool b)
{
	Ticked(TickedArgs(*this, bTicked = b));
}

void
CheckBox::PaintBox(const PaintContext& pc, const Size& s)
{
	RectDrawCheckBox(pc, s, GetCursorState(), IsFocusedByShell(*this), bTicked,
		IsFocused(*this), IsEnabled(*this));
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
	const auto& s(GetSizeOf(*this));

	PaintBox(e, {13, 13});
	Margin.Left += 13;
	DrawText(s, ForeColor, e);
	Margin.Left -= 13;
}

} // namespace UI;

} // namespace YSLib;

