/*
	Copyright (C) by Franksoft 2009 - 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yglobal.cpp
\ingroup Helper
\brief 平台相关的全局对象和函数定义。
\version 0.2602;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-22 15:28:52 + 08:00;
\par 修改时间:
	2010-12-04 07:22 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Helper::YGlobal;
*/


#include "yglobal.h"
#include "../ysbuild.h"
#include <exception>
//#include <clocale>

using namespace platform;

YSL_BEGIN

using namespace Runtime;

//全局常量。
extern CSTR DEF_DIRECTORY; //<! 默认目录。
extern CSTR G_COMP_NAME; //<! 制作组织名称。
extern CSTR G_APP_NAME; //!< 产品名称。
extern CSTR G_APP_VER; //!< 产品版本。
const SDST SCRW(SCREEN_WIDTH), SCRH(SCREEN_HEIGHT);
const IO::Path YApplication::CommonAppDataPath(DEF_DIRECTORY);
const String YApplication::CompanyName(G_COMP_NAME);
const String YApplication::ProductName(G_APP_NAME);
const String YApplication::ProductVersion(G_APP_VER);

//全局变量。
YScreen *pScreenUp, *pScreenDown;
YDesktop *pDesktopUp, *pDesktopDown;
#ifdef YSL_USE_MEMORY_DEBUG
MemoryList DebugMemory(NULL);
#endif
YLog DefaultLog;

/*!
\ingroup PublicObject
\brief 全局变量映射。
\note 需要保证 YApplication::DefaultShellHandle 在 theApp 初始化之后初始化，
	因为 YShellMain 的基类 YShell 的构造函数调用了 theApp 的非静态成员函数。
*/
/*! @{ */
YApplication& theApp(YApplication::GetApp(pScreenUp, pDesktopUp));
const HSHL YApplication::DefaultShellHandle(new YShellMain());
/*! @} */


namespace
{
	bool
	operator!=(const KeysInfo& a, const KeysInfo& b)
	{
		return a.up != b.up || a.down != b.down || a.held != b.held;
	}

	/*!
	\note 转换指针设备光标位置为屏幕点。
	*/
	inline Drawing::Point
	ToSPoint(const Runtime::CursorInfo& c)
	{
		return Drawing::Point(c.GetX(), c.GetY());
	}

	//图形用户界面输入等待函数。
	void
	WaitForGUIInput()
	{
		static KeysInfo Key;
		static CursorInfo TouchPos_Old, TouchPos;
		static Message /*InputMessage_Old, */InputMessage;

		if(Key.held & KeySpace::Touch)
			TouchPos_Old = TouchPos;
		scanKeys();
		WriteKeysInfo(Key, TouchPos);

		const Point pt(ToSPoint(Key.held & KeySpace::Touch
			? TouchPos : TouchPos_Old));

		if((theApp.GetDefaultMessageQueue().empty()
			|| Key != *reinterpret_cast<KeysInfo*>(InputMessage.GetWParam())
			|| pt != InputMessage.GetCursorLocation())
			&& pt != Point::FullScreen)
			InsertMessage((InputMessage = Message(
				NULL, SM_INPUT, 0x40, reinterpret_cast<WPARAM>(&Key), 0, pt)));
	/*
		InputMessage = Message(NULL, SM_INPUT, 0x40,
			reinterpret_cast<WPARAM>(&Key), 0, ToSPoint(tp));

		if(InputMessage != InputMessage_Old)
		{
			InsertMessage(InputMessage);
			InputMessage_Old = InputMessage;
		}
	*/
	}
}


void
Idle()
{
//	if(DefaultShellHandle->insRefresh)
//		InsertMessage(NULL, SM_SCRREFRESH, 0x80,
//			DefaultShellHandle->scrType, 0);
	WaitForGUIInput();
}

bool
InitConsole(YScreen& scr, Drawing::PixelType fc, Drawing::PixelType bc)
{
	if(&scr == pScreenUp)
		YConsoleInit(true, fc, bc);
	else if(&scr == pScreenDown)
		YConsoleInit(false, fc, bc);
	else
		return false;
	return true;
}

void
Destroy_Static(YObject&, EventArgs&)
{
	//释放显示设备。
	delete pDesktopUp;
	delete pScreenUp;
	delete pDesktopDown;
	delete pScreenDown;
}

