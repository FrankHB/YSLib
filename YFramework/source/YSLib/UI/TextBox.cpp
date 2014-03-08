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
\brief 样式无关的用户界面文本框。
\version r121
\author FrankHB <frankhb1989@gmail.com>
\since build 482
\par 创建时间:
	2014-03-02 16:21:22 +0800
\par 修改时间:
	2014-03-07 16:35 +0800
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

namespace YSLib
{

namespace UI
{

GAnimationSession<InvalidationUpdater> Caret::caret_animation;

Caret::Caret(IWidget& wgt)
{
	yunseq(
	FetchEvent<GotFocus>(wgt) += [this](UIEventArgs&& e){
		Restart(caret_animation, e.GetSender(),
			InvalidationUpdater::DefaultInvalidateControl);
	},
	FetchEvent<LostFocus>(wgt) += [this](UIEventArgs&&){
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
	caret_animation.Reset();
}


TextBox::TextBox(const Rect& r, const Drawing::Font& fnt)
	: Control(r), MLabel(fnt),
	CaretBrush(std::bind(&TextBox::PaintDefaultCaret, this,
	std::placeholders::_1)), caret(*this)
{
	yunseq(
	FetchEvent<Paint>(*this) += [this](PaintEventArgs&& e){
		if(caret.Check(e.GetSender()))
			CaretBrush(std::move(e));
	},
	FetchEvent<Paint>(*this).Add(BorderBrush(), BackgroundPriority)
	);
}

void
TextBox::Refresh(PaintEventArgs&& e)
{
	DrawText(GetSizeOf(*this), ForeColor, e);
}

void
TextBox::PaintDefaultCaret(PaintEventArgs&& e)
{
	const Rect inner_bounds(Rect(e.Location, GetSizeOf(*this)) + Margin);
	const auto lh(Font.GetHeight());
	const auto nmargin(FetchMargin(inner_bounds, e.Target.GetSize()));

	DrawVLineSeg(e.Target, e.ClipArea, inner_bounds.X + FetchStringWidth(Font,
		Text.substr(0, min(XOffset, Text.length()))), YOffset * lh
		+ nmargin.Top, YOffset * lh + lh + GetVerticalOf(nmargin), ForeColor);
}

} // namespace UI;

} // namespace YSLib;

