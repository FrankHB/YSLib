/*
	© 2009-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yshell.cpp
\ingroup Core
\brief Shell 定义。
\version r2398
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-13 21:09:15 +0800
\par 修改时间:
	2013-10-25 03:45 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YShell
*/


#include "YSLib/Core/yshell.h"
#include "YSLib/Core/yapp.h"

namespace YSLib
{

namespace Shells
{

using namespace Messaging;

Shell::~Shell()
{
	// TODO: Reconsider of using weak pointer in messages.
//	FetchAppInstance().Queue.Remove(this, 0xFF);
}

bool
Shell::IsActive() const
{
	// NOTE: %get_raw used here for performance.
	return get_raw(FetchAppInstance().GetShellHandle()) == this;
}

void
Shell::DefShlProc(const Message& msg)
{
	switch(msg.GetMessageID())
	{
	case SM_Set:
		{
			auto h(FetchTarget<SM_Set>(msg));

			FetchAppInstance().Switch(h);
		}
		break;
	case SM_Quit:
		std::exit(FetchTarget<SM_Quit>(msg));
		break;
	case SM_Bound:
		{
			const auto& pr(FetchTarget<SM_Bound>(msg));
			const auto h_shl(pr.first.lock());

			if(YB_LIKELY(h_shl))
				h_shl->OnGotMessage(pr.second);
		}
		break;
	case SM_Task:
		FetchTarget<SM_Task>(msg)();
	default:
		break;
	}
}

} // namespace Shells;

} // namespace YSLib;

