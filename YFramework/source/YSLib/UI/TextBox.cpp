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
\version r79
\author FrankHB <frankhb1989@gmail.com>
\since build 482
\par 创建时间:
	2014-03-02 16:21:22 +0800
\par 修改时间:
	2014-03-03 20:20 +0800
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

TextBox::TextBox(const Rect& r, const Drawing::Font& fnt)
	: Control(r), MLabel(fnt),
	CaretBrush(std::bind(&TextBox::PaintDefaultCaret, this,
	std::placeholders::_1)), caret_animation()
{
	yunseq(
	FetchEvent<Paint>(*this) += [this](PaintEventArgs&& e){
		const auto& sender(e.GetSender());

		if(IsEnabled(sender) && IsFocusedCascade(sender)
			&& CaretTimer.RefreshRemainder() < CaretTimer.Interval / 2)
			CaretBrush(std::move(e));
	},
	FetchEvent<Paint>(*this).Add(BorderBrush(), BackgroundPriority)
	);
	Restart(caret_animation, *this,
		InvalidationUpdater::DefaultInvalidateControl);
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
		String(Text.substr(0, min(XOffset, Text.length())))), YOffset * lh
		+ nmargin.Top, YOffset * lh + lh + GetVerticalOf(nmargin), ForeColor);
}

} // namespace UI;

} // namespace YSLib;

