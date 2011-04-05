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
\brief 平台无关的图形用户界面窗口实现。
\version 0.3587;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-22 17:28:28 +0800;
\par 修改时间:
	2011-04-05 20:08 +0800;
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

bool
Show(HWND hWnd)
{
	if(hWnd)
	{
		hWnd->SetVisible(true);
		hWnd->SetRefresh(true);
		SetContainerBgRedrawedOf(*hWnd, false);
		return true;
	}
	return false;
}

bool
Hide(HWND hWnd)
{
	if(hWnd)
	{
		hWnd->SetVisible(false);
		hWnd->SetRefresh(false);
		SetContainerBgRedrawedOf(*hWnd, false);
		return true;
	}
	return false;
}


MWindow::MWindow(const GHStrong<YImage> i, HWND hWnd)
	: MWindowObject(hWnd),
	prBackImage(i), bRefresh(false), bUpdate(false)
{}


AWindow::AWindow(const Rect& r, const GHStrong<YImage> i, HWND hWnd)
	: Control(r), MWindow(i, hWnd)
{}

BitmapPtr
AWindow::GetBackgroundPtr() const
{
	return prBackImage ? prBackImage->GetImagePtr() : NULL;
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

	if(prBackImage)
	{
		ConstBitmapPtr imgBg(prBackImage->GetImagePtr());

		if(imgBg)
		{
			mmbcpy(GetContext().GetBufferPtr(), imgBg,
				GetContext().GetSizeOfBuffer());
			return true;
		}
	}
	return false;
}

void
AWindow::DrawBackground()
{
	YWindowAssert(this, Forms::AWindow, DrawBackground);

	if(!DrawBackgroundImage())
		BeFilledWith(BackColor);
}

void
AWindow::Draw()
{
	DrawContents();
	bUpdate = true;
}

void
AWindow::Refresh()
{
	if(!IsBgRedrawed() || bRefresh)
	{
		Draw();

		YAssert(IsBgRedrawed(),
			"Background is not redrawed @ AWindow::Refresh");

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
		if(GetWindowHandle())
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
	const HWND hWnd(GetWindowHandle());

	if(hWnd)
		UpdateTo(hWnd->GetContext(), LocateForParentWindow(*this));
}


AFrame::AFrame(const Rect& r, const GHStrong<YImage> i, HWND hWnd)
	: AWindow(r, i, hWnd), MUIContainer()
{}
AFrame::~AFrame()
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

bool
AFrame::operator-=(IWidget* p)
{
	if(p && p->GetContainerPtr() == this)
	{
		p->GetContainerPtr() = NULL;
		return MUIContainer::operator-=(p);
	}
	return false;
}
bool
AFrame::operator-=(IControl* p)
{
	if(p && p->GetContainerPtr() == this)
	{
		p->GetContainerPtr() = NULL;
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


YFrame::YFrame(const Rect& r, const GHStrong<YImage> i, HWND hWnd)
	: YComponent(),
	AFrame(r, i, hWnd), Buffer()
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
	bool background_changed(DrawContensBackground());

	for(WGTs::iterator i(sWgtSet.begin()); i != sWgtSet.end(); ++i)
	{
		IWidget& w(**i);

		if(w.IsVisible())
			w.DrawForeground();
	}
//	DrawForeground();

	bool result(background_changed || !IsBgRedrawed());

	SetBgRedrawed(true);
	return result;
}

bool
YFrame::DrawContensBackground()
{
	YWindowAssert(this, Forms::YFrame, DrawContents);

	bool background_changed(!IsBgRedrawed());

	for(WGTs::iterator i(sWgtSet.begin());
		!background_changed && i != sWgtSet.end(); ++i)
	{
		IWidget& w(**i);

		background_changed |= !w.IsTransparent() && w.IsVisible()
			&& !w.IsBgRedrawed();
	}
	if(background_changed)
	{
		DrawBackground();
		for(WGTs::iterator i(sWgtSet.begin()); i != sWgtSet.end(); ++i)
		{
			IWidget& w(**i);

			if(w.IsVisible() && !(IsBgRedrawed()
				&& w.IsBgRedrawed()) && !w.IsTransparent())
			{
				w.DrawBackground();
				w.SetBgRedrawed(true);
			}
		}
	}
	return background_changed;
}

YSL_END_NAMESPACE(Forms)

YSL_END_NAMESPACE(Components)

YSL_END

