// YSLib::Helper -> Shell_DS by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-3-13 14:17:14;
// UTime = 2010-7-11 15:56;
// Version = 0.1314;


#include "shlds.h"

YSL_BEGIN

ShlGUI::ShlGUI()
: YShellMain()
{}
ShlGUI::~ShlGUI()
{}

void
ShlGUI::SendDrawingMessage()
{
	DispatchWindows();
	InsertMessage(NULL, SM_PAINT, 0xE0, reinterpret_cast<WPARAM>(this), reinterpret_cast<LPARAM>(pDesktopUp));
	InsertMessage(NULL, SM_PAINT, 0xE0, reinterpret_cast<WPARAM>(this), reinterpret_cast<LPARAM>(pDesktopDown));
}
void
ShlGUI::DrawWindows()
{
	YAssert(pDesktopUp,
		"In function \"void\n"
		"DS::ShlGUI::DrawWindows()\": \n"
		"The desktop pointer is null.");
	YAssert(pDesktopDown,
		"In function \"void\n"
		"DS::ShlGUI::DrawWindows()\": \n"
		"The desktop pointer is null.");

	pDesktopUp->ClearDesktopObjects();
	pDesktopDown->ClearDesktopObjects();
	DispatchWindows();
	/*
	pDesktopUp->SetRefresh();
	pDesktopUp->Refresh();
	pDesktopDown->SetRefresh();
	pDesktopDown->Refresh();
	*/
}
void
ShlGUI::UpdateToScreen()
{
	YAssert(pDesktopUp,
		"In function \"void\nDS::ShlGUI::UpdateToScreen()\":\n"
		"The desktop pointer is null.");
	YAssert(pDesktopDown,
		"In function \"void\nDS::ShlGUI::UpdateToScreen()\":\n"
		"The desktop pointer is null.");

	pDesktopUp->Update();
	pDesktopDown->Update();
}


YSL_BEGIN_NAMESPACE(DS)

ShlDS::ShlDS()
: ShlGUI()
{}
ShlDS::~ShlDS()
{
}

LRES
ShlDS::ShlProc(const MMSG& msg)
{
	switch(msg.GetMsgID())
	{
	case SM_INPUT:
		ResponseInput(msg);
		return 0;

	default:
		return DefShellProc(msg);
	}
}


void
ResponseInput(const MMSG& msg)
{
	Runtime::KeysInfo* p(reinterpret_cast<Runtime::KeysInfo*>(msg.GetWParam()));

	if(p)
	{
		if(p->up & Runtime::Keys::Touch)
			OnTouchUp(msg.GetCursorLocation());
		else if(p->up)
			OnKeyUp(p->up);
		if(p->down & Runtime::Keys::Touch)
			OnTouchDown(msg.GetCursorLocation());
		else if(p->down)
			OnKeyDown(p->down);
		if(p->held & Runtime::Keys::Touch)
			OnTouchHeld(msg.GetCursorLocation());
		else if(p->held)
			OnKeyHeld(p->held);
	}
}

YSL_END_NAMESPACE(DS)

YSL_END

