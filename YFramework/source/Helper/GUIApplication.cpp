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
\version r275
\author FrankHB <frankhb1989@gmail.com>
\since build 396
\par 创建时间:
	2013-04-06 22:42:54 +0800
\par 修改时间:
	2014-05-27 14:03 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::GUIApplication
*/


#include "Helper/YModules.h"
#include YFM_Helper_GUIApplication
#include YFM_Helper_Host
#include YFM_Helper_Initialization
#if YF_Multithread == 1
#	include <thread> // for std::this_thread::*;
#endif

namespace YSLib
{

using namespace Drawing;

namespace
{

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
	: Application()
#if YF_Hosted
	, p_hosted()
#endif
{
	YAssert(!ApplicationPtr, "Duplicate instance found.");
	ApplicationPtr = this;
	InitializeEnviornment();
#if YF_Hosted
	p_hosted = make_unique<Host::Environment>();
#endif
}

GUIApplication::~GUIApplication()
{
	Uninitialize();
	ApplicationPtr = {};
}

#if YF_Hosted
Host::Environment&
GUIApplication::GetHost()
{
	YAssertNonnull(p_hosted);
	return *p_hosted;
}
#endif

bool
GUIApplication::DealMessage()
{
	return AccessQueue([this](MessageQueue& mq){
		using namespace Shells;

		if(mq.empty())
		//	Idle(UIResponseLimit);
			OnGotMessage(FetchIdleMessage());
		else
		{
			const auto i(mq.cbegin());

			if(YB_UNLIKELY(i->second.GetMessageID() == SM_Quit))
				return false;
			if(i->first < UIResponseLimit)
				Idle(UIResponseLimit);
			OnGotMessage(i->second);
			mq.erase(i);
		}
		return true;
	});
}


GUIApplication&
FetchGlobalInstance() ynothrow
{
	YAssertNonnull(ApplicationPtr);
	return *ApplicationPtr;
}

/* extern */Application&
FetchAppInstance() ynothrow
{
	return FetchGlobalInstance();
}


void
Execute(GUIApplication& app, shared_ptr<Shell> p_shl)
{
#if YF_Hosted
	Host::FetchEnvironment().ExitOnAllWindowThreadCompleted = true;
#endif
	if(YB_UNLIKELY(!Activate(p_shl)))
		throw LoggedEvent("Failed activating the main shell.");
	while(app.DealMessage())
		;
}

} // namespace YSLib;

