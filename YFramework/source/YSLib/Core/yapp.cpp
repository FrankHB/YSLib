/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yapp.cpp
\ingroup Core
\brief 系统资源和应用程序实例抽象。
\version r2631;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-12-27 17:12:36 +0800;
\par 修改时间:
	2012-06-15 13:53 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Core::YApplication;
*/


#include "YSLib/Core/yapp.h"

YSL_BEGIN

Application::Application()
	: Shell(), Queue(), hShell()
{}
Application::~Application()
{
//	hShell = nullptr;
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

	hShl->OnGotMessage(msg);
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
	FetchAppInstance().Queue.Push(msg, prior);
}
void
PostMessage(const weak_ptr<Shell>& wp, Messaging::ID id,
	Messaging::Priority prior, const ValueObject& c) ynothrow
{
	PostMessage(Message(wp, id, prior, c), prior);
}

void
PostQuitMessage(int nExitCode, Messaging::Priority p)
{
	PostMessage<SM_SET>(weak_ptr<Shell>(), p, shared_ptr<Shell>());
	PostMessage<SM_QUIT>(weak_ptr<Shell>(), p, nExitCode);
}

YSL_END

