/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ywidget.cpp
\ingroup Shell
\brief 平台无关的图形用户界面部件实现。
\version 0.4530;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-16 20:06:58 + 08:00;
\par 修改时间:
	2010-11-12 15:23 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YWidget;
*/


#include "ydesktop.h"
#include "ycontrol.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Widgets)

using Controls::YVisualControl;

bool
Contains(const IWidget& w, SPOS x, SPOS y)
{
	return Rect(w.GetLocation(), w.GetSize()).Contains(x, y);
}


namespace
{
	void
	GetContainersListFrom(IWidget& w, std::map<IUIBox*, Point>& m)
	{
		Point pt;
		IUIBox* pCon(dynamic_cast<IUIBox*>(&w));

		if(!pCon)
		{
			pCon = w.GetContainerPtr();
			pt = w.GetLocation();
		}
		while(pCon != NULL)
		{
			m.insert(std::make_pair(pCon, pt));
			pt += pCon->GetLocation();
			pCon = pCon->GetContainerPtr();
		}
	}
}

Point
LocateOffset(const IWidget* pCon, Point pt, IWindow* pWnd)
{
	while(pCon && dynamic_cast<const IWindow*>(pCon) != pWnd)
	{
		pt += pCon->GetLocation();
		pCon = dynamic_cast<const IWidget*>(pCon->GetContainerPtr());
	}
	return pt;
}

Point
LocateForWidget(IWidget& a, IWidget& b)
{
	std::map<IUIBox*, Point> m;

	GetContainersListFrom(b, m);

	Point pt;
	IUIBox* pCon(dynamic_cast<IUIBox*>(&a));

	if(!pCon)
	{
		pCon = a.GetContainerPtr();
		pt = a.GetLocation();
	}

	while(pCon != NULL)
	{
		std::map<IUIBox*, Point>::const_iterator i(m.find(pCon));

		if(i != m.end())
			return pt - i->second;
		pt += pCon->GetLocation();
		pCon = pCon->GetContainerPtr();
	}
	return Point::FullScreen;
}

Point
LocateForWindow(const IWidget& w)
{
	return w.GetContainerPtr()
		? LocateOffset(w.GetContainerPtr(),
		w.GetLocation(), w.GetWindowHandle()) : Point::FullScreen;
}

Point
LocateForDesktop(const IWidget& w)
{
	HWND hWnd(w.GetWindowHandle());

	if(hWnd)
	{
		YDesktop* pDsk(hWnd->GetDesktopPtr());

		if(pDsk)
			return LocateOffset(&w, Point::Zero, pDsk);
	}
	return Point::FullScreen;
}

Point
LocateForParentContainer(const IWidget& w)
{
	return w.GetContainerPtr()
		? LocateContainerOffset(*w.GetContainerPtr(),
		w.GetLocation()) : Point::FullScreen;
}

Point
LocateForParentWindow(const IWidget& w)
{
	return w.GetWindowHandle()
		? LocateWindowOffset(*w.GetWindowHandle(),
		w.GetLocation()) : Point::FullScreen;
}


Visual::Visual(const Rect& r, Color b, Color f)
	: Visible(true), Transparent(false), bBgRedrawed(false),
	Location(r.GetPoint()), Size(r.Width, r.Height),
	BackColor(b), ForeColor(f)
{}

void
Visual::_m_SetSize(SDST w, SDST h)
{
	if(Size.Width != w || Size.Height != h)
	{
		bBgRedrawed = false;
		Size.Width = w;
		Size.Height = h;
	}
}
void
Visual::SetBounds(const Rect& r)
{
	Location = r.GetPoint();
	SetSize(r.Width, r.Height);
}


Widget::Widget(HWND hWnd, const Rect& r, IUIBox* pCon,
	Color b, Color f)
	: Visual(r, b, f),
	hWindow(hWnd), pContainer(pCon ? pCon : GetPointer(hWnd))
{}

void
Widget::BeFilledWith(Color c)
{
	if(hWindow)
	{
		Graphics g(*hWindow);

		FillRect<PixelType>(g.GetBufferPtr(), g.GetSize(), GetBounds(), c);
	}
}

void
Widget::DrawBackground()
{
	YWidgetAssert(this, Widgets::Widget, DrawBackground);

	if(!Transparent)
		BeFilledWith();
}

void
Widget::DrawForeground()
{
	YWidgetAssert(this, Widgets::Widget, DrawForeground);

	if(!Transparent)
		SetBgRedrawed(false);
}

