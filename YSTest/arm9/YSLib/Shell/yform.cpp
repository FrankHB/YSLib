// YSLib::Shell::YForm by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-4-30 0:51:36;
// UTime = 2010-7-4 5:32;
// Version = 0.1414;


#include "yform.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Forms)

using namespace Controls;

YForm::YForm(const SRect& r, const HResource<YImage> i, YDesktop* pDsk, HSHL hShl, HWND hWnd)
: YFrameWindow(r, i, pDsk, hShl, hWnd)//,
//Client(this, SRect::Empty/*r*/)
{
//	SetSize(Size.Width, Size.Height);
//	DrawBackground();
}
YForm::~YForm()
{}
/*
void
YForm::SetSize(SDST w, SDST h)
{
	YFrameWindow::SetSize(w, h);
//	MBitmapBuffer::SetSize(w, h);
//	YWidget::SetSize(w, h);
}
void
YForm::SetBounds(const SRect& r)
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

