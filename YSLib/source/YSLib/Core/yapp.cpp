/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yapp.cpp
\ingroup Core
\brief 系统资源和应用程序实例抽象。
\version 0.2433;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-27 17:12:36 +0800;
\par 修改时间:
	2011-06-24 20:35 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YApplication;
*/


#include "yapp.h"
#include "../Helper/yglobal.h"
#include "ystatic.hpp"
#include "../Adaptor/yfont.h"

YSL_BEGIN

YLog& YLog::operator<<(char)
{
	return *this;
}
YLog& YLog::operator<<(const char*)
{
	return *this;
}
YLog& YLog::operator<<(const string& s)
{
	return operator<<(s);
}

void
YLog::Error(const char*)
{}
void
YLog::Error(const string& s)
{
	Error(s.c_str());
}

void
YLog::FatalError(const char* s)
{
	ShowFatalError(s);
}
void
YLog::FatalError(const string& s)
{
	FatalError(s.c_str());
}


YApplication::YApplication()
	: YObject(),
	Log(), pMessageQueue(new MessageQueue()),
	pMessageQueueBackup(new MessageQueue()),
	hShell()
{}
YApplication::~YApplication()
{
	//释放主 Shell 。
//	YReset(DefaultShellHandle);
	ApplicationExit(*this, EventArgs());
	delete pMessageQueueBackup;
	delete pMessageQueue;
}

MessageQueue&
YApplication::GetDefaultMessageQueue() ythrow(LoggedEvent)
{
	if(!pMessageQueue)
		throw LoggedEvent("Null default message queue pointer found"
			" @ YApplication::GetDefaultMessageQueue;");
	return *pMessageQueue;
}
MessageQueue&
YApplication::GetBackupMessageQueue() ythrow(LoggedEvent)
{
	if(!pMessageQueueBackup)
		throw LoggedEvent("Null backup message queue pointer found"
			" @ YApplication::GetBackupMessageQueue;");
	return *pMessageQueueBackup;
}

bool
YApplication::SetShellHandle(const shared_ptr<YShell>& h)
{
	using namespace Messaging;

	if(hShell == h)
		return false;
	if(is_valid(hShell))
		hShell->OnDeactivated(Message(h, SM_DEACTIVATED, 0xF0,
			ValueObject(hShell)));
	hShell = h;
	if(is_valid(h))
		h->OnActivated(Message(h, SM_ACTIVATED, 0xF0, ValueObject(h)));
	return is_valid(h);
}


#if YSL_DEBUG_MSG & 2

namespace
{
	int
	PeekMessage_(Message& msg, const shared_ptr<YShell>& hShl, bool bRemoveMsg);
}

int
PeekMessage(Message& msg, const shared_ptr<YShell>& hShl, bool bRemoveMsg)
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

	(Message& msg, const shared_ptr<YShell>& hShl, bool bRemoveMsg)
{
	return FetchAppInstance().GetDefaultMessageQueue().PeekMessage(
		msg, hShl, bRemoveMsg);
}

int
FetchMessage(Message& msg, MessageQueue::SizeType idle_limit,
	const shared_ptr<YShell>& hShl)
{
	if(FetchAppInstance().GetDefaultMessageQueue().GetSize() <= idle_limit)
		Idle();
	return PeekMessage(msg, hShl, true);
}

errno_t
TranslateMessage(const Message&)
{
	// TODO: impl;
	return 0;
}

int
DispatchMessage(const Message& msg)
{
	auto hShl(FetchAppInstance().GetShellHandle());

	YAssert(is_valid(hShl), "Invalid shell handle found @ DispatchMessage;");

	return FetchAppInstance().GetShellHandle()->ShlProc(msg);
}

errno_t
BackupMessageQueue(const Message& msg)
{
	FetchAppInstance().GetBackupMessageQueue().Push(msg);
	return -!msg.IsValid();
}

void
RecoverMessageQueue()
{
	FetchAppInstance().GetDefaultMessageQueue().Merge(
		FetchAppInstance().GetBackupMessageQueue());
}


void
SendMessage(const Message& msg) ynothrow
{
	try
	{
		FetchAppInstance().GetDefaultMessageQueue().Push(msg);

#if YSL_DEBUG_MSG & 1

		void YSDebug_MSG_Insert(Message&);
		YSDebug_MSG_Insert(msg);

#endif

	}
	catch(...)
	{
		FetchAppInstance().Log.FatalError("SendMessage #1;");
	}
}
void
SendMessage(const shared_ptr<YShell>& hShl, Messaging::ID id,
	Messaging::Priority prior, const ValueObject& c) ynothrow
{
	try
	{
		SendMessage(Message(hShl, id, prior, c));
	}
	catch(...)
	{
		FetchAppInstance().Log.FatalError("SendMessage #2;");
	}
}

void
PostQuitMessage(int nExitCode, Messaging::Priority p)
{
	SendMessage<SM_SET>(shared_ptr<YShell>(), p, shared_ptr<YShell>());
	SendMessage<SM_QUIT>(shared_ptr<YShell>(), p, nExitCode);
}

YSL_END