void
Widget::Refresh()
{
	if(hWindow)
		hWindow->SetRefresh(true);
}


YWidget::YWidget(HWND hWnd, const Rect& r, IUIBox* pCon)
	: YComponent(),
	Widget(hWnd, r, pCon)
{
	IUIContainer* p(dynamic_cast<IUIContainer*>(GetContainerPtr()));

	if(p)
		*p += static_cast<IWidget&>(*this);
}
YWidget::~YWidget() ythrow()
{
	IUIContainer* p(dynamic_cast<IUIContainer*>(GetContainerPtr()));

	if(p)
		*p -= static_cast<IWidget&>(*this);
}


MUIContainer::MUIContainer()
	: GMFocusResponser<IVisualControl>(),
	sWgtSet()
{}

IVisualControl*
MUIContainer::GetFocusingPtr() const
{
	return GMFocusResponser<IVisualControl>::GetFocusingPtr();
}
IWidget*
MUIContainer::GetTopWidgetPtr(const Point& pt)
{
	for(WidgetSet::const_iterator i(sWgtSet.begin()); i != sWgtSet.end(); ++i)
		if(Contains(**i, pt))
			return *i;
	return NULL;
}
IVisualControl*
MUIContainer::GetTopVisualControlPtr(const Point& pt)
{
	for(FOs::const_iterator i(sFOs.begin()); i != sFOs.end(); ++i)
	{
		if(*i && Contains(**i, pt))
			return *i;
	}
	return NULL;
}

bool
MUIContainer::ResponseFocusRequest(AFocusRequester& w)
{
	return w.RequestFocus<GMFocusResponser, IVisualControl>(*this);
}

bool
MUIContainer::ResponseFocusRelease(AFocusRequester& w)
{
	return w.ReleaseFocus<GMFocusResponser, IVisualControl>(*this);
}


YUIContainer::YUIContainer(HWND hWnd, const Rect& r,
	IUIBox* pCon)
	: YComponent(),
	Widget(hWnd, r, pCon), MUIContainer()
{
	IUIContainer* p(dynamic_cast<IUIContainer*>(GetContainerPtr()));

	if(p)
	{
		*p += static_cast<IWidget&>(*this);
		*p += static_cast<GMFocusResponser<IVisualControl>&>(*this);
	}
}
YUIContainer::~YUIContainer() ythrow()
{
	IUIContainer* p(dynamic_cast<IUIContainer*>(GetContainerPtr()));

	if(p)
	{
		*p -= static_cast<IWidget&>(*this);
		*p -= static_cast<GMFocusResponser<IVisualControl>&>(*this);
	}
}


MLabel::MLabel(const Drawing::Font& f, GHResource<Drawing::TextRegion> prTr_)
	: prTextRegion(prTr_ ? prTr_ : GetGlobalResource<Drawing::TextRegion>()),
	Font(f),
	Margin(prTextRegion->Margin), AutoSize(true), AutoEllipsis(false), Text()
{}

void
MLabel::PaintText(Widget& w, const Point& pt)
{
	HWND hWnd(w.GetWindowHandle());

	if(hWnd && prTextRegion)
	{
		prTextRegion->Font = Font;
		prTextRegion->Font.Update();
		SetPensTo(*prTextRegion);
		prTextRegion->Color = w.ForeColor;
		prTextRegion->SetSize(w.GetWidth(), w.GetHeight());
		SetMarginsTo(*prTextRegion, 2, 2, 2, 2);
		prTextRegion->PutLine(Text);

		Graphics g(*hWnd);

		prTextRegion->BlitToBuffer(g.GetBufferPtr(), RDeg0,
			g.GetSize(), Point::Zero, pt, w.GetSize());
		prTextRegion->SetSize(0, 0);
	}
}


YLabel::YLabel(HWND hWnd, const Rect& r, const Drawing::Font& f, IUIBox* pCon,
	GHResource<Drawing::TextRegion> prTr_)
	: YWidget(hWnd, r, pCon), MLabel(f, pCon
	? prTr_ : GetGlobalResource<Drawing::TextRegion>())
{}

void
YLabel::DrawForeground()
{
	YWidgetAssert(this, Widgets::YLabel, DrawForeground);

//	if(!Transparent)
	//	BeFilledWith();
	ParentType::DrawForeground();
	PaintText(*this, LocateForWindow(*this));
}

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

