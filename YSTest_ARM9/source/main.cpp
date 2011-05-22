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
\version 0.2112;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-12 21:26:30 +0800;
\par 修改时间:
	2011-05-17 12:42 +0800;
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
#include <api.h>

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

		auto c(msg.GetContent());

		iprintf("Message : 0x%04X;\n""Prior : 0x%02X;\nObject ID : %d\n"
			"Context pointer: %p\n",
			msg.GetMessageID(), msg.GetPriority(), msg.GetObjectID(), &c);
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

