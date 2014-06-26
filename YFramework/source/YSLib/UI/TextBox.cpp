/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TextBox.cpp
\ingroup UI
\brief 样式相关的用户界面文本框。
\version r428
\author FrankHB <frankhb1989@gmail.com>
\since build 482
\par 创建时间:
	2014-03-02 16:21:22 +0800
\par 修改时间:
	2014-06-22 22:22 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::TextBox
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_TextBox
#include YFM_YSLib_UI_Border
#include YFM_YSLib_UI_YGUI
#include YFM_YSLib_Service_TextLayout
#include YFM_YSLib_UI_YUIContainer // for LocateForWidget;
#include <ystdex/cast.hpp>

namespace YSLib
{

namespace UI
{

GAnimationSession<InvalidationUpdater> Caret::caret_animation;

Caret::Caret(IWidget& wgt, HBrush caret_brush,
	InvalidationUpdater::Invalidator inv)
	: CaretBrush(caret_brush), CursorInvalidator(inv)
{
	yunseq(
	FetchEvent<Paint>(wgt) += [this](PaintEventArgs&& e){
		if(Check(e.GetSender()))
			CaretBrush(std::move(e));
	},
	FetchEvent<GotFocus>(wgt) += [this](UIEventArgs&& e){
		// NOTE: Necessary cleanup.
		Stop();
		FetchGUIState().ExteralTextInputFocusPtr = &e.GetSender();
		Restart(caret_animation, e.GetSender(), CursorInvalidator);
	},
	FetchEvent<LostFocus>(wgt) += [this]{
		Stop();
	}
	);
}
Caret::~Caret()
{
	Stop();
}

bool
Caret::Check(IWidget& sender)
{
	if(caret_animation.GetConnectionPtr()
		&& caret_animation.GetConnectionRef().Ready)
	{
		YAssert(IsFocusedCascade(sender), "Wrong focus state found.");
		return IsEnabled(sender)
			&& CaretTimer.RefreshRemainder() < CaretTimer.Interval / 2;
	}
	return {};
}

void
Caret::Stop()
{
	// TODO: Consider possible per-object optimization.
	if(auto p = caret_animation.GetConnectionPtr())
		yunseq(p->Ready = {}, FetchGUIState().ExteralTextInputFocusPtr = {});
}


TextBox::TextBox(const Rect& r, const Drawing::Font& fnt,
	const pair<Color, Color>& hilight_pair)
	: Control(r, MakeBlankBrush()), MLabel(fnt), MHilightText(hilight_pair),
	Selection(), CursorCaret(*this, std::bind(&TextBox::PaintDefaultCaret, this,
	std::placeholders::_1), InvalidateDefaultCaret), h_offset()
{
	yunseq(
	FetchEvent<KeyDown>(*this) += [this](KeyEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct)
		{
			using namespace KeyCategory;
			using namespace KeyCodes;
			auto& st(FetchGUIState());
			auto& sender(e.GetSender());

			if(st.SendInput(e.Keys))
			{
				Invalidate(sender);
				return;
			}

			const auto& k(e.GetKeys());
			auto ek(FindFirstKeyInCategroy(k, Editing));

			if(ek == KeyBitsetWidth)
				ek = FindFirstKeyInCategroy(k, Navigation);
			if(ek != KeyBitsetWidth)
			{
				const bool shifted(k[Shift]);
				auto& range(Selection.Range);

				switch(ek)
				{
				case Backspace:
				case Delete:
					if(Selection.IsEmpty())
					{
						if(ek == Backspace)
						{
							if(range.second.X == 0)
								break;
							--range.second.X;
						}
						else
						{
							if(range.second.X >= Text.length())
								break;
							++range.second.X;
						}
					}
					ReplaceSelection(u"");
					break;
				case Left:
				case Right:
					if(Selection.IsEmpty() || shifted)
					{
						if(ek == Left)
						{
							if(range.second.X == 0)
								break;
							--range.second.X;
							if(shifted)
								break;
						}
						else
						{
							if(range.second.X >= Text.length())
								break;
							++range.second.X;
							if(shifted)
								break;
						}
					}
					else
						range.second.X = (ek == Left) == (range.first.X
							< range.second.X) ? range.first.X : range.second.X;
					CollapsedAndUpdateCaret();
				case Tab:
					break;
				case Space:
					if(!st.ExteralTextInputFocusPtr)
						ReplaceSelection(u" ");
				}
				Invalidate(e.GetSender());
				return;
			}
		}
	},
	FetchEvent<KeyHeld>(*this) += OnKeyHeld,
	FetchEvent<TouchDown>(*this) += [this](CursorEventArgs&& e){
		Selection.Range.second = GetCaretPosition(e.Position);
		CollapsedAndUpdateCaret();
	},
	FetchEvent<TouchHeld>(*this) += [this](CursorEventArgs&& e){
		if(FetchGUIState().GetIndependentFocusPtr() == this)
		{
			const auto& sender(e.GetSender());

			Selection.Range.second = GetCaretPosition(&sender == this
				? e.Position : LocateForWidget(*this, sender) + e.Position);
			// XXX: Optimization for block.
			UpdateCaret();
			Invalidate(*this);
		}
	},
	FetchEvent<TextInput>(*this) += [this](TextInputEventArgs&& e){
		ReplaceSelection(e.Text);
		// XXX: Optimization for block.
		Invalidate(*this);
	},
	FetchEvent<Paint>(*this).Add(BorderBrush(), BackgroundPriority),
	FetchEvent<GotFocus>(*this) += [this]{
		Invalidate(*this);
	},
	FetchEvent<LostFocus>(*this) += [this]{
		CollapsedAndUpdateCaret();
		Invalidate(*this);
	}
	);
}

Point
TextBox::GetCaretLocation() const
{
	const Rect inner_bounds(Rect(GetSizeOf(*this)) + Margin);
	const auto& cur_pos(Selection.Range.second);
	const auto lh(Font.GetHeight());

	return {inner_bounds.X + FetchStringWidth(Font, Text, cur_pos.X),
		cur_pos.Y * lh + inner_bounds.X};
}
TextSelection::Position
TextBox::GetCaretPosition(const Point& pt)
{
	const SDst max_w(max(pt.X + h_offset - Margin.Left, 0));
	auto pr(FetchStringOffsets(max_w, Font, Text));

	if(pr.first > 0
		&& FetchCharWidth(Font, Text[pr.first - 1]) / 2 < pr.second - max_w)
		--pr.first;
	return {pr.first, 0};
}

void
TextBox::DrawClippedText(const Graphics& g, const Rect& mask, TextState& ts)
{
	auto p(&Text[0]);
	auto x1(Selection.Range.first.X), x2(Selection.Range.second.X);

	if(x1 == x2)
		MLabel::DrawClippedText(g, mask, ts);
	else
	{
		if(x2 < x1)
			std::swap(x1, x2);

		// TODO: Use C++14 lambda initializers to simplify implementation.
		const auto q1(p + x1), q2(p + x2);
		CustomTextRenderer ctr([&, q1, q2](TextRenderer& tr, ucs4_t c){
			if(IsInInterval(p, q1, q2))
			{
				// TODO: Use colors from %Styles::Palette.
				FillRect(g, tr.ClipArea, ts.GetCharBounds(c), HilightBackColor);
				ts.Color = HilightTextColor;
			}
			else
				ts.Color = ForeColor;
			tr(c);
			++p;
		}, ts, g, mask);

		PutText(AutoWrapLine, ctr, p);
	}
}

bool
TextBox::InvalidateDefaultCaret(IWidget& wgt)
{
	auto& tb(ystdex::polymorphic_downcast<TextBox&>(wgt));

	InvalidateVisible(tb, Rect(tb.GetCaretLocation(), 1, tb.Font.GetHeight()
		+ GetVerticalOf(tb.Margin)));
	return true;
}

void
TextBox::Refresh(PaintEventArgs&& e)
{
	DrawText(GetSizeOf(*this), ForeColor, e);
}

void
TextBox::ReplaceSelection(const String& text)
{
	auto& r(Selection.Range);

	// XXX: Make it correct for multiline input.
	if(r.second.X < r.first.X)
		std::swap(r.first, r.second);
	Text = Text.substr(0, r.first.X) + text
		+ Text.substr(min(Text.length(), r.second.X));
	r.second.X = r.first.X + text.length(),
	CollapsedAndUpdateCaret();
}

void
TextBox::PaintDefaultCaret(PaintEventArgs&& e)
{
	const Rect inner_bounds(Rect(e.Location, GetSizeOf(*this)) + Margin);
	const auto lh(Font.GetHeight());
	const auto mask(FetchMargin(inner_bounds, e.Target.GetSize()));
	const auto& cur_pos(Selection.Range.second);

	DrawVLineSeg(e.Target, e.ClipArea, inner_bounds.X + FetchStringWidth(Font,
		Text, cur_pos.X) - h_offset, cur_pos.Y * lh + mask.Top,
		cur_pos.Y * lh + lh + mask.Top, ForeColor);
}

void
TextBox::UpdateCaret() const
{
	auto& st(FetchGUIState());

	if(st.ExteralTextInputFocusPtr == this)
		st.CaretLocation = GetCaretLocation();
}

} // namespace UI;

} // namespace YSLib;

