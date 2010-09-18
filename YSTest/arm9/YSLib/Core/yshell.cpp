// YSLib::Core::YShell by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-13 21:09:15;
// UTime = 2010-9-18 10:31;
// Version = 0.2739;


#include "../Shell/ywindow.h"
#include "../Shell/ycontrol.h"
#include "../Shell/ygui.h"
#include "yapp.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Shells)

using namespace Components;
using namespace Components::Controls;

YShell::YShell()
: sWnds(), insRefresh(false), scrType(SCR_BOTH)
{
	InsertMessage(NULL, SM_CREATE, 0xF0, reinterpret_cast<WPARAM>(this));
}
YShell::~YShell() ythrow()
{
	if(theApp.GetShellHandle() == this)
		theApp.ResetShellHandle();
	InsertMessage(NULL, SM_DESTROY, 0xF0, reinterpret_cast<WPARAM>(this));
}

bool
YShell::IsActive() const
{
	return theApp.GetShellHandle() == this;
}

bool
YShell::Activate()
{
	return theApp.SetShellHandle(this);
}

void
YShell::operator+=(IWindow& w)
{
	sWnds.push_back(&w);
}
bool
YShell::operator-=(IWindow& w)
{
	WNDs::iterator i(std::find(sWnds.begin(), sWnds.end(), &w));

	if(i == sWnds.end())
		return false;
	sWnds.erase(i);
	return true;
}
YShell::WNDs::size_type YShell::RemoveAll(IWindow& w)
{
	return erase_all(sWnds, &w);
}

void
YShell::RemoveWindow()
{
	sWnds.pop_back();
}
HWND
YShell::GetFirstWindowHandle() const
{
	return HWND(sWnds.empty() ? NULL : sWnds.front());
}
HWND
YShell::GetTopWindowHandle() const
{
	return HWND(sWnds.empty() ? NULL : sWnds.back());
}
HWND
YShell::GetTopWindowHandle(YDesktop& d, const SPoint& p) const
{
	for(WNDs::const_iterator i(sWnds.begin()); i != sWnds.end(); ++i)
	{
		if((*i)->GetDesktopPtr() == &d && (*i)->Contains(p))
			return HWND(*i);
	}
	return NULL;
}

bool
YShell::SendWindow(IWindow& w)
{
	if(std::find(sWnds.begin(), sWnds.end(), &w) != sWnds.end())
	{
		YDesktop* const pDsk(w.GetDesktopPtr());

		if(pDsk)
		{
			*pDsk += static_cast<IVisualControl&>(w);
			return true;
		}
	}
	return false;
}

void
YShell::DispatchWindows()
{
	for(WNDs::const_iterator i(sWnds.begin()); i != sWnds.end(); ++i)
	{
		YDesktop* const pDsk((*i)->GetDesktopPtr());

		if(pDsk)
			*pDsk += *static_cast<IVisualControl*>(GetPointer(*i));
	}
}

void
YShell::ClearScreenWindows(YDesktop& d)
{
	for(WNDs::const_iterator i(sWnds.begin()); i != sWnds.end(); ++i)
		d.RemoveAll(*static_cast<IVisualControl*>(GetPointer(*i)));
}

LRES
YShell::DefShlProc(const Message& msg)
{
//	const HSHL& hShl(msg.GetShellHandle());
	const WPARAM& wParam(msg.GetWParam());
	const LPARAM& lParam(msg.GetLParam());

	switch(msg.GetMsgID())
	{
	case SM_PAINT:
		if(lParam)
			reinterpret_cast<YDesktop*>(lParam)->Draw();
	/*
		hDC = BeginPaint(hWnd, &ps);
		TextOut(hDC, 0, 0, str, (int)wcslen(str));
		EndPaint(hWnd, &ps);
	*/
		return 0;

	case SM_CREATE:
		return 0;

	case SM_DESTROY:
		if(reinterpret_cast<YShell*>(lParam) == hShellMain)
			PostQuitMessage(lParam);
		return 0;

	case SM_SET:
		return -!theApp.SetShellHandle(HSHL(reinterpret_cast<YShell*>(wParam)));

	case SM_DROP:
		{
			HSHL hShl(reinterpret_cast<YShell*>(wParam));
			if(hShl == hShellMain)
				return 1;
			else if(hShl->IsActive())
				theApp.SetShellHandle(hShellMain);
			if(hShl->IsActive())
				return -1;
			YDelete(hShl);
			return 0;
		}

	case SM_SETFOCUS:
	case SM_KILLFOCUS:
	case SM_WNDCREATE:
	case SM_WNDDESTROY:
	case SM_SCRREFRESH:
		return 0;

	case SM_WNDDROP:
		YDelete(HWND(reinterpret_cast<IWindow*>(wParam)));
		return 0;

	case SM_QUIT:
		std::exit(lParam);

	default:
		break;
	}
	return 0;
}

