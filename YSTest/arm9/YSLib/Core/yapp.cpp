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
\version 0.1990;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-27 17:12:36 + 08:00;
\par 修改时间:
	2010-12-03 15:26 + 08:00;
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
YLog::FatalError(const char*)
{
	terminate();
}
void
YLog::FatalError(const string& s)
{
	FatalError(s.c_str());
}


YApplication::YApplication(YScreen*& pScr, YDesktop*& pDsk)
	: YObject(),
	Log(DefaultLog), pDefaultScreen(pScr), pDefaultDesktop(pDsk),
	pMessageQueue(new YMessageQueue()),
	pMessageQueueBackup(new YMessageQueue()),
	sShls(), hShell(NULL), pFontCache(NULL)
{
	ApplicationExit += Destroy_Static;
}
YApplication::~YApplication() ythrow()
{
	for(SHLs::const_iterator i(sShls.begin()); i != sShls.end(); ++i)
		YDelete(*i);
	//释放主 Shell 。
//	YDelete(DefaultShellHandle);
	EventArgs e;

	ApplicationExit(*this, e);
	delete pMessageQueue;
	delete pMessageQueueBackup;
}

void
YApplication::operator+=(YShell& shl)
{
	sShls.insert(&shl);
}
bool
YApplication::operator-=(YShell& shl)
{
	return sShls.erase(&shl);
}

bool
YApplication::Contains(HSHL hShl) const
{
	return sShls.count(hShl);
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
YApplication::SetShellHandle(HSHL h)
{
	if(Contains(h))
	{
		if(hShell == h)
			return false;
		if(hShell)
			hShell->OnDeactivated(Message(
				NULL, SM_DEACTIVATED, 0xF0, handle_cast<WPARAM>(hShell)));
		hShell = h;
		hShell->OnActivated(Message(
				NULL, SM_ACTIVATED, 0xF0, handle_cast<WPARAM>(h)));
	}
	return h;
}

void
YApplication::ResetShellHandle() ythrow()
{
	if(!SetShellHandle(DefaultShellHandle))
		Log.FatalError("YApplication::ResetShellHandle();");
}

YSL_END

