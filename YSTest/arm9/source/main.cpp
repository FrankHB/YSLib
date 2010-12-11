/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file main.cpp
\ingroup DS
\brief ARM9 主源文件。
\version 0.2049;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-12 21:26:30;
\par 修改时间:
	2010-12-05 06:17 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	Platform::DS::Main;
*/


//标识主源文件。
#ifdef _ARM9_MAIN_CPP_
#	error The main source file has already compiled. Try to make the macro \
		_ARM9_MAIN_CPP_ be defined iff. once in the project.
#endif

#define _ARM9_MAIN_CPP_ _MAIN_CPP_

#ifdef _MAIN_CPP_
#	error The main source file has already compiled. Try to make the macro \
		_MAIN_CPP_ be defined iff. once in the project.
#endif
#define _MAIN_CPP_

//定义使用 EFSLib 。
//#define USE_EFS

//包含文件。
#ifndef PLATFORM_H
#define PLATFORM_H <platform.h>
#endif
#include PLATFORM_H

#include YS_YSBUILD_H

using namespace platform;
using std::puts;

YSL_BEGIN

CSTR G_COMP_NAME = "Franksoft";
CSTR G_APP_NAME = "YShell Test";
CSTR G_APP_VER = "0.2600";

//调试函数。
static void YSDebug_MSG_Print(const Message& msg)
{
	iprintf("time : %u ticks\n", GetTicks());
	iprintf("Message : 0x%04X;\nPrior : 0x%02X;\nObj : %d\nW : %u;\nL : %lx;\n",
		msg.GetMsgID(), msg.GetPriority(), msg.GetID(),
		msg.GetWParam(), msg.GetLParam());
	WaitForInput();
	StartTicks();
}
void YSDebug_MSG_Insert(const Message& msg)
{
	YDebugBegin();
	puts("-YSLIB_DEBUG : MSG_Insert");
	YSDebug_MSG_Print(msg);
}
void YSDebug_MSG_Peek(const Message& msg)
{
	YDebugBegin();
	puts("-YSLIB_DEBUG : MSG_Peek");
	YSDebug_MSG_Print(msg);
}

YSL_END

using namespace YSLib;

int YMain(int argc, char* argv[])
{
	Message msg;

	//消息循环。
	while(GetMessage(msg) != SM_QUIT)
	{
		TranslateMessage(msg);
		DispatchMessage(msg);
	}

	return static_cast<int>(msg.GetLParam());
}

