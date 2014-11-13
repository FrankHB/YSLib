/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file GUIApplication.cpp
\ingroup Helper
\brief GUI 应用程序。
\version r338
\author FrankHB <frankhb1989@gmail.com>
\since build 396
\par 创建时间:
	2013-04-06 22:42:54 +0800
\par 修改时间:
	2014-11-12 04:15 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::GUIApplication
*/


#include "Helper/YModules.h"
#include YFM_Helper_GUIApplication
#include YFM_Helper_Environment
#if YF_Multithread == 1
#	include <thread> // for std::this_thread::*;
#endif

namespace YSLib
{

using namespace Drawing;

namespace
{

//! \since build 550
recursive_mutex ApplicationMutex;

//! \since build 499
GUIApplication* ApplicationPtr;

/*!
\brief 取空闲消息。
\since build 320
*/
inline Message
FetchIdleMessage()
{
	return Message(SM_Input);
}

/*!
\brief 后台消息处理程序。
\since build 320
*/
inline void
Idle(Messaging::Priority prior)
{
	// NOTE: Wait for GUI input of any shells. Post message for specific shell
	//	would cause low performance when there are many candidate messages
	//	of distinct shells.
	PostMessage(FetchIdleMessage(), prior);
}

} // unnamed namespace;


GUIApplication::GUIApplication()
	: Application(), p_env()
{
	lock_guard<recursive_mutex> lck(ApplicationMutex);

	YAssert(!ApplicationPtr, "Duplicate instance found.");
	ApplicationPtr = this;
	p_env.reset(new Environment());
}

GUIApplication::~GUIApplication()
{
	lock_guard<recursive_mutex> lck(ApplicationMutex);

	ApplicationPtr = {};
}

Environment&
GUIApplication::GetEnvironment() ynothrow
{
	return Deref(p_env);
}

bool
GUIApplication::DealMessage()
{
	return AccessQueue([this](MessageQueue& mq)->bool{
		using namespace Shells;

		if(mq.empty())
		//	Idle(UIResponseLimit);
			OnGotMessage(FetchIdleMessage());
		else
		{
			const auto i(mq.cbegin());

			if(YB_UNLIKELY(i->second.GetMessageID() == SM_Quit))
				return {};
			if(i->first < UIResponseLimit)
				Idle(UIResponseLimit);
			OnGotMessage(i->second);
			mq.erase(i);
		}
		return true;
	});
}


GUIApplication&
FetchGlobalInstance()
{
	lock_guard<recursive_mutex> lck(ApplicationMutex);

	if(ApplicationPtr)
		return *ApplicationPtr;
	throw LoggedEvent("Application instance is not ready.");
}

/* extern */Application&
FetchAppInstance()
{
	return FetchGlobalInstance();
}

locked_ptr<GUIApplication, recursive_mutex>
LockInstance()
{
	unique_lock<recursive_mutex> lck(ApplicationMutex);

	return {ApplicationPtr, std::move(lck)};
}


void
Execute(GUIApplication& app, shared_ptr<Shell> p_shl)
{
#if YF_Hosted
	FetchEnvironment().ExitOnAllWindowThreadCompleted = true;
#endif
	if(YB_UNLIKELY(!Activate(p_shl)))
		throw LoggedEvent("Failed activating the main shell.");
	while(app.DealMessage())
		;
}

} // namespace YSLib;

