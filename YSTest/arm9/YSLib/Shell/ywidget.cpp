// YSLib::Shell::YWidget by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-16 20:06:58 + 08:00;
// UTime = 2010-10-19 15:38 + 08:00;
// Version = 0.4188;


#include "ywindow.h"
#include "ycontrol.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Widgets)

using Controls::YVisualControl;


Point
GetLocationOffset(const IWidget* pCon, const Point& p, const HWND& hWnd)
{
	Point pt(p);

	while(pCon && dynamic_cast<const IWindow*>(pCon) != hWnd)
	{
		pt += pCon->GetLocation();
		pCon = dynamic_cast<const IWidget*>(pCon->GetContainerPtr());
	}
	return pt;
}


MVisual::MVisual(const Rect& r, Color b, Color f)
	: Visible(true), Transparent(false), bBgRedrawed(false),
	Location(r.GetPoint()), Size(r.Width, r.Height),
	BackColor(b), ForeColor(f)
{}

void
MVisual::_m_SetSize(SDST w, SDST h)
{
	if(Size.Width != w || Size.Height != h)
	{
		bBgRedrawed = false;
		Size.Width = w;
		Size.Height = h;
	}
}
void
MVisual::SetBounds(const Rect& r)
{
	Location = r.GetPoint();
	SetSize(r.Width, r.Height);
}


MWidget::MWidget(HWND hWnd, const Rect& r, IWidgetContainer* pCon, Color b, Color f)
	: MVisual(r, b, f),
	hWindow(hWnd), pContainer(pCon ? pCon : GetPointer(hWnd))
{}

Point
MWidget::GetLocationForWindow() const
{
	return pContainer != NULL
		? Widgets::GetLocationOffset(dynamic_cast<IWidget*>(pContainer), Location, hWindow)
		: Point::FullScreen;
}
Point
MWidget::GetLocationForParentContainer() const
{
	return pContainer != NULL
		? Widgets::GetContainerLocationOffset(*pContainer, Location)
		: Point::FullScreen;
}
Point
MWidget::GetLocationForParentWindow() const
{
	return pContainer != NULL
		? Widgets::GetWindowLocationOffset(*pContainer, Location)
		: Point::FullScreen;
}

void
MWidget::Fill(Color c)
{
	if(hWindow != NULL)
	{
		Graphics g(*hWindow);

		FillRect<PixelType>(g.GetBufferPtr(), g.GetSize(), GetBounds(), c);
	}
}
void
MWidget::DrawBackground()
{
	if(!Transparent)
		Fill();
}
void
MWidget::DrawForeground()
{
	if(!Transparent)
		SetBgRedrawed(false);
}

void
MWidget::Refresh()
{
	if(hWindow != NULL)
		hWindow->SetRefresh(true);
}


YWidget::YWidget(HWND hWnd, const Rect& r, IWidgetContainer* pCon)
	: YComponent(), MWidget(hWnd, r, pCon)
{
	if(pContainer != NULL)
		*pContainer += *this;
}
YWidget::~YWidget()
{
	if(pContainer != NULL)
		*pContainer -= *this;
}


MWidgetContainer::MWidgetContainer()
	: GMFocusResponser<IVisualControl>(),
	sWgtSet()
{}

IVisualControl*
MWidgetContainer::GetFocusingPtr() const
{
	return GMFocusResponser<IVisualControl>::GetFocusingPtr();
}
IWidget*
MWidgetContainer::GetTopWidgetPtr(const Point& pt) const
{
	for(WidgetSet::const_iterator i(sWgtSet.begin()); i != sWgtSet.end(); ++i)
		if((*i)->Contains(pt))
			return *i;
	return NULL;
}
IVisualControl*
MWidgetContainer::GetTopVisualControlPtr(const Point& pt) const
{
	for(FOs::const_iterator i(sFOs.begin()); i != sFOs.end(); ++i)
	{
		if(*i != NULL && (*i)->Contains(pt))
			return *i;
	}
	return NULL;
}

YWidgetContainer::YWidgetContainer(HWND hWnd, const Rect& r, IWidgetContainer* pCon)
	: YComponent(), MWidget(hWnd, r, pCon), MWidgetContainer()
{
	if(pContainer != NULL)
	{
		*pContainer += static_cast<IWidget&>(*this);
		*pContainer += static_cast<GMFocusResponser<IVisualControl>&>(*this);
	}
}
YWidgetContainer::~YWidgetContainer()
{
	if(pContainer != NULL)
	{
		*pContainer -= static_cast<GMFocusResponser<IVisualControl>&>(*this);
		*pContainer -= static_cast<IWidget&>(*this);
	}
}


void
MLabel::PaintText(MWidget& w)
{
	HWND hWnd(w.GetWindowHandle());

	if(hWnd != NULL && prTextRegion != NULL)
	{
		prTextRegion->Font = Font;
		prTextRegion->Font.Update();
		prTextRegion->SetPen();
		prTextRegion->Color = w.ForeColor;
		prTextRegion->SetSize(w.GetWidth(), w.GetHeight());
		prTextRegion->SetMargins(2, 2, 2, 2);
		prTextRegion->PutLine(Text);

		Point pt(w.GetLocationForWindow());
		Graphics g(*hWnd);

		prTextRegion->BlitToBuffer(g.GetBufferPtr(), RDeg0,
			g.GetSize(), Point::Zero, pt, w.GetSize());
		prTextRegion->SetSize(0, 0);
	}
}

void
YLabel::DrawForeground()
{
//	if(!Transparent)
	//	Fill();
	ParentType::DrawForeground();
	PaintText(*this);
}

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

