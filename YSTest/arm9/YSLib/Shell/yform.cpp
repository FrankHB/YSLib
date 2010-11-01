// YSLib::Shell::YForm by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-04-30 00:51:36 + 08:00;
// UTime = 2010-10-28 13:58 + 08:00;
// Version = 0.1431;


#include "yform.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Forms)

using namespace Controls;

YForm::YForm(const Rect& r, const GHResource<YImage> i,
	YDesktop* pDsk, HSHL hShl, HWND hWnd)
	: YFrameWindow(r, i, pDsk, hShl, hWnd)//,
//	Client(this, Rect::Empty/*r*/)
{
//	SetSize(Size.Width, Size.Height);
//	DrawBackground();
}
YForm::~YForm() ythrow()
{}
/*
void
YForm::SetSize(SDST w, SDST h)
{
	YFrameWindow::SetSize(w, h);
//	BitmapBuffer::SetSize(w, h);
//	YWidget::SetSize(w, h);
}
void
YForm::SetBounds(const Rect& r)
{
	YFrameWindow::SetBounds(r);
//	Location = r.GetPoint();
//	SetSize(r.Width, r.Height);
}

void
YForm::DrawBackground()
{
	YFrameWindow::DrawBackground();

//	Client.DrawBackground();
}

void
YForm::DrawForeground()
{
	YFrameWindow::DrawForeground();
//	Client.DrawForeground();

}
void
YForm::Draw()
{
	YFrameWindow::Draw();
}
*/
YSL_END_NAMESPACE(Forms)

YSL_END_NAMESPACE(Components)

YSL_END

