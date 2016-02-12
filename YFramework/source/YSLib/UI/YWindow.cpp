/*
	© 2009-2014, 2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YWindow.cpp
\ingroup UI
\brief 样式无关的 GUI 窗口。
\version r3427
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-22 17:28:28 +0800
\par 修改时间:
	2016-02-12 01:09 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YWindow
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_YDesktop
#include YFM_YSLib_Service_YBrush

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
	SetContainerPtrOf(wgt, make_observer(this));
}
void
Window::operator+=(Window& wnd)
{
	MUIContainer::Add(wnd, DefaultWindowZOrder);
	SetContainerPtrOf(wnd, make_observer(this));
}

bool
Window::operator-=(IWidget& wgt)
{
	if(FetchContainerPtr(wgt).get() == this)
	{
		SetContainerPtrOf(wgt);
		if(FetchFocusingPtr(*this).get() == &wgt)
			GetView().FocusingPtr = {};
		return MUIContainer::operator-=(wgt);
	}
	return {};
}
bool
Window::operator-=(Window& wnd)
{
	if(FetchContainerPtr(wnd).get() == this)
	{
		SetContainerPtrOf(wnd);
		if(FetchFocusingPtr(*this).get() == &wnd)
			GetView().FocusingPtr = {};
		return MUIContainer::operator-=(wnd);
	}
	return {};
}

void
Window::Add(IWidget& wgt, ZOrder z)
{
	MUIContainer::Add(wgt, z);
	SetContainerPtrOf(wgt, make_observer(this));
}

} // namespace UI;

} // namespace YSLib;

