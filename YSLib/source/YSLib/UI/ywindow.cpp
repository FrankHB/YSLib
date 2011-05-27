/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywindow.cpp
\ingroup Shell
\brief 样式无关的图形用户界面窗口。
\version 0.3756;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-22 17:28:28 +0800;
\par 修改时间:
	2011-05-26 23:11 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YWindow;
*/


#include "ywindow.h"
#include "ydesktop.h"

YSL_BEGIN

using namespace Components::Controls;
using namespace Components::Widgets;
using namespace Drawing;
using namespace Runtime;

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Forms)

void
Show(IWindow& wnd)
{
	wnd.SetVisible(true);
	wnd.SetRefresh(true);
}

void
Hide(IWindow& wnd)
{
	wnd.SetVisible(false);
	wnd.SetRefresh(false);
}


MWindow::MWindow(const shared_ptr<Image>& hImg, IWindow* pWnd)
	: MWindowObject(pWnd),
	hBgImage(hImg), bRefresh(true), bUpdate(false)
{}


AWindow::AWindow(const Rect& r, const shared_ptr<Image>& hImg, IWindow* pWnd)
	: Control(r), MWindow(hImg, pWnd)
{}

BitmapPtr
AWindow::GetBackgroundPtr() const
{
	return hBgImage ? hBgImage->GetImagePtr() : nullptr;
}

void
AWindow::SetSize(const Size& s)
{
	SetBufferSize(s);
	Control::SetSize(s);
}

bool
AWindow::DrawBackgroundImage()
{
	YWindowAssert(this, Forms::AWindow, DrawBackgroundImage);

	return hBgImage ? CopyTo(GetContext(), *hBgImage) : false;
}

void
AWindow::Draw()
{
	if(!IsTransparent())
	{
		if(!DrawBackgroundImage())
			BeFilledWith(BackColor);
	}

	YWindowAssert(this, Forms::AWindow, Draw);

	DrawContents();
	bUpdate = true;
}

void
AWindow::Paint()
{
	Refresh();
	Update();
}

void
AWindow::Refresh()
{
	if(bRefresh)
	{
		Draw();
		bRefresh = false;
	}
	if(GetContainerPtr())
		Widget::Refresh();
}

void
AWindow::Update()
{
	if(bRefresh)
		bUpdate = false;
	if(bUpdate)
		UpdateToWindow();
}

void
AWindow::UpdateTo(const Graphics& g, const Point& p) const
{
	if(IsVisible())
		CopyTo(g, GetContext(), p);
}

void
AWindow::UpdateToDesktop()
{
	Desktop* const pDsk(FetchDirectDesktopPtr(*this));

	if(pDsk)
		UpdateTo(pDsk->GetContext(), LocateForDesktop(*this));
}

void
AWindow::UpdateToWindow() const
{
	IWindow* const pWnd(GetWindowPtr());

	if(pWnd)
		UpdateTo(pWnd->GetContext(), LocateForParentWindow(*this));
}


AFrame::AFrame(const Rect& r, const shared_ptr<Image>& hImg, IWindow* pWnd)
	: AWindow(r, hImg, pWnd), MUIContainer()
{}

void
AFrame::operator+=(IWidget& wgt)
{
	MUIContainer::operator+=(wgt);
	wgt.GetContainerPtr() = this;
}
void
AFrame::operator+=(IControl& ctl)
{
	MUIContainer::operator+=(ctl);
	ctl.GetContainerPtr() = this;
}
void
AFrame::operator+=(IWindow& wnd)
{
	MUIContainer::operator+=(wnd);
	wnd.GetContainerPtr() = this;
}

bool
AFrame::operator-=(IWidget& wgt)
{
	if(wgt.GetContainerPtr() == this)
	{
		wgt.GetContainerPtr() = nullptr;
		return MUIContainer::operator-=(wgt);
	}
	return false;
}
bool
AFrame::operator-=(IControl& ctl)
{
	if(ctl.GetContainerPtr() == this)
	{
		ctl.GetContainerPtr() = nullptr;
		return MUIContainer::operator-=(ctl);
	}
	return false;
}
bool
AFrame::operator-=(IWindow& wnd)
{
	if(wnd.GetContainerPtr() == this)
	{
		wnd.GetContainerPtr() = nullptr;
		return MUIContainer::operator-=(wnd);
	}
	return false;
}

void
AFrame::Add(IControl& ctl, Widgets::ZOrderType z)
{
	MUIContainer::Add(ctl, z);
	ctl.GetContainerPtr() = this;
}

void
AFrame::ClearFocusingPtr()
{
	IControl* const p(GetFocusingPtr());

	if(p)
	{
		MUIContainer::ClearFocusingPtr();
		EventMap.GetEvent<HVisualEvent>(LostFocus)(*p, EventArgs());
	}
}


Frame::Frame(const Rect& r, const shared_ptr<Image>& hImg, IWindow* pWnd)
	: AFrame(r, hImg, pWnd),
	Buffer()
{
	Buffer.SetSize(GetSize().Width, GetSize().Height);

	Desktop* pDsk(FetchDirectDesktopPtr(*this));

	if(pDsk)
		*pDsk += *this;
}
Frame::~Frame()
{
	Desktop* pDsk(FetchDirectDesktopPtr(*this));

	if(pDsk)
		*pDsk -= *this;
}

bool
Frame::DrawContents()
{
	YWindowAssert(this, Forms::Frame, DrawContents);

	bool result(bRefresh);

	for(auto i(sWidgets.begin()); !result && i != sWidgets.end(); ++i)
	{
		IWidget* const p(i->second);

		YAssert(p, "Null widget pointer found @ Frame::DrawContents");

		result |= p->IsVisible();
	}
	if(result)
		for(auto i(sWidgets.begin()); i != sWidgets.end(); ++i)
		{
			IWidget& w(*i->second);

			if(w.IsVisible())
				w.Paint();
		}
	return result;
}

YSL_END_NAMESPACE(Forms)

YSL_END_NAMESPACE(Components)

YSL_END

