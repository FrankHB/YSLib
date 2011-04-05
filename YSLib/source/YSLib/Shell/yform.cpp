/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yform.cpp
\ingroup Shell
\brief 平台无关的 GUI 窗体实现。
\version 0.1498;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-04-30 00:51:36 +0800;
\par 修改时间:
	2011-04-03 18:56 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YForm;
*/


#include "yform.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Forms)

using namespace Controls;

YForm::YForm(const Rect& r, const GHStrong<YImage> i, HWND hWnd)
	: YFrame(r, i, hWnd)//,
//	Client(this, Rect::Empty/*r*/)
{
//	SetSize(Size.Width, Size.Height);
//	DrawBackground();
}
YForm::~YForm()
{}
/*
void
YForm::SetSize(SDst w, SDst h)
{
	YFrame::SetSize(w, h);
//	BitmapBuffer::SetSize(w, h);
//	YWidget::SetSize(w, h);
}
void
YForm::SetBounds(const Rect& r)
{
	YFrame::SetBounds(r);
//	Location = r.GetPoint();
//	SetSize(r.Width, r.Height);
}

void
YForm::DrawBackground()
{
	YFrame::DrawBackground();

//	Client.DrawBackground();
}

void
YForm::DrawForeground()
{
	YFrame::DrawForeground();
//	Client.DrawForeground();

}
void
YForm::Draw()
{
	YFrame::Draw();
}
*/
YSL_END_NAMESPACE(Forms)

YSL_END_NAMESPACE(Components)

YSL_END

