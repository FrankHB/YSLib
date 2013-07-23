/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file GUIApplication.cpp
\ingroup Helper
\brief GUI 应用程序。
\version r224
\author FrankHB <frankhb1989@gmail.com>
\since build 396
\par 创建时间:
	2013-04-06 22:42:54 +0800
\par 修改时间:
	2013-07-23 19:46 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::GUIApplication
*/


#include "Helper/GUIApplication.h"
#include "Helper/Host.h"
#include "Helper/Initialization.h"
#if YCL_MULTITHREAD == 1
#	include <thread> // for std::this_thread::*;
#endif

YSL_BEGIN

using namespace Drawing;

namespace
{

//! \since build 398
GUIApplication* pApp;

/*!
\brief 取空闲消息。
\since build 320
*/
inline Message
FetchIdleMessage()
{
	return Message(SM_INPUT);
}

/*!
\brief 后台消息处理程序。
\since build 320
*/
inline void
Idle(Messaging::Priority prior)
{
	// Note: Wait for GUI input of any shells. Post message for specific shell
	//	would cause low performance when there are many candidate messages
	//	of distinct shells.
	PostMessage(FetchIdleMessage(), prior);
}

} // unnamed namespace;


GUIApplication::GUIApplication()
	: Application(),
#if YCL_HOSTED
	p_hosted(),
#endif
	UIResponseLimit(0x40)
{
	YAssert(!YSLib::pApp, "Duplicate instance found.");

	YSLib::pApp = this;
	InitializeEnviornment();
#if YCL_HOSTED
	p_hosted = make_unique<Host::Environment>();
#endif
}

GUIApplication::~GUIApplication()
{
	Uninitialize();
}

#if YCL_HOSTED
Host::Environment&
GUIApplication::GetHost()
{
	YAssert(bool(p_hosted), "Null pointer found.");

	return *p_hosted;
}
#endif

bool
GUIApplication::DealMessage()
{
	using namespace Shells;

	if(Queue.IsEmpty())
	//	Idle(UIResponseLimit);
		OnGotMessage(FetchIdleMessage());
	else
	{
		// TODO: Consider the application queue to be locked for thread safety.
		const auto i(Queue.GetBegin());

		if(YB_UNLIKELY(i->second.GetMessageID() == SM_QUIT))
			return false;
		if(i->first < UIResponseLimit)
			Idle(UIResponseLimit);
		OnGotMessage(i->second);
		Queue.Erase(i);
	}
	return true;
}


GUIApplication&
FetchGlobalInstance() ynothrow
{
	YAssert(pApp, "Null pointer found.");

	return *pApp;
}

/* extern */Application&
FetchAppInstance() ynothrow
{
	return FetchGlobalInstance();
}


void
Execute(GUIApplication& app, shared_ptr<Shell> p_shl)
{
#if YCL_HOSTED
	Host::FetchEnvironment().ExitOnAllWindowThreadCompleted = true;
#endif
	if(YB_UNLIKELY(!Activate(p_shl)))
		throw LoggedEvent("Failed launching the main shell;");
	while(app.DealMessage())
		;
}

YSL_END

