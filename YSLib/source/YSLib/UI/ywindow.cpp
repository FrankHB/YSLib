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
\version 0.4082;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-22 17:28:28 +0800;
\par 修改时间:
	2011-08-02 11:45 +0800;
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

void
Show(IWindow& wnd)
{
	wnd.SetVisible(true);
	SetInvalidationToParent(wnd);
}

void
Hide(IWindow& wnd)
{
	wnd.SetVisible(false);
	SetInvalidationToParent(wnd);
}

void
SetInvalidationToParent(IWindow& wnd)
{
	auto pWnd(wnd.GetWindowPtr());

	if(pWnd)
		pWnd->GetRenderer().CommitInvalidation(Rect(LocateForParentWindow(wnd),
			wnd.GetSize()));
}


MWindow::MWindow(const Rect& r, const shared_ptr<Image>& hImg, IWindow* pWnd)
	: MWindowObject(pWnd),
	hBgImage(hImg)
{}

BitmapPtr
MWindow::GetBackgroundPtr() const
{
	return hBgImage ? hBgImage->GetImagePtr() : nullptr;
}


AWindow::AWindow(const Rect& r, const shared_ptr<Image>& hImg, IWindow* pWnd)
	: Control(r), MWindow(r, hImg, pWnd)
{
	SetRenderer(unique_raw(new BufferedWidgetRenderer()));
	GetRenderer().SetSize(r);
}

void
AWindow::SetSize(const Size& s)
{
	GetRenderer().SetSize(s);
	Control::SetSize(s);
}

void
AWindow::SetInvalidation()
{
	GetRenderer().CommitInvalidation(Rect(Point::Zero, GetSize()));
}

bool
AWindow::DrawBackgroundImage()
{
	if(hBgImage)
	{
		const auto& g(GetRenderer().GetContext());

		if(g.IsValid())
		{
			Rect r(Point::Zero, g.GetSize());

			GetRenderer().GetInvalidatedArea(r);
			return CopyTo(g.GetBufferPtr(), *hBgImage, g.GetSize(), r, r, r);
		}
	}
	return false;
}

Rect
AWindow::Refresh(const Graphics&, const Point&, const Rect&)
{
	if(!(IsTransparent() || DrawBackgroundImage()))
		GetRenderer().FillInvalidation(BackColor);
	DrawContents();
	GetEventMap().DoEvent<HVisualEvent>(Paint, *this, EventArgs());
	return GetBoundsOf(*this);
}

void
AWindow::Update()
{
	if(!GetRenderer().RequiresRefresh())
	{
		IWindow* const pWnd(GetWindowPtr());

		if(pWnd)
			Widgets::Update(*this, FetchContext(*pWnd),
				LocateForParentWindow(*this));
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
	: AFrame(r, hImg, pWnd)
{
	auto pDsk(FetchDesktopPtr(*this));

	if(pDsk)
		*pDsk += *this;
}
Frame::~Frame()
{
	auto pDsk(FetchDesktopPtr(*this));

	if(pDsk)
		*pDsk -= *this;
}

bool
Frame::DrawContents()
{
	if(!FetchContext(*this).IsValid())
		return false;

	bool result(GetRenderer().RequiresRefresh());

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
				//	pt = LocateOffset(this, Point::Zero, &w);
				Point pt(w.GetLocation());
				Rect r;

				GetRenderer().GetInvalidatedArea(r);
				r = Intersect(Rect(pt, w.GetSize()), r);
				if(r != Rect::Empty)
					GetRenderer().CommitInvalidation(Render(w,
						FetchContext(*this), pt, r));
			}
		}
	return result;
}

YSL_END_NAMESPACE(Forms)

YSL_END_NAMESPACE(Components)

YSL_END

