/*
	© 2009-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywindow.cpp
\ingroup UI
\brief 样式无关的 GUI 窗口。
\version r3413
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-22 17:28:28 +0800
\par 修改时间:
	2013-12-23 23:50 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YWindow
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_YDesktop
#include YFM_YSLib_UI_YBrush

namespace YSLib
{

using namespace Drawing;

namespace UI
{

Window::Window(const Rect& r, unique_ptr<Renderer> p_renderer)
	: Panel(r)
{
	if(p_renderer)
		SetRenderer(std::move(p_renderer));
}

void
Window::operator+=(IWidget& wgt)
{
	MUIContainer::operator+=(wgt);
	SetContainerPtrOf(wgt, this);
}
void
Window::operator+=(Window& wnd)
{
	MUIContainer::Add(wnd, DefaultWindowZOrder);
	SetContainerPtrOf(wnd, this);
}

bool
Window::operator-=(IWidget& wgt)
{
	if(FetchContainerPtr(wgt) == this)
	{
		SetContainerPtrOf(wgt);
		if(FetchFocusingPtr(*this) == &wgt)
			GetView().FocusingPtr = {};
		return MUIContainer::operator-=(wgt);
	}
	return false;
}
bool
Window::operator-=(Window& wnd)
{
	if(FetchContainerPtr(wnd) == this)
	{
		SetContainerPtrOf(wnd);
		if(FetchFocusingPtr(*this) == &wnd)
			GetView().FocusingPtr = {};
		return MUIContainer::operator-=(wnd);
	}
	return false;
}

void
Window::Add(IWidget& wgt, ZOrderType z)
{
	MUIContainer::Add(wgt, z);
	SetContainerPtrOf(wgt, this);
}

} // namespace UI;

} // namespace YSLib;

