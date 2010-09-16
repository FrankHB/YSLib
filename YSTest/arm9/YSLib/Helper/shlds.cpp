// YSLib::Helper -> Shell_DS by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-3-13 14:17:14;
// UTime = 2010-9-16 23:29;
// Version = 0.1398;


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
	InsertMessage(NULL, SM_PAINT, 0xE0, reinterpret_cast<WPARAM>(this), reinterpret_cast<LPARAM>(pDesktopUp));
	InsertMessage(NULL, SM_PAINT, 0xE0, reinterpret_cast<WPARAM>(this), reinterpret_cast<LPARAM>(pDesktopDown));
}

void
ShlGUI::UpdateToScreen()
{
/*
	YAssert(pDesktopUp != NULL,
		"In function \"void\nDS::ShlGUI::UpdateToScreen()\":\n"
		"The desktop pointer is null.");
	YAssert(pDesktopDown != NULL,
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
ShlDS::~ShlDS() ythrow()
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
		if(p->up & Runtime::Key::Touch)
			OnTouchUp(msg.GetCursorLocation());
		else if(p->up)
			OnKeyUp(p->up);
		if(p->down & Runtime::Key::Touch)
			OnTouchDown(msg.GetCursorLocation());
		else if(p->down)
			OnKeyDown(p->down);
		if(p->held & Runtime::Key::Touch)
			OnTouchHeld(msg.GetCursorLocation());
		else if(p->held)
			OnKeyHeld(p->held);
	}
}

YSL_END_NAMESPACE(DS)

YSL_END