LRES
YShell::OnActivated(const Message& m)
{
	InsertMessage(m);
	return 0;
}

LRES
YShell::OnDeactivated(const Message& m)
{
	InsertMessage(m);
	return 0;
}


YShellMain::YShellMain()
: YShell()
{
	theApp += *this;
}


void
PostQuitMessage(int nExitCode)
{
	InsertMessage(Message(NULL, SM_QUIT, 0xFF));
}

#if YSLIB_DEBUG_MSG & 2

static IRES
PeekMessage_(Message& msg, HSHL hShl, MSGID wMsgFilterMin, MSGID wMsgFilterMax, MSGID wRemoveMsg);

IRES
PeekMessage(Message& msg, HSHL hShl, MSGID wMsgFilterMin, MSGID wMsgFilterMax, MSGID wRemoveMsg)
{
	void YSDebug_MSG_Peek(Message&);
	IRES t(PeekMessage_(msg, hShl, wMsgFilterMin, wMsgFilterMax, wRemoveMsg));

	YSDebug_MSG_Peek(msg);
	return t;
}

inline IRES
PeekMessage_

#else

IRES
PeekMessage

#endif

	(Message& msg, HSHL hShl, MSGID wMsgFilterMin, MSGID wMsgFilterMax, u32 wRemoveMsg)
{
	if(!theApp.GetDefaultMessageQueue().empty())
	{
		std::vector<Message> mqt;
		Message m;
	//	void (YMessageQueue::*fngmq)(Message&) = wRemoveMsg & PM_REMOVE ? &YMessageQueue::GetMessage : (void (YMessageQueue::*)(Message&))&YMessageQueue::PeekMessage;
	//	(theApp.GetDefaultMessageQueue().*fngmq)(m);

		while(!theApp.GetDefaultMessageQueue().empty())
		{
			theApp.GetDefaultMessageQueue().GetMessage(m);
			if(!hShl || !m.GetShellHandle() || hShl == m.GetShellHandle())
			{
				MSGID msgID(m.GetMsgID());

				if(!(wMsgFilterMin || wMsgFilterMax) || (wMsgFilterMin <= msgID && msgID <= wMsgFilterMax))
				{
					msg = m;

					if(wRemoveMsg == PM_NOREMOVE)
						theApp.GetDefaultMessageQueue().InsertMessage(m);
					Merge(theApp.GetDefaultMessageQueue(), mqt);
					if(msgID == SM_QUIT)
						return 0;
					return msgID;
				}
			}
			mqt.push_back(m);
		}
		Merge(theApp.GetDefaultMessageQueue(), mqt);
	}
	return -1;
}

IRES
GetMessage(Message& msg, HSHL hShl, MSGID wMsgFilterMin, MSGID wMsgFilterMax)
{
	if(theApp.GetDefaultMessageQueue().empty())
		Def::Idle();
	return PeekMessage(msg, hShl, wMsgFilterMin, wMsgFilterMax, PM_REMOVE);
}

ERRNO
TranslateMessage(const Message& msg)
{
	return 0;
}

LRES
DispatchMessage(const Message& msg)
{
	return theApp.GetShellHandle()->ShlProc(msg);
}

ERRNO
BackupMessage(const Message& msg)
{
	return -!theApp.GetBackupMessageQueue().InsertMessage(msg);
}

void
RecoverMessageQueue()
{
	Merge(theApp.GetDefaultMessageQueue(), theApp.GetBackupMessageQueue());
}

YSL_END_NAMESPACE(Shells)

YSL_END

