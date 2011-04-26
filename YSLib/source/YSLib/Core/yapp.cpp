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
\version 0.2297;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-27 17:12:36 +0800;
\par 修改时间:
	2011-04-25 12:51 +0800;
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


YApplication::YApplication()
	: YObject(),
	Log(), pMessageQueue(new YMessageQueue()),
	pMessageQueueBackup(new YMessageQueue()),
	hShell(nullptr), pFontCache(nullptr)
{
	ApplicationExit += Destroy_Static;
}
YApplication::~YApplication() ynothrow
{
	//释放主 Shell 。
//	YReset(DefaultShellHandle);
	ApplicationExit(*this, GetStaticRef<EventArgs>());
	delete pMessageQueueBackup;
	delete pMessageQueue;
}

YApplication*
YApplication::GetInstancePtr() ynothrow
{
	//单例对象。
	static YApplication* pInstance(new YApplication());

	return pInstance;
}
YApplication&
YApplication::GetInstance() ynothrow
{
	try
	{
		YApplication* pInstance(GetInstancePtr());
		return *pInstance;
	}
	catch(...)
	{
		YAssert(false, "Fatal error @ YApplication::GetInstance:"
			" the application instance pointer is null.");
	}
}
YMessageQueue&
YApplication::GetDefaultMessageQueue() ythrow(LoggedEvent)
{
	if(!pMessageQueue)
		throw LoggedEvent("Null default message queue pointer found"
			" @ YApplication::GetDefaultMessageQueue;");
	return *pMessageQueue;
}
YMessageQueue&
YApplication::GetBackupMessageQueue() ythrow(LoggedEvent)
{
	if(!pMessageQueueBackup)
		throw LoggedEvent("Null backup message queue pointer found"
			" @ YApplication::GetBackupMessageQueue;");
	return *pMessageQueueBackup;
}
YFontCache&
YApplication::GetFontCache() const ythrow(LoggedEvent)
{
	if(!pFontCache)
		throw LoggedEvent("Null font cache pointer found"
			" @ YApplication::GetFontCache;");
	return *pFontCache;
}

bool
YApplication::SetShellHandle(GHandle<YShell> h)
{
	using namespace Messaging;

	if(h)
	{
		if(hShell == h)
			return false;

		if(hShell)
			hShell->OnDeactivated(Message(h, SM_DEACTIVATED, 0xF0,
				new GHandleContext<GHandle<YShell> >(hShell)));
		hShell = h;
		h->OnActivated(Message(h, SM_ACTIVATED, 0xF0,
			new GHandleContext<GHandle<YShell> >(h)));
	}
	return h;
}

void
YApplication::ResetShellHandle() ynothrow
{
	if(!SetShellHandle(GetMainShellHandle()))
		Log.FatalError("Error occured @ YApplication::ResetShellHandle;");
}

void
YApplication::ResetFontCache(CPATH path) ythrow(LoggedEvent)
{
	try
	{
		ydelete(pFontCache);
		pFontCache = ynew YFontCache(path);
	}
	catch(...)
	{
		throw LoggedEvent("Error occured @ YApplication::ResetFontCache;");
	}
}

void
YApplication::DestroyFontCache()
{
	ydelete(pFontCache);
	pFontCache = nullptr;
}


void
SendMessage(const Message& msg) ynothrow
{
	try
	{
		GetApp().GetDefaultMessageQueue().Insert(msg);

#if YSL_DEBUG_MSG & 1

		void YSDebug_MSG_Insert(Message&);
		YSDebug_MSG_Insert(msg);

#endif
		
	}
	catch(...)
	{
		GetApp().Log.FatalError("SendMessage #1;");
	}
}
void
SendMessage(GHandle<YShell> hShl, Messaging::ID id,
	Messaging::Priority prior, Messaging::IContext* pContext) ynothrow
{
	try
	{
		SendMessage(Message(hShl, id, prior, pContext));
	}
	catch(...)
	{
		GetApp().Log.FatalError("SendMessage #2;");
	}
}

YSL_END

