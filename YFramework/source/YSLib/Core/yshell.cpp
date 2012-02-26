/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yshell.cpp
\ingroup Core
\brief Shell 定义。
\version r3334;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-13 21:09:15 +0800;
\par 修改时间:
	2012-02-26 18:29 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Core::YShell;
*/


#include "YSLib/Core/yshell.h"
#include "YSLib/Core/yapp.h"
#include "YSLib/Helper/yglobal.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Shells)

using namespace Messaging;

Shell::~Shell()
{
	if(FetchAppInstance().GetShellHandle() == this)
		FetchAppInstance().SetShellHandle(shared_ptr<Shell>());
}

bool
Shell::IsActive() const
{
	return raw(FetchAppInstance().GetShellHandle()) == this;
}

int
Shell::DefShlProc(const Message& msg)
{
	switch(msg.GetMessageID())
	{
	case SM_SET:
		return -!FetchAppInstance().SetShellHandle(FetchTarget<SM_SET>(msg));

	case SM_QUIT:
		std::exit(FetchTarget<SM_QUIT>(msg));
		break;
	case SM_TASK:
		FetchTarget<SM_TASK>(msg)();

	default:
		break;
	}
	return 0;
}

YSL_END_NAMESPACE(Shells)

YSL_END

