// YSLib::Helper -> Shell_DS by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-03-13 14:17:14 + 08:00;
// UTime = 2010-10-27 12:32 + 08:00;
// Version = 0.1418;


#include "shlds.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Shells)

LRES
ShlCLI::OnActivated(const Message&)
{
	return 0;
}

LRES
ShlCLI::OnDeactivated(const Message&)
{
	return 0;
}

IRES
ShlCLI::ExecuteCommand(const uchar_t* s)
{
	return 0;
}


LRES
ShlGUI::OnDeactivated(const Message&)
{
	ClearScreenWindows(*pDesktopUp);
	ClearScreenWindows(*pDesktopDown);
	return 0;
}

void
ShlGUI::SendDrawingMessage()
{
//	pDesktopUp->ClearDesktopObjects();
//	pDesktopDown->ClearDesktopObjects();
	DispatchWindows();
	InsertMessage(NULL, SM_PAINT, 0xE0,
		reinterpret_cast<WPARAM>(this), reinterpret_cast<LPARAM>(pDesktopUp));
	InsertMessage(NULL, SM_PAINT, 0xE0,
		reinterpret_cast<WPARAM>(this), reinterpret_cast<LPARAM>(pDesktopDown));
}

void
ShlGUI::UpdateToScreen()
{
/*
	YAssert(pDesktopUp,
		"In function \"void\nDS::ShlGUI::UpdateToScreen()\":\n"
		"The desktop pointer is null.");
	YAssert(pDesktopDown,
		"In function \"void\nDS::ShlGUI::UpdateToScreen()\":\n"
		"The desktop pointer is null.");
*/
	pDesktopUp->Refresh();
	pDesktopUp->Update();
	pDesktopDown->Refresh();
	pDesktopDown->Update();
}

YSL_END_NAMESPACE(Shells)


YSL_BEGIN_NAMESPACE(DS)

ShlDS::ShlDS()
	: ShlGUI()
{}

LRES
ShlDS::ShlProc(const Message& msg)
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

LRES
ShlDS::OnDeactivated(const Message& m)
{
	ShlGUI::OnDeactivated(m);
	YDelete(hWndUp);
	YDelete(hWndDown);
	return 0;
}


void
ResponseInput(const Message& msg)
{
	Runtime::KeysInfo* p(reinterpret_cast<Runtime::KeysInfo*>(msg.GetWParam()));

	if(p)
	{
		using namespace Runtime::KeySpace;

		if(p->up & Touch)
			OnTouchUp(msg.GetCursorLocation());
		else if(p->up)
			OnKeyUp(p->up);
		if(p->down & Touch)
			OnTouchDown(msg.GetCursorLocation());
		else if(p->down)
			OnKeyDown(p->down);
		if(p->held & Touch)
			OnTouchHeld(msg.GetCursorLocation());
		else if(p->held)
			OnKeyHeld(p->held);
	}
}

YSL_END_NAMESPACE(DS)

YSL_END

