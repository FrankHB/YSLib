/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywidget.cpp
\ingroup Shell
\brief 平台无关的图形用户界面部件实现。
\version 0.4888;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-16 20:06:58 + 08:00;
\par 修改时间:
	2011-01-22 09:59 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YWidget;
*/


#include "ydesktop.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

#ifdef YCL_USE_YASSERT

void
yassert(bool exp, const char* msg, int line, const char* file,
	const char* comp, const char* func)
{
	if(!exp)
	{
		YConsole dbg;

		iprintf(
			"At line %i in file %s: \n"
			"An error occured in precedure %s of \n"
			"Components::%s:\n"
			"%s", line, file, func, comp, msg);
		dbg.Pause();
	}
}

#endif

YSL_BEGIN_NAMESPACE(Widgets)

using Controls::YVisualControl;

bool
Contains(const IWidget& w, SPOS x, SPOS y)
{
	return Rect(w.GetLocation(), w.GetSize()).Contains(x, y);
}

bool
ContainsVisible(const IWidget& w, SPOS x, SPOS y)
{
	return w.IsVisible() && Contains(w, x, y);
}


Rect
GetBoundsOf(IWidget& w)
{
	return Rect(w.GetLocation(), w.GetSize());
}

void
SetBoundsOf(IWidget& w, const Rect& r)
{
	w.SetLocation(r.GetPoint());
	w.SetSize(r.GetSize());
}


Visual::Visual(const Rect& r, Color b, Color f)
	: visible(true), transparent(false), background_redrawed(false),
	location(r.GetPoint()), size(r.Width, r.Height),
	BackColor(b), ForeColor(f)
{}

void
Visual::SetSize(const Size& s)
{
	if(size != s)
	{
		size = s;
		background_redrawed = false;
	}
}


Widget::Widget(const Rect& r, IUIBox* pCon, Color b, Color f)
	: Visual(r, b, f),
	pContainer(pCon)
{}

void
Widget::DrawBackground()
{
	YWidgetAssert(this, Widgets::Widget, DrawBackground);

	if(!IsTransparent())
		Fill(*ystdex::polymorphic_crosscast<IWidget*>(this), BackColor);
}

void
Widget::DrawForeground()
{
	YWidgetAssert(this, Widgets::Widget, DrawForeground);

	if(!IsTransparent())
		SetBgRedrawed(false);
}

void
Widget::Refresh()
{
	IWindow* pWnd(FetchWindowPtr(
		*ystdex::polymorphic_crosscast<IWidget*>(this)));

	while(pWnd && !pWnd->IsRefreshRequired())
	{
		pWnd->SetRefresh(true);
		pWnd = FetchWindowPtr(*pWnd);
	}
}


YWidget::YWidget(const Rect& r, IUIBox* pCon)
	: YComponent(),
	Widget(r, pCon)
{
	IUIContainer* const p(dynamic_cast<IUIContainer*>(GetContainerPtr()));

	if(p)
		*p += static_cast<IWidget&>(*this);
}
YWidget::~YWidget() ythrow()
{
	IUIContainer* const p(dynamic_cast<IUIContainer*>(GetContainerPtr()));

	if(p)
		*p -= static_cast<IWidget&>(*this);
}

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

