// YSLib::Shell::YWidget by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-16 20:06:58 + 08:00;
// UTime = 2010-10-01 02:50 + 08:00;
// Version = 0.4076;


#include "ywindow.h"
#include "ycontrol.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Widgets)

using namespace Runtime;
using Controls::YVisualControl;


SPoint
GetLocationOffset(IWidget* pCon, const SPoint& p, const HWND& hWnd)
{
	SPoint pt(p);

	while(pCon && dynamic_cast<IWindow*>(pCon) != hWnd)
	{
		pt += pCon->GetLocation();
		pCon = dynamic_cast<IWidget*>(pCon->GetContainerPtr());
	}
	return pt;
}


MVisual::MVisual(const SRect& r, PixelType b, PixelType f)
: Visible(true), Transparent(false), bBgRedrawed(false),
Location(r.GetPoint()), Size(r.Width, r.Height),
BackColor(b), ForeColor(f)
{}

void
MVisual::SetSize(SDST w, SDST h)
{
	if(Size.Width != w || Size.Height != h)
	{
		bBgRedrawed = false;
		Size.Width = w;
		Size.Height = h;
	}
}
void
MVisual::SetBounds(const SRect& r)
{
	Location = r.GetPoint();
	SetSize(r.Width, r.Height);
}


MWidget::MWidget(HWND hWnd, const SRect& r, IWidgetContainer* pCon, PixelType b, PixelType f)
: MVisual(r, b, f),
hWindow(hWnd), pContainer(pCon ? pCon : GetPointer(hWnd))
{}

SPoint
MWidget::GetLocationForWindow() const
{
	return pContainer ? Widgets::GetLocationOffset(dynamic_cast<IWidget*>(pContainer), Location, hWindow) : SPoint::FullScreen;
}
SPoint
MWidget::GetLocationForParentContainer() const
{
	return pContainer ? pContainer->GetContainerLocationOffset(Location) : SPoint::FullScreen;
}
SPoint
MWidget::GetLocationForParentWindow() const
{
	return pContainer ? pContainer->GetWindowLocationOffset(Location) : SPoint::FullScreen;
}

void
MWidget::Refresh()
{
	if(hWindow != NULL)
		hWindow->SetRefresh();
}


YWidget::YWidget(HWND hWnd, const SRect& r, IWidgetContainer* pCon)
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

void
YWidget::DrawBackground()
{
}
void
YWidget::DrawForeground()
{
}


MWidgetContainer::MWidgetContainer()
: GMFocusResponser<IVisualControl>(),
sWgtSet()
{
}

IVisualControl*
MWidgetContainer::GetFocusingPtr() const
{
	return Runtime::GMFocusResponser<IVisualControl>::GetFocusingPtr();
}
IWidget*
MWidgetContainer::GetTopWidgetPtr(const SPoint& pt) const
{
	for(WidgetSet::const_iterator i(sWgtSet.begin()); i != sWgtSet.end(); ++i)
		if((*i)->Contains(pt))
			return *i;
	return NULL;
}
IVisualControl*
MWidgetContainer::GetTopVisualControlPtr(const SPoint& pt) const
{
	for(FOs::const_iterator i(sFOs.begin()); i != sFOs.end(); ++i)
	{
		IWidget* const p(dynamic_cast<IWidget*>(*i));

		if(p && p->Contains(pt))
			return *i;
	}
	return NULL;
}

YWidgetContainer::YWidgetContainer(HWND hWnd, const SRect& r, IWidgetContainer* pCon)
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

SPoint
YWidgetContainer::GetContainerLocationOffset(const SPoint& p) const
{
	return p + Location;
}
SPoint
YWidgetContainer::GetWindowLocationOffset(const SPoint& p) const
{
	return Widgets::GetLocationOffset(const_cast<YWidgetContainer*>(this), p, hWindow);
}

void
YWidgetContainer::DrawBackground()
{
	if(hWindow != NULL)
		hWindow->SetRefresh();
}
void
YWidgetContainer::DrawForeground()
{
	if(hWindow != NULL)
		hWindow->SetRefresh();
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

		SPoint pt(w.GetLocationForWindow());

		prTextRegion->BlitToBuffer(hWnd->GetBufferPtr(), RDeg0,
			hWnd->GetSize(), SPoint::Zero, pt, w.GetSize());
		prTextRegion->SetSize(0, 0);
	}
}

void
YLabel::DrawForeground()
{
	if(!Transparent)
		FillRect(hWindow->GetBufferPtr(), hWindow->GetSize(), GetBounds(), BackColor);
	ParentType::DrawForeground();
	PaintText(*this);
}

YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

