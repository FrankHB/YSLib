/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file form.cpp
\ingroup UI
\brief 样式无关的图形用户界面窗体。
\version r1542;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-04-30 00:51:36 +0800;
\par 修改时间:
	2011-11-05 11:32 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Form;
*/


#include "YSLib/UI/form.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

Form::Form(const Rect& r, const shared_ptr<Image>& hImg, IWidget* pCon)
	: Frame(r, hImg, pCon)//,
//	Client(this, Rect::Empty/*r*/)
{
//	SetSize(Size.Width, Size.Height);
}
/*
void
Form::SetSize(SDst w, SDst h)
{
	Frame::SetSize(w, h);
//	BitmapBuffer::SetSize(w, h);
//	YWidget::SetSize(w, h);
}
void
Form::SetBounds(const Rect& r)
{
	Frame::SetBounds(r);
//	Location = r.GetPoint();
//	SetSize(r.Width, r.Height);
}

void
Form::Draw()
{
	Frame::Draw();
//	Client.Draw();

}
*/

YSL_END_NAMESPACE(Components)

YSL_END