LRES
ShlProc(HSHL hShl, const Message& msg)
{
	return hShl->ShlProc(msg);
}

//非 yglobal.h 声明的平台相关函数。

bool
InitAllScreens()
{
	using namespace Runtime;

	InitVideo();
	pScreenUp->SetPtr(DS::InitScrUp(pScreenUp->bg));
	pScreenDown->SetPtr(DS::InitScrDown(pScreenDown->bg));
	return true;
}


namespace
{
	//! \brief 全局非静态资源释放函数。
	void
	YDestroy()
	{
		//释放默认字体资源。
		DestroySystemFontCache();
	}

	//! \brief 初始化函数。
	void
	YInit()
	{
		//设置默认异常终止函数。
		std::set_terminate(terminate);

		//启用设备。
		powerOn(POWER_ALL);

		//启用 LibNDS 默认异常处理。
		defaultExceptionHandler();

		//注册退出函数。
	#ifdef YSL_USE_MEMORY_DEBUG
		atexit(OnExit_DebugMemory);
	#endif
		atexit(YDestroy);

		//初始化主控制台。
		InitYSConsole();

	/*	if(!setlocale(LC_ALL, "zh_CN.GBK"))
		{
			EpicFail();
			platform::yprintf("setlocale() with %s failed.\n", "zh_CN.GBK");
			terminate();
		}*/

		//初始化文件系统。
		//初始化 EFSLib 和 LibFAT 。
		//当 .nds 文件大于32MB时， EFS 行为异常。
	#ifdef USE_EFS
		if(!EFS_Init(EFS_AND_FAT | EFS_DEFAULT_DEVICE, NULL))
		{
			//如果初始化 EFS 失败则初始化 FAT 。
	#endif
			if(!fatInitDefault())
				LibfatFail();
			IO::ChangeDirectory(Text::StringToMBCS(
				YApplication::CommonAppDataPath));
	#ifdef USE_EFS
		}
	#endif

		//检查程序是否被正确安装。
		CheckInstall();

		//初始化系统字体资源。
		InitializeSystemFontCache();

		//初始化显示设备。
		try
		{
			pScreenUp = new YScreen(SCRW, SCRH);
			pScreenDown = new YScreen(SCRW, SCRH);
		}
		catch(...)
		{
			throw LoggedEvent("Screen initialization failed.");
		}
		try
		{
			pDesktopUp = new YDesktop(*pScreenUp);
			pDesktopDown = new YDesktop(*pScreenDown);
		}
		catch(...)
		{
			throw LoggedEvent("Desktop initialization failed.");
		}
		//注册全局应用程序对象。
		theApp.ResetShellHandle();
		//theApp.SetOutputPtr(pDesktopUp);
		//DefaultShellHandle->SetShlProc(ShlProc);
	}
}

#ifdef YSL_USE_MEMORY_DEBUG

void
OnExit_DebugMemory()
{
	YDebugSetStatus();
	YDebugBegin();
	std::puts("Normal exit;");

//	std::FILE* fp(std::fopen("memdbg.log", "w"));
	MemoryList::MapType::size_type s(DebugMemory.GetSize());

	if(s != 0)
	{
		iprintf("%i memory leak(s) detected:\n", s);
		DebugMemory.PrintAll(stdout);
	//	DebugMemory.PrintAll(fp);
	}
//	std::fclose(fp);
	std::puts("Press key to terminate...");
	WaitForInput();
}

#endif

YSL_END

/*!
\brief 程序默认入口函数。
*/
extern int
YMain(int argc, char* argv[]);

int
main(int argc, char* argv[])
{
	try
	{
		//全局初始化。
		YSL_ YInit();

		return YMain(argc, argv);
	}
	catch(std::bad_alloc&)
	{
		YSL_ theApp.Log.FatalError("Unhandled std::bad_alloc"
			" @@ int main(int, char*[]);");
	}
	catch(std::bad_cast&)
	{
		YSL_ theApp.Log.FatalError("Unhandled std::bad_cast"
			" @@ int main(int, char*[]);");
	}
	catch(YSL_ LoggedEvent& e)
	{
		YSL_ theApp.Log.FatalError(e.what());
	}
	catch(...)
	{
		YSL_ theApp.Log.FatalError("Unhandled exception"
			" @@ int main(int, char*[]);");
	}
}

