/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywidget.cpp
\ingroup UI
\brief 样式无关的图形用户界面部件。
\version 0.4944;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2011-07-09 17:28 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Widget;
*/


#include "ywidget.h"
#include "ydesktop.h"
#include "ywindow.h"
#include "yuicont.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Widgets)

using Controls::Control;

bool
Contains(const IWidget& w, SPos x, SPos y)
{
	return Rect(w.GetLocation(), w.GetSize()).Contains(x, y);
}

bool
ContainsVisible(const IWidget& w, SPos x, SPos y)
{
	return w.IsVisible() && Contains(w, x, y);
}


void
RequestToTop(IWidget& w)
{
	Desktop* pDsk(FetchDirectDesktopPtr(w));

	if(pDsk && pDsk == w.GetContainerPtr())
	{
		IControl* pCon(dynamic_cast<IControl*>(&w));

		if(pCon)
			pDsk->MoveToTop(*pCon);
	}
}


void
SetBoundsOf(IWidget& w, const Rect& r)
{
	w.SetLocation(r.GetPoint());
	w.SetSize(r.GetSize());
}


void
Invalidate(IWidget& wgt)
{
	wgt.Invalidate(Rect(Point::Zero, wgt.GetSize()));
}


Visual::Visual(const Rect& r, Color b, Color f)
	: visible(true), transparent(false),
	location(r.GetPoint()), size(r.Width, r.Height),
	BackColor(b), ForeColor(f)
{}

void
Visual::SetSize(const Size& s)
{
	size = s;
}


Widget::Widget(const Rect& r, Color b, Color f)
	: Visual(r, b, f),
	pContainer()
{}

void
Widget::Invalidate(const Rect& r)
{
	IWidget* pWgt(this);
	IWindow* pWnd;
	Rect rect(r);

	while((pWnd = FetchWidgetDirectNodePtr<IWindow>(
		pWgt->GetContainerPtr(), rect += pWgt->GetLocation())))
	{
		pWnd->CommitInvalidatedArea(rect);
		rect = pWnd->GetInvalidatedArea();
		pWgt = pWnd;
	}
}

void
Widget::Refresh()
{
	YWidgetAssert(this, Widgets::Widget, Refresh);

	if(!IsTransparent())
		Fill(*this, BackColor);
}

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

