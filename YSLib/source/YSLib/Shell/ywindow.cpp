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
\version 0.3697;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-22 17:28:28 +0800;
\par 修改时间:
	2011-04-25 12:47 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YWindow;
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


MWindow::MWindow(const GHandle<YImage> i, IWindow* pWnd)
	: MWindowObject(pWnd),
	spBgImage(i), bRefresh(true), bUpdate(false)
{}


AWindow::AWindow(const Rect& r, const GHandle<YImage> i, IWindow* pWnd)
	: Control(r), MWindow(i, pWnd)
{}

BitmapPtr
AWindow::GetBackgroundPtr() const
{
	return spBgImage ? spBgImage->GetImagePtr() : nullptr;
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

	return spBgImage ? CopyTo(GetContext(), *spBgImage) : false;
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
	YDesktop* const pDsk(FetchDirectDesktopPtr(*this));

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


AFrame::AFrame(const Rect& r, const GHandle<YImage> i, IWindow* pWnd)
	: AWindow(r, i, pWnd), MUIContainer()
{}

void
AFrame::operator+=(IWidget* p)
{
	if(p)
	{
		MUIContainer::operator+=(p);
		p->GetContainerPtr() = this;
	}
}
void
AFrame::operator+=(IControl* p)
{
	if(p)
	{
		MUIContainer::operator+=(p);
		p->GetContainerPtr() = this;
	}
}
void
AFrame::operator+=(IWindow* p)
{
	if(p)
	{
		MUIContainer::operator+=(p);
		p->GetContainerPtr() = this;
	}
}

bool
AFrame::operator-=(IWidget* p)
{
	if(p && p->GetContainerPtr() == this)
	{
		p->GetContainerPtr() = nullptr;
		return MUIContainer::operator-=(p);
	}
	return false;
}
bool
AFrame::operator-=(IControl* p)
{
	if(p && p->GetContainerPtr() == this)
	{
		p->GetContainerPtr() = nullptr;
		return MUIContainer::operator-=(p);
	}
	return false;
}
bool
AFrame::operator-=(IWindow* p)
{
	if(p && p->GetContainerPtr() == this)
	{
		p->GetContainerPtr() = nullptr;
		return MUIContainer::operator-=(p);
	}
	return false;
}

void
AFrame::ClearFocusingPtr()
{
	IControl* const p(GetFocusingPtr());

	if(p)
	{
		MUIContainer::ClearFocusingPtr();
		EventMap.GetEvent<HVisualEvent>(LostFocus)(*p,
			GetStaticRef<EventArgs>());
	}
}


YFrame::YFrame(const Rect& r, const GHandle<YImage> i, IWindow* pWnd)
	: YComponent(),
	AFrame(r, i, pWnd), Buffer()
{
	Buffer.SetSize(GetSize().Width, GetSize().Height);

	YDesktop* pDsk(FetchDirectDesktopPtr(*this));

	if(pDsk)
		*pDsk += static_cast<IControl*>(this);
}
YFrame::~YFrame()
{
	YDesktop* pDsk(FetchDirectDesktopPtr(*this));

	if(pDsk)
		*pDsk -= this;
}

bool
YFrame::DrawContents()
{
	YWindowAssert(this, Forms::YFrame, DrawContents);

	bool result(bRefresh);

	for(WGTs::iterator i(WidgetsList.begin());
		!result && i != WidgetsList.end(); ++i)
	{
		IWidget* const p(*i);

		YAssert(p, "Null widget pointer found @ YFrame::DrawContents");

		result |= p->IsVisible();
	}
	if(result)
	{
		for(WGTs::iterator i(WidgetsList.begin()); i != WidgetsList.end(); ++i)
		{
			IWidget& w(**i);

			if(w.IsVisible())
				w.Paint();
		}
	}
	return result;
}

YSL_END_NAMESPACE(Forms)

YSL_END_NAMESPACE(Components)

YSL_END

