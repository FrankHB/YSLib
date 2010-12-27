/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywindow.cpp
\ingroup Shell
\brief 平台无关的图形用户界面窗口实现。
\version 0.3364;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-22 17:28:28 + 08:00;
\par 修改时间:
	2010-12-27 13:58 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YWindow;
*/


#include "ydesktop.h"
#include "ycontrol.h"

YSL_BEGIN

using namespace Components::Controls;
using namespace Components::Widgets;
using namespace Drawing;
using namespace Runtime;

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Forms)

MWindow::MWindow(const GHStrong<YImage> i, HWND hWnd)
	: MWindowObject(hWnd),
	Buffer(), prBackImage(i), bRefresh(false), bUpdate(false)
{}


AWindow::AWindow(const Rect& r, const GHStrong<YImage> i, HWND hWnd)
	: AVisualControl(r, GetPointer(hWnd)), MWindow(i, hWnd)
{}

BitmapPtr
AWindow::GetBackgroundPtr() const
{
	return prBackImage ? prBackImage->GetImagePtr() : NULL;
}

void
AWindow::SetSize(const Size& s)
{
	Buffer.SetSize(s.Width, s.Height);
	Widget::SetSize(s);
}

void
AWindow::RequestToTop()
{
	YDesktop* pDsk(FetchDirectDesktopPtr(*this));

	if(pDsk)
		pDsk->MoveToTop(*this);
}

bool
AWindow::DrawBackgroundImage()
{
	YWindowAssert(this, Forms::AWindow, DrawBackgroundImage);

	if(prBackImage && GetBufferPtr())
	{
		ConstBitmapPtr imgBg(prBackImage->GetImagePtr());

		if(imgBg)
		{
			std::memcpy(GetBufferPtr(), imgBg, Buffer.GetSizeOfBuffer());
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
	DrawBackground();
	DrawWidgets();
	bUpdate = true;
}

void
AWindow::Refresh()
{
	if(bRefresh)
	{
		bRefresh = false;
		Draw();
	}
	Widget::Refresh();
}

void
AWindow::Update()
{
	if(bUpdate)
	{
		bUpdate = false;

		HWND hWnd(GetWindowHandle());

		if(hWnd)
		{
			UpdateToWindow();
			hWnd->SetUpdate(true);
		}
	}
}

void
AWindow::UpdateToScreen(YDesktop& d) const
{
	if(IsVisible())
		Buffer.CopyToBuffer(d.GetBackgroundPtr(), RDeg0, d.GetSize(),
		Point::Zero, GetLocation(), Buffer.GetSize());
}

void
AWindow::UpdateToWindow(IWindow& w) const
{
	if(IsVisible())
	{
		const Graphics& g(w);

		Buffer.CopyToBuffer(g.GetBufferPtr(), RDeg0, g.GetSize(),
			Point::Zero, GetLocation(), Buffer.GetSize());
	}
}

void
AWindow::UpdateToScreen() const
{
	YDesktop* const pDsk(FetchDirectDesktopPtr(*this));

	if(pDsk)
		UpdateToScreen(*pDsk);
}

void
AWindow::UpdateToWindow() const
{
	const HWND hWnd(GetWindowHandle());

	if(hWnd)
		UpdateToWindow(*hWnd);
}

void
AWindow::Show()
{
	SetVisible(true);
	Draw();
	UpdateToScreen();
}


YFrameWindow::YFrameWindow(const Rect& r, const GHStrong<YImage> i, HWND hWnd)
	: YComponent(),
	AWindow(r, i, hWnd), MUIContainer()
{
	IUIContainer* p(dynamic_cast<IUIContainer*>(GetContainerPtr()));

	if(p)
		*p += static_cast<GMFocusResponser<IVisualControl>&>(*this);
	Buffer.SetSize(GetSize().Width, GetSize().Height);

	YDesktop* pDsk(FetchDirectDesktopPtr(*this));

	if(pDsk)
		*pDsk += static_cast<IVisualControl&>(*this);
}
YFrameWindow::~YFrameWindow() ythrow()
{
	IUIContainer* p(dynamic_cast<IUIContainer*>(GetContainerPtr()));

	if(p)
		*p -= static_cast<GMFocusResponser<IVisualControl>&>(*this);

	YDesktop* pDsk(FetchDirectDesktopPtr(*this));

	if(pDsk)
		pDsk->RemoveAll(*this);
}

bool
YFrameWindow::DrawWidgets()
{
	YWindowAssert(this, Forms::YFrameWindow, DrawWidgets);

	bool bBgChanged(!IsBgRedrawed());
	WidgetSet::iterator i;

	for(i = sWgtSet.begin(); !bBgChanged && i != sWgtSet.end(); ++i)
	{
		IWidget& w(**i);

		bBgChanged |= !w.IsTransparent() && w.IsVisible() && !w.IsBgRedrawed();
	}
	if(bBgChanged)
	{
		DrawBackground();
		for(i = sWgtSet.begin(); i != sWgtSet.end(); ++i)
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
	for (i = sWgtSet.begin(); i != sWgtSet.end(); ++i)
	{
		IWidget& w(**i);

		if(w.IsVisible())
			w.DrawForeground();
	}
	DrawForeground();

	bool result(bBgChanged || !IsBgRedrawed());

	SetBgRedrawed(true);
	return result;
}

YSL_END_NAMESPACE(Forms)

YSL_END_NAMESPACE(Components)

YSL_END

