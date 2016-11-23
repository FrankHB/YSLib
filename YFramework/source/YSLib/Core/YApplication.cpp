/*
	© 2009-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YApplication.cpp
\ingroup Core
\brief 系统资源和应用程序实例抽象。
\version r1740
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-27 17:12:36 +0800
\par 修改时间:
	2016-11-23 13:09 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YApplication
*/


#include "YSLib/Core/YModules.h"
#include YFM_YSLib_Core_YApplication
#include YFM_YSLib_Core_YException // for ExtractAndTrace;

namespace YSLib
{

Application::Application()
	: Shell()
{}
Application::~Application()
{
	// NOTE: It is necessary to cleanup to make sure all shells are destroyed.
	qMain.clear();
//	hShell = {};
	// NOTE: All shells must have been released.
	YAssert(!hShell, "Active shell found.");
	YTraceDe(Notice, "Uninitialization entered with %zu handler(s) to be"
		" called.", on_exit.size());
	// NOTE: This is needed because the standard containers (and adaptors)
	//	guarantee nothing about destruction order of contained elements.
	while(!on_exit.empty())
		on_exit.pop();
}

void
Application::OnGotMessage(const Message& msg)
{
	TryExpr(Deref(GetShellHandle()).OnGotMessage(msg))
	CatchIgnore(Messaging::MessageSignal&)
#ifndef NDEBUG
	CatchExpr(std::exception& e, YTraceDe(Emergent, "Unexpected exception."),
		ExtractAndTrace(e, Emergent), throw)
	CatchExpr(..., YTraceDe(Emergent, "Unknown unexpected exception."), throw)
#endif
}

bool
Application::Switch(shared_ptr<Shell>& h) ynothrow
{
	if(YB_LIKELY(hShell != h))
	{
		std::swap(hShell, h);
		return true;
	}
	return {};
}


void
PostMessage(const Message& msg, Messaging::Priority prior)
{
	FetchAppInstance().AccessQueue([=, &msg](MessageQueue& mq){
		mq.Push(msg, prior);
	});
}

void
PostQuitMessage(int n, Messaging::Priority prior)
{
	YTraceDe(Informative, "Ready to post quit message with exit code = %d,"
		" priority = %u.", n, unsigned(prior));
	PostMessage<SM_Set>(prior, shared_ptr<Shell>());
	PostMessage<SM_Quit>(prior, n);
}

} // namespace YSLib;

