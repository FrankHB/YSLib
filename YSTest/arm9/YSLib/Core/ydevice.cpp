// YSLib::Core::YDevice by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-28 16:39:51;
// UTime = 2010-6-11 15:10;
// Version = 0.2650;


#include "ydevice.h"
#include "yshell.h"
#include "../Shell/ygdi.h"

YSL_BEGIN

using namespace Drawing;

YSL_BEGIN_NAMESPACE(Device)

bool YScreen::S_InitScr(true);

YScreen::YScreen(SDST w, SDST h, BitmapPtr p)
: YGraphicDevice(w, h, p),
bg(-1)
{}

void
YScreen::CheckInit()
{
	if(S_InitScr)
		InitScreen();
}

void
YScreen::Reset()
{
	InitScreen();
}

void
YScreen::Update(BitmapPtr buf)
{
	platform::scrCopy(GetPtr(), buf);
}
void
YScreen::Update(PixelType c)
{
	FillSeq(GetPtr(), GetArea(), c);
}

YSL_END_NAMESPACE(Device)

YSL_END

