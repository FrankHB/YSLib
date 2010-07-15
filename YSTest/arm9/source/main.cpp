// YShell DS 测试程序 -> ARM9 主程序 by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-12 21:26:30;
// UTime = 2010-7-14 15:09;
// Version = 0.1997;


//标识主源文件。
#ifdef _ARM9_MAIN_CPP_
#	error The main source file has already compiled. Try to make the macro _ARM9_MAIN_CPP_ be defined iff. once in the project.
#endif

#define _ARM9_MAIN_CPP_ _MAIN_CPP_

#ifdef _MAIN_CPP_
#	error The main source file has already compiled. Try to make the macro _MAIN_CPP_ be defined iff. once in the project.
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

YSL_BEGIN

CSTR G_COMP_NAME = "Franksoft";
CSTR G_APP_NAME = "YShell Test";
CSTR G_APP_VER = "0.2600";

//调试函数。
static void YSDebug_MSG_Print(const MMSG& msg)
{
	iprintf("time : %u ticks\n", GetTicks());
	iprintf("MMSG : 0x%04X;\nPrior : 0x%02X;\nObj : %d\nW : %u;\nL : %lx;\n", msg.GetMsgID(), msg.GetPriority(), msg.GetID(), msg.GetWParam(), msg.GetLParam());
	waitForInput();
	StartTicks();
}
void YSDebug_MSG_Insert(const MMSG& msg)
{
	YDebugBegin();
	iputs("-YSLIB_DEBUG : MSG_Insert");
	YSDebug_MSG_Print(msg);
}
void YSDebug_MSG_Peek(const MMSG& msg)
{
	YDebugBegin();
	iputs("-YSLIB_DEBUG : MSG_Peek");
	YSDebug_MSG_Print(msg);
}

YSL_END

using namespace YSLib;

int YMain(int argc, char* argv[])
{
	MMSG msg;

	//消息循环。
	while(GetMessage(msg))
	{
		TranslateMessage(msg);
		DispatchMessage(msg);
	}

	return static_cast<int>(msg.GetWParam());
}

