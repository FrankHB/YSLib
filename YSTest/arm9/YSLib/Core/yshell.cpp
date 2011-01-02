/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yshell.cpp
\ingroup Core
\brief Shell 定义。
\version 0.3105;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-13 21:09:15 + 08:00;
\par 修改时间:
	2011-01-01 19:26 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YShell;
*/


#include "yapp.h"
#include "../Shell/ygui.h"
#include "../Helper/yglobal.h"

YSL_BEGIN

/*!
\brief 主 Shell 处理函数。
*/
extern int
MainShlProc(const Message&);

YSL_BEGIN_NAMESPACE(Shells)

using namespace Components;
using namespace Components::Controls;
using namespace Messaging;

YShell::YShell()
	: YObject()
{}
YShell::~YShell() ythrow()
{
	if(theApp.DefaultShellHandle == this)
		theApp.Log.FatalError("Default shell destructed.");
	if(theApp.GetShellHandle() == this)
		theApp.ResetShellHandle();
}

bool
YShell::IsActive() const
{
	return theApp.GetShellHandle() == this;
}

int
YShell::DefShlProc(const Message& msg)
{
	switch(msg.GetMessageID())
	{
	case SM_SET:
	case SM_DROP:
		{
			GHandleContext<GHHandle<YShell> >* const
				p(FetchContextRawPtr<GHandleContext<GHHandle<YShell> > >(msg));

			if(p)
			{
				GHHandle<YShell> h(p->Handle);

				switch(msg.GetMessageID())
				{

				case SM_SET:
					return -!theApp.SetShellHandle(h);

				case SM_DROP:
					{
						if(h == YApplication::DefaultShellHandle)
							return 1;
						else if(h->IsActive())
							theApp.SetShellHandle(YApplication
								::DefaultShellHandle);
						if(h->IsActive())
							return -1;
						YReset(h);
						return 0;
					}
				default:
					break;
				}
			}
		}

	case SM_PAINT:
		{
			GHandleContext<GHHandle<YDesktop> >* const
				p(CastMessage<SM_PAINT>(msg));

			if(p && p->Handle)
				p->Handle->Draw();
		}
		return 0;

	case SM_QUIT:
		{
			GObjectContext<int>* const p(CastMessage<SM_QUIT>(msg));

			if(p)
				std::exit(p->Object);
		}

	default:
		break;
	}
	return 0;
}

int
YShell::OnActivated(const Message& m)
{
	SendMessage(m);
	return 0;
}

int
YShell::OnDeactivated(const Message& m)
{
	SendMessage(m);
	return 0;
}


YMainShell::YMainShell()
	: YShell()
{}

int
YMainShell::ShlProc(const Message& msg)
{
	return MainShlProc(msg);
}


bool
Activate(GHHandle<YShell> h)
{
	return theApp.SetShellHandle(h);
}


void
PostQuitMessage(int nExitCode, Priority p)
{
	SendMessage(NULL, SM_SET, p,
		new GHandleContext<GHHandle<YShell> >(theApp.DefaultShellHandle));
	SendMessage(Message(NULL, SM_QUIT, p,
		new GObjectContext<int>(nExitCode)));
}

#if YSL_DEBUG_MSG & 2

static int
PeekMessage_(Message& msg, GHHandle<YShell> hShl,
	ID wMsgFilterMin, ID wMsgFilterMax, ID wRemoveMsg);

int
PeekMessage(Message& msg, GHHandle<YShell> hShl,
	ID wMsgFilterMin, ID wMsgFilterMax, ID wRemoveMsg)
{
	void YSDebug_MSG_Peek(Message&);
	int t(PeekMessage_(msg, hShl, wMsgFilterMin, wMsgFilterMax, wRemoveMsg));

	YSDebug_MSG_Peek(msg);
	return t;
}

inline int
PeekMessage_

#else

int
PeekMessage

#endif

	(Message& msg, GHHandle<YShell> hShl,
		ID wMsgFilterMin, ID wMsgFilterMax,
		u32 wRemoveMsg)
{
	if(!theApp.GetDefaultMessageQueue().empty())
	{
		vector<Message> mqt;
		Message m;

		while(!theApp.GetDefaultMessageQueue().empty())
		{
			theApp.GetDefaultMessageQueue().GetMessage(m);
			if(!hShl || !m.GetShellHandle() || hShl == m.GetShellHandle())
			{
				ID msgID(m.GetMessageID());

				if(!(wMsgFilterMin || wMsgFilterMax)
					|| (wMsgFilterMin <= msgID && msgID <= wMsgFilterMax))
				{
					msg = m;

					if(wRemoveMsg == PM_NOREMOVE)
						theApp.GetDefaultMessageQueue().Insert(m);
					Merge(theApp.GetDefaultMessageQueue(), mqt);
					return msgID;
				}
			}
			mqt.push_back(m);
		}
		Merge(theApp.GetDefaultMessageQueue(), mqt);
	}
	return -1;
}

int
GetMessage(Message& msg, GHHandle<YShell> hShl,
	ID wMsgFilterMin, ID wMsgFilterMax)
{
	if(theApp.GetDefaultMessageQueue().empty())
		Idle();
	return PeekMessage(msg, hShl, wMsgFilterMin, wMsgFilterMax, PM_REMOVE);
}

ystdex::errno_t
TranslateMessage(const Message& /*msg*/)
{
	// TODO: impl;
	return 0;
}

int
DispatchMessage(const Message& msg)
{
	return theApp.GetShellHandle()->ShlProc(msg);
}

ystdex::errno_t
BackupMessage(const Message& msg)
{
	return -!theApp.GetBackupMessageQueue().Insert(msg);
}

void
RecoverMessageQueue()
{
	Merge(theApp.GetDefaultMessageQueue(), theApp.GetBackupMessageQueue());
}

YSL_END_NAMESPACE(Shells)

YSL_END

