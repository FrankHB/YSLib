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
\version r2580;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-12-27 17:12:36 +0800;
\par 修改时间:
	2012-03-20 16:13 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Core::YApplication;
*/


#include "YSLib/Core/yapp.h"

YSL_BEGIN

Application::Application()
	: Queue(), BackupQueue(), hShell(), msg()
{}
Application::~Application()
{
	//释放主 Shell 。
//	YReset(DefaultShellHandle);
	if(ApplicationExit)
		ApplicationExit();
}

errno_t
Application::BackupMessage(const Message& msg)
{
	BackupQueue.Push(msg);
	return -!msg.IsValid();
}

int
Application::Dispatch(const Message& msg)
{
	const auto hShl(GetShellHandle());

	YAssert(bool(hShl), "Null shell handle found @ DispatchMessage;");

	return hShl->OnGotMessage(msg);
}

void
Application::RecoverMessageQueue()
{
	Queue.Merge(BackupQueue);
}

bool
Application::Switch(shared_ptr<Shell>& h) ynothrow
{
	if(YCL_LIKELY(hShell != h))
	{
		std::swap(hShell, h);
		return true;
	}
	return false;
}


void
SendMessage(const Message& msg) ynothrow
{
	FetchAppInstance().Queue.Push(msg);

#if YSL_DEBUG_MSG & 1

	void YSDebug_MSG_Insert(Message&);
	YSDebug_MSG_Insert(msg);

#endif

}
void
SendMessage(const shared_ptr<Shell>& hShl, Messaging::ID id,
	Messaging::Priority prior, const ValueObject& c) ynothrow
{
	SendMessage(Message(hShl, id, prior, c));
}

void
PostQuitMessage(int nExitCode, Messaging::Priority p)
{
	SendMessage<SM_SET>(shared_ptr<Shell>(), p, shared_ptr<Shell>());
	SendMessage<SM_QUIT>(shared_ptr<Shell>(), p, nExitCode);
}

YSL_END

