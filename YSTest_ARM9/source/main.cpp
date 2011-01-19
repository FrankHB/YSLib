/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file main.cpp
\ingroup DS
\brief ARM9 主源文件。
\version 0.2075;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-12 21:26:30;
\par 修改时间:
	2010-01-01 19:15 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	Platform::DS::Main;
*/


//标识主源文件。
#ifdef YSL_ARM9_MAIN_CPP_
#	error The main source file has already compiled. Try to make the macro \
		YSL_ARM9_MAIN_CPP_ be defined iff. once in the project.
#endif

#define YSL_ARM9_MAIN_CPP_ YSL_MAIN_CPP_

#ifdef YSL_MAIN_CPP_
#	error The main source file has already compiled. Try to make the macro \
		YSL_MAIN_CPP_ be defined iff. once in the project.
#endif
#define YSL_MAIN_CPP_

//定义使用 EFSLib 。
//#define USE_EFS

//包含文件。
#ifndef PLATFORM_H
#define PLATFORM_H <platform.h>
#endif
#include PLATFORM_H

#include YSL_YSBUILD_H

using namespace platform;
using std::puts;

YSL_BEGIN

const char* G_COMP_NAME = "Franksoft";
const char* G_APP_NAME = "YShell Test";
const char* G_APP_VER = "0.2600";

//调试函数。
namespace
{
	void
	YSDebug_MSG_Print(const Message& msg)
	{
		iprintf("time : %u ticks\n", GetTicks());

		Messaging::IContext* pContext(GetPointer(msg.GetContextPtr()));

		iprintf("Message : 0x%04X;\n""Prior : 0x%02X;\nObject ID : %d\n"
			"Context pointer: %p\n",
			msg.GetMessageID(), msg.GetPriority(), msg.GetObjectID(), pContext);
		//	"W : %u;\nL : %lx;\n";
		WaitForInput();
		StartTicks();
	}
}

void
YSDebug_MSG_Insert(const Message& msg)
{
	YDebugBegin();
	puts("-YSLIB_DEBUG : MSG_Insert");
	YSDebug_MSG_Print(msg);
}

void
YSDebug_MSG_Peek(const Message& msg)
{
	YDebugBegin();
	puts("-YSLIB_DEBUG : MSG_Peek");
	YSDebug_MSG_Print(msg);
}

YSL_END

using namespace YSLib;

int
YMain(int /*argc*/, char* /*argv*/[])
{
	using namespace Shells;

	Message msg;

	//消息循环。
	while(GetMessage(msg) != SM_QUIT)
	{
		TranslateMessage(msg);
		DispatchMessage(msg);
	}

	return msg.GetContextPtr() ? 0 : -1;
}

