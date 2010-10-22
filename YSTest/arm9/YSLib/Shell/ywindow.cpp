// YSLib::Shell::YWindow by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-22 17:28:28 + 08:00;
// UTime = 2010-10-22 13:53 + 08:00;
// Version = 0.3094;


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
	: MDesktopObject(pDsk),
	Buffer(), hShell(hShl), prBackImage(i), bRefresh(false), bUpdate(false)
{}


AWindow::AWindow(const Rect& r, const GHResource<YImage> i, YDesktop* pDsk, HSHL hShl, HWND hWnd)
	: AVisualControl(hWnd ? hWnd : HWND(pDsk), r), MWindow(i, pDsk, hShl)
{}

BitmapPtr
AWindow::GetBackgroundPtr() const
{
	return prBackImage ? prBackImage->GetImagePtr() : NULL;
}

void
AWindow::SetSize(const Drawing::Size& s)
{
	Buffer.SetSize(s.Width, s.Height);
	MWidget::SetSize(s);
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
		Fill(BackColor);
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

		HWND hWnd(GetWindowHandle());

		if(hWnd != NULL)
		{
			UpdateToWindow();
			hWnd->SetUpdate(true);
		}
	}
}

void
AWindow::UpdateToScreen(YDesktop& d) const
{
	if(Visible)
		Buffer.CopyToBuffer(d.GetBackgroundPtr(), RDeg0, d.GetSize(), Point::Zero, Location, Buffer);
}

void
AWindow::UpdateToWindow(IWindow& w) const
{
	if(Visible)
	{
		const Graphics g(w);

		Buffer.CopyToBuffer(g.GetBufferPtr(), RDeg0, g.GetSize(), Point::Zero, Location, Buffer);
	}
}

void
AWindow::Show()
{
	Visible = true;
	Draw();
	UpdateToScreen();
}


YFrameWindow::YFrameWindow(const Rect& r, const GHResource<YImage> i, YDesktop* pDsk, HSHL hShl, HWND hWnd)
	: YComponent(), AWindow(r, i, pDsk, hShl, hWnd), MWidgetContainer()
{
	if(pContainer)
	{
		*pContainer += static_cast<IWidget&>(*this);
		*pContainer += static_cast<GMFocusResponser<IVisualControl>&>(*this);
	}
	SetSize(GetSize());
	DrawBackground();
	InsertMessage(hShell, SM_WNDCREATE, 0xF0, handle_cast<WPARAM>(GetWindowHandle()), reinterpret_cast<LPARAM>(this));
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
	InsertMessage(hShell, SM_WNDDESTROY, 0xF0, handle_cast<WPARAM>(GetWindowHandle()), reinterpret_cast<LPARAM>(this));
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
				w.SetBgRedrawed(true);
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

	SetBgRedrawed(true);
	return result;
}

YSL_END_NAMESPACE(Forms)

YSL_END_NAMESPACE(Components)

YSL_END

