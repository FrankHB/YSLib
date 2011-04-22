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
\version 0.3197;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-13 21:09:15 +0800;
\par 修改时间:
	2011-04-22 19:10 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YShell;
*/


#include "yshell.h"
#include "yapp.h"
#include "../Helper/yglobal.h"

YSL_BEGIN

/*!
\brief 主 Shell 处理函数。
*/
extern int
MainShlProc(const Message&);

YSL_BEGIN_NAMESPACE(Shells)

using namespace Messaging;

YShell::YShell()
	: YObject()
{}
YShell::~YShell()
{
	if(GetMainShellHandle() == this)
		GetApp().Log.FatalError("Default shell destructed.");
	if(GetApp().GetShellHandle() == this)
		GetApp().ResetShellHandle();
}

bool
YShell::IsActive() const
{
	return GetApp().GetShellHandle() == this;
}

int
YShell::DefShlProc(const Message& msg)
{
	switch(msg.GetMessageID())
	{
	case SM_SET:
	case SM_DROP:
		{
			GHandleContext<GHandle<YShell> >* const
				p(FetchContextRawPtr<GHandleContext<GHandle<YShell> > >(msg));

			if(p)
			{
				GHandle<YShell> h(p->Handle);

				switch(msg.GetMessageID())
				{

				case SM_SET:
					return -!GetApp().SetShellHandle(h);

				case SM_DROP:
					{
						if(h == GetMainShellHandle())
							return 1;
						else if(h->IsActive())
							GetApp().SetShellHandle(GetMainShellHandle());
						if(h->IsActive())
							return -1;
						ResetHandle(h);
					}
				default:
					break;
				}
			}
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
YShell::OnActivated(const Message& msg)
{
	SendMessage(msg);
	return 0;
}

int
YShell::OnDeactivated(const Message& msg)
{
	SendMessage(msg);
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


GHandle<YShell>
GetCurrentShellHandle() ythrow()
{
	return GetApp().GetShellHandle();
}

bool
Activate(GHandle<YShell> h)
{
	return GetApp().SetShellHandle(h);
}


void
PostQuitMessage(int nExitCode, Priority p)
{
	SendMessage(NULL, SM_SET, p,
		new GHandleContext<GHandle<YShell> >(GetMainShellHandle()));
	SendMessage(NULL, SM_QUIT, p,
		new GObjectContext<int>(nExitCode));
}

#if YSL_DEBUG_MSG & 2

static int
PeekMessage_(Message& msg, GHandle<YShell> hShl, bool bRemoveMsg);

int
PeekMessage(Message& msg, GHandle<YShell> hShl, bool bRemoveMsg)
{
	void YSDebug_MSG_Peek(Message&);
	int t(PeekMessage_(msg, hShl, bRemoveMsg));

	YSDebug_MSG_Peek(msg);
	return t;
}

inline int
PeekMessage_

#else

int
PeekMessage

#endif

	(Message& msg, GHandle<YShell> hShl, bool bRemoveMsg)
{
	list<Message> mqt;
	int r(-1);

	while(!GetApp().GetDefaultMessageQueue().IsEmpty())
	{
		Message m(GetApp().GetDefaultMessageQueue().GetMessage());

		if(!hShl || !m.GetShellHandle() || hShl == m.GetShellHandle())
		{
			msg = m;
			if(!bRemoveMsg)
				GetApp().GetDefaultMessageQueue().Insert(m);
			r = m.GetMessageID();
			break;
		}
		else if(!bRemoveMsg)
			mqt.push_back(m);
	}
	Merge(GetApp().GetDefaultMessageQueue(), mqt);
	return r;
}

int
GetMessage(Message& msg, GHandle<YShell> hShl)
{
	if(GetApp().GetDefaultMessageQueue().IsEmpty())
		Idle();
	return PeekMessage(msg, hShl, true);
}

errno_t
TranslateMessage(const Message& /*msg*/)
{
	// TODO: impl;
	return 0;
}

int
DispatchMessage(const Message& msg)
{
	return GetApp().GetShellHandle()->ShlProc(msg);
}

errno_t
BackupMessageQueue(const Message& msg)
{
	return -!GetApp().GetBackupMessageQueue().Insert(msg);
}

void
RecoverMessageQueue()
{
	Merge(GetApp().GetDefaultMessageQueue(), GetApp().GetBackupMessageQueue());
}

YSL_END_NAMESPACE(Shells)

YSL_END

