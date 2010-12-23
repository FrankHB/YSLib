/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yapp.cpp
\ingroup Core
\brief 应用程序实例类抽象。
\version 0.2141;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-27 17:12:36 + 08:00;
\par 修改时间:
	2010-12-23 13:02 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YApplication;
*/


#include "yapp.h"
#include "../Helper/yglobal.h"

YSL_BEGIN

YLog::YLog()
{}
YLog::~YLog()
{}

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


YApplication::YApplication(GHHandle<YScreen>& hScr, GHHandle<YDesktop>& hDsk)
	: YObject(),
	Log(DefaultLog), hDefaultScreen(hScr), hDefaultDesktop(hDsk),
	pMessageQueue(new YMessageQueue()),
	pMessageQueueBackup(new YMessageQueue()),
	hShell(NULL), pFontCache(NULL)
{
	ApplicationExit += Destroy_Static;
}
YApplication::~YApplication() ythrow()
{
	//释放主 Shell 。
//	YReset(DefaultShellHandle);
	EventArgs e;

	ApplicationExit(*this, e);
	delete pMessageQueue;
	delete pMessageQueueBackup;
}

YApplication*
YApplication::GetInstancePtr(GHHandle<YScreen>& hScr, GHHandle<YDesktop>& hDsk)
{
	//单例对象。
	static YApplication* pInstance(new YApplication(hScr, hDsk));

	return pInstance;
}

YMessageQueue&
YApplication::GetDefaultMessageQueue() ythrow(LoggedEvent)
{
	if(!pMessageQueue)
		throw LoggedEvent("Get default message queue failed @@ YApplication.");
	return *pMessageQueue;
}
YMessageQueue&
YApplication::GetBackupMessageQueue() ythrow(LoggedEvent)
{
	if(!pMessageQueueBackup)
		throw LoggedEvent("Get backup message queue failed @@ YApplication.");
	return *pMessageQueueBackup;
}

bool
YApplication::SetShellHandle(GHHandle<YShell> h)
{
	using namespace Messaging;

	if(h)
	{
		if(hShell == h)
			return false;

		if(hShell)
			hShell->OnDeactivated(Message(NULL, SM_DEACTIVATED, 0xF0,
				new GHandleContext<GHHandle<YShell> >(hShell)));
		hShell = h;
		h->OnActivated(Message(NULL, SM_ACTIVATED, 0xF0,
			new GHandleContext<GHHandle<YShell> >(h)));
	}
	return h;
}

void
YApplication::ResetShellHandle() ythrow()
{
	if(!SetShellHandle(DefaultShellHandle))
		Log.FatalError("YApplication::ResetShellHandle();");
}


void
SendMessage(const Message& msg) ythrow()
{
	try
	{
		theApp.GetDefaultMessageQueue().Insert(msg);

#if YSL_DEBUG_MSG & 1

		void YSDebug_MSG_Insert(Message&);
		YSDebug_MSG_Insert(msg);

#endif
		
	}
	catch(...)
	{
		theApp.Log.FatalError("SendMessage #1;");
	}
}
void
SendMessage(GHHandle<YShell> hShl, Messaging::ID id,
	Messaging::Priority prior, Messaging::IContext* pContext) ythrow()
{
	try
	{
		SendMessage(Message(hShl, id, prior, pContext));
	}
	catch(...)
	{
		theApp.Log.FatalError("SendMessage #2;");
	}
}

YSL_END

