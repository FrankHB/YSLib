/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yapp.cpp
\ingroup Core
\brief 系统资源和应用程序实例抽象。
\version r1688
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-27 17:12:36 +0800
\par 修改时间:
	2014-05-23 09:40 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YApplication
*/


#include "YSLib/Core/YModules.h"
#include YFM_YSLib_Core_YApplication

namespace YSLib
{

Application::Application()
	: Shell(), qMain(), hShell()
{}
Application::~Application()
{
	// NOTE: It is necessary to cleanup to make sure all shells are destroyed.
	qMain.clear();
//	hShell = {};
	// NOTE: All shells must have been released.
	YAssert(!hShell, "Active shell found.");
	if(ApplicationExit)
		ApplicationExit();
}

void
Application::OnGotMessage(const Message& msg)
{
	const auto hShl(GetShellHandle());

	YAssert(bool(hShl), "Null handle found.");
	try
	{
		hShl->OnGotMessage(msg);
	}
	catch(Messaging::MessageSignal&)
	{}
}

bool
Application::Switch(shared_ptr<Shell>& h) ynothrow
{
	if(YB_LIKELY(hShell != h))
	{
		std::swap(hShell, h);
		return true;
	}
	return false;
}


void
PostMessage(const Message& msg, Messaging::Priority prior) ynothrow
{
	FetchAppInstance().AccessQueue([=, &msg](MessageQueue& mq){
		mq.Push(msg, prior);
	});
}

void
PostQuitMessage(int nExitCode, Messaging::Priority prior)
{
	PostMessage<SM_Set>(prior, shared_ptr<Shell>());
	PostMessage<SM_Quit>(prior, nExitCode);
}

} // namespace YSLib;

