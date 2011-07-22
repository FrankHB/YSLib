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
\version 0.3932;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-22 17:28:28 +0800;
\par 修改时间:
	2011-07-22 10:31 +0800;
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

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Forms)

bool
RequiresRefresh(const IWindow& wnd)
{
	return !FetchInvalidatedArea(wnd).IsEmpty();
}

void
SetInvalidationOf(IWindow& wnd)
{
	CommitInvalidatedAreaTo(wnd, Rect(Point::Zero, wnd.GetSize()));
}

void
SetInvalidateonToParent(IWindow& wnd)
{
	if(wnd.GetWindowPtr())
	{
		CommitInvalidatedAreaTo(*wnd.GetWindowPtr(),
			Rect(LocateForParentWindow(wnd), wnd.GetSize()));
	}
}

void
CommitInvalidatedAreaTo(IWindow& wnd, const Rect& r)
{
	wnd.GetInvalidatedAreaRef() = Unite(FetchInvalidatedArea(wnd), r);
}


void
Show(IWindow& wnd)
{
	wnd.SetVisible(true);
	SetInvalidateonToParent(wnd);
}

void
Hide(IWindow& wnd)
{
	wnd.SetVisible(false);
	SetInvalidateonToParent(wnd);
}


MWindow::MWindow(const shared_ptr<Image>& hImg, IWindow* pWnd)
	: MWindowObject(pWnd),
	hBgImage(hImg), rInvalidated()
{}


AWindow::AWindow(const Rect& r, const shared_ptr<Image>& hImg,
	IWindow* pWnd)
	: Control(r), MWindow(hImg, pWnd)
{
	static_cast<Size&>(rInvalidated) = r;
}

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
	if(hBgImage)
	{
		const auto& g(GetContext());
		const auto& r(FetchInvalidatedArea(*this));

		return CopyTo(g.GetBufferPtr(), *hBgImage, g.GetSize(), r, r, r);
	}
	return false;
}

void
AWindow::DrawRaw()
{
	if(!IsTransparent())
	{
		if(!DrawBackgroundImage())
			FillRect(GetContext(), FetchInvalidatedArea(*this), BackColor);
	}
	DrawContents();
	GetEventMap().DoEvent<HVisualEvent>(Paint, *this, EventArgs());
}

Rect
AWindow::Refresh(const Graphics&, const Point&, const Rect& r)
{
	Validate();
	Update();
	return GetBoundsOf(*this);
}

void
AWindow::Update()
{
	if(!Forms::RequiresRefresh(*this))
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
	Desktop* const pDsk(FetchDesktopPtr(*this));

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

void
AWindow::Validate()
{
	if(Forms::RequiresRefresh(*this))
	{
		DrawRaw();
		ResetInvalidatedAreaOf(*this);
	}
}


AFrame::AFrame(const Rect& r, const shared_ptr<Image>& hImg, IWindow* pWnd)
	: AWindow(r, hImg, pWnd), MUIContainer()
{}

void
AFrame::operator+=(IWidget& wgt)
{
	MUIContainer::operator+=(wgt);
	wgt.GetContainerPtrRef() = this;
}
void
AFrame::operator+=(IControl& ctl)
{
	MUIContainer::operator+=(ctl);
	ctl.GetContainerPtrRef() = this;
}
void
AFrame::operator+=(IWindow& wnd)
{
	MUIContainer::Add(wnd, DefaultWindowZOrder);
	wnd.GetContainerPtrRef() = this;
}

bool
AFrame::operator-=(IWidget& wgt)
{
	if(wgt.GetContainerPtrRef() == this)
	{
		wgt.GetContainerPtrRef() = nullptr;
		return MUIContainer::operator-=(wgt);
	}
	return false;
}
bool
AFrame::operator-=(IControl& ctl)
{
	if(ctl.GetContainerPtrRef() == this)
	{
		ctl.GetContainerPtrRef() = nullptr;
		return MUIContainer::operator-=(ctl);
	}
	return false;
}
bool
AFrame::operator-=(IWindow& wnd)
{
	if(wnd.GetContainerPtrRef() == this)
	{
		wnd.GetContainerPtrRef() = nullptr;
		return MUIContainer::operator-=(wnd);
	}
	return false;
}

void
AFrame::Add(IControl& ctl, Widgets::ZOrderType z)
{
	MUIContainer::Add(ctl, z);
	ctl.GetContainerPtrRef() = this;
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

	Desktop* pDsk(FetchDesktopPtr(*this));

	if(pDsk)
		*pDsk += *this;
}
Frame::~Frame()
{
	Desktop* pDsk(FetchDesktopPtr(*this));

	if(pDsk)
		*pDsk -= *this;
}

bool
Frame::DrawContents()
{
	if(!GetContext().IsValid())
		return false;

	bool result(Forms::RequiresRefresh(*this));

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
			{
				auto pWnd(dynamic_cast<IWindow*>(&w));
				Point pt, pt_p;

				if(pWnd)
					pt_p = w.GetLocation();
				else
				{
					pt = LocateOffset(this, Point::Zero, &w);
					pt_p = pt;
				}

				const Rect& r(Intersect(Rect(pt_p, w.GetSize()),
					FetchInvalidatedArea(*this)));

				if(r != Rect::Empty)
					CommitInvalidatedAreaTo(*this,
						w.Refresh(GetContext(), pt, r));
			}
		}
	return result;
}

YSL_END_NAMESPACE(Forms)

YSL_END_NAMESPACE(Components)

YSL_END

