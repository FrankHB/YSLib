/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywindow.cpp
\ingroup UI
\brief 样式无关的图形用户界面窗口。
\version r4346;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-12-22 17:28:28 +0800;
\par 修改时间:
	2011-11-30 14:35 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YWindow;
*/


#include "YSLib/UI/ydesktop.h"

YSL_BEGIN

using namespace Drawing;

YSL_BEGIN_NAMESPACE(Components)

Window::Window(const Rect& r, const shared_ptr<Image>& hImg)
	: Panel(r), MBackground(hImg)
{
	SetRenderer(unique_raw(new BufferedRenderer()));
}

void
Window::operator+=(IWidget& wgt)
{
	MUIContainer::operator+=(wgt);
	wgt.GetView().pContainer = this;
}
void
Window::operator+=(Window& wnd)
{
	MUIContainer::Add(wnd, DefaultWindowZOrder);
	wnd.GetView().pContainer = this;
}

bool
Window::operator-=(IWidget& wgt)
{
	if(FetchContainerPtr(wgt) == this)
	{
		wgt.GetView().pContainer = nullptr;
		if(FetchFocusingPtr(*this) == &wgt)
			GetView().pFocusing = nullptr;
		return MUIContainer::operator-=(wgt);
	}
	return false;
}
bool
Window::operator-=(Window& wnd)
{
	if(FetchContainerPtr(wnd) == this)
	{
		wnd.GetView().pContainer = nullptr;
		if(FetchFocusingPtr(*this) == &wnd)
			GetView().pFocusing = nullptr;
		return MUIContainer::operator-=(wnd);
	}
	return false;
}

void
Window::Add(IWidget& wgt, ZOrderType z)
{
	MUIContainer::Add(wgt, z);
	wgt.GetView().pContainer = this;
}

bool
Window::MoveToTop(IWidget& wgt)
{
	auto i(std::find_if(sWidgets.begin(), sWidgets.end(),
		[&](const WidgetMap::value_type& val){
		return val.second == &wgt;
	}));

	if(i != sWidgets.end())
	{
		const ZOrderType z(i->first);

		sWidgets.erase(i);
		sWidgets.insert(make_pair(z, static_cast<IWidget*>(&wgt)));
		Invalidate(wgt);
		return true;
	}
	return false;
}

Rect
Window::Refresh(const PaintContext& pc)
{
	const Rect& r(pc.ClipArea);
	bool result(!r.IsUnstrictlyEmpty()
		|| CheckVisibleChildren(sWidgets.begin(), sWidgets.end()));
	
	if(result)
	{
		if(hBgImage)
		{
			if(!IsTransparent())
			{
				const auto& g(pc.Target);

				CopyTo(g.GetBufferPtr(), *hBgImage, g.GetSize(), r, r, r);
			}
		}
		else
			Widget::Refresh(pc);
		return PaintChildren(pc);
	}
	return pc.ClipArea;
}

void
Window::Update()
{
	if(!GetRenderer().RequiresRefresh())
	{
		const auto pCon(FetchContainerPtr(*this));

		if(pCon)
			Components::Update(*this, PaintContext(FetchContext(*pCon),
				GetLocationOf(*this), GetBoundsOf(*this)));
	}
}

YSL_END_NAMESPACE(Components)

YSL_END

