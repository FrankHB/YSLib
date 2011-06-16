/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yshell.cpp
\ingroup Core
\brief Shell 定义。
\version 0.3301;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-13 21:09:15 +0800;
\par 修改时间:
	2011-06-16 02:56 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YShell;
*/


#include "yshell.h"
#include "yapp.h"
#include "../Helper/yglobal.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Shells)

using namespace Messaging;

YShell::~YShell()
{
	if(FetchAppInstance().GetShellHandle() == this)
		FetchAppInstance().SetShellHandle(shared_ptr<YShell>());
}

bool
YShell::IsActive() const
{
	return raw(FetchAppInstance().GetShellHandle()) == this;
}

int
YShell::DefShlProc(const Message& msg)
{
	switch(msg.GetMessageID())
	{
	case SM_SET:
	case SM_DROP:
		{
			auto hShl(FetchTarget<SM_SET>(msg));

			switch(msg.GetMessageID())
			{

			case SM_SET:
				return -!FetchAppInstance().SetShellHandle(hShl);

			case SM_DROP:
				{
					if(hShl->IsActive())
						FetchAppInstance().SetShellHandle(shared_ptr<YShell>());
					if(hShl->IsActive())
						return -1;
					reset(hShl);
				}
			default:
				break;
			}
		}
		return 0;

	case SM_QUIT:
		std::exit(FetchTarget<SM_QUIT>(msg));

	default:
		break;
	}
	return 0;
}

int
YShell::OnActivated(const Message& msg)
{
	SendMessage(msg);
	return 0;
}

int
YShell::OnDeactivated(const Message& msg)
{
	SendMessage(msg);
	return 0;
}

YSL_END_NAMESPACE(Shells)

YSL_END

