// YSLib::Shell::YWindow by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-22 17:28:28;
// UTime = 2010-9-1 14:41;
// Version = 0.2960;


#include "ydesktop.h"
#include "ycontrol.h"

YSL_BEGIN

using namespace Components::Controls;
using namespace Components::Widgets;
using namespace Drawing;
using namespace Runtime;

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Forms)

MWindow::MWindow(const GHResource<YImage> i, YDesktop* pDsk, HSHL hShl)
: MDesktopObject(pDsk), MVisualControl(~0, 0),
Buffer(), hShell(hShl), prBackImage(i), bRefresh(false), bUpdate(false)
{}


AWindow::AWindow(const SRect& r, const GHResource<YImage> i, YDesktop* pDsk, HSHL hShl, HWND hWnd)
: MWidget(hWnd ? hWnd : HWND(pDsk), r), MWindow(i, pDsk, hShl)
{
}
AWindow::~AWindow()
{
}

BitmapPtr
AWindow::GetBackgroundPtr() const
{
	return prBackImage ? prBackImage->GetImagePtr() : NULL;
}

void
AWindow::SetSize(SDST w, SDST h)
{
	Buffer.SetSize(w, h);
	MWidget::SetSize(w, h);
}
void
AWindow::SetBounds(const SRect& r)
{
	Location = r.GetPoint();
	SetSize(r.Width, r.Height);
}

void
AWindow::RequestToTop()
{
	if(pDesktop)
		pDesktop->MoveToTop(*this);
}

bool
AWindow::DrawBackgroundImage()
{
	if(prBackImage != NULL && GetBufferPtr() != NULL)
	{
		ConstBitmapPtr imgBg(prBackImage->GetImagePtr());

		if(imgBg != NULL)
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
	if(!DrawBackgroundImage())
		Buffer.Fill(BackColor);
	bUpdate = true;
}
void
AWindow::DrawForeground()
{
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
	MWidget::Refresh();
}

void
AWindow::Update()
{
	if(bUpdate)
	{
		bUpdate = false;
		if(hWindow != NULL)
		{
			UpdateToWindow();
			hWindow->SetUpdate();
		}
	}
}

void
AWindow::RequestFocus(const MEventArgs& e)
{
	GMFocusResponser<IVisualControl>* const p(CheckFocusContainer(pContainer));

	if(p && AFocusRequester::RequestFocus(*p))
		EventMap[EControl::GotFocus](*this, e);
}
void
AWindow::ReleaseFocus(const MEventArgs& e)
{
	GMFocusResponser<IVisualControl>* const p(CheckFocusContainer(pContainer));

	if(p && AFocusRequester::ReleaseFocus(*p))
		EventMap[EControl::LostFocus](*this, e);
}

void
AWindow::UpdateToScreen(YDesktop& d) const
{
	if(Visible)
		Buffer.CopyToBuffer(d.GetBackgroundPtr(), RDeg0, d.GetSize(), SPoint::Zero, Location, Buffer);
}
void
AWindow::UpdateToWindow(IWindow& w) const
{
	if(Visible)
		Buffer.CopyToBuffer(w.GetBufferPtr(), RDeg0, w.GetSize(), SPoint::Zero, Location, Buffer);
}
void
AWindow::Show()
{
	Visible = true;
	Draw();
	UpdateToScreen();
}

void
AWindow::OnGotFocus(IControl& c, const MEventArgs& e)
{
	try
	{
		dynamic_cast<AWindow&>(c).Refresh();
	}
	catch(std::bad_cast&)
	{}
}
void
AWindow::OnLostFocus(IControl& c, const MEventArgs& e)
{
	OnGotFocus(c, e);
/*	try
	{
		dynamic_cast<AWindow&>(c).Refresh();
	}
	catch(std::bad_cast&)
	{}*/
}


YFrameWindow::YFrameWindow(const SRect& r, const GHResource<YImage> i, YDesktop* pDsk, HSHL hShl, HWND hWnd)
: YComponent(), AWindow(r, i, pDsk, hShl, hWnd), MWidgetContainer()
{
	if(pContainer)
	{
		*pContainer += static_cast<IWidget&>(*this);
		*pContainer += static_cast<GMFocusResponser<IVisualControl>&>(*this);
	}
	SetSize(Size.Width, Size.Height);
	DrawBackground();
	InsertMessage(hShell, SM_WNDCREATE, 0xF0, handle_cast<WPARAM>(hWindow), reinterpret_cast<LPARAM>(this));
	*hShl += *this;
	if(pDesktop)
		*pDesktop += static_cast<IVisualControl&>(*this);
}
YFrameWindow::~YFrameWindow()
{
	if(pContainer != NULL)
	{
		*pContainer -= static_cast<GMFocusResponser<IVisualControl>&>(*this);
		*pContainer -= static_cast<IWidget&>(*this);
	}
	hShell->RemoveAll(*this);
	if(pDesktop)
		pDesktop->RemoveAll(*this);
	InsertMessage(hShell, SM_WNDDESTROY, 0xF0, handle_cast<WPARAM>(hWindow), reinterpret_cast<LPARAM>(this));
}

SPoint
YFrameWindow::GetContainerLocationOffset(const SPoint& p) const
{
	return p + Location;
}
SPoint
YFrameWindow::GetWindowLocationOffset(const SPoint& p) const
{
	return Widgets::GetLocationOffset(const_cast<YFrameWindow*>(this), p, hWindow);
}

bool
YFrameWindow::DrawWidgets()
{
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

			if(w.IsVisible() && !(IsBgRedrawed() && w.IsBgRedrawed()) && !w.IsTransparent())
			{
				w.DrawBackground();
				w.SetBgRedrawed();
			}
		}
		bBgChanged = true;
	}
	for (i = sWgtSet.begin(); i != sWgtSet.end(); ++i)
	{
		IWidget& w(**i);

		if(w.IsVisible())
			w.DrawForeground();
	}
	DrawForeground();

	bool result(bBgChanged || !IsBgRedrawed());

	SetBgRedrawed();
	return result;
}

YSL_END_NAMESPACE(Forms)

YSL_END_NAMESPACE(Components)

YSL_END

